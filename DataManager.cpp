//0123456789012345678901234/56789
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <math.h>
#include <time.h>
#include <stdint.h>
#include <regex>
#include <iostream>
#include <algorithm> 
#include "DataManager.hpp"
#include "common/constants.hpp"
#include "Coordinates.hpp"
#include <algorithm>

//#define DUMP(myString) fprintf(fOut,"%s",myString.c_str());

#define STATE_TAGENDB    10001
#define STATE_ATTRVALUEB 10002

using namespace std;
using namespace fidx;

	bool sequence::check(const char* s, short length)
	{
		if (length > 16) return false;
		else return  (memcmp(s,c,length) == 0);
	}
	
	void sequence::dump(){
		printf("%s\n",c);
	}
	
	void sequence::append(uint32_t utf8Char)
	{
		memmove(bigChar,&bigChar[1],3*sizeof(uint32_t));
		
		bigChar[3] = utf8Char;	
	
		short index = 0;
		
		uint32_t myChar;
		
			myChar = bigChar[0];
		
			while (myChar)
			{
				c[index] = (myChar & 0xFF);
				myChar = myChar >> 8;
				index++;
			}
		
			myChar = bigChar[1];
		
			while (myChar)
			{
				c[index] = (myChar & 0xFF);
				myChar = myChar >> 8;
				index++;
			}
		
			myChar = bigChar[2];
		
			while (myChar)
			{
				c[index] = (myChar & 0xFF);
				myChar = myChar >> 8;
				index++;
			}
		
			myChar = bigChar[3];
		
			while (myChar)
			{
				c[index] = (myChar & 0xFF);
				myChar = myChar >> 8;
				index++;
			}
			c[index] = 0;
		
		
	}


void DataManager::select(sequence& sq, bool full)
{


	switch(context.state)
	{
		case STATE_UNKNOWN:

		 if (sq.check("</",2))
		{
			context.state = STATE_TAGENDB;
		}
		else if (sq.check("<",1))
		{
			context.state = STATE_TAGNAME;
			context.curBalise = new Balise;
			if(context.rootBalise == NULL) context.rootBalise = context.curBalise;
			context.curBalise->startsAt = sq.lastInfPos;;
		}
		else
		{
		}
		break;

		case STATE_TAGNAME:
		//check for tag end or space
		if(sq.check(">",1))
		{
			context.state = STATE_UNKNOWN;
			context.curBalise->baliseName = context.baliseName.toString();
			context.baliseName.clear();
			//if(full) printf("queuing : %s \n",context.curBalise->baliseName.c_str());
			context.tagStack.push_back(context.curBalise);
		}
		else if(sq.check("/>",2) && (context.curBalise != NULL) )
		{
			context.curBalise->endsAt = sq.lastSupPos;
			if(full) {
				//printf("trying to add child %s \n", context.curBalise->baliseName.c_str());
				Balise* b = getParent();
				if(b!=NULL) {
					b->addChild(context.curBalise);
				}
			} else {	
				delete context.curBalise;
			}
			context.state = STATE_UNKNOWN;
		}
		else if(isspace(sq.c[0]))
		{
			context.state = STATE_ATTRNAME;
			context.attrName.clear();
		}
		else
		{
			if(!isspace(sq.c[0]))	context.baliseName.appendChar(sq.c[0]);
		}
		break;

		case STATE_TAGENDB:
			context.state = STATE_TAGEND;
		break;

		case STATE_TAGEND:
		//check for tag end or space
		    if(sq.check(">",1)) {
				//if(full) printf("unqueuing : %s \n",context.curBalise->baliseName.c_str());
				context.curBalise = context.tagStack.back();
				context.tagStack.pop_back();
				if(context.curBalise != NULL) {
					context.curBalise->endsAt = sq.lastSupPos;
					if(full) {
						//printf("try add %s\n",context.curBalise->baliseName.c_str());
						Balise* b = getParent();
						if(b!=NULL) {
							b->addChild(context.curBalise);
						}
					} else {
						delete context.curBalise;
						context.curBalise = NULL;
					}
				}
				context.state = STATE_UNKNOWN;
			}
		break;

		case STATE_ATTRNAME:
		//check for = or space
		if (sq.check(">",1) || sq.check("/>",1))
		{
			context.state = STATE_UNKNOWN;
			if(context.attrName.count)
			{
				context.curBalise->keyValues[context.attrName.toString()] = context.attrValue.toString();
			}
			context.curBalise->baliseName = context.baliseName.toString();
			context.baliseName.clear();
			if(sq.check(">",1))
			{
				context.tagStack.push_back(context.curBalise);
			}else if(context.curBalise != NULL){
				context.curBalise->endsAt = sq.lastSupPos;
				if(full) {
						//printf("try add %s\n",context.curBalise->baliseName.c_str());
						Balise* b = getParent();
						if(b!=NULL) {
							b->addChild(context.curBalise);
						}
				}
				else {	
					delete context.curBalise;
					context.curBalise = NULL;
				}
			}
		}
		else if (sq.check("=\"",2))
		{
			context.state = STATE_ATTRVALUEB;
		}
		//add char to current attrName
		else
		{
			if(!isspace(sq.c[0]))context.attrName.appendChar(sq.c[0]);
		}
		break;

		case STATE_ATTRVALUEB:
		context.state = STATE_ATTRVALUE;
		context.attrValue.clear();
		break;

		case STATE_ATTRVALUE:
		if(sq.c[0] == '"')
		{
			context.state = STATE_ATTRNAME;
			if((context.attrName.count)&&(context.curBalise != NULL))
			{
				context.curBalise->keyValues[context.attrName.toString()] = context.attrValue.toString();
			}
			context.attrName.clear();
		}
		else
		{
			context.attrValue.appendChar(sq.c[0]);
		}
		break;

	}
}

Balise* DataManager::getParent(){
	if(context.tagStack.size() > 0)
		return context.tagStack.back();
	else return NULL;	
}

uint32_t DataManager::fillPoints(Balise& b, GeoWayIndex idx) {
	//b.points = (GeoPoint*) malloc(sizeof(GeoPoint) * idx.psize);
	b.points = wayPoints->getData(idx.pstart, idx.psize);
	b.pointsCount = idx.psize;
	//fseek(waypoints, idx.pstart* sizeof(GeoPoint), SEEK_SET);
	//fread(b.points, sizeof(GeoPoint), idx.psize,waypoints);
	b.rect.x0 = b.rect.x1 = UINT32_C(0xFFFFFFFF);
    b.rect.y0 = b.rect.y1 = UINT32_C(0xFFFFFFFF);
    if(idx.psize > 0) {
		b.firstNodeRef = mergeBits(b.points[0].x, b.points[0].y);
		b.lastNodeRef = mergeBits(b.points[idx.psize - 1].x, b.points[idx.psize - 1].y);
	} else {
		b.firstNodeRef = UINT64_C(0xFFFFFFFFFFFFFFFF);
		b.lastNodeRef = UINT64_C(0xFFFFFFFFFFFFFFFF);
	}
    for(int i = 0 ; i < idx.psize ; i++) {
		Rectangle r;
		r.x1 = r.x0 = b.points[i].x;
		r.y1 = r.y0 = b.points[i].y;
		b.rect = b.rect + r;
	}
	b.isClosed = (b.firstNodeRef == b.lastNodeRef);
}


Balise* DataManager::loadBalise(long start, long length){
	char* buffer;
	char c;
	int len;
	uint32_t utf8CharData;
	short bitNum;
	bool cont = false;
	short initSeq;
	sequence seq;
	FILE *stream  = fIn;
	fseek(stream, start, SEEK_SET);
	initSeq = 0;
	long lengthUsed = 0;
	
	context.state = STATE_UNKNOWN;
	context.previous_state = STATE_UNKNOWN;

	context.attrName.clear();
	context.attrValue.clear();
	context.curBalise = NULL;
	context.rootBalise = NULL;
	context.baliseName.clear();
	buffer = (char*) malloc(length);
	len = fread(buffer,length,1,stream);
	while(lengthUsed < length)
	{
		c=buffer[lengthUsed];
		lengthUsed++;
		if(bitNum == 1) {
			utf8CharData=c;
		}
		else 
		{
			utf8CharData = utf8CharData << 8;
			utf8CharData += c;
		}
		if (c > 0x7F)
		{
			cont = true;
			bitNum++;
		}else{
			cont = false;
			bitNum = 1;
		}

		if(!cont && utf8CharData != '\n')
		{
			seq.append(utf8CharData);
			if(initSeq < 3) initSeq++;
			else select(seq, true);
		}
	}
	for(int i=0; i < 15; i++)
	{
		seq.append(0);
		select(seq, true);
	}
	context.rootBalise->startsAt = start;
	context.rootBalise->endsAt = start + length;
	free(buffer);
	context.rootBalise->isLoaded = true;
	return context.rootBalise;
}


Balise* DataManager::loadBaliseFull(long start, long length){
	return loadBaliseFull(start, length, 16, NULL);
}

Balise* DataManager::completeBalise(Balise* b) {
	return loadBaliseFull(0,0,16,b);
}

Balise* DataManager::completeBalise(Balise* b, short recurs) {
	return loadBaliseFull(0,0,recurs,b);
}

Balise* DataManager::loadBaliseFull(long start, long length, short recurs, Balise* b){
	Balise* res;
	if(b) {
        res = b;
        if(res->isFullyLoaded) return res;
    } else {
        res = loadBalise(start,length);
	}

    if(res->pointsCount == 0) {
        res->rect.x0 = res->rect.x1 = UINT32_C(0xFFFFFFFF);
        res->rect.y0 = res->rect.y1 = UINT32_C(0xFFFFFFFF);
        res->firstNodeRef = UINT64_C(0xFFFFFFFFFFFFFFFF);
        res->lastNodeRef = UINT64_C(0xFFFFFFFFFFFFFFFF);
    }
    Balise* lastWay = NULL;
    BaliseType bType = unknown;
    int openWays = 0;
    std::vector<Balise*>::iterator it;
    for(it = res->childs.begin(); it != res->childs.end(); it++) {
        Balise* oldChild = *it;
        bType = unknown;
        // quel est le type d'entité à inserer
        if (oldChild->baliseName == BALISENAME_NDSHORT) bType = point;
        else if (oldChild->baliseName == BALISENAME_MEMBERSHORT) {
            bType = unknown;
            if(oldChild->keyValues["type"] == "node") bType = point;
            else if(oldChild->keyValues["type"] == "way") bType = way;
            else if(oldChild->keyValues["type"] == "relation") bType = relation;
        }
        
        uint64_t myIndex;
        uint64_t key;key = atoll(oldChild->keyValues["ref"].c_str());
        if(oldChild->keyValues["ref"] == "") {
            key = 999999999999LL;
        }
        bool found = false;
        GeoIndex g;
        GeoPointIndex gp;
        GeoWayIndex gw;
        switch (bType) {
           // suivre les fils et construire une nouvelle balise
           // remplacer l'enfant
            case point :
				//key = atoll(oldChild->keyValues["ref"].c_str());
				if (nodeIndex->get(key, &gp) && (recurs > 0)) {
					//if(gp.size == 0) {
						*it = new Balise;
						(*it)->baliseName = BALISENAME_NODESHORT;
                        delete oldChild;
					//} else {
					//	*it = loadBaliseFull(gp.start, gp.size, recurs -1, NULL);
					//	delete oldChild;
					//}
					//(*it)->keyValues["lon"] = Coordinates::toHex(gp.x);
					//(*it)->keyValues["lat"] = Coordinates::toHex(gp.y);
                        (*it)->rect.x0 = (*it)->rect.x1 = gp.x;
                        (*it)->rect.y0 = (*it)->rect.y1 = gp.y;
                    if(res->pointsCount == 0) {
                        res->rect = res->rect + (*it)->rect;
                        if(res->baliseName == BALISENAME_WAYSHORT) {
                            if(res->firstNodeRef == UINT64_C(0xFFFFFFFFFFFFFFFF)) res->firstNodeRef = mergeBits(gp.x, gp.y);
                            res->lastNodeRef = mergeBits(gp.x, gp.y);
                            std::cout << "should not be here 2!";
                        }
                    }
				} 
                break;
            case way :
				//key = atoll(oldChild->keyValues["ref"].c_str());
				if (wayIndex->get(key, &gw) && (recurs > 0)) {
					uint64_t pos=0;
					uint64_t debut;
					bool lastIsFirst = false;
                    *it = loadBalise(gw.start, gw.size);
					fillPoints(**it, gw);
					*it = completeBalise(*it, recurs -1);
					if((lastWay == NULL) && ! (*it)->isClosed) { // ouverture d'une boucle
						openWays++;
						lastWay = *it;
						lastIsFirst = true;
						debut = (*it)->firstNodeRef;
					}
					else if ((*it)->isClosed) { // ajout boucle simple
						lastWay = NULL;
						lastIsFirst = false;
					}else if ((debut == (*it)->lastNodeRef)
						&& (lastWay != NULL)
						&& (lastWay->lastNodeRef == (*it)->firstNodeRef))
					{//fermeture boucle
						lastWay = NULL;
						lastIsFirst = false;
						openWays --;
					}else if ((debut == (*it)->firstNodeRef) 
						&& (lastWay != NULL)
						&& (lastWay->lastNodeRef == (*it)->lastNodeRef))
					{//fermeture boucle avec inversion
						std::reverse((*it)->childs.begin(),(*it)->childs.end());
                        fidx::FileRawData<GeoPoint>::revert((*it)->points, (*it)->pointsCount);
						pos = (*it)->firstNodeRef;
						(*it)->firstNodeRef = (*it)->lastNodeRef;
						(*it)->lastNodeRef = pos;

						lastWay = NULL;
						lastIsFirst = false;
						debut = 0;
						openWays --;
					} else {
						if(lastWay->lastNodeRef == (*it)->firstNodeRef) {
							lastWay = *it;
							lastIsFirst = false;
						} else if ((lastWay->firstNodeRef == (*it)->lastNodeRef)&&lastIsFirst) {
							std::reverse(lastWay->childs.begin(),lastWay->childs.end());
                            fidx::FileRawData<GeoPoint>::revert(lastWay->points, lastWay->pointsCount);
							std::reverse((*it)->childs.begin(),(*it)->childs.end());
                            fidx::FileRawData<GeoPoint>::revert((*it)->points, (*it)->pointsCount);

							pos = lastWay->firstNodeRef;
							lastWay->firstNodeRef = lastWay->lastNodeRef;
							lastWay->lastNodeRef = pos;
	
							pos = (*it)->firstNodeRef;
							(*it)->firstNodeRef = (*it)->lastNodeRef;
							(*it)->lastNodeRef = pos;
							
							lastIsFirst = false;
							lastWay = *it;
						} else if ((lastWay->firstNodeRef == (*it)->firstNodeRef)&&lastIsFirst) {
							std::reverse(lastWay->childs.begin(),lastWay->childs.end());
                            fidx::FileRawData<GeoPoint>::revert(lastWay->points, lastWay->pointsCount);
							pos = lastWay->firstNodeRef;
							lastWay->firstNodeRef = lastWay->lastNodeRef;
							lastWay->lastNodeRef = pos;

							lastIsFirst = false;
							lastWay = *it;
						} else if (lastWay->lastNodeRef == (*it)->lastNodeRef) {
							std::reverse((*it)->childs.begin(),(*it)->childs.end());
                            fidx::FileRawData<GeoPoint>::revert((*it)->points, (*it)->pointsCount);
							lastWay = *it;
	
							pos = (*it)->firstNodeRef;
							(*it)->firstNodeRef = (*it)->lastNodeRef;
							(*it)->lastNodeRef = pos;
							lastIsFirst = false;

						}
					}

                    res->rect = res->rect + (*it)->rect;
					delete oldChild;
				} 
                break;
            case relation:
				//key = atoll(oldChild->keyValues["ref"].c_str());
				if (relationIndex->get(key, &g) && (recurs > 0)) {
					*it = loadBaliseFull(g.start, g.size, recurs - 1, NULL);
                    
                    res->rect = res->rect + (*it)->rect;
					delete oldChild;
				} 
                break;
            case unknown:
                default:
                break;
        }
    }
    if ((res->firstNodeRef == res->lastNodeRef) && (openWays == 0) )res->isClosed = true;
    res->isLoaded = true;
    res->isFullyLoaded = true;
    //cout << res->baliseName << ":" << res->isClosed << ":" << std::to_string(res->firstNodeRef) << ":" << std::to_string(res->lastNodeRef) <<"\n";
    return res;
}

