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
#include "ShipFactory.h"
#include "SaxTagsUtils.h"
#include <stdio.h>
#include <string.h>
#include "Level.h"
#include "Barrage.h"
#include "RenderableAbstractFactory.h"

static Ship * currentShip;
static XML_Parser currentParer;
static XML_StartElementHandler oldStart;
static XML_EndElementHandler oldEnd;
static void * oldUserData;
static RenderableAbstractFactory * g_renderableFactory;
void ship_xmlend(void *data, const char *el);

void ship_xmlstart(void *data, const char *el, const char **attr) 
{
	int iAttrCount = XML_GetSpecifiedAttributeCount(currentParer);
	//printf("ShipFactory TAG : %s %d attributes\n",el,iAttrCount);
	const char * id;
	int topx = 0;
	int topy = 0;
	int bottomx = 0;
	int bottomy = 0;

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
		
		currentShip->setBoundingBox(BoundingBox(topx,topy,bottomx,bottomy));
		break;
	case RENDERABLE:
		g_renderableFactory->setFactoryCallbacks(currentParer, ship_xmlstart, ship_xmlend);
		currentShip->setRenderable(g_renderableFactory->getRenderable());
		break;
	case BARRAGE :
		Level * level = (Level*)oldUserData;
		id = getAttribute("id",attr,iAttrCount);
		//printf("ShipFactory Barrage : %s\n",id);
		Barrage * b = level->getFriendlyBarrageManager()->getBarrage(id);
		if (b != NULL)
		{
			BarrageInstance * binstance = level->getFriendlyBarrageManager()->getBarrageInstance(b);
			binstance->setBarrageManager(level->getFriendlyBarrageManager());
			currentShip->addBarrage(b);
			currentShip->addBarrageInstance(binstance);
		}
		else
		{
			printf("ERROR : COULDN'T FIND BARRAGE WITH ID %s PLEASE CHECK CONFIGURATION\n",id);
			exit(0);
		}
		break;

	}
}

void ship_xmlend(void *data, const char *el)
{
	switch(getTagType(el))
	{
	case SHIP :
		//Put back passed handler
		XML_SetUserData(currentParer, oldUserData);
		XML_SetElementHandler(currentParer, oldStart, oldEnd);
		break;
	}
}

void ShipFactory::setFactoryCallbacks(XML_Parser p, XML_StartElementHandler start, XML_EndElementHandler end, RenderableAbstractFactory * renderableFactory)
{
	currentShip = new Ship();
	currentShip->setInvicibleTimer(0);

	g_renderableFactory = renderableFactory;

	oldStart = start;
	oldEnd = end;
	oldUserData = XML_GetUserData(p);
	currentParer = p;

	//Put hostile factory handler
	XML_SetUserData(currentParer, currentShip);
	XML_SetElementHandler(currentParer, ship_xmlstart, ship_xmlend);
}

Ship * ShipFactory::get()
{
	return currentShip;
}