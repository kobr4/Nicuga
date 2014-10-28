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
#include "Ship.h"
#include "Barrage.h"

Ship::Ship(void)
{
}


Ship::~Ship(void)
{
}


void Ship::onCollision(Shoot shot)
{
}

BoundingBox Ship::getBoundingBox()
{
	return this->boundingBox;
}

void Ship::setBoundingBox(BoundingBox box)
{
	this->boundingBox = box;
}

Vector2D Ship::getPosition()
{
	return this->position; 
}

#include <stdio.h>
void Ship::addToPosition(Vector2D position)
{
	this->position.set(this->position.getX()+position.getX(),this->position.getY()+position.getY());

	for (unsigned int i = 0;i < this->barrageList.size();i++)
	{
		this->barrageList[i]->setPosition(this->position.getX()+position.getX(),this->position.getY()+position.getY());
	}

	/*
	this->boundingBox.setBottomRight(this->boundingBox.getBottomRight().getX()+position.getX(),
		this->boundingBox.getBottomRight().getY()+position.getY());

	this->boundingBox.setTopLeft(this->boundingBox.getTopLeft().getX()+position.getX(),
		this->boundingBox.getTopLeft().getY()+position.getY());

	printf("SHIP bounding box (%f %f) (%f %f)\n",this->boundingBox.getBottomRight().getX(),this->boundingBox.getBottomRight().getY(),this->position.getX(),this->position.getY());
	*/
}

void Ship::addBarrage(Barrage * barrage)
{
	this->barrageDataList.push_back(barrage);
}

void Ship::addBarrageInstance(BarrageInstance * barrage)
{
	barrage->setPosition(this->position.getX(),this->position.getY());
	this->barrageList.push_back(barrage);
}

void Ship::runBarrage(long dt)
{
	for (unsigned int i = 0;i < this->barrageList.size();i++)
	{
		if (this->barrageList[i]->isActive())
		{			
			this->barrageList[i]->run(dt);
		}
	}
}

void Ship::setPosition(float x, float y)
{
	this->position.set(x,y);
}

bool Ship::isActive()
{
	return this->active;
}

void Ship::setActive(bool active)
{
	this->active = active;
}

int Ship::getInvicibleTimer()
{
	return this->invicibleTimer;
}
	
void Ship::setInvicibleTimer(int timer)
{
	this->invicibleTimer = timer;
}