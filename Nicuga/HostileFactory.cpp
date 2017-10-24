/*
* Copyright (c) 2012, Nicolas My
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Nicolas My ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Nicolas My BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "HostileFactory.h"
#include "SaxTagsUtils.h"
#include <stdio.h>
#include "Level.h"
#include "Barrage.h"
#include <map>
#include "RenderableAbstractFactory.h"

static Hostile * currentHostile;
static XML_Parser currentParer;
static XML_StartElementHandler oldStart;
static XML_EndElementHandler oldEnd;
static void * oldUserData;
static RenderableAbstractFactory * g_renderableFactory;
void hostile_xmlend(void *data, const char *el);
void hostile_xmlstart(void *data, const char *el, const char **attr) 
{
	int iAttrCount = XML_GetSpecifiedAttributeCount(currentParer);
	const char * id;
	const char * lifestr;
	int topx = 0;
	int topy = 0;
	int bottomx = 0;
	int bottomy = 0;
	Barrage * b;
	Level * level;
	//printf("HostileFactory TAG : %s\n",el);

	switch(getTagType(el))
	{
	case BOUNDINGBOX :	
        for (int i = 0;i < iAttrCount;i++){
            //printf("attr[%d]= %s\n",i,attr[i]);
			if (i%2 == 0)
			{
				
				if (!strcmp(attr[i],"top"))
					sscanf(attr[i+1],"%d,%d",&topx,&topy);
				if (!strcmp(attr[i],"bottom"))
					sscanf(attr[i+1],"%d,%d",&bottomx,&bottomy);
			}
        }
		
		currentHostile->setBoundingBox(BoundingBox((short)topx,(short)topy,(short)bottomx,(short)bottomy));
		break;
	case BARRAGE :
		level = (Level*)oldUserData;
		id = getAttribute("id",attr,iAttrCount);
		b = level->getBarrageManager()->getBarrage(id);
		if (b != NULL)
		{
			currentHostile->addBarrage(b);
		}
		else
		{
			printf("ERROR : COULDN'T FIND BARRAGE WITH ID %s PLEASE CHECK CONFIGURATION\n",id);
			exit(0);
		}
		break;
	case LIFE :
		lifestr = getAttribute("value",attr,iAttrCount);
		currentHostile->setLife(atoi(lifestr));
		break;
	case SCORE :
		lifestr = getAttribute("value",attr,iAttrCount);
		currentHostile->setScore(atoi(lifestr));
		break;
	case RENDERABLE:
		g_renderableFactory->setFactoryCallbacks(currentParer,hostile_xmlstart,hostile_xmlend);
		currentHostile->setRenderable(g_renderableFactory->getRenderable());
		break;
		
	}
}

void hostile_xmlend(void *data, const char *el)
{
	switch(getTagType(el))
	{
	case HOSTILE :
		//Put back passed handler
		XML_SetUserData(currentParer, oldUserData);
		XML_SetElementHandler(currentParer, oldStart, oldEnd);
		break;
	}
}

void HostileFactory::setFactoryCallbacks(XML_Parser p, XML_StartElementHandler start, XML_EndElementHandler end,RenderableAbstractFactory * renderableFactory)
{
	currentHostile = new Hostile();
	oldStart = start;
	oldEnd = end;
	oldUserData = XML_GetUserData(p);
	currentParer = p;

	g_renderableFactory = renderableFactory;

	//Put hostile factory handler
	XML_SetUserData(currentParer, currentHostile);
	XML_SetElementHandler(currentParer, hostile_xmlstart, hostile_xmlend);
}

Hostile * HostileFactory::loadHostile()
{
	return currentHostile;
}

static std::hash<const char *> fn;

Hostile * HostileManager::getHostile(const char * id)
{
	printf("GETTING HOSTILE ID=[%s] %d\n",id,this->hostileMap[id]);
	return this->hostileMap[id];
}

void HostileManager::addHostile(const char * id, Hostile * hostile)
{
	printf("ADDING HOSTILE ID=[%s] hostile=%d\n",id,hostile);
	char * tmp = new char[strlen(id)+1];
	for (unsigned int i = 0;i < strlen(id)+1;i++)
		tmp[i] = id[i];
	this->hostileMap[tmp] = hostile;
}


HostileInstance * HostileManager::createHostileInstance(const char * id)
{
	HostileInstance * hostile = NULL;
	for (unsigned int i = 0;i < this->hostileInstanceList.size();i++)
	{
		if (this->hostileInstanceList[i]->isActive() == false)
		{
			hostile = this->hostileInstanceList[i];
		}
	
	}
	
	if (hostile == NULL)
	{
		hostile = new HostileInstance();
		this->hostileInstanceList.push_back(hostile);
	}

	hostile->setAge(0);
	hostile->setSpeed(0);
	hostile->setDirection(0);
	hostile->setActive(true);
	if (this->getHostile(id) == NULL)
	{
		printf("ERROR : hostile not found with id %s\n",id);
	}
	hostile->setHostile(this->getHostile(id));
	return hostile;
}

HostileInstance * HostileManager::createHostileInstance(Hostile * h, Vector2D position)
{
	if (h == NULL)
	{
		printf("ERROR : hostile parameter NULL\n");
	}

	//puts("CREATE HOSTILE");
	HostileInstance * hostile = NULL;
	int k = 0;
	for (unsigned int i = 0;i < this->hostileInstanceList.size();i++)
	{
		if (this->hostileInstanceList[i]->isActive() == false)
		{
			this->hostileInstanceList[i]->setActive(true);
			hostile = this->hostileInstanceList[i];
			//printf("GETTING EXISTING HOSTILE %d %d %d\n",i,hostile,this->hostileInstanceList[i]->isActive());
			k = i;
			break;
		}
	
	}
	
	if (hostile == NULL)
	{
		hostile = new HostileInstance();
		this->hostileInstanceList.push_back(hostile);
	}
	else
	{
	}
	hostile->setAge(0);
	hostile->setSpeed(0);
	hostile->setDirection(0);
	hostile->setLife(1);
	hostile->setActive(true);
	hostile->setHostile(h);
	hostile->setPosition(position);
	return hostile;
}

std::vector<HostileInstance*> * HostileManager::getHostileInstanceList()
{
	return &this->hostileInstanceList;
}

void HostileManager::run(long dt)
{
	for (unsigned int i = 0; i < this->hostileInstanceList.size();i++)
	{
		if (this->hostileInstanceList[i]->isActive())
		{
			this->hostileInstanceList[i]->run(dt);
		}
	}
}

int HostileManager::countActiveInstance()
{
	int count = 0;
	for (unsigned int i = 0; i < this->hostileInstanceList.size();i++)
	{
		if (this->hostileInstanceList[i]->isActive())
		{
			count++;
		}
	}
	return count;
}