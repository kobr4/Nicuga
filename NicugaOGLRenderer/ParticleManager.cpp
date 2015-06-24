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

#include "ParticleManagager.h"
#include "FastMath.h"
#include <stdio.h>
#include <stdlib.h>
#include "Sprite.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Vector2D.h"
#include "Renderer.h"

ParticleManager::ParticleManager() {
	for (int i = 0;i < 1000;i++)
	{
		this->particles[i].active = false;
		this->particles[i].initial_x = 0;
		this->particles[i].x = 0;
		this->particles[i].initial_y = 0;
		this->particles[i].y = 0;
		this->particles[i].lifetime = 0;
		this->particles[i].currentLifeTime = 0;
		this->particles[i].speed = 0;
		this->particles[i].color = 0;
		this->particles[i].direction = 0;
		this->particles[i].speed_x = 0;
		this->particles[i].speed_y = 0;
		this->particles[i].trailId = 0;
		this->particles[i].sprite = NULL;
	}
}

void ParticleManager::addParticleRandom(float x, float y,unsigned int color, Sprite * sprite)
{
	float speed = (float)(rand()%10) / 10.f + 0.1f;
	unsigned int direction = rand()%360;
	unsigned lifetime = rand()%2000;

	this->addParticle(x, y, speed,direction, color,lifetime,-1, sprite);
}

void drawSquare(SDL_Surface * dest,int x,int y, int width, int height,int color)
{
    if ( SDL_MUSTLOCK(dest) ) {
        if ( SDL_LockSurface(dest) < 0 ) {
            return;
        }
    }

	SDL_Rect rect;
	rect.h = width;
	rect.w = height;
	rect.x = x;
	rect.y = y;
	//unsigned int rgbColor = color;
	unsigned char r = color & 0xff0000 >> 16;
	unsigned char g = color & 0x00ff00 >> 8;
	unsigned char b = color & 0x0000ff;
	SDL_FillRect(dest , &rect , 0xff0000 );

    if ( SDL_MUSTLOCK(dest) ) {
        SDL_UnlockSurface(dest);
    }
}

void ParticleManager::addGravitySink(float x, float y, float mass,unsigned int lifetime) {
	for (int i = 0;i < 1000;i++)
	{
		if (this->gravitySink[i].active == false)
		{
			this->gravitySink[i].active = true;
			this->gravitySink[i].initial_x = x;
			this->gravitySink[i].x = x;
			this->gravitySink[i].initial_y = y;
			this->gravitySink[i].y = y;
			this->gravitySink[i].lifetime = lifetime;
			this->gravitySink[i].mass = mass;
			this->gravitySink[i].currentLifeTime = 0;
			return;
		}
	}
}

void ParticleManager::deactivateTrail(int trailId) {
	for (int i = 0;i < 1000;i++)
	{
		if ((this->particles[i].active == true)&&(this->particles[i].trailId == trailId)){
			this->particles[i].active = false;
		}
	}
}

void ParticleManager::addParticle(float x, float y, float speed,unsigned int direction,unsigned int color,unsigned int lifetime,int trailId, Sprite*sprite)
{
	for (int i = 0;i < 1000;i++)
	{
		if (this->particles[i].active == false)
		{
			this->particles[i].active = true;
			this->particles[i].initial_x = x;
			this->particles[i].x = x;
			this->particles[i].initial_y = y;
			this->particles[i].y = y;
			this->particles[i].lifetime = lifetime;
			this->particles[i].currentLifeTime = 0;
			this->particles[i].speed = speed;
			this->particles[i].color = color;
			this->particles[i].direction = direction;
			this->particles[i].speed_x = this->particles[i].speed * FastMath::fastCos(this->particles[i].direction);
			this->particles[i].speed_y = this->particles[i].speed * FastMath::fastSin(this->particles[i].direction);
			this->particles[i].trailId = trailId;
			this->particles[i].sprite = sprite;

			//printf("PARTICLE ADDED TRAILID=%d\n",trailId);
			return;
		}
	}
}

void ParticleManager::run(long dt)
{
	bool hasGravitySink = false;
	for (int i = 0;i < 10;i++) {
		if (this->gravitySink[i].active) {
			hasGravitySink = true;
			this->gravitySink[i].currentLifeTime += dt;
			if (this->gravitySink[i].currentLifeTime > this->gravitySink[i].lifetime) {
				this->gravitySink[i].active = false;
			}
		}
	}

	for (int i = 0;i < 1000;i++)
	{
		if (this->particles[i].active == true)
		{
			this->particles[i].currentLifeTime += dt;
			if (this->particles[i].currentLifeTime > this->particles[i].lifetime)
			{
				this->particles[i].active = false;
				continue;
			}
			
			float speedX = this->particles[i].speed_x;
			float speedY = this->particles[i].speed_y;
			//float speedX = this->particles[i].speed * FastMath::fastCos(this->particles[i].direction);
			//float speedY = this->particles[i].speed * FastMath::fastSin(this->particles[i].direction);
			if (hasGravitySink) {
				for (int j = 0;j < 10;j++) {
					if (this->gravitySink[j].active) {
						Vector2D vect(this->gravitySink[j].x,this->gravitySink[j].y);
						float h = vect.distance(this->particles[i].x,this->particles[i].y);
						speedX = speedX + ((this->gravitySink[j].x - this->particles[i].x) / h ) * this->gravitySink[j].mass;
						speedY = speedY + ((this->gravitySink[j].y - this->particles[i].y) / h ) * this->gravitySink[j].mass;
					}
				}

			}
			this->particles[i].speed_x = speedX;
			this->particles[i].speed_y = speedY;


			// Removed all calculation
			//this->particles[i].x = this->particles[i].initial_x + this->particles[i].speed * ((float)this->particles[i].currentLifeTime) * FastMath::fastCos(this->particles[i].direction);
			//this->particles[i].y = this->particles[i].initial_y + this->particles[i].speed * ((float)this->particles[i].currentLifeTime) * FastMath::fastSin(this->particles[i].direction);

			this->particles[i].x = this->particles[i].x + this->particles[i].speed_x * (float)dt;
			this->particles[i].y = this->particles[i].y + this->particles[i].speed_y * (float)dt;


			//printf("PARTICLE MOVE %d %d %d %d\n",(int)this->particles[i].x,(int)this->particles[i].y,this->particles[i].currentLifeTime,this->particles[i].direction);
		}
	}
}



void ParticleManager::drawParticles(Shader * shader,Sprite * sprite, Renderer * renderer)
{
	for (int i = 0;i < 1000;i++)
	{
		if (this->particles[i].active == true)
		{
			if (((int)this->particles[i].y < 0) ||(((int)this->particles[i].x < 0)))
			{
				this->particles[i].active = false;
				continue;
			}


			int head = -1;
			int trailId = this->particles[i].trailId;
			if (trailId != -1) {
				for (int j = 0;j < 1000;j++) {
					if ((this->particles[j].active == true) && (this->particles[j].trailId == trailId)&& (i!=j)) {
						if (this->particles[i].currentLifeTime > this->particles[j].currentLifeTime) {
							if (head == -1) {
								head = j;
							} else if ((this->particles[i].currentLifeTime - this->particles[j].currentLifeTime) < 
								(this->particles[i].currentLifeTime - this->particles[head].currentLifeTime)){
								head = j;
							}				
						}

					}
				}
			}

			/*
			if (((int)this->particles[i].y > surface->h) ||(((int)this->particles[i].x > surface->w)))
			{
				this->particles[i].active = false;
				continue;
			}
			*/
			//printf("PARTICLE DRAW %d %d %d %d\n",(int)this->particles[i].x,(int)this->particles[i].y,this->particles[i].currentLifeTime,this->particles[i].lifetime);
			/*
			Uint8 * p = ((Uint8 *)surface->pixels) + (int)this->particles[i].y * surface->pitch + (int)this->particles[i].x * surface->format->BytesPerPixel;
			*p++= this->particles[i].color & 0xff;
			*p++= (this->particles[i].color & 0xff00) >> 8;
			*p++= (this->particles[i].color & 0xff0000) >> 16;
			*p= 0xff;
			*/
			//drawSquare(surface,(int)this->particles[i].x-3,(int)this->particles[i].y-3, 6, 6,0xff000000);
			
			//glm::mat4 myMatrix = glm::translate(glm::mat4(), glm::vec3(this->particles[i].x-3, this->particles[i].y-3, 0.0f));
			//memcpy(shader->getModelViewMatrix(), glm::value_ptr(myMatrix),sizeof(float)*16);

			shader->bind_attributes();

			Sprite * renderingSprite = this->particles[i].sprite!=NULL ? this->particles[i].sprite : sprite; 

			if (head != -1) {
				float xdest = this->particles[i].x > this->particles[head].x ? -abs(this->particles[i].x - this->particles[head].x) : abs(this->particles[i].x - this->particles[head].x);
				float ydest = this->particles[i].y > this->particles[head].y ? -abs(this->particles[i].y - this->particles[head].y) : abs(this->particles[i].y - this->particles[head].y);
				
				//if (ydest > 20.f) {
				//	printf("BUG ! head=%d i lf=%d head lf=%d f=%f\n",head, this->particles[i].currentLifeTime, this->particles[head].currentLifeTime,this->particles[head].y);
				//}

				// No sprite rescale
				//renderingSprite->updateQuad(0,0,xdest,ydest,xdest+5.f,ydest+0.f,5.f,0.f);
			}
			float scale = 1.0f;
			
			// scale effect lifetime proportional.
			/*
			if (this->particles[i].trailId == -1) {
				scale = (this->particles[i].lifetime - this->particles[i].currentLifeTime) / 100.f;
			}
			*/
			int direction = this->particles[i].direction;
			renderer->drawSprite(shader,renderingSprite,this->particles[i].x-3,this->particles[i].y-3,direction,this->particles[i].color,scale);
			//renderingSprite->draw();

			if (head != -1) {
				//renderingSprite->updateQuad(0,0,5.f,5.f);
			}
		}
	}
}

