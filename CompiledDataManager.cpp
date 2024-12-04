#include "CompiledDataManager.hpp"
#include <math.h>

std::shared_ptr<Way> CompiledDataManager::loadWay(uint64_t id, bool fast)
{
    GeoWayIndex record;
    bool found = wayIndex->get(id,&record);
    if(found && record.psize != 0)
    {
        auto w = std::make_shared<Way>();
        w->id = id;
        fillPoints(&(w->points), w->pointsCount,record.pstart,record.psize);
        if(w->pointsCount > 0 && ! fast)
        {
            GeoPoint *curpoint = nullptr;
            GeoPoint *lastpoint = nullptr;
            curpoint = w->points;
            w->rect.x0 =w->rect.x1 = curpoint->x;
            w->rect.y0 =w->rect.y1 = curpoint->y;
            for(lastpoint = curpoint + w->pointsCount ; curpoint  < lastpoint; curpoint++)
            {
                if(curpoint->x < w->rect.x0) w->rect.x0 = curpoint->x;
                else if(curpoint->x > w->rect.x1) w->rect.x1 = curpoint->x;
                if(curpoint->y < w->rect.y0) w->rect.y0 = curpoint->y;
                else if(curpoint->y > w->rect.y1) w->rect.y1 = curpoint->y;
            }
        }
        fillTags(w->tags,record.tstart,record.tsize);
        w->layer=6;
        if(w->tags["layer"] != "")
        {
            w->layer += atoiSW( w->tags["layer"]);
        }
        return w;
    }
    else return nullptr;
}

std::shared_ptr<Point> CompiledDataManager::loadPoint(uint64_t id)
{
    GeoPointIndex record;
    bool found = nodeIndex->get(id,&record);
    if(found)
    {
        auto p = std::make_shared<Point>();
        p->layer = 6;
        p->id = id;
        p->x = record.x;
        p->y = record.y;
        fillTags(p->tags,record.tstart,record.tsize);
        if(p->tags["layer"] != "")
        {
            p->layer += atoiSW( p->tags["layer"]);
        }
        return p;
    }
    else return nullptr;

}
/*
Relation* CompiledDataManager::loadRelation(uint64_t id)
{
    return loadRelation(id, 2);
}*/

std::shared_ptr<Relation> CompiledDataManager::loadRelation(uint64_t id, short recurs, bool fast)
{
    recurs --;
    if (recurs < 1) return nullptr;
    GeoIndex record;
    bool found = relationIndex->get(id,&record);
    if(found)
    {
        auto r = std::make_shared<Relation>();
        r->id = id;
        r->isPyramidal = false;
        r->rect.x0 = r->rect.x1 = UINT32_C(0xFFFFFFFF);
        r->rect.y0 = r->rect.y1 = UINT32_C(0xFFFFFFFF);
        fillTags(r->tags,record.tstart,record.tsize);
        r->layer = 6;
        if(r->tags["layer"] != "")
        {
            r->layer += atoiSW( r->tags["layer"]);
        }
        if(r->tags["type"] == "land_area")
        {
            r->isPyramidal = true;
        }
        fillLinkedItems(*r,record.mstart,record.msize, recurs, fast);
        //std::cout << r->tags["name"] << ":" << record.msize << "\n";
        return r;
    }
    else return nullptr;
}

std::shared_ptr<Relation> CompiledDataManager::loadRelationFast(uint64_t id)
{
    GeoIndex record;
    bool found = relationIndex->get(id,&record);
    if(found)
    {
        auto r = std::make_shared<Relation>();
        r->rect.x0 = r->rect.x1 = UINT32_C(0xFFFFFFFF);
        r->rect.y0 = r->rect.y1 = UINT32_C(0xFFFFFFFF);
        fillTags(r->tags,record.tstart,record.tsize);
        return r;
    }
    else return nullptr;
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

void CompiledDataManager::fillLinkedItems(Relation& r, uint64_t start, uint64_t size, short recurs, bool fast)
{
    GeoMember* buffer = relMembers->getData(start,size);
    std::shared_ptr<Way> newWay = nullptr;
    std::shared_ptr<Point> newPoint = nullptr;
    std::shared_ptr<Relation> newRel = nullptr;
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
            newWay = loadWay(buffer[i].id, fast);
            if(newWay)
            {
                r.shape.mergePoints(newWay->points, newWay->pointsCount, newWay->points[0] == newWay->points[newWay->pointsCount - 1] );
                r.ways.push_back(newWay);
                r.rect = r.rect + newWay->rect;
            }
            break;
        case BaliseType::relation:
            newRel = loadRelation(buffer[i].id, recurs, fast);
            if(newRel)
            {
                r.relations.push_back(newRel);
                r.rect = r.rect + newRel->rect;
                if(r.isPyramidal)
                {
                    for(Line* l : newRel->shape.closedLines)
                    {
                        if(!fast)r.shape.mergePoints(l->points, l->pointsCount, true);
                    }
                    for(Line* l : newRel->shape.openedLines)
                    {
                        if(!fast)r.shape.mergePoints(l->points, l->pointsCount, false);
                    }
                    for(auto rel : newRel->relations)
                    {
                        for(Line* l : rel->shape.closedLines)
                        {
                            if(!fast) r.shape.mergePoints(l->points, l->pointsCount, true);
                        }
                        for(Line* l : rel->shape.openedLines)
                        {
                            if(!fast) r.shape.mergePoints(l->points, l->pointsCount, false);
                        }
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
        for (auto it = r.shape.openedLines.begin(); it != r.shape.openedLines.end(); )
        {
                delete (*it);
                it = r.shape.openedLines.erase(it);
        }
    }

    r.isClosed = r.shape.openedLines.empty();

    free(buffer);
}

bool Line::isClosed()
{
    if(closed) return true;
    if ( points[0] == points[pointsCount - 1] ) closed = true;
    return closed;
}

void do_crop(GeoPoint*& points, uint64_t &pointsCount, Rectangle& r)
{
    uint64_t i;
    GeoPoint* newPoints;
    uint64_t newPointsCount = 0;
    bool isinside = false;

    if(pointsCount < 3) return;


    bool closed = (points[0] == points[pointsCount - 1]);

    // add intersections with min x

    newPoints = static_cast<GeoPoint*> (malloc ((2 * pointsCount +1)* sizeof(GeoPoint)));
    newPointsCount = 0;
    isinside = (points[0].x >= r.x0);
    newPoints[newPointsCount++] = points[0];
    for(i = 1; i < pointsCount; i++)
    {
        bool isinsidenew = (points[i].x >= r.x0);
        if(isinside ^ isinsidenew)
        {
            double ratio = ((double)(points[i].x) - (double)(r.x0)) / ((double)(points[i].x) - (double)(points[i-1].x));
            uint32_t new_x = r.x0;;
            uint32_t new_y = ratio * points[i-1].y  + (1 - ratio) * points[i].y;
            newPoints[newPointsCount++] = {new_x, new_y};
        }
        newPoints[newPointsCount++] = points[i];
        isinside = isinsidenew;
    }

    free(points);
    points = newPoints;
    assert(newPointsCount <= 2 * pointsCount +1);
    pointsCount = newPointsCount;

    // remove useless points for min x

    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc ((pointsCount + 1) * sizeof(GeoPoint)));
    for(i = 0; i < pointsCount; i++)
    {
        if(points[i].x >= r.x0)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    free(points);

    if(closed && (newPointsCount > 0) && !(newPoints[0] == newPoints[newPointsCount-1]))
    {
        newPoints[newPointsCount] = newPoints[0];
        newPointsCount++;
    }
    points = newPoints;
    assert(newPointsCount <= pointsCount);
    pointsCount = newPointsCount;
    if(pointsCount == 0) return;

    // add intersections with max x

    newPoints = static_cast<GeoPoint*> (malloc ((2 * pointsCount +1)* sizeof(GeoPoint)));
    newPointsCount = 0;
    isinside = (points[0].x <= r.x1);
    newPoints[newPointsCount++] = points[0];
    for(i = 1; i < pointsCount; i++)
    {
        bool isinsidenew = (points[i].x <= r.x1);
        if((!isinside && isinsidenew) || (isinside && !isinsidenew))
        {
            double ratio = ((double)(points[i].x) - (double)(r.x1)) / ((double)(points[i].x) - (double)(points[i-1].x));
            uint32_t new_x = r.x1;;
            uint32_t new_y = ratio * points[i-1].y  + (1 - ratio) * points[i].y;
            newPoints[newPointsCount++] = {new_x, new_y};
        }
        newPoints[newPointsCount++] = points[i];
        isinside = isinsidenew;
    }
    free(points);

    assert(newPointsCount <= 2 * pointsCount +1);
    points = newPoints;
    pointsCount = newPointsCount;

    // remove useless points for max x

    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc ((pointsCount + 1) * sizeof(GeoPoint)));
    for(i = 0; i < pointsCount; i++)
    {
        if(points[i].x <= r.x1)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    free(points);

    if(closed && (newPointsCount > 0) && !(newPoints[0] == newPoints[newPointsCount-1]))
    {
        newPoints[newPointsCount] = newPoints[0];
        newPointsCount++;
    }

    assert(newPointsCount <= pointsCount);
    points = newPoints;
    pointsCount = newPointsCount;
    if(pointsCount == 0) return;

    // add intersections with min y

    newPoints = static_cast<GeoPoint*> (malloc ((2 * pointsCount +1)* sizeof(GeoPoint)));
    newPointsCount = 0;
    isinside = (points[0].y >= r.y0);
    if(isinside) newPoints[newPointsCount++] = points[0];
    for(i = 1; i < pointsCount; i++)
    {
        bool isinsidenew = (points[i].y >= r.y0);
        if((!isinside && isinsidenew) || (isinside && !isinsidenew))
        {
            double ratio = ((double)(points[i].y) - (double)(r.y0)) / ((double)(points[i].y) - (double)(points[i-1].y));
            uint32_t new_y = r.y0;;
            uint32_t new_x = ratio * points[i-1].x  + (1 - ratio) * points[i].x;
            newPoints[newPointsCount++] = {new_x, new_y};
        }
        newPoints[newPointsCount++] = points[i];
        isinside = isinsidenew;
    }

    free(points);
    assert(newPointsCount <= 2 * pointsCount +1);
    points = newPoints;
    pointsCount = newPointsCount;

    // remove useless points for min y

    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*> (malloc ((pointsCount + 1) * sizeof(GeoPoint)));
    for(i = 0; i < pointsCount; i++)
    {
        if(points[i].y >= r.y0)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    free(points);

    if(closed && (newPointsCount > 0) && !(newPoints[0] == newPoints[newPointsCount-1]))
    {
        newPoints[newPointsCount] = newPoints[0];
        newPointsCount++;
    }


    assert(newPointsCount <= pointsCount);
    points = newPoints;
    pointsCount = newPointsCount;
    if(pointsCount == 0) return;

    // add intersections with max y

    newPoints = static_cast<GeoPoint*> (malloc ((2 * pointsCount +1)* sizeof(GeoPoint)));
    newPointsCount = 0;
    isinside = (points[0].y <= r.y1);
    if(isinside) newPoints[newPointsCount++] = points[0];
    for(i = 1; i < pointsCount; i++)
    {
        bool isinsidenew = (points[i].y <= r.y1);
        if((!isinside && isinsidenew) || (isinside && !isinsidenew))
        {
            double ratio = ((double)(points[i].y) - (double)(r.y1)) / ((double)(points[i].y) - (double)(points[i-1].y));
            uint32_t new_y = r.y1;;
            uint32_t new_x = ratio * points[i-1].x  + (1 - ratio) * points[i].x;
            newPoints[newPointsCount++] = {new_x, new_y};
        }
        newPoints[newPointsCount++] = points[i];
        isinside = isinsidenew;
    }
    free(points);


    points = newPoints;
    pointsCount = newPointsCount;


    // remove useless points for max y

    newPointsCount = 0;
    newPoints = static_cast<GeoPoint*>( malloc ((pointsCount + 1) * sizeof(GeoPoint)));
    for(i = 0; i < pointsCount; i++)
    {
        if(points[i].y <= r.y1)
        {
            newPoints[newPointsCount] = points[i];
            newPointsCount++;
        }
    }
    if(closed && (newPointsCount > 0) && !(newPoints[0] == newPoints[newPointsCount-1]))
    {
        newPoints[newPointsCount] = newPoints[0];
        newPointsCount++;
    }

    free(points);
    points = newPoints;
    pointsCount = newPointsCount;

    if(pointsCount == 2 && points[0] == points[1])
    {
        pointsCount = 0;
        return;
    }

}



void Line::crop(Rectangle& r)
{
    do_crop(points, pointsCount, r);
}


void Way::fillrec()
{
    if (!pointsCount) return;
    rect.x0 = rect.x1 = points[0].x;
    rect.y0 = rect.y1 = points[0].y;
    for(unsigned int i = 0; i < pointsCount; i++)
    {
        if(points[i].x < rect.x0) rect.x0 = points[i].x;
        if(points[i].x > rect.x1) rect.x1 = points[i].x;
        if(points[i].y < rect.y0) rect.y0 = points[i].y;
        if(points[i].y > rect.y1) rect.y1 = points[i].y;
    }

}


void Way::crop(Rectangle& r)
{
    do_crop(points, pointsCount, r);
}

void Way::reduce(uint32_t dx, uint32_t dy)
{
    uint64_t newPointsCount = 1;
    for(uint64_t i = 1; i < pointsCount; i++)
    {
        if(i == pointsCount - 1) points[newPointsCount++] = points[i];
        else if(round(points[i].x/(dx*1.0))  != round(points[newPointsCount -1].x/(dx*1.0)))  points[newPointsCount++] = points[i];
        else if(round(points[i].y/(dy*1.0))  != round(points[newPointsCount -1].y/(dy*1.0)))  points[newPointsCount++] = points[i];
    }
    pointsCount = newPointsCount;
}


bool Line::mergePoints (GeoPoint* points, uint64_t pointsCount)
{
    if(this->points == nullptr) return true;
    if(points == nullptr) return true;
//    if(!this->isClosed())
//    {
        if(this->points[0] == points[0])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            fidx::FileRawData<GeoPoint>::revert(this->points, this->pointsCount);
            memcpy(this->points + this->pointsCount - 1, points , pointsCount*sizeof(GeoPoint) );
            this->pointsCount += ( pointsCount - 1 );
            return true;
        }
        else if (this->points[this->pointsCount - 1] == points[0])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            memcpy(this->points + (this->pointsCount - 1), points , pointsCount *sizeof(GeoPoint));
            this->pointsCount += (pointsCount - 1);
            return true;
        }
        else if (this->points[this->pointsCount - 1] == points[pointsCount - 1])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            memcpy(this->points + this->pointsCount -1 , points, pointsCount *sizeof(GeoPoint));
            fidx::FileRawData<GeoPoint>::revert(this->points+this->pointsCount - 1 ,pointsCount );
            this->pointsCount += pointsCount - 1;
            return true;
        }
        else if (this->points[0] == points[pointsCount - 1])
        {
            this->points = static_cast<GeoPoint*> (realloc(this->points,(pointsCount + this->pointsCount)*sizeof(GeoPoint)));
            memmove(this->points + pointsCount , this->points + 1, ( this->pointsCount - 1) *sizeof(GeoPoint));
            memcpy(this->points, points, pointsCount *sizeof(GeoPoint));
            this->pointsCount += pointsCount - 1 ;
            return true;
        }
//    }
    return false;
}


void Shape::mergePoints(GeoPoint* points, uint64_t pointsCount, bool closed)
{
    bool merged = false;
    unsigned int i = 0;
    if(!closed)
    {
        for(i = 0; i < openedLines.size(); i++)
        {
            Line* l = openedLines[i];
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
        if(closed) closedLines.push_back(l);
        else openedLines.push_back(l);
    }
    else if(openedLines[i]->points[0] == openedLines[i]->points[openedLines[i]->pointsCount - 1])
    {
        closedLines.push_back(openedLines[i]);
        openedLines.erase(openedLines.begin() + i);
    }
    else
    {
        merged = false;
        unsigned int j;
        for(j = i+1; j < openedLines.size(); j++)
        {
            Line* l = openedLines[j];
            if(l->mergePoints(openedLines[i]->points, openedLines[i]->pointsCount))
            {
                merged = true;
                break;
            }
        }
        if(merged)
        {
            if(openedLines[j]->points[0] == openedLines[j]->points[openedLines[j]->pointsCount - 1])
            {
                closedLines.push_back(openedLines[j]);
                openedLines.erase(openedLines.begin() + j);
            }
            delete openedLines[i];
            openedLines.erase(openedLines.begin() + i);
        }
    }
}
