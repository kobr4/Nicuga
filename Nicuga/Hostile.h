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
#include "BoundingBox.h"
#include "Shoot.h"
#include <vector>
class BarrageInstance;
class Barrage;
class IRenderable;
class Hostile
{
public:
	Hostile(void);
	~Hostile(void);
	void onCollision(Shoot shoot);
	BoundingBox getBoundingBox();
	void setBoundingBox(BoundingBox box);
	void addBarrage(Barrage * barrage);
	std::vector<Barrage*> * getBarrageList();
	int getLife();
	void setLife(int life);
	void setScore(int score);
	void setRenderable(IRenderable * renderable);
	IRenderable * getRenderable();
	friend class HostileInstance;
private:
	int life;
	int score;
	BoundingBox boundingBox;
	std::vector<Barrage*> barrageDataList;
	IRenderable * renderable;
};

class HostileInstance
{
public :
	Vector2D getPosition();
	void setPosition(Vector2D pos);
	bool isActive();
	void setActive(bool active);
	void run(long dt);
	void deactive();
	std::vector<HostileInstance*> hostileList;
	BoundingBox getBoundingBox();
	void onCollision(Shoot shoot);
	void addBarrage(BarrageInstance*);
	void init();
	int getScore();
	IRenderable * getRenderable();

	//Setters
	void setHostile(Hostile * hostile);
	void setAge(int age);
	void setSpeed(float speed);
	void setDirection(float direction);
	void setUseScript(bool useScript);
	void setBarrage(Barrage * barrage);

	//Called from bulletML
	void changeDirection(float newSpeed);
	void changeSpeed(float newDirection);
	float getDirection();
	float getSpeed();
	int getLife();
	void setLife(int life);
private :
	int age;
	int life;
	Hostile * hostile;
	Vector2D position;
	Vector2D initialPosition;
	bool active;
	std::vector<BarrageInstance*> barrageList;
	float speed;
	float direction;
	bool useScript;
	Barrage * barrage;
	tExecutionContext executionContext;
};