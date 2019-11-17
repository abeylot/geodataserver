#include "CompiledDataManager.hpp"


Way* CompiledDataManager::loadWay(uint64_t id)
{
    GeoWayIndex record;
    bool found = wayIndex->get(id,&record);
    if(found)
    {
        Way* w = new Way();
        w->id = id;
        GeoPoint *curpoint, *lastpoint;
        fillPoints(&(w->points), w->pointsCount,record.pstart,record.psize);
        if(w->pointsCount > 0)
        {
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
    //recurs --;
    //if (recurs < 1) return NULL;
    GeoIndex record;
    bool found = relationIndex->get(id,&record);
    if(found)
    {
        Relation* r = new Relation();
        r->rect.x0 = r->rect.x1 = UINT32_C(0xFFFFFFFF);
        r->rect.y0 = r->rect.y1 = UINT32_C(0xFFFFFFFF);
        fillTags(r->tags,record.tstart,record.tsize);
        //fillLinkedItems(*r,record.mstart,record.msize, recurs);
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
    /*size_t used = 0;
    while( used < size)
    {
        unsigned char s = (unsigned char) buffer[used];
        used ++;
        std::string tag = std::string(buffer+used, s);
        used += s;
        s = (unsigned char) buffer[used];
        used++;
        std::string value = std::string(buffer+used, s);
        used += s;
        tags[tag] = value;
    }*/
    //free(buffer);
}

void CompiledDataManager::fillLinkedItems(Relation& r, uint64_t start, uint64_t size, short recurs)
{
    GeoMember* buffer = relMembers->getData(start,size);
    //Way* lastWay = NULL;
    Way* newWay = NULL;
    //bool isClosed = false;
    //GeoPoint* first = NULL;
    //GeoPoint* last = NULL;
    Point* newPoint;
    Relation* newRel;
    //int openWays = 0;
    for(uint64_t i = 0; i < size; i++)
    {
        switch(buffer[i].type)
        {
        case BaliseType::point:
            newPoint = loadPoint(buffer[i].id);
            if(newPoint)
            {
                r.points.push_back(newPoint);
                //r.rect.addPoint(newPoint->x, newPoint->y);
            }
            break;
        case BaliseType::way:
            newWay = loadWay(buffer[i].id);
            if(newWay)
            {
                r.shape.mergePoints(newWay->points, newWay->pointsCount);
                /*isClosed = (newWay->points[0] == newWay->points[newWay->pointsCount -1]);

                if(!isClosed && lastWay == NULL) {// ouverture d'une boucle
                	openWays++;
                	lastWay = newWay;
                	first = &(newWay->points[0]);
                	last = &(newWay->points[newWay->pointsCount -1]);
                } else if (isClosed) { // ajout boucle simple
                	lastWay = NULL;
                }else if ((*first ==  newWay->points[newWay->pointsCount -1])// fermeture boucle
                			&&(*last == newWay->points[0])) {
                	openWays--;
                	lastWay = NULL;
                	first = NULL;
                	last = NULL;
                } else if ((*last ==  newWay->points[newWay->pointsCount -1])
                			&&(*first == newWay->points[0])) { //fermeture boucle avec inversion
                	openWays--;
                	lastWay = NULL;
                	first = NULL;
                	last = NULL;
                	fidx::FileRawData<GeoPoint>::revert(newWay->points, newWay->pointsCount);
                } else if (*last == newWay->points[0]) { // poursuite boucle
                	lastWay = newWay;
                	last = &(newWay->points[newWay->pointsCount -1]);
                } else if (*last == newWay->points[newWay->pointsCount -1]) { // poursuite boucle avec inversion
                	lastWay = newWay;
                	fidx::FileRawData<GeoPoint>::revert(newWay->points, newWay->pointsCount);
                	last = &(newWay->points[newWay->pointsCount -1]);
                } else if ((lastWay != NULL)&&(*first == newWay->points[0])) {//first way must be reverted
                	fidx::FileRawData<GeoPoint>::revert(lastWay->points, lastWay->pointsCount);
                	first = &(lastWay->points[0]);
                	last = &(newWay->points[newWay->pointsCount -1]);
                } else 	if ((lastWay != NULL)&&(*first == newWay->points[newWay->pointsCount -1])) { // revert both
                	fidx::FileRawData<GeoPoint>::revert(newWay->points, newWay->pointsCount);
                	fidx::FileRawData<GeoPoint>::revert(lastWay->points, lastWay->pointsCount);
                	first = &(lastWay->points[0]);
                	last = &(newWay->points[newWay->pointsCount -1]);
                } else {//ne se rattache pas
                	openWays ++;
                	lastWay = newWay;
                	first = &(newWay->points[0]);
                	last = &(newWay->points[newWay->pointsCount -1]);
                }*/
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
        for(unsigned int i = 0; i < r.shape.lines.size(); i++)
        {
            if (! r.shape.lines[i]->isClosed())
            {
                delete r.shape.lines[i];
                r.shape.lines.erase(r.shape.lines.begin() + i);
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
    return points[0] == points[pointsCount - 1];
}

void Line::crop(Rectangle& r)
{
    if(!isClosed()) return;

    uint64_t newPointsCount, i;
    GeoPoint* newPoints;

// xmin = r.x0
// remove all poins where previous and next ar outside
//std::cout << "start crop\n";
    newPoints = (GeoPoint*) malloc (pointsCount * sizeof(GeoPoint));
    newPointsCount = 0;
    i = 0;
    while ((i < pointsCount)&&(pointsCount > 2))
    {
        bool discarded = false;
        if(points[i].x < r.x0)
        {
            GeoPoint* prev;
            GeoPoint* next;
            if(i == 0) prev = &(points[pointsCount -1]);
            else  prev = &(points[i - 1]);
            if ( i == (pointsCount -1) ) next = &(points[0]);
            else next = &(points[i + 1]);
            if(points[i].x < r.x0)
            {
                if((prev->x < r.x0)&&(next->x < r.x0))
                {
                    discarded = true;
                }
            }
        }
        if(!discarded)
        {
            memcpy(newPoints + newPointsCount, points + i, sizeof(GeoPoint));
            newPointsCount++;
        }
        i++;
    }
    delete points;
    points = newPoints;
    pointsCount = newPointsCount;
    newPoints = (GeoPoint*) malloc (pointsCount * sizeof(GeoPoint));
    newPointsCount = 0;
    i = 0;
    while ((i < pointsCount)&&(pointsCount > 2))
    {
        bool discarded = false;
        if(points[i].x > r.x1)
        {
            GeoPoint* prev;
            GeoPoint* next;
            if(i == 0) prev = &(points[pointsCount -1]);
            else  prev = &(points[i - 1]);
            if ( i == (pointsCount -1) ) next = &(points[0]);
            else next = &(points[i + 1]);
            if(points[i].x > r.x1)
            {
                if((prev->x > r.x1)&&(next->x > r.x1))
                {
                    discarded = true;
                }
            }
        }
        if(!discarded)
        {
            memcpy(newPoints + newPointsCount, points + i, sizeof(GeoPoint));
            newPointsCount++;
        }
        i++;
    }
    delete points;
    points = newPoints;
    pointsCount = newPointsCount;
    newPoints = (GeoPoint*) malloc (pointsCount * sizeof(GeoPoint));
    newPointsCount = 0;
    i = 0;
    while ((i < pointsCount)&&(pointsCount > 2))
    {
        bool discarded = false;
        if(points[i].y < r.y0)
        {
            GeoPoint* prev;
            GeoPoint* next;
            if(i == 0) prev = &(points[pointsCount -1]);
            else  prev = &(points[i - 1]);
            if ( i == (pointsCount -1) ) next = &(points[0]);
            else next = &(points[i + 1]);
            if(points[i].y < r.y0)
            {
                if((prev->y < r.y0)&&(next->y < r.y0))
                {
                    discarded = true;
                }
            }
        }
        if(!discarded)
        {
            memcpy(newPoints + newPointsCount, points + i, sizeof(GeoPoint));
            newPointsCount++;
        }
        i++;
    }
    delete points;
    points = newPoints;
    pointsCount = newPointsCount;
    newPoints = (GeoPoint*) malloc (pointsCount * sizeof(GeoPoint));
    newPointsCount = 0;
    i = 0;
    while ((i < pointsCount)&&(pointsCount > 2))
    {
        bool discarded = false;
        if(points[i].y > r.y1)
        {
            GeoPoint* prev;
            GeoPoint* next;
            if(i == 0) prev = &(points[pointsCount -1]);
            else  prev = &(points[i - 1]);
            if ( i == (pointsCount -1) ) next = &(points[0]);
            else next = &(points[i + 1]);
            if(points[i].y > r.y1)
            {
                if((prev->y > r.y1)&&(next->y > r.y1))
                {
                    discarded = true;
                }
            }
        }
        if(!discarded)
        {
            memcpy(newPoints + newPointsCount, points + i, sizeof(GeoPoint));
            newPointsCount++;
        }
        i++;
    }
    delete points;
    points = newPoints;
    pointsCount = newPointsCount;

}

void Way::crop(Rectangle& r)
{

    GeoPoint* newPoints = (GeoPoint*) malloc (pointsCount * sizeof(GeoPoint));
    uint64_t newPointsCount = 0;
// xmin = r.x0
// remove all poins where previous and next ar outside
//std::cout << "start crop\n";
    uint64_t i = 0;
    while ((i < pointsCount)&&(pointsCount > 2))
    {
        bool discarded = false;
        if((points[i].x < r.x0)||(points[i].x > r.x1)||(points[i].y < r.y0)||(points[i].y > r.y1))
        {
            GeoPoint* prev;
            GeoPoint* next;
            if(i == 0) prev = &(points[pointsCount -1]);
            else  prev = &(points[i - 1]);
            if ( i == (pointsCount -1) ) next = &(points[0]);
            else next = &(points[i + 1]);
            if(points[i].x < r.x0)
            {
                if((prev->x < r.x0)&&(next->x < r.x0))
                {
                    discarded = true;
                }
            }
            else if(points[i].x > r.x1)
            {
                if((prev->x > r.x1)&&(next->x > r.x1))
                {
                    discarded = true;
                }
            }
            else if(points[i].y < r.y0)
            {
                if((prev->y < r.y0)&&(next->y < r.y0))
                {
                    discarded = true;
                }
            }
            else if(points[i].y > r.y1)
            {
                if((prev->y > r.y1)&&(next->y > r.y1))
                {
                    discarded = true;
                }
            }
        }
        if(!discarded)
        {
            memcpy(newPoints + newPointsCount, points + i, sizeof(GeoPoint));
            newPointsCount++;
        }
        i++;
    }
    delete points;
    points = newPoints;
    pointsCount = newPointsCount;
}


bool Line::mergePoints (GeoPoint* points, uint64_t pointsCount)
{
    if(this->points == NULL) return true;
    if(points == NULL) return true;
    if(!this->isClosed())
    {
        if(this->points[0] == points[0])
        {
            this->points = (GeoPoint*) realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint));
            fidx::FileRawData<GeoPoint>::revert(this->points, this->pointsCount);
            memcpy(this->points + this->pointsCount, points , pointsCount*sizeof(GeoPoint) );
            this->pointsCount += pointsCount;
            return true;
        }
        else if (this->points[this->pointsCount - 1] == points[0])
        {
            this->points = (GeoPoint*) realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint));
            memcpy(this->points + this->pointsCount, points , pointsCount *sizeof(GeoPoint));
            this->pointsCount += pointsCount;
            return true;
        }
        else if (this->points[this->pointsCount - 1] == points[pointsCount - 1])
        {
            this->points = (GeoPoint*) realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint));
            memcpy(this->points + this->pointsCount, points, pointsCount *sizeof(GeoPoint));
            fidx::FileRawData<GeoPoint>::revert(this->points+this->pointsCount,pointsCount );
            this->pointsCount += pointsCount;
            //std::cout << " merged c\n";
            return true;
        }
        else if (this->points[0] == points[pointsCount - 1])
        {
            this->points = (GeoPoint*) realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint));
            fidx::FileRawData<GeoPoint>::revert(this->points, this->pointsCount);
            memcpy(this->points + this->pointsCount, points, pointsCount *sizeof(GeoPoint));
            fidx::FileRawData<GeoPoint>::revert(this->points+this->pointsCount,pointsCount );
            this->pointsCount += pointsCount ;
            //std::cout << " merged d\n";
            return true;
        }
    }
    return false;
}


void Shape::mergePoints(GeoPoint* points, uint64_t pointsCount)
{
    //std::cout << " merging " << pointsCount << "\n";
    bool merged = false;
    unsigned int i = 0;
    for(i = 0; i < lines.size(); i++)
    {
        Line* l = lines[i];
        if(l->mergePoints(points, pointsCount))
        {
            merged = true;
            break;
        }
    }
    if(!merged)
    {
        //std::cout << " new line \n";
        GeoPoint* newPoints = (GeoPoint*) malloc(pointsCount*sizeof(GeoPoint));
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
