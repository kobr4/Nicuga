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

#pragma once
#include "Vector2D.h"
#include "bulletml.h"

class BarrageInstance;
class IRenderable;
class Shoot
{
public:
	Shoot(void);
	~Shoot(void);
	bool isActive();
	void setActive(bool active);
	Vector2D getPosition();
	void setPosition(Vector2D newpos);
	void onCollision();
	void run(long dt);
	BarrageInstance * getBarrageInstance();
	void init(tParsingData * pstData, int iStartValue);
	int getDamage();
	IRenderable * getRenderable();

	//Setters
	void setBarrageInstance(BarrageInstance * barrage);
	void setAge(int age);
	void setUseScript(bool usescript);
	void setSpeed(float speed);
	void setDirection(float direction);

	//Called from bulletML
	void changeDirection(float newSpeed);
	void changeSpeed(float newDirection);
	float getDirection();
	float getSpeed();
	void setRenderable(IRenderable * renderable);

private:
	bool active;
	bool useScript;
	Vector2D position;
	Vector2D initialPosition;
	tExecutionContext executionContext;
	int age;
	float speed;
	float direction;
	BarrageInstance * barrageInstance;
	IRenderable * renderable;
};

