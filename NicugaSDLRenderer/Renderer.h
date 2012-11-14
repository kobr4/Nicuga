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
#include "SDL.h"
#undef main
#include <SDL_ttf.h>
class Game;
class Ship;
class HostileInstance;
class ParticleManager;
class SoundManager;
enum RendererTextAlign
{
	ALIGNTOP,
	ALIGNBOTTOM,
	ALIGNCENTER,
	ALIGNLEFT,
	ALIGNRIGHT
};

class Renderer
{
public :
	void init();

	void loop();
	void draw();
	bool exitstate();
	int getFps();
	ParticleManager * getParticleManager();
	SoundManager * getSoundManager();
private :
	SDL_Surface *screen;
	SDL_Surface *bulletscreen;
	SDL_Thread *drawthread;
	TTF_Font *font; 
	Game * game;
	SoundManager * soundManager;
	int fps;
	ParticleManager * particleManager;
	bool bExit;
	void drawFps();
	void drawShip(Ship * ship);
	void drawHostileInstance(HostileInstance * hostile);
	void drawHostiles();
	void drawHostileBullets();
	void drawShipBullets();
	void drawSquare(SDL_Surface * dest,int x,int y, int width, int height,int color);
	void drawSprite(SDL_Surface* dest, SDL_Surface* src, int x, int y);
	void drawMessage(const char * message,RendererTextAlign hAlign,RendererTextAlign vAlign);
	

};