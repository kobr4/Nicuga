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


void ParticleManager::addParticleRandom(float x, float y,unsigned int color)
{
	float speed = (float)(rand()%10) / 10.f;
	unsigned int direction = rand()%360;
	unsigned lifetime = rand()%1000;

	this->addParticle(x, y, speed,direction, color,lifetime);
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

void ParticleManager::addParticle(float x, float y, float speed,unsigned int direction,unsigned int color,unsigned int lifetime)
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
			//puts("PARTICLE ADDED");
			return;
		}
	}
}

void ParticleManager::run(long dt)
{
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
			
			this->particles[i].x = this->particles[i].initial_x + this->particles[i].speed * ((float)this->particles[i].currentLifeTime) * FastMath::fastCos(this->particles[i].direction);
			this->particles[i].y = this->particles[i].initial_y + this->particles[i].speed * ((float)this->particles[i].currentLifeTime) * FastMath::fastSin(this->particles[i].direction);

			//printf("PARTICLE MOVE %d %d %d %d\n",(int)this->particles[i].x,(int)this->particles[i].y,this->particles[i].currentLifeTime,this->particles[i].direction);
		}
	}
}



void ParticleManager::drawParticles(SDL_Surface * surface)
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

			if (((int)this->particles[i].y > surface->h) ||(((int)this->particles[i].x > surface->w)))
			{
				this->particles[i].active = false;
				continue;
			}

			//printf("PARTICLE DRAW %d %d %d %d\n",(int)this->particles[i].x,(int)this->particles[i].y,this->particles[i].currentLifeTime,this->particles[i].lifetime);
			/*
			Uint8 * p = ((Uint8 *)surface->pixels) + (int)this->particles[i].y * surface->pitch + (int)this->particles[i].x * surface->format->BytesPerPixel;
			*p++= this->particles[i].color & 0xff;
			*p++= (this->particles[i].color & 0xff00) >> 8;
			*p++= (this->particles[i].color & 0xff0000) >> 16;
			*p= 0xff;
			*/
			drawSquare(surface,(int)this->particles[i].x-3,(int)this->particles[i].y-3, 6, 6,0xff000000);
		}
	}
}

