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
#include "OGLRenderableFactory.h"
#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include "FrameBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include "BackgroundLayer.h"
#include "FastMath.h"

unsigned char g_texdata[]= { 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255};

void onDestroyCallback(void * userdata, unsigned int bulletId,Ship * ship, HostileInstance * hostile, float x, float y);
void onHitCallback(void * userdata, unsigned int bulletId,Ship * ship, HostileInstance * hostile, float x, float y);

int thread_func(void *data);
SDL_Surface* CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display);


int Renderer::screenHeight = 640;
int Renderer::screenWidth = 480;
bool Renderer::autofire = true;
float Renderer::tsensitivity = 1000.0f;

void Renderer::init()
{
	for (int i = 0;i < 10;i++) {
		effectList[i].duration = 0;
	}

	if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Impossible d'initialiser SDL: %s\n", SDL_GetError());
		exit(1);
	}

    //atexit(SDL_Quit);

     SDL_Init(SDL_INIT_VIDEO);

    // Version d'OpenGL
      
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
      
      
    // Double Buffer
      
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


    displayWindow = SDL_CreateWindow("Test SDL 2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Renderer::screenWidth, Renderer::screenHeight, SDL_WINDOW_OPENGL);
    
    // Création du contexte OpenGL
  
    contexteOpenGL = SDL_GL_CreateContext(displayWindow);
  
    if(contexteOpenGL == 0)
    {
        std::cout << SDL_GetError() << std::endl; //// >> AFFICHE : " the specified window isn't an OpenGL window"
        SDL_DestroyWindow(displayWindow);
        SDL_Quit();
  
        exit(-1); //// >> PLANTE ICI : return -1 ..
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


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    unsigned int rmask = 0xff000000;
    unsigned int gmask = 0x00ff0000;
    unsigned int bmask = 0x0000ff00;
    unsigned int amask = 0x000000ff;
#else
    unsigned int rmask = 0x000000ff;
    unsigned int gmask = 0x0000ff00;
    unsigned int bmask = 0x00ff0000;
    unsigned int amask = 0xff000000;
#endif
	this->textSurface = SDL_CreateRGBSurface( 0, Renderer::screenWidth, Renderer::screenHeight, 32, rmask, gmask, bmask, amask);

	OGLRenderableFactory factory = OGLRenderableFactory();

	this->game = GameFactory::loadGame("game.xml");
	this->game->loadCurrentLevel(&factory);
	bExit = false;

	
	soundManager = new SoundManager();
	soundManager->init();
	
	//soundManager->loadMusic("A991Project.ogg");
	//soundManager->playCurrentMusic();
	
	//soundManager->loadExplosionSound("Grenade-SoundBible.com-1777900486.wav");
	


	particleManager = new ParticleManager();
	/*
    drawthread = SDL_CreateThread(thread_func, this);
    if ( drawthread == NULL ) {
        fprintf(stderr, "Unable to create draw thread: %s\n", SDL_GetError());
        exit(0);
    }
	*/
	this->game->setOnDestroyCallback(onDestroyCallback,this);
	this->game->setOnHitCallback(onHitCallback,this);

	this->fbAccumulation = NULL;
	this->fbDrawing = NULL;
	this->fbHalfRes1 = NULL;
	this->fbHalfRes2 = NULL;

	shaderTexturing = new Shader();
	shaderTexturing->load_fragment("fragment_texturing.gl");
	shaderTexturing->load_vertex("test.gl");

	shaderDistort = new Shader();
	shaderDistort->load_fragment("fragment_distort.gl");
	shaderDistort->load_vertex("test.gl");

	shaderGaussianBlurVertical = new Shader();
	shaderGaussianBlurVertical->load_fragment("fragment_gaussian_vertical.gl");
	shaderGaussianBlurVertical->load_vertex("test02.gl");

	shaderGaussianBlurHorizontal = new Shader();
	shaderGaussianBlurHorizontal->load_fragment("fragment_gaussian_horizontal.gl");
	shaderGaussianBlurHorizontal->load_vertex("test02.gl");


	Texture * texture = new Texture(2,2,(unsigned char*)g_texdata);

	unsigned int * pixels = (unsigned int *)malloc(sizeof(unsigned int)*30*30);
	TextureGenerator::generateTriangle(10,4,pixels,30,0xffffffff);

	this->spriteAccumulation = new Sprite(texture,100.f,100.f,0,0,1,1);
	this->spriteBullet = new Sprite(texture,5.f,5.f,1,1,0,0);
	this->spriteDrawing = new Sprite(texture,100.f,100.f,0,0,1,1);
	this->spriteDummy = new Sprite(texture,100.f,100.f,0,0,1,1);
	this->spriteCovering = new Sprite(texture,Renderer::screenWidth,Renderer::screenHeight,0,0,1,1);
	this->spriteRectangle = new Sprite(texture,10.f,1.f,0,0,1,1);


	unsigned int * pixels2 = (unsigned int *)malloc(sizeof(unsigned int)*30*30);
	TextureGenerator::generateCircle(0,0,pixels2,30,0xffffffff);
	this->spriteCircle = new Sprite(new Texture(30,30,(unsigned char*)pixels2),10.f,10.f,1,1,0,0);
	//this->spriteCircle = this->spriteBullet;

	Texture * textSurfaceTexture = new Texture(Renderer::screenWidth,Renderer::screenHeight,(unsigned char*)this->textSurface->pixels);
	this->spriteTextSurface = new Sprite(textSurfaceTexture,Renderer::screenWidth,Renderer::screenHeight,0,1,1,0);

	this->spriteShip = new Sprite(new Texture(30,30,(unsigned char*)pixels),30.f,30.f,1,1,0,0);

	unsigned int * pixelsgrid = (unsigned int*)malloc(sizeof(unsigned int)* 100 * 100);
	TextureGenerator::generateGrid(pixelsgrid,100,100,20,0xff0909aa);
	this->spriteGrid = new Sprite(new Texture(100,100,(unsigned char*)pixelsgrid),100.f,100.f,0,0,1,1);
	BackgroundLayer * backgroundLayer = new BackgroundLayer(Renderer::screenWidth,Renderer::screenHeight);
	backgroundLayer->addElement(this->spriteGrid,100,100);
	this->backgroundManager.addLayer(backgroundLayer);
	
	unsigned int * pixelsStarfield = (unsigned int*)calloc(sizeof(unsigned int), 100 * 100);
	TextureGenerator::generateStarfield(pixelsStarfield,100,100,10);
	backgroundLayer = new BackgroundLayer(Renderer::screenWidth,Renderer::screenHeight,0.0,-1.5f);
	backgroundLayer->addElement(new Sprite(new Texture(100,100,(unsigned char*)pixelsStarfield),100.f,100.f,0,0,1,1),100,100);
	this->backgroundManager.addLayer(backgroundLayer);

	unsigned int * pixelsStarfield2 = (unsigned int*)calloc(sizeof(unsigned int), 100 * 100);
	TextureGenerator::generateStarfield(pixelsStarfield2,100,100,10);
	backgroundLayer = new BackgroundLayer(Renderer::screenWidth,Renderer::screenHeight,0.0,-1.0f);
	backgroundLayer->addElement(new Sprite(new Texture(100,100,(unsigned char*)pixelsStarfield2),100.f,100.f,0,0,1,1),100,100);
	this->backgroundManager.addLayer(backgroundLayer);
	
}


bool Renderer::exitstate()
{
	return bExit;
}

void Renderer::drawSprite(Shader * shader, Sprite * sprite, float x, float y, int orientation, unsigned int color, float scale)
{
	glm::mat4 Model  = glm::mat4();

	glm::mat4 myMatrix = glm::translate(Model, glm::vec3(x, y, 0.0f));

	if (orientation != 0) {
		myMatrix = glm::translate(myMatrix, glm::vec3(sprite->getWidth()/2.0f, sprite->getHeight()/2.0f, 0.0f));
		myMatrix = glm::rotate(myMatrix, (float)orientation, glm::vec3(0, 0, 1.f)); 
		myMatrix = glm::translate(myMatrix, glm::vec3(-sprite->getWidth()/2.0f,-sprite->getHeight()/2.0f, 0.0f));
		
	} 

	if (scale != 1.0f) {
		myMatrix = glm::scale(myMatrix,glm::vec3(scale));
	}

	memcpy(shader->getModelViewMatrix(), glm::value_ptr(myMatrix),sizeof(float)*16);

	if (color != 0xffffffff) {
		float r = (float)((color & 0xff0000) >> 16) / 256.0;
		float g = (float)((color & 0xff00) >> 8) / 256.0;
		float b = (float)((color & 0x0000ff)) / 256.0;
		shader->getColorVector()[0] = r;
		shader->getColorVector()[1] = g;
		shader->getColorVector()[2] = b;
		shader->getColorVector()[3] = 1.0;
	} else {
	shader->getColorVector()[0] = 1.0;
	shader->getColorVector()[1] = 1.0;
	shader->getColorVector()[2] = 1.0;
	shader->getColorVector()[3] = 1.0;
	}
	shader->bind_attributes();
	sprite->draw();

	shader->getColorVector()[0] = 1.0;
	shader->getColorVector()[1] = 1.0;
	shader->getColorVector()[2] = 1.0;
	shader->getColorVector()[3] = 1.0;
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


void Renderer::drawFps()
{
	char s[256];
	sprintf(s,"FPS %.3d",this->getFps());
	drawMessage(s,ALIGNRIGHT,ALIGNTOP);
}

void Renderer::drawMessage(const char * message,float x,float y) {
	if (strlen(message) == 0)
		return;

	SDL_Color fontcolor = {255, 255, 255};

	SDL_Surface * textSur = TTF_RenderText_Solid(this->font,message,fontcolor);	//set the text surface
	if (textSur == NULL)
	{
		
        fprintf(stderr, "Unable to create draw surface: %s\n", SDL_GetError());
		printf("%s\n",message);
        return;
	}

	drawMessage(textSur,x,y);
	SDL_FreeSurface(textSur);	
}

void Renderer::drawMessage(SDL_Surface * textSur,float x,float y) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_BlitSurface(textSur,NULL,this->textSurface,&rect);
}

void Renderer::drawMessage(const char * message,RendererTextAlign hAlign,RendererTextAlign vAlign)
{
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
		rect.x = this->textSurface->w - textSur->w;
		break;
	case ALIGNCENTER:
		rect.x = (this->textSurface->w - textSur->w)/2;
		break;
	}

	switch (vAlign)
	{
	case ALIGNTOP:
		rect.y = 0;
		break;
	case ALIGNBOTTOM:
		rect.y = this->textSurface->h - textSur->h;
		break;
	case ALIGNCENTER:
		rect.y = (this->textSurface->h - textSur->h)/2;
		break;
	}

	drawMessage(textSur,rect.x,rect.y);
	SDL_FreeSurface(textSur);	
}


void Renderer::drawShip(Shader * shader, Ship * ship)
{
	Vector2D position = ship->getPosition();
	Vector2D topleft = ship->getBoundingBox().getTopLeft();
	Vector2D bottomRight = ship->getBoundingBox().getBottomRight();

	int x = (int)(position.getX() + bottomRight.getX());
	int y = (int)(position.getY() + bottomRight.getY());
	int w = (int)(topleft.getX() - bottomRight.getX());
	int h = (int)(topleft.getY() - bottomRight.getY());

	static int orientation = 0;
	//orientation++;
	if (ship->getRenderable() == NULL) {
		this->drawSprite(shader, this->spriteShip, x, y, orientation);
	}
	else {
		OGLRenderable * renderable = (OGLRenderable *)ship->getRenderable();
		this->drawSprite(shader, renderable->sprite, x, y, orientation);
	}
}

void Renderer::drawHostileInstance(Shader * shader, HostileInstance * hostile)
{

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
		OGLRenderable * renderable = (OGLRenderable *)hostile->getRenderable();
		unsigned int colorcomp = 100 + (unsigned int)(155.f*( (FastMath::fastCos(getFrameCount()*2)+1.f) / 2.0f));
		unsigned int color = 0xff000000 | colorcomp << 16 | colorcomp << 8 | colorcomp;
		unsigned int orientation = 0;
		if (renderable->animationType == 1) {
			orientation = (this->getFrameCount()%360 )* 2;
			//printf("orientation=%d\n",orientation);
		}

		if (renderable->blink_hint-- > 0) {
			shader->getColorOverrideVector()[0] = 1.0f;
			shader->getColorOverrideVector()[1] = 1.0f;
			shader->getColorOverrideVector()[2] = 1.0f;
			shader->getColorOverrideVector()[3] = 1.0f;
			this->drawSprite(shader,renderable->sprite,x,y,orientation,0x00000000);
			shader->getColorOverrideVector()[0] = 0.0f;
			shader->getColorOverrideVector()[1] = 0.0f;
			shader->getColorOverrideVector()[2] = 0.0f;
			shader->getColorOverrideVector()[3] = 0.0f;
		} else {
			this->drawSprite(shader,renderable->sprite,x,y,orientation,color);
		}
	}
	else this->drawSprite(shader,this->spriteDummy,x,y,0);
}

void Renderer::drawHostileBullets(Shader * shader)
{
	std::vector<Shoot*> * bulletList;

	bulletList = this->game->getCurrentLevel()->getBarrageManager()->getBulletList();
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			Vector2D pos = (*bulletList)[i]->getPosition();
			//this->drawSquare(this->bulletscreen,-3+(int)pos.getX(),-3+(int)pos.getY(),6,6);

			if ((*bulletList)[i]->getRenderable() != NULL)
			{
				OGLRenderable * renderable = (OGLRenderable *)(*bulletList)[i]->getRenderable();
				switch (renderable->animationType) {
				case 1 : 
					//this->particleManager->addParticle(pos.getX(), pos.getY(), 0.0f,(*bulletList)[i]->getDirection(), 0xff111111, 500,1000+i);
					break;
				}
				this->drawSprite(shader,renderable->sprite,(int)pos.getX(),(int)pos.getY(),0);
			}
			else
			this->drawSprite(shader,this->spriteBullet,(int)pos.getX(),(int)pos.getY(),0);
		}
	}


}

void Renderer::drawShipBullets(Shader * shader)
{
	std::vector<Shoot*> * bulletList;
	bulletList = this->game->getCurrentLevel()->getFriendlyBarrageManager()->getBulletList();
	for (unsigned int i = 0;i < bulletList->size();i++)
	{
		if ((*bulletList)[i]->isActive())
		{
			Vector2D pos = (*bulletList)[i]->getPosition();
			//this->drawSquare(this->screen,-3+(int)pos.getX(),-3+(int)pos.getY(),12,6, 0xffffff);
			//this->drawSprite(shader,this->spriteBullet,pos.getX(),pos.getY(),0);

			//this->particleManager->addParticle(pos.getX(), pos.getY(), 0.0f,(*bulletList)[i]->getDirection(), 0xff111111, 500,1000+i);
			if ((*bulletList)[i]->getRenderable() != NULL)
			{
				OGLRenderable * renderable = (OGLRenderable *)(*bulletList)[i]->getRenderable();
				switch (renderable->animationType) {
				//trail
				case 1 : 
					this->particleManager->addParticle(pos.getX(), pos.getY(), 0.0f,0, 0xffaa3333, renderable->iparam1,1000+i,this->spriteCircle,1.0f,-0.04f);
					break;
				}
				this->drawSprite(shader,renderable->sprite,(int)pos.getX(),(int)pos.getY(),0);
			}
			else
			this->drawSprite(shader,this->spriteBullet,(int)pos.getX(),(int)pos.getY(),0);
		}
	}
}

void Renderer::drawHostiles(Shader * shader)
{
	std::vector<HostileInstance*> * hostileVector = this->game->getCurrentLevel()->getHostileManager()->getHostileInstanceList();
	for (unsigned int i = 0;i < hostileVector->size();i++)
	{
		if ((*hostileVector)[i]->isActive())
		{
			drawHostileInstance(shader,(*hostileVector)[i]);
		}
	}
}

void Renderer::blur_framebuffer() {
	this->shaderTexturing->unbind();
	/*
	this->fbHalfRes1->bind();
	memcpy(this->shaderGaussianBlurHorizontal->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderGaussianBlurHorizontal->bind();
	this->spriteAccumulation->draw();
	this->shaderGaussianBlurHorizontal->unbind();
	this->fbHalfRes1->unbind(640,480);
	
	this->fbAccumulation->bind();
	//memcpy(this->shaderGaussianBlurVertical->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	//this->shaderGaussianBlurVertical->bind();
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind();
	this->spriteHalfRes1->draw();
	this->shaderTexturing->unbind();
	this->fbAccumulation->unbind(640,480);
	
	this->fbHalfRes1->bind();
	memcpy(this->shaderGaussianBlurVertical->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderGaussianBlurVertical->bind();
	this->spriteAccumulation->draw();
	this->shaderGaussianBlurVertical->unbind();
	this->fbHalfRes1->unbind(640,480);
	
	this->fbAccumulation->bind();
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind();
	this->spriteHalfRes1->draw();
	this->shaderTexturing->unbind();
	this->fbAccumulation->unbind(640,480);
	*/
	this->fbHalfRes1->bind();
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind();
	this->spriteAccumulation->draw();
	this->shaderTexturing->unbind();
	this->fbHalfRes1->unbind(Renderer::screenWidth,Renderer::screenHeight);
	
	this->fbHalfRes2->bind();
	memcpy(this->shaderGaussianBlurHorizontal->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderGaussianBlurHorizontal->bind();
	this->spriteHalfRes1->draw();
	this->shaderGaussianBlurHorizontal->unbind();
	this->fbHalfRes2->unbind(Renderer::screenWidth,Renderer::screenHeight);
	
	this->fbHalfRes1->bind();
	memcpy(this->shaderGaussianBlurVertical->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderGaussianBlurVertical->bind();
	this->spriteHalfRes2->draw();
	this->shaderGaussianBlurVertical->unbind();
	this->fbHalfRes1->unbind(Renderer::screenWidth,Renderer::screenHeight);
	
	this->fbAccumulation->bind();
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind();
	this->spriteHalfRes1->draw();
	this->shaderTexturing->unbind();
	this->fbAccumulation->unbind(Renderer::screenWidth,Renderer::screenHeight);

	this->shaderTexturing->bind();
}


void four1(double data[], int nn, int isign);

void Renderer::draw()
{
	SDL_FillRect(this->textSurface, NULL, 0x000000);

	if (this->fbDrawing == NULL){ 
		this->fbDrawing = new FrameBuffer(Renderer::screenWidth,Renderer::screenHeight);
		this->fbDrawing->do_register();
		this->spriteDrawing = new Sprite(this->fbDrawing->getTexture(),Renderer::screenWidth,Renderer::screenHeight,0,0,1,1);		
	}
	
	if (this->fbAccumulation == NULL) {
		this->fbAccumulation= new FrameBuffer(Renderer::screenWidth,Renderer::screenHeight);
		this->fbAccumulation->do_register();
		this->spriteAccumulation = new Sprite(this->fbAccumulation->getTexture(),Renderer::screenWidth,Renderer::screenHeight,0,0,1,1);
	}
	
	if (this->fbHalfRes1 == NULL) {
		this->fbHalfRes1= new FrameBuffer(Renderer::screenWidth/2,Renderer::screenHeight/2);
		this->fbHalfRes1->do_register();
		this->spriteHalfRes1 = new Sprite(this->fbHalfRes1->getTexture(),Renderer::screenWidth,Renderer::screenHeight,0,0,1,1);
	}

	if (this->fbHalfRes2 == NULL) {
		this->fbHalfRes2= new FrameBuffer(Renderer::screenWidth/2,Renderer::screenHeight/2);
		this->fbHalfRes2->do_register();
		this->spriteHalfRes2 = new Sprite(this->fbHalfRes2->getTexture(),Renderer::screenWidth,Renderer::screenHeight,0,0,1,1);
	}

	//OpenGL setup
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT); 

	GLfloat matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

	//int screenHeight = 480;
	glViewport((GLint) (matrix[12]),
				(GLint) (-matrix[13]),
				(GLsizei) Renderer::screenWidth,
				(GLsizei) Renderer::screenHeight);
	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(matrix[12], matrix[12] + 640, matrix[13] + 480,matrix[13], -1, 1);
	*/
	glMatrixMode(GL_MODELVIEW);

	//
	glDisable(GL_DEPTH_TEST);

	glm::mat4 projMat = glm::ortho( 0.f, (float)Renderer::screenWidth,(float)Renderer::screenHeight, 0.f, -1.f, 1.f );
	memcpy(this->shaderGaussianBlurHorizontal->getProjectionMatrix(), glm::value_ptr(projMat),sizeof(float)*16);
	memcpy(this->shaderGaussianBlurVertical->getProjectionMatrix(), glm::value_ptr(projMat),sizeof(float)*16);
	memcpy(this->shaderDistort->getProjectionMatrix(), glm::value_ptr(projMat),sizeof(float)*16);
	memcpy(this->shaderTexturing->getProjectionMatrix(), glm::value_ptr(projMat),sizeof(float)*16);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	this->shaderTexturing->bind();

	this->fbAccumulation->bind();
	glClear(GL_COLOR_BUFFER_BIT); 


	//glDisable(GL_BLEND);
	this->particleManager->drawParticles(this->shaderTexturing,this->spriteBullet,this);
	//glEnable(GL_BLEND);
	//
	this->drawHostileBullets(this->shaderTexturing);
	this->drawShipBullets(this->shaderTexturing);
		
	if (this->game->getCurrentLevel()->getShip()->isActive())
		this->drawShip(this->shaderTexturing,this->game->getCurrentLevel()->getShip());
	this->drawHostiles(this->shaderTexturing);
	//
	this->fbAccumulation->unbind(Renderer::screenWidth,Renderer::screenHeight);

	//NO BLUR
	/*
	glDisable(GL_BLEND);
	for (int i = 0;i < 2;i++)
		blur_framebuffer();
	glEnable(GL_BLEND);
	*/

	glBlendFunc(GL_ONE, GL_ONE);
	

	this->fbDrawing->bind();
	glClear(GL_COLOR_BUFFER_BIT); 
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind_attributes();
	this->backgroundManager.draw(this->shaderTexturing);
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind_attributes();

	//glDisable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	this->particleManager->drawParticles(this->shaderTexturing,this->spriteCircle,this);
	glBlendFunc(GL_ONE, GL_ONE);
	//glEnable(GL_BLEND);

	this->drawHostileBullets(this->shaderTexturing);
	this->drawShipBullets(this->shaderTexturing);
		
	if (this->game->getCurrentLevel()->getShip()->isActive())
		this->drawShip(this->shaderTexturing,this->game->getCurrentLevel()->getShip());
	this->drawHostiles(this->shaderTexturing);

	//glBlendFunc(GL_DST_COLOR, GL_ZERO);
	
	//glBlendEquation(GL_ADD);
	//glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ONE, GL_ONE);
	
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind_attributes();
	
	//glDisable(GL_BLEND);
	//this->spriteAccumulation->draw();
	//glBlendFunc(GL_DST_COLOR, GL_ZERO);
	this->spriteAccumulation->draw();
	//glEnable(GL_BLEND);

	glBlendFunc(GL_ONE, GL_ONE);




	for (int i = 0;i < 10;i++) {
		if (effectList[i].duration > 0) {
			processEffect(effectList[i]);
		}
	}

	//if(getSoundManager()->bassDetect > 0) {

	if (this->getSoundManager()->bassDetect == 0) {
	double d = 0.f;
	four1(this->getSoundManager()->tab,1024,1);
	
	for (int i = 0;i < 10;i++) {
		d += sqrt(this->getSoundManager()->tab[i*2+1]*this->getSoundManager()->tab[i*2+1] + this->getSoundManager()->tab[i*2+2]*this->getSoundManager()->tab[i*2+2]);
	}
	
	this->getSoundManager()->bassDetect = (unsigned int)d / 10;	
	}

	char s_bass[256];
		unsigned int toto = getSoundManager()->bassDetect / 100000;
		sprintf(s_bass,"Bass %d",toto);
		this->drawMessage(s_bass,ALIGNLEFT,ALIGNBOTTOM);
		memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
		this->shaderTexturing->bind_attributes();	
		this->spriteBullet->updateQuad(0,0,toto*10.f,toto*10.0f);
		this->drawSprite(this->shaderTexturing,this->spriteBullet,0,0,0);
		this->spriteBullet->updateQuad(0,0,5.f,5.f);
	//}


	this->fbDrawing->unbind(Renderer::screenWidth,Renderer::screenHeight);
	glDisable(GL_BLEND);	



	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind_attributes();
	this->spriteDrawing->draw();
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

	glEnable(GL_BLEND);
	this->spriteTextSurface->updateTexture();
	this->spriteTextSurface->draw();
	glDisable(GL_BLEND);	
	this->shaderTexturing->unbind();
	SDL_GL_SwapWindow(displayWindow);
}

void Renderer::loop()
{
	SDL_Event event;
	T_EFFECT effect;
	int told = SDL_GetTicks();

		int asyncup = 0;
		int asyncdown = 0;
		int asynleft = 0;
		int asynright = 0;
		int asyncspace = 0;
		int firedelay = 500;
		int prev_fire_tick = 0;

	while (!bExit)
	{
		frameCounter++;
		int tnew = SDL_GetTicks();
		int dt = tnew - told;
		told = tnew;

		if (dt != 0)
			this->fps = 1000/dt;

		this->game->run(dt);
		this->backgroundManager.update(dt);

		
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
			case SDL_FINGERMOTION:
					//puts("FINGER MOTION");
					//event.motion.x
					//printf("X=%f Y=%f DX=%f DY=%f\n", event.tfinger.x, event.tfinger.y, event.tfinger.dx,event.tfinger.dy);
				this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(event.tfinger.dx*Renderer::tsensitivity, event.tfinger.dy*Renderer::tsensitivity));
					break;
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
						case SDLK_7 :
							effect.duration = 30;
							effect.effectType = 3;
							addEffect(effect);			
							break;
						case SDLK_8 :
							for (int i = 0;i < 10;i++) {
								this->getParticleManager()->addParticle(rand()%Renderer::screenWidth,rand()%Renderer::screenHeight,0.2,rand()%360,0xffffffff,10000,-1);
							}
							break;
						case SDLK_9 :
							effect.duration = 30;
							effect.effectType = 2;
							addEffect(effect);			
							break;
						case SDLK_0 :
							effect.duration = 30;
							effect.effectType = 1;
							addEffect(effect);
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

			for (int i = 0;i < 2;i++) {
				this->getParticleManager()->addParticle(
					this->game->getCurrentLevel()->getShip()->getPosition().getX()+10.f,
					this->game->getCurrentLevel()->getShip()->getPosition().getY()+10.f,
					0.1,90+rand()%20-10,0x11111111,1000,-1,this->spriteBullet);
			}			
		}

		if (asyncdown)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(0,2));

			for (int i = 0;i < 2;i++) {
				this->getParticleManager()->addParticle(
					this->game->getCurrentLevel()->getShip()->getPosition().getX()+10.f,
					this->game->getCurrentLevel()->getShip()->getPosition().getY()+10.f,
					0.1,-90+rand()%20-10,0x11111111,1000,-1,this->spriteBullet);
			}	
		}

		if (asynleft)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(-2,0));

			for (int i = 0;i < 2;i++) {
				this->getParticleManager()->addParticle(
					this->game->getCurrentLevel()->getShip()->getPosition().getX()+10.f,
					this->game->getCurrentLevel()->getShip()->getPosition().getY()+10.f,
					0.1,0+rand()%20-10,0x11111111,1000,-1,this->spriteBullet);
			}
		}

		if (asynright)
		{
			this->game->getCurrentLevel()->getShip()->addToPosition(Vector2D(2,0));


			for (int i = 0;i < 2;i++) {
				this->getParticleManager()->addParticle(
					this->game->getCurrentLevel()->getShip()->getPosition().getX()+10.f,
					this->game->getCurrentLevel()->getShip()->getPosition().getY()+10.f,
					0.1,180+rand()%20-10,0x11111111,1000,-1,this->spriteBullet);
			}
		}

		if (Renderer::autofire) {
			this->game->getCurrentLevel()->getShip()->runBarrage(dt);
		}

		/*
		if (asyncspace) {
			this->game->getCurrentLevel()->getShip()->runBarrage(dt);
		}
		*/

		/*
		if (dt < 16)
		{
			SDL_Delay(16-dt);
		}
		*/
		//this->particleManager->addParticleRandom(100,100,0xffffff);
		this->particleManager->run(dt);

		this->draw();

		//SDL_Delay(10);
	}

    
	SDL_GL_DeleteContext(contexteOpenGL);
    SDL_DestroyWindow(displayWindow);
    SDL_Quit();
}

int Renderer::getFps()
{
	return this->fps;
}

void Renderer::processEffect(T_EFFECT &effect) {
	effect.duration--;
	switch (effect.effectType) {
	case 4 : {
				this->drawMessage(effect.str_param,effect.fl_param1,effect.fl_param2);
			 }
			 break;
	case 3 : {
			glDisable(GL_BLEND);
			this->fbDrawing->unbind(Renderer::screenWidth,Renderer::screenHeight);
			this->fbAccumulation->bind();
			glm::mat4 matrix = glm::mat4();
			matrix = glm::translate(matrix,glm::vec3(0.0f, FastMath::fastCos(360/30*effect.duration)*10.f, 0.0f));
			this->shaderTexturing->bind();
			memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(matrix),sizeof(float)*16);
			this->shaderTexturing->bind_attributes();
			this->spriteDrawing->draw();
			//this->shaderDistort->unbind();
			this->fbAccumulation->unbind(Renderer::screenWidth,Renderer::screenHeight);
			this->fbDrawing->bind();
			this->shaderTexturing->bind();
			memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
			this->shaderTexturing->bind_attributes();
			
			this->spriteAccumulation->draw();
			glEnable(GL_BLEND);
			}
			break;
	case 2  :
			//glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			this->fbDrawing->unbind(Renderer::screenWidth,Renderer::screenHeight);
			this->fbAccumulation->bind();

			this->shaderDistort->bind();
			{
			float * v = this->shaderDistort->getColorVector();
			float color =  ((float)effect.duration) / 30.f;
			//v[0] = 1.f/((float)effect.duration*10000.f);
			//v[0] = 3000 - (float)effect.duration * 100.f;
			//v[0] = (float)effect.duration * 10.f + 10.f;




			if (effect.duration == 29) {
				float x = (float)(rand()%Renderer::screenWidth);
				float y = (float)(rand()%Renderer::screenHeight);
				this->getParticleManager()->addParticle(x,y,0.0,rand()%360,rand(),100000,-1);
				this->getParticleManager()->addGravitySink(x,y,0.01f,5000);
				v[1] = x / (float)Renderer::screenWidth;
				v[2] = y / (float)Renderer::screenHeight;
				v[0] = 41.f + (float)effect.duration ;
			}

			//v[1] = 0.5f;
			//v[2] = 0.9f;
			
			// BIG WARP
			//v[0] = 20.f + (float)effect.duration ;
			//v[0] = 40.f + (float)effect.duration ;
			//v[0] = v[0] - 1.f;
			v[0] = 10.f + (float)effect.duration ;
			memcpy(this->shaderDistort->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
			}
			this->shaderDistort->bind_attributes();
			
			this->spriteDrawing->draw();
			//this->spriteCovering->draw();
			this->shaderDistort->unbind();
			this->fbAccumulation->unbind(Renderer::screenWidth,Renderer::screenHeight);
			this->fbDrawing->bind();
			
			this->shaderTexturing->bind();
			memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
			this->shaderTexturing->bind_attributes();
			
			this->spriteAccumulation->draw();
			glEnable(GL_BLEND);
			//this->spriteCovering->draw();
			//glEnable(GL_DEPTH_TEST);
			break;
	case 1 : 
			glEnable(GL_BLEND);
			//glBlendFunc(GL_SRC_COLOR, GL_ONE);
			float * v = this->shaderTexturing->getColorVector();
			float color =  ((float)effect.duration) / 30.f;
			v[0] = color;
			v[1] = color;
			v[2] = color;
			memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
			this->shaderTexturing->bind_attributes();
			this->spriteCovering->draw();
			//glDisable(GL_BLEND);
			break;
	}
}

void Renderer::addEffect(T_EFFECT effect) {
	for(int i = 0;i < 10;i++) {
		if (effectList[i].duration == 0) {
			effectList[i].duration = effect.duration;
			effectList[i].effectType = effect.effectType;
			strcpy(effectList[i].str_param, effect.str_param);
			effectList[i].fl_param1 = effect.fl_param1;
			effectList[i].fl_param2 = effect.fl_param2;
			return;
		}
	}
}


ParticleManager * Renderer::getParticleManager()
{
	return this->particleManager;
}

SoundManager * Renderer::getSoundManager()
{
	return this->soundManager;
}

void onDestroyCallback(void * userdata, unsigned int bulletId,Ship * ship, HostileInstance * hostile, float x, float y)
{
	Renderer * renderer =(Renderer *)userdata; 
	renderer->getSoundManager()->playExplosionSound();


	if (hostile != NULL) {
		T_EFFECT effect;
		effect.duration = 60;
		effect.fl_param1 = x;
		effect.fl_param2 = y;
		effect.effectType = 4;
		sprintf(effect.str_param,"%d",hostile->getScore());
		renderer->addEffect(effect);


		if (hostile->getScore() >= 100) {
			effect.duration = 30;
			effect.effectType = 3;
			renderer->addEffect(effect);	
		}

		/*
		if (hostile->getScore() > 1000){
			effect.duration = 30;
			effect.effectType = 1;
			renderer->addEffect(effect;)w
		}
		*/
	}

	

	if (bulletId != -1) {
		renderer->getParticleManager()->deactivateTrail(bulletId);
	}

	for (int i = 0;i < 10;i++)
	{
		//unsigned int color = (rand()%200 << 16) |  (rand()%200) << 8 | (rand()%200);

		unsigned int a = rand()%100+100;
		unsigned int color = (a << 16) |  (a) << 8 | (a);
		//unsigned int color = 0xffff00000;
		renderer->getParticleManager()->addParticleRandom(x,y,color,renderer->spriteBullet);
		
	}
}

void onHitCallback(void * userdata, unsigned int bulletId,Ship * ship, HostileInstance * hostile, float x, float y)
{
	if (hostile != NULL) {
		OGLRenderable * renderable = (OGLRenderable*)hostile->getRenderable();
		renderable->blink_hint = 10;
	}
}