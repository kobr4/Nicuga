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

#include <SDL.h>
class Shader;
class Sprite;
class Renderer;
typedef struct
{
	float x;
	float y;
	float initial_x;
	float initial_y;
	float speed;
	float speed_x;
	float speed_y;
	float mass;
	unsigned int direction;
	unsigned int lifetime;
	unsigned int currentLifeTime;
	unsigned int color;
	bool active;
	int trailId;
	Sprite * sprite;
}tParticle;

class ParticleManager
{
public :
	void run(long dt);
	void addParticle(float x, float y, float speed, unsigned int direction,unsigned int color,unsigned int lifetime,int trailId=-1,Sprite*sprite=NULL);
	void addParticleRandom(float x, float y,unsigned int color,Sprite * sprite);
	void drawParticles(Shader * shader,Sprite * sprite, Renderer * renderer);
	void addGravitySink(float x, float y,float mass,unsigned int lifetime);
	void deactivateTrail(int trailId);
	ParticleManager();
private :
	tParticle gravitySink[10];
	tParticle particles[1000];
};