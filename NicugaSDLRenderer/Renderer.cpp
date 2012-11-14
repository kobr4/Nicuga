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
#include "Renderer.h"
#include "SDL.h"
#undef main
#include <SDL_ttf.h>
#include "GameFactory.h"
#include "Game.h"
#include "Level.h"
#include "Ship.h"
#include "Hostile.h"
#include "HostileFactory.h"
#include "TextureGenerator.h"
#include "ParticleManagager.h"
#include "SoundManager.h"
#include "SDLRenderableFactory.h"

void onDestroyCallback(void * userdata, float x, float y);
int thread_func(void *data);
SDL_Surface* CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display);
void Renderer::init()
{
	if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Impossible d'initialiser SDL: %s\n", SDL_GetError());
		exit(1);
	}

    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	//screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE  | SDL_DOUBLEBUF);
    if ( screen == NULL ) {
        fprintf(stderr, "Impossible de passer en 640x480 en 16 bpp: %s\n", SDL_GetError());
        exit(1);
    }

	if (TTF_Init() < 0) {
		puts("ERROR : unable to initialize font library");
		exit(1);
	}

	//this->font = TTF_OpenFont( "INSPIRAT.ttf", 28 ); 
	this->font = TTF_OpenFont( "digital display tfb.ttf",28);
	if (this->font == NULL)
	{
		puts("ERROR : unable to load font");
		exit(1);
	}

	SDLRenderableFactory factory = SDLRenderableFactory(screen);

	this->game = GameFactory::loadGame("game.xml");
	this->game->loadCurrentLevel(&factory);
	bExit = false;


	soundManager = new SoundManager();
	soundManager->init();
	soundManager->loadMusic("A991Project.ogg");
	soundManager->playCurrentMusic();
	soundManager->loadExplosionSound("explosion-01.wav");

	this->bulletscreen = CreateSurface(this->screen->flags,this->screen->w,this->screen->h,this->screen);
	if (this->bulletscreen == NULL)
	{
		printf("ERROR : unable to create backing screen (%s)",SDL_GetError());
        exit(1);
	}

	particleManager = new ParticleManager();

    drawthread = SDL_CreateThread(thread_func, this);
    if ( drawthread == NULL ) {
        fprintf(stderr, "Unable to create draw thread: %s\n", SDL_GetError());
        exit(0);
    }

	this->game->setOnDestroyCallback(onDestroyCallback,this);
}

void blitFullScreen(SDL_Surface* src,SDL_Surface* dest)
{
	SDL_BlitSurface(src, NULL, dest, NULL);
}

bool Renderer::exitstate()
{
	return bExit;
}

void Renderer::drawSprite(SDL_Surface* dest, SDL_Surface* src, int x, int y)
{
   if ( SDL_MUSTLOCK(this->screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return;
        }
    }
	SDL_Rect  destR;

	destR.x = x;
	destR.y = y;

	SDL_BlitSurface(src, NULL, dest, &destR);

    if ( SDL_MUSTLOCK(this->screen) ) {
        SDL_UnlockSurface(screen);
    }
}

int thread_func(void *data)
{
	Renderer * renderer = (Renderer*)data;
    int last_value = 0;

    while ( renderer->exitstate() != true ) {
		renderer->draw();
    }
    printf("Thread quitting\n");
    return(0);
}


void Renderer::drawSquare(SDL_Surface * dest,int x,int y, int width, int height,int color)
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
	unsigned int rgbColor = color;
	SDL_FillRect(dest , &rect , SDL_MapRGB(this->screen->format , rgbColor , rgbColor , rgbColor ) );

    if ( SDL_MUSTLOCK(dest) ) {
        SDL_UnlockSurface(dest);
    }
}

void Renderer::drawFps()
{
	char s[256];
	sprintf(s,"FPS %.3d",this->getFps());
	drawMessage(s,ALIGNRIGHT,ALIGNTOP);
}


void Renderer::drawMessage(const char * message,RendererTextAlign hAlign,RendererTextAlign vAlign)
{
    if ( SDL_MUSTLOCK(this->screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            return;
        }
    }

	SDL_Color fontcolor = {255, 255, 255};
	//if (strlen(message) == 0)
	//	return;
	SDL_Surface * textSur = TTF_RenderText_Solid(this->font,message,fontcolor);	//set the text surface
	if (textSur == NULL)
	{
		
        fprintf(stderr, "Unable to create draw surface: %s\n", SDL_GetError());
		printf("%s\n",message);
        return;
	}
	SDL_Rect rect;
	switch (hAlign)
	{
	case ALIGNLEFT:
		rect.x = 0;
		break;
	case ALIGNRIGHT:
		rect.x = this->screen->w - textSur->w;
		break;
	case ALIGNCENTER:
		rect.x = (this->screen->w - textSur->w)/2;
		break;
	}

	switch (vAlign)
	{
	case ALIGNTOP:
		rect.y = 0;
		break;
	case ALIGNBOTTOM:
		rect.y = this->screen->h - textSur->h;
		break;
	case ALIGNCENTER:
		rect.y = (this->screen->h - textSur->h)/2;
		break;
	}

	SDL_BlitSurface(textSur,NULL,this->screen,&rect);		//blit text
	SDL_FreeSurface(textSur);

    if ( SDL_MUSTLOCK(this->screen) ) {
        SDL_UnlockSurface(screen);
    }
	
}

void drawstarfield(SDL_Surface * screen)
{
	static unsigned short * data =  (unsigned short*)calloc(5000 * 4,sizeof(unsigned short));
	const int maxPartPerFrame = 1;
	int counter = 0;
	for (int i = 0;i < 5000;i++)
	{
		int i4 = i*4;
		if (((data[i4]&0xff) == 0)&&(counter<maxPartPerFrame))
		{
			data[i4+1] = rand()%640;
			data[i4+2] = 0;
			data[i4+3] = rand();
			data[i4] = 1 | (rand() & 0xff00);
			counter++;
		}
		if ((data[i4]&0xff) == 1)
		//else
		{
			data[i4+2]+=2;
			if (data[i4+2] >= 1920)
			{
				data[i4] = data[i4] & 0xff00;
			}
			else
			{
				Uint8 * p = ((Uint8 *)screen->pixels) + (data[i4+2]>>2) * screen->pitch + data[i4+1] * screen->format->BytesPerPixel;
				*p++= data[i4+3] & 0x9f;
				//*p++= 0;
				*p++= (data[i4+3] & 0xff00) >> 8;
				*p++= data[i4] >> 8;
				*p= 0xff;
			}
		}
	}
}

void dither_surface(SDL_Surface * screen)
{
	unsigned int * pixels = (unsigned int*)screen->pixels;
	int totalpixelcount = screen->w*screen->h;
	
	for (int i = 0;i < totalpixelcount;i++)
	{
		int comp = 1;
		//int b = pixels[i] & 0xff;
		//int g = (pixels[i] & 0xff00) >> 8;
		//int r = (pixels[i] & 0xff0000) >> 16;

		int b = 0;
		int g = 0;
		int r = 0;

		if (i > screen->w)
		{
			b += pixels[i-screen->w] & 0xff;
		    g += (pixels[i-screen->w] & 0xff00) >> 8;
		    r += (pixels[i-screen->w] & 0xff0000) >> 16;
		}
		else
		{
			b += pixels[totalpixelcount-i-screen->w] & 0xff;
		    g += (pixels[totalpixelcount-i-screen->w] & 0xff00) >> 8;
		    r += (pixels[totalpixelcount-i-screen->w] & 0xff0000) >> 16;
		}

		if (i < totalpixelcount - screen->w)
		{
			b += pixels[i+screen->w] & 0xff;
		    g += (pixels[i+screen->w] & 0xff00) >> 8;
		    r += (pixels[i+screen->w] & 0xff0000) >> 16;
		}
		else
		{
			b += pixels[i+screen->w-totalpixelcount] & 0xff;
		    g += (pixels[i+screen->w-totalpixelcount] & 0xff00) >> 8;
		    r += (pixels[i+screen->w-totalpixelcount] & 0xff0000) >> 16;
		}

		if (i > 1)
		{
			b += pixels[i-1] & 0xff;
			g += (pixels[i-1] & 0xff00) >> 8;
			r += (pixels[i-1] & 0xff0000) >> 16;
		}
		else
		{
			b += pixels[totalpixelcount-1] & 0xff;
			g += (pixels[totalpixelcount-1] & 0xff00) >> 8;
			r += (pixels[totalpixelcount-1] & 0xff0000) >> 16;
		}

		if (i < totalpixelcount-1)
		{
			b += pixels[i+1] & 0xff;
			g += (pixels[i+1] & 0xff00) >> 8;
			r += (pixels[i+1] & 0xff0000) >> 16;
		}
		else
		{
			b += pixels[0] & 0xff;
			g += (pixels[0] & 0xff00) >> 8;
			r += (pixels[0] & 0xff0000) >> 16;
		}

		
		b = b >> 2;
		g = g >> 2;
		r = r >> 2;
		//printf("%d\n",b);
		pixels[i] = b | (g << 8) | (r << 16);
		//| (0xff<<24);
		//pixels[i] = b | (g << 8) | (r << 16) | (0xff<<24);
	}
}

SDL_Surface* CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display)
{
  // 'display' is the surface whose format you want to match
  //  if this is really the display format, then use the surface returned from SDL_SetVideoMode

  const SDL_PixelFormat& fmt = *(display->format);
  return SDL_CreateRGBSurface(flags,width,height,
                  fmt.BitsPerPixel,
                  fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask );
}


void Renderer::drawShip(Ship * ship)
{
	static SDL_Surface * surfaceTexture = NULL;

	if (surfaceTexture == NULL)
	{
		surfaceTexture = CreateSurface(this->screen->flags,20,20,this->screen);
		SDL_SetColorKey(surfaceTexture, SDL_SRCCOLORKEY , 0);
		//TextureGenerator::generateCircle((unsigned int*)surfaceTexture->pixels,20,0xff0000ff);
		//TextureGenerator::generateDualCircle((unsigned int*)surfaceTexture->pixels,20,15,0x0000ff00,0x000000ff);
		
		TextureGenerator::generateTriangle((unsigned int*)surfaceTexture->pixels,20,0x00ffffff);
		//TextureGenerator::generateDualTriangle((unsigned int*)surfaceTexture->pixels,20,15,0,0x00ffffff);
		//dither_surface(surfaceTexture);
		//TextureGenerator::generateDualTriangle((unsigned int*)surfaceTexture->pixels,20,15,0,0x00ffffff);
		//dither_surface(surfaceTexture);
		//dither_surface(surfaceTexture);
		//dither_surface(surfaceTexture);
		
	}

	Vector2D position = ship->getPosition();
	Vector2D topleft = ship->getBoundingBox().getTopLeft();
	Vector2D bottomRight = ship->getBoundingBox().getBottomRight();

	int x = (int)(position.getX() + bottomRight.getX());
	int y = (int)(position.getY() + bottomRight.getY());
	int w = (int)(topleft.getX() - bottomRight.getX());
	int h = (int)(topleft.getY() - bottomRight.getY());
	//printf("x=%d y=%d w=%d h=%d\n",x,y,w,h);
	
	//this->drawSquare(x,y,w,h);
	this->drawSprite(this->screen,surfaceTexture,x,y);
}

void Renderer::drawHostileInstance(HostileInstance * hostile)
{
	static SDL_Surface * surfaceTexture = NULL;

	if (surfaceTexture == NULL)
	{
		surfaceTexture = CreateSurface(this->screen->flags,20,20,this->screen);
		SDL_SetColorKey(surfaceTexture, SDL_SRCCOLORKEY , 0);
		//TextureGenerator::generateCircle((unsigned int*)surfaceTexture->pixels,20,0xff0000ff);
		TextureGenerator::generateDualCircle((unsigned int*)surfaceTexture->pixels,20,15,0x0000ff00,0x000000ff);
		//TextureGenerator::generateTriangle((unsigned int*)surfaceTexture->pixels,50,0x000000ff);
	}

	Vector2D position = hostile->getPosition();
	Vector2D topleft = hostile->getBoundingBox().getTopLeft();
	Vector2D bottomRight = hostile->getBoundingBox().getBottomRight();
	
	int x = (int)(position.getX() + bottomRight.getX());
	int y = (int)(position.getY() + bottomRight.getY());
	int w = (int)(topleft.getX() - bottomRight.getX());
	int h = (int)(topleft.getY() - bottomRight.getY());
	//printf("HOSTILE: pos(%f,%f) bottom(%f,%f) x=%d y=%d w=%d h=%d\n",position.getX(),position.getY(),bottomRight.getX(),bottomRight.getY(),x,y,w,h);
	//this->drawSquare(x,y,w,h);
	
	if (hostile->getRenderable() != NULL)
	{
		SDLRenderable * renderable = (SDLRenderable *)hostile->getRenderable();
		this->drawSprite(this->screen,renderable->surface,x,y);
	}
	else this->drawSprite(this->screen,surfaceTexture,x,y);
}

void Renderer::drawHostileBullets()
{
	std::vector<Shoot*> * bulletList;
	static SDL_Surface * surfaceTexture = NULL;

	if (surfaceTexture == NULL)
	{
		surfaceTexture = CreateSurface(this->screen->flags,6,6,this->screen);
		SDL_SetColorKey(surfaceTexture, SDL_SRCCOLORKEY , 0);
		TextureGenerator::generateDualCircle((unsigned int*)surfaceTexture->pixels,6,5,0xffff0000,0xffffff00);
	}

	bulletList = this->game->getCurrentLevel()->getBarrageManager()->getBulletList();
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			Vector2D pos = (*bulletList)[i]->getPosition();
			//this->drawSquare(this->bulletscreen,-3+(int)pos.getX(),-3+(int)pos.getY(),6,6);

			if ((*bulletList)[i]->getRenderable() != NULL)
			{
				SDLRenderable * renderable = (SDLRenderable *)(*bulletList)[i]->getRenderable();
				this->drawSprite(this->bulletscreen,renderable->surface,(int)pos.getX(),(int)pos.getY());
			}
			else
			this->drawSprite(this->bulletscreen,surfaceTexture,(int)pos.getX(),(int)pos.getY());
		}
	}


}

void Renderer::drawShipBullets()
{
	std::vector<Shoot*> * bulletList;
	bulletList = this->game->getCurrentLevel()->getFriendlyBarrageManager()->getBulletList();
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			Vector2D pos = (*bulletList)[i]->getPosition();
			this->drawSquare(this->screen,-3+(int)pos.getX(),-3+(int)pos.getY(),12,6, 0xffffff);
			//this->drawSprite(this->screen,surfaceTexture,pos.getX(),pos.getY());
		}
	}
}

void Renderer::drawHostiles()
{
	std::vector<HostileInstance*> * hostileVector = this->game->getCurrentLevel()->getHostileManager()->getHostileInstanceList();
	for (unsigned int i = 0;i < hostileVector->size();i++)
	{
		if ((*hostileVector)[i]->isActive())
		{
			drawHostileInstance((*hostileVector)[i]);
		}
	}
}


void Renderer::draw()
{
		drawstarfield(this->bulletscreen);

		this->particleManager->drawParticles(this->bulletscreen);
		this->drawHostileBullets();
		
		dither_surface(this->bulletscreen);
		blitFullScreen(this->bulletscreen,this->screen);
		this->drawShipBullets();
		
		if (this->game->getCurrentLevel()->getShip()->isActive())
			this->drawShip(this->game->getCurrentLevel()->getShip());
		this->drawHostiles();


		this->drawFps();

		char s_score[256];
		s_score[0];
		sprintf(s_score,"Score %d",this->game->getScore());
		//printf("Score %d\n",this->game->getScore());
		this->drawMessage(s_score,ALIGNLEFT,ALIGNTOP);

		s_score[0];
		sprintf(s_score,"Level %d",this->game->getCurrentLevelPosition()+1);
		this->drawMessage(s_score,ALIGNRIGHT,ALIGNBOTTOM);

		s_score[0];
		sprintf(s_score,"Remaining lives %d",this->game->getRemainingPlayerLife());
		this->drawMessage(s_score,ALIGNCENTER,ALIGNTOP);


		if (this->game->getCurrentLevel()->atEnd())
		{
			this->drawMessage("Congratulations level completed",ALIGNCENTER,ALIGNCENTER);
		}

		if (this->game->isOver())
		{
			this->drawMessage("Game Over",ALIGNCENTER,ALIGNCENTER);
		}


		SDL_Flip(screen);
}

void Renderer::loop()
{
	SDL_Event event;
	
	int told = SDL_GetTicks();

		int asyncup = 0;
		int asyncdown = 0;
		int asynleft = 0;
		int asynright = 0;
		int asyncspace = 0;

	SDL_FillRect(this->screen, NULL, 0);
	while (!bExit)
	{
		



		int tnew = SDL_GetTicks();
		int dt = tnew - told;
		told = tnew;

		if (dt != 0)
			this->fps = 1000/dt;

		this->game->run(dt);



		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym )
					{
						case SDLK_SPACE :
							asyncspace = 1;
							break;
						case SDLK_UP:
							asyncup = 1;
							break;
						case SDLK_DOWN:
							asyncdown = 1;
							break;
						case SDLK_LEFT:
							asynleft = 1;
							break;
						case SDLK_RIGHT:
							asynright = 1;
							break;
						case SDLK_ESCAPE:
							bExit = true;
							break;
						default:
							break;
					}
				break;
				case SDL_KEYUP:
					switch( event.key.keysym.sym )
					{
						case SDLK_SPACE :
							asyncspace = 0;
							break;
						case SDLK_UP:
							asyncup = 0;
							break;
						case SDLK_DOWN:
							asyncdown = 0;
							break;
						case SDLK_LEFT:
							asynleft = 0;
							break;
						case SDLK_RIGHT:
							asynright = 0;
							break;
						case SDLK_ESCAPE:
							bExit = true;
							break;
						default:
							break;
					}
				break;
			}
                    
		}

		if (asyncup)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(0,-2));
			
		}

		if (asyncdown)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(0,2));
		}

		if (asynleft)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(-2,0));
		}

		if (asynright)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(2,0));
		}

		if (asyncspace)
		{
			this->game->getCurrentLevel()->getShip()->runBarrage(dt);
		}

		/*
		if (dt < 16)
		{
			SDL_Delay(16-dt);
		}
		*/
		//this->particleManager->addParticleRandom(100,100,0xffffff);
		this->particleManager->run(dt);
		SDL_Delay(10);
	}

	SDL_WaitThread(this->drawthread, NULL);
}

int Renderer::getFps()
{
	return this->fps;
}


ParticleManager * Renderer::getParticleManager()
{
	return this->particleManager;
}

SoundManager * Renderer::getSoundManager()
{
	return this->soundManager;
}

void onDestroyCallback(void * userdata, float x, float y)
{
	Renderer * renderer =(Renderer *)userdata; 
	renderer->getSoundManager()->playExplosionSound();
	for (int i = 0;i < 50;i++)
	{
		renderer->getParticleManager()->addParticleRandom(x,y,0xff0000);
		
	}
}