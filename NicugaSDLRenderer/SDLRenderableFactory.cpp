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
#include "RenderableAbstractFactory.h"
#include "SDLRenderableFactory.h"
#include "TextureGenerator.h"
#include <string.h>
#include <stdio.h>



static SDLRenderable * currentRenderable;
static XML_Parser currentParer;
static XML_StartElementHandler oldStart;
static XML_EndElementHandler oldEnd;
static void * oldUserData;
static SDL_Surface * currentScreen;

static SDL_Surface* CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display)
{
  // 'display' is the surface whose format you want to match
  //  if this is really the display format, then use the surface returned from SDL_SetVideoMode

  const SDL_PixelFormat& fmt = *(display->format);
  return SDL_CreateRGBSurface(flags,width,height,
                  fmt.BitsPerPixel,
                  fmt.Rmask,fmt.Gmask,fmt.Bmask,fmt.Amask );
}

enum tRenderableTag
{
	RENDERABLE,
	CIRCLE,
	DUALCIRCLE,
	IMAGE,
	UNKNOWN
};

tRenderableTag getRenderableTagType(const char * el)
{
	if (!strcmp(el,"renderable"))
		return RENDERABLE;
	if (!strcmp(el,"circle"))
		return CIRCLE;
	if (!strcmp(el,"dualcircle"))
		return DUALCIRCLE;
	if (!strcmp(el,"image"))
		return IMAGE;
	return UNKNOWN;
}

static const char * getAttribute(char * attrname, const char ** attrlist, int attrcount)
{
    for (int i = 0;i < attrcount;i++){
		if (i%2 == 0)
		{
				
			if (!strcmp(attrlist[i],attrname))
				return attrlist[i+1];
		}
    }
	return NULL;
}

void sdlrenderable_xmlstart(void *data, const char *el, const char **attr) 
{
	printf("SDLRenderable TAG : %s\n",el);
	//exit(0);
	int iAttrCount = XML_GetSpecifiedAttributeCount(currentParer);
	const char * parameter;
	
	int width = 0;
	int innerwidth = 0;
	int height = 0;
	unsigned int color;
	unsigned int color2;
	switch(getRenderableTagType(el))
	{
	case CIRCLE :
		parameter = getAttribute("width",attr,iAttrCount);
		width = atoi(parameter);
		parameter = getAttribute("color",attr,iAttrCount);
		sscanf(parameter,"%x",&color);
		currentRenderable->surface = CreateSurface(currentScreen->flags,width,width,currentScreen);
		TextureGenerator::generateCircle((unsigned int *)currentRenderable->surface->pixels,width,color);
		SDL_SetColorKey(currentRenderable->surface, SDL_SRCCOLORKEY , 0);
		break;
	case DUALCIRCLE :
		parameter = getAttribute("width",attr,iAttrCount);
		width = atoi(parameter);
		parameter = getAttribute("innerwidth",attr,iAttrCount);
		innerwidth = atoi(parameter);
		parameter = getAttribute("color",attr,iAttrCount);
		sscanf(parameter,"%x",&color);
		parameter = getAttribute("color2",attr,iAttrCount);
		sscanf(parameter,"%x",&color2);
		currentRenderable->surface = CreateSurface(currentScreen->flags,width,width,currentScreen);
		TextureGenerator::generateDualCircle((unsigned int *)currentRenderable->surface->pixels,width,innerwidth,color,color2);
		SDL_SetColorKey(currentRenderable->surface, SDL_SRCCOLORKEY , 0);
		break;
	}
}

void sdlrenderable_xmlend(void *data, const char *el)
{
	switch(getRenderableTagType(el))
	{
	case RENDERABLE :
		//Put back passed handler
		XML_SetUserData(currentParer, oldUserData);
		XML_SetElementHandler(currentParer, oldStart, oldEnd);
		break;
	}
}

SDLRenderableFactory::SDLRenderableFactory(SDL_Surface * surface)
{
	this->screen = surface;
	currentScreen = surface;
}

void SDLRenderableFactory::setFactoryCallbacks(XML_Parser p, XML_StartElementHandler start, XML_EndElementHandler end)
{
	oldStart = start;
	oldEnd = end;
	oldUserData = XML_GetUserData(p);
	currentParer = p;

	currentRenderable = new SDLRenderable();

	//Put hostile factory handler
	XML_SetUserData(currentParer, currentRenderable);
	XML_SetElementHandler(currentParer, sdlrenderable_xmlstart, sdlrenderable_xmlend);
}

IRenderable * SDLRenderableFactory::getRenderable()
{
	return (IRenderable *)currentRenderable;
}