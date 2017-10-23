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
#include "Shoot.h"
#include "FastMath.h"
#include <stdio.h>
#include "Barrage.h"

Shoot::Shoot(void)
{
}


Shoot::~Shoot(void)
{
}

void Shoot::onCollision()
{

}

Vector2D Shoot::getPosition()
{
	return this->position;
}

bool Shoot::isActive()
{
	return this->active;
}

void Shoot::setActive(bool active)
{
	this->active = active;
}


void Shoot::setPosition(Vector2D newpos)
{
	this->initialPosition.set(newpos.getX(),newpos.getY());
	this->position.set(newpos.getX(),newpos.getY());
}

BarrageInstance * Shoot::getBarrageInstance()
{
	return this->barrageInstance;
}

void Shoot::setBarrageInstance(BarrageInstance * barrage)
{
	this->barrageInstance = barrage;
}
	
void Shoot::setAge(int age)
{
	this->age = age;
}
	
void Shoot::setUseScript(bool usescript)
{
	this->useScript = usescript;
}

void Shoot_create_bullet_callback(void * pUserData, float fSpeed, float fDirection,int iStartValue)
{
	puts("ERROR : CREATE BULLET CALLBACK NOT IMPLEMENTED IN SHOOT CLASS");
}

void Shoot_set_speed_callback(void * pUserData, float fSpeed)
{
	printf("Shoot_set_speed_callback %f\n",fSpeed);
	Shoot * bullet = (Shoot*)pUserData;
	bullet->changeSpeed(fSpeed);
}
 
void Shoot_set_direction_callback(void * pUserData, float fDirection)
{
	Shoot * bullet = (Shoot*)pUserData;
	bullet->changeDirection(fDirection);
}
  
void Shoot_get_aim_direction_callback(void *pUserData, float *pfDirection)
{
	
	Shoot * bullet = (Shoot*)pUserData;
	Vector2D aimAt = bullet->getBarrageInstance()->getBarrageManager()->getAimAtPosition();
	float adj = (aimAt.getX() - bullet->getPosition().getX());
	float opp = (aimAt.getY() - bullet->getPosition().getY());
	float deg;

	       /*             
	if (adj >= 0.0)
		deg = FastMath::fastArcTan(adj/opp) * (float)FastMath::M_180_PI;
	else
		deg =  180 + FastMath::fastArcTan(-adj/opp) * (float)FastMath::M_180_PI;
		*/
	
	if (opp >= 0.0)
		deg = FastMath::fastArcTan(adj/opp) * (float)FastMath::M_180_PI;
	else
		deg =  180 - FastMath::fastArcTan(adj/-opp) * (float)FastMath::M_180_PI;
	
	//deg = FastMath::fastArcTan(adj/opp) * (float)FastMath::M_180_PI;
	int tmpdir =(int)(*pfDirection)%360;
	if (abs(tmpdir - deg) > abs(tmpdir - 360.f + deg)) {
		//printf("pfDirection=%f\n",*pfDirection);
		*pfDirection = 360.f - deg;

	} else {
		*pfDirection = deg;
	}
	
	//*pfDirection = deg;
	//printf("Shoot_get_aim_direction_callback %f %f %f target(%f,%f)\n",deg,opp,adj,aimAt.getX(),aimAt.getY());
}
   
void Shoot_get_direction_callback(void *pUserData, float *pfDirection)
{
	Shoot * bullet = (Shoot*)pUserData;
	*pfDirection = bullet->getDirection();
}
 
void Shoot_get_speed_callback(void *pUserData, float *pfSpeed)
{
	Shoot * bullet = (Shoot*)pUserData;
	*pfSpeed = bullet->getSpeed();
}

void Shoot::init(tParsingData * pstData, int iStartValue)
{
	if ((pstData != NULL)&&(iStartValue != -1))
	{
		this->executionContext.pUserData = this;
		
		this->executionContext.create_bullet_callback = Shoot_create_bullet_callback;
		this->executionContext.get_aim_direction_callback = Shoot_get_aim_direction_callback;
		this->executionContext.get_direction_callback = Shoot_get_direction_callback;
		this->executionContext.get_speed_callback = Shoot_get_speed_callback;
		this->executionContext.set_direction_callback = Shoot_set_direction_callback;
		this->executionContext.set_speed_callback = Shoot_set_speed_callback;

		init_bulletmlcontext(&this->executionContext, pstData);
		this->executionContext.iCurrentItem = iStartValue;
	}
}

void Shoot::run(long dt)
{
	this->age += dt;

	short posx = this->initialPosition.getX() + FastMath::fastCos((int)direction + 90) * this->speed * (float)this->age * 1/1000.0f;
	short posy = this->initialPosition.getY() + FastMath::fastSin((int)direction + 90) * this->speed * (float)this->age * 1/1000.0f;
	this->position.set(posx,posy);

	if (this->useScript)
	{
		run_bulletml(&this->executionContext);
	}
}

void Shoot::setSpeed(float speed)
{
	this->speed = speed;
}

void Shoot::setDirection(float direction)
{
	this->direction = direction;
}

float Shoot::getDirection()
{
	return this->direction;
}

float Shoot::getSpeed()
{
	return this->speed;
}

void Shoot::changeDirection(float direction)
{
	//Origin for current motion vector is updated
	this->initialPosition.set(this->position.getX(),this->position.getY());
	this->age = 0;
	this->direction = direction;
}

void Shoot::changeSpeed(float newspeed)
{
	//Origin for current motion vector is updated
	this->initialPosition.set(this->position.getX(),this->position.getY());
	this->age = 0;
	this->speed = newspeed;
}

int Shoot::getDamage()
{
	return this->barrageInstance->getDamage();
}

IRenderable * Shoot::getRenderable()
{
	//return this->barrageInstance->getRenderable();
	return renderable;
}

void Shoot::setRenderable(IRenderable * renderable) {
	this->renderable = renderable;
}
