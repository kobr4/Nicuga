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

#include "Level.h"
#include "expat.h"
#include <stdio.h>
#include "SaxTagsUtils.h"
#include "Barrage.h"
#include "Hostile.h"
#include "HostileFactory.h"
#include "ShipFactory.h"
#include "RenderableAbstractFactory.h"

#define BUFFSIZE	8192
static char Buff[BUFFSIZE];

static	XML_Parser p;
//static Barrage * currentBarrage;
static char currentBarrageId[255];
void level_start(void *data, const char *el, const char **attr);
void level_end(void *data, const char *el);
void barrage_start(void *data, const char *el, const char **attr);
void barrage_end(void *data, const char *el);

Level::Level(void)
{
	this->totalTime = 0;
	this->sequencePosition = 0;
	this->barrageManager = new BarrageManager();
	this->friendlyBarrageManager = new BarrageManager();
	this->hostileManager = new HostileManager();
}


Level::~Level(void)
{
}

void sequence_start(void *data, const char *el, const char **attr){
	//printf("SEQ START TAG : %s\n",el);
	Level * level = (Level*)data;
	int iAttrCount = XML_GetSpecifiedAttributeCount(p);

	//Seq variable
	const char * hostileId;
	Hostile * hostile;
	Barrage * barrage;
	const char * tstr;
	const char * posstr;
	const char * speedstr;
	const char * directionstr;
	const char * barrageidstr;
	const char * blocking;
	int posx;
	int posy;
	int tseq;
	int speed = 0;
	int direction =0;
	bool isblocking;
	switch (getTagType(el))
	{
	case HOSTILE :
		isblocking = false;
		hostileId = getAttribute("id", attr, iAttrCount);
		hostile = level->getHostileManager()->getHostile(hostileId);
		if (hostile == NULL)
		{
			printf("ERROR: In sequence , cannot find hostile with id=%s\n",hostileId);
			exit(0);
		}
		tstr = getAttribute("t", attr, iAttrCount);
		tseq = atoi(tstr);
		posstr = getAttribute("pos", attr, iAttrCount);
		sscanf(posstr,"%d,%d",&posx,&posy);
		speed = 0;
		direction = 0;
		speedstr = getAttribute("speed", attr, iAttrCount);
		if (speedstr != NULL)
			speed = atoi(speedstr);
		directionstr = getAttribute("direction", attr, iAttrCount);
		if (directionstr != NULL)
			direction = atoi(directionstr);
		barrageidstr = getAttribute("barrageid", attr, iAttrCount);
		if (barrageidstr != NULL)
			barrage = level->getBarrageManager()->getBarrage(barrageidstr);
		else
			barrage = NULL;

		blocking = getAttribute("blocking", attr, iAttrCount);
		if (blocking != NULL)
			if (!strcmp(blocking,"true"))
				isblocking = true;
		level->addHostileSequence(hostile,tseq,posx,posy,speed,direction,barrage,isblocking);

		break;
	default :
		break;
	}
}

void sequence_end(void *data, const char *el){
	switch (getTagType(el))
	{
	case LEVELSEQUENCE :
		//End sequence parsing
		XML_SetElementHandler(p, level_start, level_end);
		break;
	default :
		break;
	}
}

int Sequence::getTime()
{
	return this->time;
}

bool Sequence::getBlocking()
{
	return this->blocking;
}

void Sequence::setBlocking(bool isblocking)
{
	this->blocking = isblocking;
}

void Sequence::setTime(int time)
{
	this->time = time;
}

void Sequence::execute(Level * level)
{
	//printf("Sequence::execute call %f %f\n",this->position.getX(),this->position.getY());


	HostileInstance * h = level->getHostileManager()->createHostileInstance(this->hostile,this->position);

	h->setSpeed((float)this->speed);
	h->setDirection((float)this->direction);
	h->setBarrage(this->barrage);
	h->setLife(this->hostile->getLife());

	//h->setPosition(Vector2D(this->position.getX(),this->position.getY()));
	if (this->barrage != NULL)
	{
		//puts("Hostile use script");
		h->setUseScript(true);
		h->init();
	}
	else
	{
		h->setUseScript(false);
	}

	std::vector<Barrage*> * barrageList = this->hostile->getBarrageList();
	for (unsigned int i = 0;i < barrageList->size();i++)
	{
		Barrage * barrage = (*barrageList)[i];
		BarrageInstance * b = level->getBarrageManager()->getBarrageInstance(barrage);
		b->init();
		b->setBarrageManager(level->getBarrageManager());
		b->setActive(true);
		h->addBarrage(b);
	}
}




void level_start(void *data, const char *el, const char **attr) {
	printf("TAG : %s\n",el);
	Level * level = (Level*)data;
	int iAttrCount = XML_GetSpecifiedAttributeCount(p);
	const char * id;
	const char * file;
	const char * damagestr;
	const char * offsetposition;
	int topx = 0;
	int topy = 0;
	int bottomx = 0;
	int bottomy = 0;
	int damage = 0;
	Barrage * barrage;
	switch (getTagType(el))
	{
	case HOSTILE :
		id = getAttribute("id",attr,iAttrCount);
		HostileFactory::setFactoryCallbacks(p,level_start,level_end,level->getRenderableFactory());
		level->getHostileManager()->addHostile(id,HostileFactory::loadHostile());
		break;
	case SHIP :
		ShipFactory::setFactoryCallbacks(p, level_start, level_end, level->getRenderableFactory());
		level->setShip(ShipFactory::get());
		level->getShip()->setActive(true);
		break;
	case BARRAGE :
		id = getAttribute("id",attr,iAttrCount);
		file = getAttribute("file",attr,iAttrCount);
		offsetposition = getAttribute("offsetposition",attr,iAttrCount);
		damagestr = getAttribute("damage",attr,iAttrCount);
		if (damagestr != NULL)
			damage = atoi(damagestr);
		if (offsetposition != NULL) {
			sscanf(offsetposition,"%d,%d",&topx,&topy);
		}
		barrage = level->getBarrageManager()->addBarrage(id,file,damage);
		barrage->setOffsetPosition(topx,topy);
		barrage = level->getFriendlyBarrageManager()->addBarrage(id,file,damage);
		barrage->setOffsetPosition(topx,topy);
		strcpy(currentBarrageId,id);

		

		//Place barrage callback only meant to load renderable tag
		XML_SetElementHandler(p, barrage_start, barrage_end);
		break;
	case LEVELSEQUENCE :
		XML_SetElementHandler(p, sequence_start, sequence_end);
		break;
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
		level->setBoundingBox(BoundingBox((short)topx,(short)topy,(short)bottomx,(short)bottomy));
		break;
	default :
		break;
	}
}

void level_end(void *data, const char *el){
	/*
	switch (getTagType(el))
	{
	default :

		break;
	}
	*/
}

void barrage_start(void *data, const char *el, const char **attr)
{
	Level * level = (Level*)data;
	switch (getTagType(el))
	{
	case RENDERABLE :
		level->getRenderableFactory()->setFactoryCallbacks(p,barrage_start,barrage_end);
		level->getBarrageManager()->getBarrage(currentBarrageId)->setRenderable(level->getRenderableFactory()->getRenderable());
		level->getFriendlyBarrageManager()->getBarrage(currentBarrageId)->setRenderable(level->getRenderableFactory()->getRenderable());
		break;
	}
}

void barrage_end(void *data, const char *el){
	switch (getTagType(el))
	{
	case BARRAGE :
		XML_SetElementHandler(p, level_start, level_end);
		break;
	}
}

void Level::addHostileSequence(Hostile * hostile, int time, int posx, int posy,int speed, int direction, Barrage * barrage,bool isblocking)
{
	sequenceVector.push_back(new Sequence(hostile,time,Vector2D((float)posx,(float)posy),speed,direction,barrage,isblocking));
}

void Level::load(char * filename,RenderableAbstractFactory * renderableFactory)
{
	printf("Loading level : %s\n",filename);
	this->renderableFactory = renderableFactory;

	p = XML_ParserCreate(NULL);

	if (! p) {
		fprintf(stderr, "Couldn't allocate memory for parser\n");
		exit(-1);
	}

	XML_SetUserData(p, this);
	XML_SetElementHandler(p, level_start, level_end);

	FILE * f;
	fopen_s(&f,filename,"rb"); 
	if (f == NULL)
	printf("error opening file\n");

	for (;;) {
	int done;
	int len;
	len = fread(Buff, 1, BUFFSIZE, f);
	if (ferror(stdin)) {
		fprintf(stderr, "Read error\n");
		exit(-1);
	}
	done = feof(f);
	if (! XML_Parse(p, Buff, len, done)) {
		fprintf(stderr, "Parse error at line %d:\n%s\n",
			XML_GetCurrentLineNumber(p),
			XML_ErrorString(XML_GetErrorCode(p)));
		exit(-1);
	}

	if (done)
		break;
	}
	XML_ParserFree(p);

	if (f != NULL)
		fclose(f);

	this->respawnShip();

}

void Level::respawnShip()
{
	this->ship->setPosition((this->boundingBox.getTopLeft().getX()-this->boundingBox.getBottomRight().getX())/2,
		(this->boundingBox.getTopLeft().getY()-this->boundingBox.getBottomRight().getY())/2);
}

HostileManager * Level::getHostileManager()
{
	return this->hostileManager;
}

BarrageManager * Level::getBarrageManager()
{
	return this->barrageManager;
}

BarrageManager * Level::getFriendlyBarrageManager()
{
	return this->friendlyBarrageManager;
}

void Level::run(long dt)
{
	this->totalTime += dt;
	bool runSequence = true;
	while (runSequence)
	{
		if (sequencePosition < this->sequenceVector.size())
		{
			Sequence * s = this->sequenceVector[sequencePosition];
			if (sequencePosition > 0)
			{
				Sequence * sold = this->sequenceVector[sequencePosition];
				if (sold->getBlocking())
				{
					if (this->getHostileManager()->countActiveInstance())
					{
						break;
					}
					else
					{
						this->sequenceVector[sequencePosition]->setBlocking(false);
						for (unsigned int i = sequencePosition;i < this->sequenceVector.size();i++)
						{

							if (this->sequenceVector[i]->getBlocking())
							{
								break;
							}

							int t = this->sequenceVector[i]->getTime();
							this->sequenceVector[i]->setTime(t+this->totalTime);
						}
					}
				}
			}

			if ((long)(s->getTime()) < this->totalTime)
			{
				s->execute(this);
				sequencePosition++;
			}
			else
			{
				runSequence = false;
			}
		}
		else
		{
			runSequence = false;
		}
	}


	//Cleaning off-screen bullets
	std::vector<Shoot*> * bulletList;
	bulletList = this->getBarrageManager()->getBulletList();
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			if (!this->boundingBox.isBound((*bulletList)[i]->getPosition()))
			{
				//printf("bullet deactivation\n");
				(*bulletList)[i]->setActive(false);
			}
		}
	}
	
	//Cleaning off-screen hostiles
	std::vector<HostileInstance * > * hostileInstanceList = this->getHostileManager()->getHostileInstanceList();
	for (unsigned int i = 0;i < hostileInstanceList->size();i++)
	{
		if ((*hostileInstanceList)[i]->isActive())
		{
			if (!this->boundingBox.isBound((*hostileInstanceList)[i]->getPosition()))
			{
				//printf("hostile deactivation\n");
				(*hostileInstanceList)[i]->deactive();
			}
		}
	}

	//Cleaning off-screen friendly bullets
	bulletList = this->getFriendlyBarrageManager()->getBulletList();
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			if (!this->boundingBox.isBound((*bulletList)[i]->getPosition()))
			{
				//printf("friendly bullet deactivation %f %f\n",(*bulletList)[i].getPosition().getX(),(*bulletList)[i].getPosition().getY());
				(*bulletList)[i]->setActive(false);
			}
		}
	}



	this->getBarrageManager()->setAimAtPosition(this->getShip()->getPosition());
	this->getHostileManager()->run(dt);
	this->getBarrageManager()->run(dt);
	

	for (unsigned int i = 0;i < hostileInstanceList->size();i++)
	{
		if ((*hostileInstanceList)[i]->isActive())
		{
			this->getFriendlyBarrageManager()->setAimAtPosition((*hostileInstanceList)[i]->getPosition());
			break;
		}
	}
	this->getFriendlyBarrageManager()->run(dt);
}

Ship * Level::getShip()
{
	return this->ship;
}

bool Level::atEnd()
{
	

	int activeHostile = 0;
	for (unsigned int i = 0;i < this->getHostileManager()->getHostileInstanceList()->size();i++)
	{
		if((*this->getHostileManager()->getHostileInstanceList())[i]->isActive())
			activeHostile++;
	}

	//printf("Sequence position : %d %d %d\n",sequencePosition,this->sequenceVector.size(),activeHostile);

	if (activeHostile > 0)
		return false;

	
	if (this->sequenceVector.size() == sequencePosition)
	{
		this->barrageManager->disableAllBullets();
		return true;
	}
	else return false;


}

void Level::setShip(Ship * ship)
{
	this->ship = ship;
}

Sequence::Sequence(Hostile * hostile, int time,Vector2D pos,int speed, int direction,Barrage * barrage,bool isblocking)
{
	this->hostile = hostile;
	this->time = time;
	this->position.set(pos.getX(),pos.getY());
	this->speed = speed;
	this->direction = direction;
	this->barrage = barrage;
	this->blocking = isblocking;
}

void Level::setBoundingBox(BoundingBox box)
{
	this->boundingBox = box;
}

RenderableAbstractFactory * Level::getRenderableFactory()
{
	return this->renderableFactory;
}