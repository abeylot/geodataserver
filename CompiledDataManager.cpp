#include "CompiledDataManager.hpp"


Way* CompiledDataManager::loadWay(uint64_t id)
{
    GeoWayIndex record;
    bool found = wayIndex->get(id,&record);
    if(found)
    {
        Way* w = new Way();
        w->id = id;
        fillPoints(&(w->points), w->pointsCount,record.pstart,record.psize);
        if(w->pointsCount > 0)
        {
            GeoPoint *curpoint = NULL;
            GeoPoint *lastpoint = NULL;
            curpoint = w->points;
            w->rect.x0 =w->rect.x1 = curpoint->x;
            w->rect.y0 =w->rect.y1 = curpoint->y;
            for(lastpoint = curpoint + w->pointsCount ; curpoint  < lastpoint; curpoint++)
            {
                if(curpoint->x < w->rect.x0) w->rect.x0 = curpoint->x;
                if(curpoint->x > w->rect.x1) w->rect.x1 = curpoint->x;
                if(curpoint->y < w->rect.y0) w->rect.y0 = curpoint->y;
                if(curpoint->y > w->rect.y1) w->rect.y1 = curpoint->y;
            }
        }
        fillTags(w->tags,record.tstart,record.tsize);
        return w;
    }
    else return NULL;
}

Point* CompiledDataManager::loadPoint(uint64_t id)
{
    GeoPointIndex record;
    bool found = nodeIndex->get(id,&record);
    if(found)
    {
        Point* p = new Point();
        p->id = id;
        p->x = record.x;
        p->y = record.y;
        fillTags(p->tags,record.tstart,record.tsize);
        return p;
    }
    else return NULL;

}

Relation* CompiledDataManager::loadRelation(uint64_t id)
{
    return loadRelation(id, 2);
}

Relation* CompiledDataManager::loadRelation(uint64_t id, short recurs)
{
    recurs --;
    if (recurs < 1) return NULL;
    GeoIndex record;
    bool found = relationIndex->get(id,&record);
    if(found)
    {
        Relation* r = new Relation();
        r->id = id;
        r->isPyramidal = false;
        r->rect.x0 = r->rect.x1 = UINT32_C(0xFFFFFFFF);
        r->rect.y0 = r->rect.y1 = UINT32_C(0xFFFFFFFF);
        fillTags(r->tags,record.tstart,record.tsize);
        if(r->tags["type"] == "land_area")
        {
            r->isPyramidal = true;
        }
        fillLinkedItems(*r,record.mstart,record.msize, recurs);
        return r;
    }
    else return NULL;
}

Relation* CompiledDataManager::loadRelationFast(uint64_t id)
{
    GeoIndex record;
    bool found = relationIndex->get(id,&record);
    if(found)
    {
        Relation* r = new Relation();
        r->rect.x0 = r->rect.x1 = UINT32_C(0xFFFFFFFF);
        r->rect.y0 = r->rect.y1 = UINT32_C(0xFFFFFFFF);
        fillTags(r->tags,record.tstart,record.tsize);
        return r;
    }
    else return NULL;
}

void CompiledDataManager::fillPoints(GeoPoint ** points, uint64_t& pointsCount, uint64_t start, uint64_t size)
{
    *points = wayPoints->getData(start, size);
    pointsCount = size;
}

void CompiledDataManager::fillTags(Tags& tags, uint64_t start, uint64_t size)
{
    tags.data = baliseTags->getData(start, size);
    tags.data_size = size;
}

void CompiledDataManager::fillLinkedItems(Relation& r, uint64_t start, uint64_t size, short recurs)
{
    GeoMember* buffer = relMembers->getData(start,size);
    Way* newWay = NULL;
    Point* newPoint;
    Relation* newRel;
    for(uint64_t i = 0; i < size; i++)
    {
        switch(buffer[i].type)
        {
        case BaliseType::point:
            newPoint = loadPoint(buffer[i].id);
            if(newPoint)
            {
                r.points.push_back(newPoint);
            }
            break;
        case BaliseType::way:
            newWay = loadWay(buffer[i].id);
            if(newWay)
            {
                r.shape.mergePoints(newWay->points, newWay->pointsCount);
                r.ways.push_back(newWay);
                r.rect = r.rect + newWay->rect;
            }
            break;
        case BaliseType::relation:
            newRel = loadRelation(buffer[i].id, recurs);
            if(newRel)
            {
                r.relations.push_back(newRel);
                r.rect = r.rect + newRel->rect;
                if(r.isPyramidal)
                {
                    for(Line* l : newRel->shape.lines)
                    {
                        r.shape.mergePoints(l->points, l->pointsCount);
                    }
                    for(Relation* rel : newRel->relations)
                        for(Line* l : rel->shape.lines)
                        {
                            r.shape.mergePoints(l->points, l->pointsCount);
                        }
                }
            }
            break;
        default:
            break;
        }
    }
    r.isClosed = true;

    if (r.isPyramidal )
    {
        /*for(unsigned int i = 0; i < r.shape.lines.size(); i++)
        {
            if (! r.shape.lines[i]->isClosed())
            {
                delete r.shape.lines[i];
                r.shape.lines.erase(r.shape.lines.begin() + i);
            }
        }*/
        for (auto it = r.shape.lines.begin(); it != r.shape.lines.end(); )
        {
        	if (!(*it)->isClosed())
        	{
        		delete (*it);
            	it = r.shape.lines.erase(it);
        	}
        	else
        	{
            	++it;
            }
        }
    }

    for(Line* l : r.shape.lines)
    {
        r.isClosed = r.isClosed && l->isClosed();
    }
    free(buffer);
}

bool Line::isClosed()
{
    if(closed) return true;
    if ( points[0] == points[pointsCount - 1] ) closed = true;
    return closed;
}

void Line::crop(Rectangle& r)
{
	if (pointsCount < 3) return;
    uint64_t i;
    GeoPoint* newPoints;
    uint64_t newPointsCount = 0;
    bool discarded;
    newPoints = static_cast<GeoPoint*> (malloc (pointsCount * sizeof(GeoPoint)));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].x < r.x0)&&(points[i].x < r.x0)&&(points[i+1].x < r.x0)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;

    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc (pointsCount * sizeof(GeoPoint)));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].x > r.x1)&&(points[i].x > r.x1)&&(points[i+1].x > r.x1)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc (pointsCount * sizeof(GeoPoint)));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].y < r.y0)&&(points[i].y < r.y0)&&(points[i+1].y < r.y0)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc (pointsCount * sizeof(GeoPoint)));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].y > r.y1)&&(points[i].y > r.y1)&&(points[i+1].y > r.y1)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
}

void Way::crop(Rectangle& r)
{
    if(pointsCount < 3) return;
    uint64_t i;
    GeoPoint* newPoints;
    uint64_t newPointsCount = 0;
    bool discarded;
    newPoints = static_cast<GeoPoint*> (malloc ((pointsCount * sizeof(GeoPoint))));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].x < r.x0)&&(points[i].x < r.x0)&&(points[i+1].x < r.x0)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc ((pointsCount * sizeof(GeoPoint))));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].x > r.x1)&&(points[i].x > r.x1)&&(points[i+1].x > r.x1)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc (pointsCount * sizeof(GeoPoint)));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].y < r.y0)&&(points[i].y < r.y0)&&(points[i+1].y < r.y0)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*>( malloc (pointsCount * sizeof(GeoPoint)));
    newPoints[0] = points[0];
    newPointsCount++;
    for(i = 1; i < (pointsCount - 1); i++)
    {
        discarded = false;
        if((points[i-1].y > r.y1)&&(points[i].y > r.y1)&&(points[i+1].y > r.y1)) discarded = true;
        if(!discarded)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    newPoints[newPointsCount] = points[pointsCount - 1];
    newPointsCount++;
    free(points);
    points = newPoints;
    pointsCount = newPointsCount;
}


bool Line::mergePoints (GeoPoint* points, uint64_t pointsCount)
{
    if(this->points == NULL) return true;
    if(points == NULL) return true;
    if(!this->isClosed())
    {
        /*if(this->points[0] == points[0])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            fidx::FileRawData<GeoPoint>::revert(this->points, this->pointsCount);
            memcpy(this->points + this->pointsCount, points , pointsCount*sizeof(GeoPoint) );
            this->pointsCount += pointsCount;
            return true;
        }
        else*/ if (this->points[this->pointsCount - 1] == points[0])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            memcpy(this->points + (this->pointsCount - 1), points , pointsCount *sizeof(GeoPoint));
            this->pointsCount += (pointsCount - 1);
            return true;
        }
        /*else if (this->points[this->pointsCount - 1] == points[pointsCount - 1])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            memcpy(this->points + this->pointsCount, points, pointsCount *sizeof(GeoPoint));
            fidx::FileRawData<GeoPoint>::revert(this->points+this->pointsCount,pointsCount );
            this->pointsCount += pointsCount;
            return true;
        }*/
        else if (this->points[0] == points[pointsCount - 1])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            memmove(this->points + pointsCount , this->points + 1, ( this->pointsCount - 1) *sizeof(GeoPoint));
            memcpy(this->points, points, pointsCount *sizeof(GeoPoint));
            this->pointsCount += pointsCount - 1 ;
            return true;
        }
    }
    return false;
}


void Shape::mergePoints(GeoPoint* points, uint64_t pointsCount)
{
    bool merged = false;
    unsigned int i = 0;
    if(!(points[0] == points[pointsCount - 1]))
    {
        for(i = 0; i < lines.size(); i++)
        {
            Line* l = lines[i];
            if(l->mergePoints(points, pointsCount))
            {
                merged = true;
                break;
            }
        }
    }
    if(!merged)
    {
        GeoPoint* newPoints = static_cast<GeoPoint*> (malloc(pointsCount*sizeof(GeoPoint)));
        memcpy(newPoints, points, pointsCount*sizeof(GeoPoint));
        Line* l = new Line();
        l->points = newPoints;
        l->pointsCount = pointsCount;
        lines.push_back(l);
    }
    else
    {
        merged = false;
        unsigned int j;
        for(j = i+1; j < lines.size(); j++)
        {
            Line* l = lines[j];
            if(l->mergePoints(lines[i]->points, lines[i]->pointsCount))
            {
                merged = true;
                break;
            }
        }
        if(merged)
        {
            delete lines[i];
            lines.erase(lines.begin() + i);
        }
    }
}
