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

#include "Hostile.h"

#include "Barrage.h"
#include "FastMath.h"

Hostile::Hostile(void)
{
	this->score = 0;
	this->renderable = NULL;
}


Hostile::~Hostile(void)
{
}



void Hostile::onCollision(Shoot shoot)
{
}

Vector2D HostileInstance::getPosition()
{
	return this->position;
}

void HostileInstance::setPosition(Vector2D pos)
{
	this->position.set(pos.getX(),pos.getY());
	this->initialPosition.set(pos.getX(),pos.getY());

	for (unsigned int i = 0;i < this->barrageList.size();i++)
	{
		this->barrageList[i]->setPosition(pos.getX(),pos.getY());
	}
}

BoundingBox Hostile::getBoundingBox()
{
	return this->boundingBox;
}

void Hostile::setBoundingBox(BoundingBox box)
{
	this->boundingBox = box;
}

std::vector<Barrage*> * Hostile::getBarrageList()
{
	return &this->barrageDataList;
}

void Hostile::addBarrage(Barrage * barrage)
{
	this->barrageDataList.push_back(barrage);
}

void Hostile::setScore(int score)
{
	this->score = score;
}


void Hostile::setLife(int life)
{
	this->life = life;
}

int Hostile::getLife()
{
	return this->life;
}


void Hostile::setRenderable(IRenderable * renderable)
{
	this->renderable = renderable;
}
IRenderable * Hostile::getRenderable()
{
	return this->renderable;
}

void HostileInstance::deactive()
{
	this->active = false;

	for (unsigned int i = 0;i < this->barrageList.size();i++)
	{
		this->barrageList[i]->setActive(false);
	}

	this->barrageList.clear();

	for (unsigned int i = 0;i < this->hostileList.size();i++)
	{
		this->hostileList[i]->deactive();
	}

	this->hostileList.clear();
}

void HostileInstance::run(long dt)
{
	this->age += dt;


	for (unsigned int i = 0;i < barrageList.size();i++)
	{
		//printf("HostileInstance::run %d\n",barrageList[i]->isActive());
		if (barrageList[i]->isActive())
		{
			barrageList[i]->run(dt);
		}
	}

	float posx = this->initialPosition.getX() + FastMath::fastCos((int)direction + 90) * this->speed * (float)this->age * 1/1000.0f;
	float posy = this->initialPosition.getY() + FastMath::fastSin((int)direction + 90) * this->speed * (float)this->age * 1/1000.0f;
	//printf("HOSTILE INSTANCE POS(%f,%f)\n",direction,(float)this->age);
	this->position.set(posx,posy);
	for (unsigned int i = 0;i < this->barrageList.size();i++)
	{
		this->barrageList[i]->setPosition(posx,posy);
	}

	if (this->useScript)
	{
		run_bulletml(&this->executionContext);		
	}
}

bool HostileInstance::isActive()
{
	return this->active;
}

void HostileInstance::setActive(bool active)
{
	this->active = active;
}

void HostileInstance::setAge(int age)
{
	this->age = age;
}

void HostileInstance::setSpeed(float speed)
{
	this->speed = speed;
}

void HostileInstance::setDirection(float direction)
{
	this->direction = direction;
}

BoundingBox HostileInstance::getBoundingBox()
{
	return this->hostile->getBoundingBox();
}

void HostileInstance::onCollision(Shoot shoot)
{
	this->hostile->onCollision(shoot);
}

void HostileInstance::setHostile(Hostile * hostile)
{
	this->hostile = hostile;
}

void HostileInstance::addBarrage(BarrageInstance * barrage)
{
	barrage->setPosition(this->position.getX(),this->position.getY());
	this->barrageList.push_back(barrage);
}

void HostileInstance::setUseScript(bool useScript)
{
	this->useScript = useScript;
}

void HostileInstance::setBarrage(Barrage * barrage)
{
	this->barrage = barrage;
}

float HostileInstance::getDirection()
{
	return this->direction;
}

float HostileInstance::getSpeed()
{
	return this->speed;
}

void HostileInstance::changeDirection(float direction)
{
	//Origin for current motion vector is updated
	this->initialPosition.set(this->position.getX(),this->position.getY());
	this->age = 0;
	this->direction = direction;
}

void HostileInstance::changeSpeed(float newspeed)
{
	//Origin for current motion vector is updated
	this->initialPosition.set(this->position.getX(),this->position.getY());
	this->age = 0;
	this->speed = newspeed;
}

void Hostile_create_bullet_callback(void * pUserData, float fSpeed, float fDirection,int iStartValue)
{
	puts("ERROR : CREATE BULLET CALLBACK NOT IMPLEMENTED IN HOSTILE CLASS");
}

void Hostile_set_speed_callback(void * pUserData, float fSpeed)
{
	HostileInstance * hostile = (HostileInstance*)pUserData;
	hostile->changeSpeed(fSpeed);
}
 
void Hostile_set_direction_callback(void * pUserData, float fDirection)
{
	//printf("set direction %f\n",fDirection);
	HostileInstance * hostile = (HostileInstance*)pUserData;
	hostile->changeDirection(fDirection);
}
  
void Hostile_get_aim_direction_callback(void *pUserData, float *pfDirection)
{
	//NYI
}
   
void Hostile_get_direction_callback(void *pUserData, float *pfDirection)
{
	HostileInstance * hostile = (HostileInstance*)pUserData;
	*pfDirection = hostile->getDirection();
}
 
void Hostile_get_speed_callback(void *pUserData, float *pfSpeed)
{
	HostileInstance * hostile = (HostileInstance*)pUserData;
	*pfSpeed = hostile->getSpeed();
}

void HostileInstance::init()
{
	if (this->useScript)
	{
		this->executionContext.pUserData = this;

		this->executionContext.create_bullet_callback = Hostile_create_bullet_callback;
		this->executionContext.get_aim_direction_callback = Hostile_get_aim_direction_callback;
		this->executionContext.get_direction_callback = Hostile_get_direction_callback;
		this->executionContext.get_speed_callback = Hostile_get_speed_callback;
		this->executionContext.set_direction_callback = Hostile_set_direction_callback;
		this->executionContext.set_speed_callback = Hostile_set_speed_callback;

		init_bulletmlcontext(&this->executionContext,&this->barrage->parsingData);		
	}
}

void HostileInstance::setLife(int life)
{
	this->life = life;
}

int HostileInstance::getLife()
{
	return this->life;
}

int HostileInstance::getScore()
{
	return this->hostile->score;
}

IRenderable * HostileInstance::getRenderable()
{
	return this->hostile->renderable;
}