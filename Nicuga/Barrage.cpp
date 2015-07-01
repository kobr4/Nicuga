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

#include "Barrage.h"
#include "Level.h"
#include "FastMath.h"

Barrage::Barrage(void)
{
	this->renderable = NULL;
}


Barrage::~Barrage(void)
{
}

Barrage * Barrage::createBarrage(char * filename)
{
	Barrage * barrage = new Barrage();
	barrage->loadBulletML(filename);
	return barrage;
}

void Barrage::loadBulletML(char * filename)
{
	load_bulletml(filename,&this->parsingData);
}

void Barrage::setDamage(int damage)
{
	this->damage = damage;
}

void Barrage::setRenderable(IRenderable * renderable)
{
	this->renderable = renderable;
}

void BarrageManager::addBarrage(const char * id,Barrage * barrage)
{
	char * tmp = new char[strlen(id)+11];
	for (unsigned int i = 0;i < strlen(id)+1;i++)
		tmp[i] = id[i];
	
	this->barrageMap[tmp] = barrage;
}

Barrage * BarrageManager::addBarrage(const char * id, const char * file,int damage)
{
	
	Barrage *  barrage = Barrage::createBarrage((char*)file);
	barrage->setDamage(damage);
	this->addBarrage(id,barrage);
	return barrage;
}

Barrage * BarrageManager::getBarrage(const char * id)
{
	return this->barrageMap[id];	
}

void Barrage::setOffsetPosition(int offsetx,int offsety) {
	this->offset_position.set((float)offsetx,(float)offsety);
}

BarrageInstance * BarrageManager::getBarrageInstance(const char * id)
{
	BarrageInstance * barrageInstance = NULL;
	for (unsigned int i = 0;i < this->barrageInstanceList.size();i++)
	{
		if (!this->barrageInstanceList[i]->isActive())
		{
			barrageInstance =  this->barrageInstanceList[i];
		}
	}

	if (barrageInstance == NULL)
	{
		barrageInstance = new BarrageInstance();
		this->barrageInstanceList.push_back(barrageInstance);
	}

	barrageInstance->setBarrage(this->barrageMap[id]);
	barrageInstance->init();
	barrageInstance->setActive(true);

	return barrageInstance;
}

void BarrageInstance::createBullet(float fSpeed, float fDirection,int iStartValue)
{
	Shoot * bullet = this->getBarrageManager()->getBulletInstance();
	bullet->setActive(true);
	bullet->setPosition(this->getPosition());
	bullet->setBarrageInstance(this);
	bullet->setAge(0);
	bullet->setSpeed(fSpeed);
	bullet->setDirection(fDirection);
	bullet->setRenderable(this->getRenderable());
	if (iStartValue != -1)
	{
		bullet->setUseScript(true);
		bullet->init(&this->barrage->parsingData,iStartValue);
	}
	else
	{
		bullet->setUseScript(false);
	}
}

void create_bullet_callback(void * pUserData, float fSpeed, float fDirection,int iStartValue)
{
	//printf("CREATE BULLET speed=%f direction=%f startvalue=%d",fSpeed,fDirection,iStartValue);
	BarrageInstance * binstance = (BarrageInstance*)pUserData;
	binstance->createBullet(fSpeed,fDirection,iStartValue);
}
   
void set_speed_callback(void * pUserData, float fSpeed)
{

}
 
void set_direction_callback(void * pUserData, float fDirection)
{
	BarrageInstance * binstance = (BarrageInstance*)pUserData;
	//binstance->setDirection();
}
  
void get_aim_direction_callback(void *pUserData, float *pfDirection)
{
	BarrageInstance * binstance = (BarrageInstance*)pUserData;
	Vector2D aimAt = binstance->getBarrageManager()->getAimAtPosition();
	float adj = -(aimAt.getX() - binstance->getPosition().getX());
	float opp = (aimAt.getY() - binstance->getPosition().getY());
	float deg;

	if (opp >= 0.0)
		deg = FastMath::fastArcTan(adj/opp) * (float)FastMath::M_180_PI;
	else
		deg =  180 - FastMath::fastArcTan(adj/-opp) * (float)FastMath::M_180_PI;

	*pfDirection = deg;
	//printf("Barrage_get_aim_direction_callback %f %f %f\n",deg,opp,adj);
}
   
void get_direction_callback(void *pUserData, float *pfDirection)
{

}
 
void get_speed_callback(void *pUserData, float *pfSpeed)
{

}


BarrageInstance::BarrageInstance()
{
	this->executionContext.create_bullet_callback = create_bullet_callback;
	this->executionContext.get_aim_direction_callback = get_aim_direction_callback;
	this->executionContext.get_direction_callback = get_direction_callback;
	this->executionContext.get_speed_callback = get_speed_callback;
	this->executionContext.set_direction_callback = set_direction_callback;
	this->executionContext.set_speed_callback = set_speed_callback;
	this->executionContext.pUserData = this;

}

void BarrageInstance::setBarrageManager(BarrageManager * barrageManager)
{
	this->barrageManager = barrageManager;
}

BarrageManager * BarrageInstance::getBarrageManager()
{
	return this->barrageManager;
}

BarrageInstance * BarrageManager::getBarrageInstance(Barrage * barrage)
{
	BarrageInstance * barrageInstance = NULL;
	for (unsigned int i = 0;i < this->barrageInstanceList.size();i++)
	{
		if (!this->barrageInstanceList[i]->isActive())
		{
			barrageInstance =  this->barrageInstanceList[i];
		}
	}

	if (barrageInstance == NULL)
	{
		barrageInstance = new BarrageInstance();
		this->barrageInstanceList.push_back(barrageInstance);
	}

	barrageInstance->setBarrage(barrage);
	barrageInstance->init();
	barrageInstance->setActive(true);

	return barrageInstance;
}

bool BarrageInstance::isActive()
{
	return this->active;
}

void BarrageInstance::setActive(bool active)
{
	this->active = active;
}

void BarrageInstance::setBarrage(Barrage * barrage)
{
	this->barrage = barrage;
}

void BarrageInstance::init()
{
	this->executionContext.pUserData = this;
	init_bulletmlcontext(&this->executionContext,&this->barrage->parsingData);
}

void BarrageInstance::run(long dt)
{
	//puts("BarrageInstance::run");
	run_bulletml(&this->executionContext);
}

void BarrageInstance::setPosition(float x, float y)
{
	this->position.set(x,y);
}

int BarrageInstance::getDamage()
{
	return this->barrage->damage;
}

IRenderable * BarrageInstance::getRenderable()
{
	return this->barrage->renderable;
}

Shoot* BarrageManager::getBulletInstance()
{
	Shoot * bullet = NULL;
	for (unsigned int i = 0; i < this->bulletList.size();i++)
	{
		if (!this->bulletList[i]->isActive())
		{
			bullet = this->bulletList[i];
		}
	}
	
	if (bullet == NULL)
	{
		Shoot * shot = new Shoot();
		this->bulletList.push_back(shot);
		bullet = this->bulletList.back();
		//bullet = shot;
	}

	return bullet;
}

Vector2D BarrageInstance::getPosition()
{
	return this->position + this->barrage->offset_position;
}

std::vector<Shoot*> * BarrageManager::getBulletList()
{
	return &this->bulletList;
}


void BarrageManager::run(long dt)
{
	for (unsigned int i = 0;i < this->bulletList.size();i++)
	{
		if (this->bulletList[i]->isActive())
		{
			this->bulletList[i]->run(dt);
		}
	}
}

void BarrageManager::setAimAtPosition(Vector2D position)
{
	this->aimAtPosition.set(position.getX(),position.getY());
}

Vector2D BarrageManager::getAimAtPosition()
{
	return this->aimAtPosition;
}

void BarrageManager::disableAllBullets()
{
	for (unsigned int i = 0;i < this->bulletList.size();i++)
	{
		this->bulletList[i]->setActive(false);
	}
}