#include "CompiledDataManager.hpp"
#include <math.h>

std::shared_ptr<Way> CompiledDataManager::loadWay(uint64_t id, Rectangle* rect)
{
    GeoWayIndex record;
    bool found = wayIndex->get(id,&record);
    if(found && rect && !overlaps(*rect, record.rect)) return nullptr;
    if(found && record.psize != 0)
    {
        auto w = std::make_shared<Way>();
        w->id = id;
        fillPoints(&(w->points), w->pointsCount,record.pstart,record.psize);
        /*if(w->pointsCount > 0 && ! fast)
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
        }*/
        w->rect = record.rect;
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
    auto cached = pointCache.get(id);
    if (cached) return cached;

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
        pointCache.put(id, p);
        return p;
    }
    else return nullptr;

}
/*
Relation* CompiledDataManager::loadRelation(uint64_t id)
{
    return loadRelation(id, 2);
}*/

std::shared_ptr<Relation> CompiledDataManager::loadRelation(uint64_t id, short recurs, bool computeShape, Rectangle* rect)
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
        fillLinkedItems(*r,record.mstart,record.msize, recurs, computeShape, rect);
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

void CompiledDataManager::fillLinkedItems(Relation& r, uint64_t start, uint64_t size, short recurs, bool computeShape, Rectangle* rect)
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
            newWay = loadWay(buffer[i].id, rect);
            if(newWay)
            {
                if(computeShape) r.shape.mergePoints(newWay->points, newWay->pointsCount, newWay->points[0] == newWay->points[newWay->pointsCount - 1] );
                r.ways.push_back(newWay);
                r.rect = r.rect + newWay->rect;
            }
            break;
        case BaliseType::relation:
            newRel = loadRelation(buffer[i].id, recurs, rect);
            if(newRel)
            {
                r.relations.push_back(newRel);
                r.rect = r.rect + newRel->rect;
                if(computeShape)
                {
                    for(Line* l : newRel->shape.closedLines)
                    {
                        r.shape.mergePoints(l->points, l->pointsCount, true);
                    }
                    for(Line* l : newRel->shape.openedLines)
                    {
                        r.shape.mergePoints(l->points, l->pointsCount, false);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    r.isClosed = true;

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
    if(pointsCount < 3) return;

    bool closed = (points[0] == points[pointsCount - 1]);
    uint64_t i;
    GeoPoint* newPoints;
    uint64_t newPointsCount;

    // Clip against x >= r.x0
    newPoints = static_cast<GeoPoint*>(malloc((2 * pointsCount + 1) * sizeof(GeoPoint)));
    newPointsCount = 0;
    {
        bool prev_inside = (points[0].x >= r.x0);
        if(prev_inside) newPoints[newPointsCount++] = points[0];
        for(i = 1; i < pointsCount; i++)
        {
            bool curr_inside = (points[i].x >= r.x0);
            if(prev_inside != curr_inside)
            {
                double ratio = ((double)points[i].x - (double)r.x0) / ((double)points[i].x - (double)points[i-1].x);
                uint32_t ny = (uint32_t)(ratio * (double)points[i-1].y + (1.0 - ratio) * (double)points[i].y);
                newPoints[newPointsCount++] = {r.x0, ny};
            }
            if(curr_inside) newPoints[newPointsCount++] = points[i];
            prev_inside = curr_inside;
        }
        if(closed && newPointsCount > 0 && !(newPoints[0] == newPoints[newPointsCount-1]))
            newPoints[newPointsCount++] = newPoints[0];
    }
    free(points); points = newPoints; pointsCount = newPointsCount;
    if(!pointsCount) return;

    // Clip against x <= r.x1
    newPoints = static_cast<GeoPoint*>(malloc((2 * pointsCount + 1) * sizeof(GeoPoint)));
    newPointsCount = 0;
    {
        bool prev_inside = (points[0].x <= r.x1);
        if(prev_inside) newPoints[newPointsCount++] = points[0];
        for(i = 1; i < pointsCount; i++)
        {
            bool curr_inside = (points[i].x <= r.x1);
            if(prev_inside != curr_inside)
            {
                double ratio = ((double)points[i].x - (double)r.x1) / ((double)points[i].x - (double)points[i-1].x);
                uint32_t ny = (uint32_t)(ratio * (double)points[i-1].y + (1.0 - ratio) * (double)points[i].y);
                newPoints[newPointsCount++] = {r.x1, ny};
            }
            if(curr_inside) newPoints[newPointsCount++] = points[i];
            prev_inside = curr_inside;
        }
        if(closed && newPointsCount > 0 && !(newPoints[0] == newPoints[newPointsCount-1]))
            newPoints[newPointsCount++] = newPoints[0];
    }
    free(points); points = newPoints; pointsCount = newPointsCount;
    if(!pointsCount) return;

    // Clip against y >= r.y0
    newPoints = static_cast<GeoPoint*>(malloc((2 * pointsCount + 1) * sizeof(GeoPoint)));
    newPointsCount = 0;
    {
        bool prev_inside = (points[0].y >= r.y0);
        if(prev_inside) newPoints[newPointsCount++] = points[0];
        for(i = 1; i < pointsCount; i++)
        {
            bool curr_inside = (points[i].y >= r.y0);
            if(prev_inside != curr_inside)
            {
                double ratio = ((double)points[i].y - (double)r.y0) / ((double)points[i].y - (double)points[i-1].y);
                uint32_t nx = (uint32_t)(ratio * (double)points[i-1].x + (1.0 - ratio) * (double)points[i].x);
                newPoints[newPointsCount++] = {nx, r.y0};
            }
            if(curr_inside) newPoints[newPointsCount++] = points[i];
            prev_inside = curr_inside;
        }
        if(closed && newPointsCount > 0 && !(newPoints[0] == newPoints[newPointsCount-1]))
            newPoints[newPointsCount++] = newPoints[0];
    }
    free(points); points = newPoints; pointsCount = newPointsCount;
    if(!pointsCount) return;

    // Clip against y <= r.y1
    newPoints = static_cast<GeoPoint*>(malloc((2 * pointsCount + 1) * sizeof(GeoPoint)));
    newPointsCount = 0;
    {
        bool prev_inside = (points[0].y <= r.y1);
        if(prev_inside) newPoints[newPointsCount++] = points[0];
        for(i = 1; i < pointsCount; i++)
        {
            bool curr_inside = (points[i].y <= r.y1);
            if(prev_inside != curr_inside)
            {
                double ratio = ((double)points[i].y - (double)r.y1) / ((double)points[i].y - (double)points[i-1].y);
                uint32_t nx = (uint32_t)(ratio * (double)points[i-1].x + (1.0 - ratio) * (double)points[i].x);
                newPoints[newPointsCount++] = {nx, r.y1};
            }
            if(curr_inside) newPoints[newPointsCount++] = points[i];
            prev_inside = curr_inside;
        }
        if(closed && newPointsCount > 0 && !(newPoints[0] == newPoints[newPointsCount-1]))
            newPoints[newPointsCount++] = newPoints[0];
    }
    free(points); points = newPoints; pointsCount = newPointsCount;

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


static inline uint64_t ep_key(const GeoPoint& p)
{
    return ((uint64_t)p.x << 32) | p.y;
}

static void ep_add(std::unordered_multimap<uint64_t, Line*>& ep, const GeoPoint& pt, Line* l)
{
    ep.emplace(ep_key(pt), l);
}

static void ep_remove(std::unordered_multimap<uint64_t, Line*>& ep, const GeoPoint& pt, Line* l)
{
    auto range = ep.equal_range(ep_key(pt));
    for(auto it = range.first; it != range.second; ++it)
    {
        if(it->second == l) { ep.erase(it); return; }
    }
}

static Line* ep_find(std::unordered_multimap<uint64_t, Line*>& ep, const GeoPoint& pt)
{
    auto it = ep.find(ep_key(pt));
    return it != ep.end() ? it->second : nullptr;
}

void Shape::mergePoints(GeoPoint* points, uint64_t pointsCount, bool closed)
{
    if(closed)
    {
        GeoPoint* newPoints = static_cast<GeoPoint*>(malloc(pointsCount * sizeof(GeoPoint)));
        memcpy(newPoints, points, pointsCount * sizeof(GeoPoint));
        Line* l = new Line();
        l->points = newPoints;
        l->pointsCount = pointsCount;
        closedLines.push_back(l);
        return;
    }

    // O(1) endpoint lookup — try both ends of the incoming segment
    Line* L = ep_find(_ep, points[0]);
    if(!L) L = ep_find(_ep, points[pointsCount - 1]);

    if(L)
    {
        // Detach L's endpoints from the map before the merge changes them
        ep_remove(_ep, L->points[0], L);
        ep_remove(_ep, L->points[L->pointsCount - 1], L);

        L->mergePoints(points, pointsCount);

        if(L->points[0] == L->points[L->pointsCount - 1])
        {
            closedLines.push_back(L);
            openedLines.erase(std::find(openedLines.begin(), openedLines.end(), L));
            return;
        }

        // Secondary merge: check if L's new endpoints connect to another open line
        Line* M = ep_find(_ep, L->points[0]);
        if(!M) M = ep_find(_ep, L->points[L->pointsCount - 1]);

        if(M)
        {
            ep_remove(_ep, M->points[0], M);
            ep_remove(_ep, M->points[M->pointsCount - 1], M);

            M->mergePoints(L->points, L->pointsCount);

            openedLines.erase(std::find(openedLines.begin(), openedLines.end(), L));
            delete L;

            if(M->points[0] == M->points[M->pointsCount - 1])
            {
                closedLines.push_back(M);
                openedLines.erase(std::find(openedLines.begin(), openedLines.end(), M));
                return;
            }
            ep_add(_ep, M->points[0], M);
            ep_add(_ep, M->points[M->pointsCount - 1], M);
        }
        else
        {
            ep_add(_ep, L->points[0], L);
            ep_add(_ep, L->points[L->pointsCount - 1], L);
        }
    }
    else
    {
        GeoPoint* newPoints = static_cast<GeoPoint*>(malloc(pointsCount * sizeof(GeoPoint)));
        memcpy(newPoints, points, pointsCount * sizeof(GeoPoint));
        Line* l = new Line();
        l->points = newPoints;
        l->pointsCount = pointsCount;
        openedLines.push_back(l);
        ep_add(_ep, l->points[0], l);
        ep_add(_ep, l->points[l->pointsCount - 1], l);
    }
}
