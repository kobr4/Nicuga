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

#include "Game.h"
#include "Level.h"
#include "FastMath.h"
#include "Hostile.h"
#include "HostileFactory.h"
#include "RenderableAbstractFactory.h"

Game::Game(void)
{
	FastMath::init();
	currentLevelPosition = 0;
	score = 0;
	onDestroyCallback = NULL;
	remainingPlayerLife = 0;
	
}


Game::~Game(void)
{
}

int Game::getScore()
{
	return score;
}

void Game::loadCurrentLevel(RenderableAbstractFactory * renderableFactory)
{
	std::string s = this->levelList[this->currentLevelPosition];
	this->loadLevel((char*)s.c_str(), renderableFactory);
}

void Game::loadLevel(char * filename,RenderableAbstractFactory * renderableFactory)
{
	this->currentLevel = new Level();
	this->currentLevel->load(filename, renderableFactory);
}

void Game::run(long dt)
{
	//runHostile(dt);
	//checkHostileCollision();
	//checkShipCollision();
	//onGameUpdate();
	
	if ((this->currentLevel != NULL)&&(this->currentLevel->getShip()->isActive() == true))
	{
		int ivt = this->currentLevel->getShip()->getInvicibleTimer();
		if (ivt > 0)
			this->currentLevel->getShip()->setInvicibleTimer(ivt - dt);

		this->currentLevel->run(dt);
		checkHostileCollision();
		checkShipCollision();
	}
}

void Game::runHostile(long dt)
{
	/*
	for (int i = 0;i < GAMEMAXHOSTILE;i++)
	{
		if (arrayHostile[i].isActive())
		{
			arrayHostile[i].run(dt);	
		}
	}
	*/
}



//For each friendly shoot check is within hostile boundaries
void Game::checkHostileCollision()
{
	std::vector<Shoot*> * bulletList;
	bulletList = this->currentLevel->getFriendlyBarrageManager()->getBulletList();
	std::vector<HostileInstance * > * hostileInstanceList = this->currentLevel->getHostileManager()->getHostileInstanceList();
	//Check friendly bullet collision
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			for (unsigned int j = 0;j < hostileInstanceList->size();j++)
			{
				if ((*hostileInstanceList)[j]->isActive())
				{
					BoundingBox hotileBoundingBox = BoundingBox(
						(*hostileInstanceList)[j]->getBoundingBox().getTopLeft().getX() + (*hostileInstanceList)[j]->getPosition().getX(),
						(*hostileInstanceList)[j]->getBoundingBox().getTopLeft().getY() + (*hostileInstanceList)[j]->getPosition().getY(),
						(*hostileInstanceList)[j]->getBoundingBox().getBottomRight().getX() + (*hostileInstanceList)[j]->getPosition().getX(),
						(*hostileInstanceList)[j]->getBoundingBox().getBottomRight().getY() + (*hostileInstanceList)[j]->getPosition().getY()
						);
					if (hotileBoundingBox.isBound((*bulletList)[i]->getPosition()))
					{
						(*bulletList)[i]->setActive(false);
						int life = (*hostileInstanceList)[j]->getLife();
						life = life - (*bulletList)[i]->getDamage();
						(*hostileInstanceList)[j]->setLife(life);


						//printf("Hostile hit remaining life: %d\n",life);
						if (life < 0)
						{
							if (this->onDestroyCallback != NULL)
							{
								this->onDestroyCallback(this->onDestroyCallbackUserData,(*hostileInstanceList)[j]->getPosition().getX(),(*hostileInstanceList)[j]->getPosition().getY());
							}

							this->score += (*hostileInstanceList)[j]->getScore();
							(*hostileInstanceList)[j]->deactive();
						}
					}
				}
			}
		}
	}
}

//
void Game::checkShipCollision()
{
	if (this->currentLevel->getShip()->getInvicibleTimer() <= 0)
	{
		//Check hostile bullet collision
		BoundingBox shipBoundingBox = BoundingBox(
			this->currentLevel->getShip()->getBoundingBox().getTopLeft().getX() + this->currentLevel->getShip()->getPosition().getX(),
			this->currentLevel->getShip()->getBoundingBox().getTopLeft().getY() + this->currentLevel->getShip()->getPosition().getY(),
			this->currentLevel->getShip()->getBoundingBox().getBottomRight().getX() + this->currentLevel->getShip()->getPosition().getX(),
			this->currentLevel->getShip()->getBoundingBox().getBottomRight().getY() + this->currentLevel->getShip()->getPosition().getY()
			);

		std::vector<Shoot*> * bulletList;
		bulletList = this->currentLevel->getBarrageManager()->getBulletList();
		for (unsigned int i = 0;i < bulletList->size();i++)
		{
			if ((*bulletList)[i]->isActive())
			{
				if (shipBoundingBox.isBound((*bulletList)[i]->getPosition()))
				{
					//printf("GAME OVER !\n");
					//exit(0);
						if (this->onDestroyCallback != NULL)
						{
							this->onDestroyCallback(this->onDestroyCallbackUserData,this->currentLevel->getShip()->getPosition().getX(),this->currentLevel->getShip()->getPosition().getY());
						}

					this->currentLevel->getShip()->setActive(false);
					this->respawnShip();
				}
			}
		}
	}
}

void Game::onGameUpdate()
{
	/*
	//Hostile out of screen/game area are desactivated 
	for (int i = 0;i < GAMEMAXHOSTILE;i++)
	{
		if (arrayHostile[i].isActive())
		{
			if (!this->screenBoundaries.isBound(arrayHostile[i].getPosition()))
			{
				arrayHostile[i].setActive(false);
			}
		}
	}

	//Hostile shots of screen/game area are desactivated
	for (int i = 0;i < GAMEMAXHOSTILESHOOT;i++)
	{
		if (arrayHostileShoot[i].isActive())
		{
			if (!this->screenBoundaries.isBound(arrayHostileShoot[i].getPosition()))
			{
				arrayHostileShoot[i].setActive(false);
			}
		}
	}

	//Friendly shots of screen/game area are desactivated
	for (int i = 0;i < GAMEMAXFRIENDLYSHOOT;i++)
	{
		if (arrayFriendlyShoot[i].isActive())
		{
			if (!this->screenBoundaries.isBound(arrayFriendlyShoot[i].getPosition()))
			{
				arrayFriendlyShoot[i].setActive(false);
			}
		}
	}
	*/
}

void Game::addLevel(const char * file)
{
	this->levelList.push_back(std::string(file));
}

Level * Game::getCurrentLevel()
{
	return this->currentLevel;
}

int Game::getCurrentLevelPosition()
{
	return this->currentLevelPosition;
}

void Game::setOnDestroyCallback(void (*onDestroyCallback)(void * userdata, float x, float y),void * userdata)
{
	this->onDestroyCallback = onDestroyCallback;
	this->onDestroyCallbackUserData = userdata;
}

int Game::getRemainingPlayerLife()
{
	return this->remainingPlayerLife;
}

bool Game::isOver()
{
	if ((this->currentLevel->getShip()->isActive() == false)&&(this->remainingPlayerLife == 0))
	{
		return true;
	}

	return false;
}

void Game::respawnShip()
{
	
	if (this->remainingPlayerLife > 0)
	{
		this->currentLevel->getShip()->setActive(true);
		this->currentLevel->getShip()->setInvicibleTimer(3000);
		this->remainingPlayerLife --;
		this->currentLevel->respawnShip();
	}
}

void Game::setPlayerLives(int lives)
{
	this->remainingPlayerLife = lives;
}