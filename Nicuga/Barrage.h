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
#include <hash_map>
#include "bulletml.h"
#include "Shoot.h"
#include <map>
#include <cstring>
#include <string>

class Level;
class IRenderable;
class Barrage
{
public:
	static Barrage *
	createBarrage(char * filename);
	Barrage(void);
	~Barrage(void);
	void loadBulletML(char * filename);
	void run(long dt);
	int getDamage();
	void setDamage(int damage);
	void setRenderable(IRenderable * renderable);
	friend class BarrageInstance;
	friend class HostileInstance;
	void setOffsetPosition(int offsetx,int offsety);
private:
	IRenderable * renderable;
	int damage;
	tParsingData parsingData;
	Vector2D offset_position;
};

struct cmp_str2
{
   bool operator()(char const *a, char const *b) const
   {
      return strcmp(a, b) < 0;
   }
};


class BarrageManager;
class BarrageInstance
{
public :
	void setActive(bool active);
	void run(long dt);
	bool isActive();
	void setBarrage(Barrage * barrage);
	void init();
	void setBarrageManager(BarrageManager * barrageManager);
	BarrageManager * getBarrageManager();
	BarrageInstance();
	Vector2D getPosition();
	void createBullet(float fSpeed, float fDirection,int iStartValue);
	void setPosition(float x, float y);
	int getDamage();
	IRenderable * getRenderable();
private :
	tExecutionContext executionContext;
	Barrage * barrage;
	BarrageManager * barrageManager;
	bool active;
	Vector2D position;
};

class BarrageManager
{
private :
	std::map<const char *, Barrage *,cmp_str2> barrageMap;
	std::vector<BarrageInstance*> barrageInstanceList;
	std::vector<Shoot*> bulletList; 
	Vector2D aimAtPosition;
public :
	void addBarrage(const char * id,Barrage * barrage);
	Barrage * addBarrage(const char * id, const char * file,int damage);
	Barrage * getBarrage(const char * id);
	BarrageInstance * getBarrageInstance(const char * id);
	BarrageInstance * getBarrageInstance(Barrage * barrage);
	Shoot * getBulletInstance();
	std::vector<Shoot*> * getBulletList();
	void run(long dt);
	void setAimAtPosition(Vector2D position);
	Vector2D getAimAtPosition();
	void disableAllBullets();
};
