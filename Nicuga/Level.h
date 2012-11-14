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
#include <vector>
#include "Vector2D.h"
#include "BoundingBox.h"

class BarrageManager;
class HostileManager;
class Hostile;
class Level;
class Ship;
class Barrage;
class RenderableAbstractFactory;

class Sequence
{
private :
	Hostile * hostile;
	int time;
	Vector2D position;
	int speed;
	int direction;
	Barrage * barrage;
	bool blocking;
public :
	Sequence(Hostile * hostile, int time, Vector2D pos,int speed, int direction, Barrage * barrage, bool isblocking);
	int getTime();
	void execute(Level * level);
	bool getBlocking();
	void setBlocking(bool isblocking);
	void setTime(int time);
};

class Level
{
private :
	BarrageManager * barrageManager;
	BarrageManager * friendlyBarrageManager;
	HostileManager * hostileManager;
	std::vector<Sequence*> sequenceVector;
	unsigned int sequencePosition;
	long totalTime;
	Ship * ship;
	BoundingBox boundingBox;
	RenderableAbstractFactory * renderableFactory;
public:
	Level(void);
	~Level(void);
	void load(char * filename,RenderableAbstractFactory * renderableFactory);
	void run(long dt);
	void addHostileSequence(Hostile * hostile, int time, int posx, int posy,int speed, int direction, Barrage * barrage,bool isblocking);
	BarrageManager * getBarrageManager();
	BarrageManager * getFriendlyBarrageManager();
	HostileManager * getHostileManager();
	Ship * getShip();
	void setShip(Ship * ship);
	void setBoundingBox(BoundingBox box);
	bool atEnd();
	void respawnShip();
	RenderableAbstractFactory * getRenderableFactory();
};
