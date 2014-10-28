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
#include "BackgroundLayer.h"
class Game;
class Ship;
class HostileInstance;
class ParticleManager;
class SoundManager;
class Sprite;
class FrameBuffer;
class Shader;

enum RendererTextAlign
{
	ALIGNTOP,
	ALIGNBOTTOM,
	ALIGNCENTER,
	ALIGNLEFT,
	ALIGNRIGHT
};

typedef struct {
	int effectType;
	int duration;
	char str_param[30];
	float fl_param1;
	float fl_param2;
} T_EFFECT;


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
	void addEffect(T_EFFECT effect);
	void drawSprite(Shader * shader, Sprite * sprite, float x, float y, int orientation,unsigned int color = 0xffffffff,float scale = 1.0f);
	unsigned int getFrameCount() { return frameCounter; };
private :
	unsigned int frameCounter;
	FrameBuffer * fbDrawing;
	FrameBuffer * fbHalfRes1;
	FrameBuffer * fbHalfRes2;
	Sprite * spriteHalfRes1;
	Sprite * spriteHalfRes2;
	Sprite * spriteDrawing;
	FrameBuffer * fbAccumulation;
	Sprite * spriteAccumulation;
	Shader * shaderGaussianBlurHorizontal;
	Shader * shaderGaussianBlurVertical;
	Shader * shaderTexturing;
	Shader * shaderDistort;
	Sprite * spriteShip;
	Sprite * spriteDummy;
	Sprite * spriteBullet;
	Sprite * spriteGrid;
	Sprite * spriteCovering;
	Sprite * spriteTextSurface;
	Sprite * spriteRectangle;

	BackgroundManager backgroundManager;
	SDL_Window* displayWindow;
	SDL_GLContext contexteOpenGL;
	SDL_Surface * textSurface;

	TTF_Font *font; 
	Game * game;
	SoundManager * soundManager;
	int fps;
	ParticleManager * particleManager;
	bool bExit;
	void drawFps();
	void drawShip(Shader * shader,Ship * ship);
	void drawHostileInstance(Shader * shader,HostileInstance * hostile);
	void drawHostiles(Shader * shader);
	void drawHostileBullets(Shader * shader);
	void drawShipBullets(Shader * shader);
	void drawSquare(int x,int y, int width, int height,int color);
	
	void drawMessage(const char * message,RendererTextAlign hAlign,RendererTextAlign vAlign);
	void drawMessage(const char * message,float x,float y);
	void drawMessage(SDL_Surface * textSur,float x,float y);
	void blur_framebuffer();

	//Effect stack
	T_EFFECT effectList[10];
	void processEffect(T_EFFECT &effect);
};