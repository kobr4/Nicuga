#include "OGLRenderableFactory.h"
#include "TextureGenerator.h"
#include "Texture.h"

static OGLRenderable * currentRenderable;
static XML_Parser currentParer;
static XML_StartElementHandler oldStart;
static XML_EndElementHandler oldEnd;
static void * oldUserData;

enum tRenderableTag
{
	RENDERABLE,
	CIRCLE,
	DUALCIRCLE,
	SQUARE,
	IMAGE,
	GENERATOR,
	UNKNOWN
};

tRenderableTag getRenderableTagType(const char * el)
{
	if (!strcmp(el,"renderable"))
		return RENDERABLE;
	if (!strcmp(el,"circle"))
		return CIRCLE;
	if (!strcmp(el,"square"))
		return SQUARE;
	if (!strcmp(el,"dualcircle"))
		return DUALCIRCLE;
	if (!strcmp(el,"image"))
		return IMAGE;
	if (!strcmp(el,"generator"))
		return GENERATOR;
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

OGLRenderableFactory::OGLRenderableFactory() {
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
	int iparam = 0;
	int iteration;
	unsigned int animationType = 0;
	unsigned int color;
	unsigned int color2;
	unsigned int * pixels;
	switch(getRenderableTagType(el))
{	
	case SQUARE :
		parameter = getAttribute("animationType",attr,iAttrCount);
		if (parameter != NULL) {
			animationType = atoi(parameter);
		}
		parameter = getAttribute("width",attr,iAttrCount);
		width = atoi(parameter);
		parameter = getAttribute("innerwidth",attr,iAttrCount);
		innerwidth = atoi(parameter);
		parameter = getAttribute("color",attr,iAttrCount);
		sscanf(parameter,"%x",&color);
		parameter = getAttribute("color2",attr,iAttrCount);
		sscanf(parameter,"%x",&color2);
		parameter = getAttribute("iparam1",attr,iAttrCount);
		if (parameter != NULL) {
			iparam = atoi(parameter);
		}
		pixels = (unsigned int *)malloc(sizeof(unsigned int) * (width) * (width));
		TextureGenerator::generateDualRectangle(pixels,width,width,color,color2);
		currentRenderable->animationType = animationType;
		currentRenderable->iparam1 = iparam;
		currentRenderable->sprite = new Sprite(new Texture(width,width,(unsigned char*)pixels),width,width,0,0,1,1);
		break;
	case CIRCLE :
		parameter = getAttribute("animationType",attr,iAttrCount);
		if (parameter != NULL) {
			animationType = atoi(parameter);
		}
		parameter = getAttribute("width",attr,iAttrCount);
		width = atoi(parameter);
		parameter = getAttribute("color",attr,iAttrCount);
		sscanf(parameter,"%x",&color);
		pixels = (unsigned int *) malloc(sizeof(unsigned int) * width * width);
		TextureGenerator::generateCircle(pixels,width,color);
		currentRenderable->animationType = animationType;
		currentRenderable->sprite = new Sprite(new Texture(width,width,(unsigned char*)pixels),width,width,0,0,1,1);
		break;
	case DUALCIRCLE :
		parameter = getAttribute("animationType",attr,iAttrCount);
		if (parameter != NULL) {
			animationType = atoi(parameter);
		}
		parameter = getAttribute("width",attr,iAttrCount);
		width = atoi(parameter);
		parameter = getAttribute("innerwidth",attr,iAttrCount);
		innerwidth = atoi(parameter);
		parameter = getAttribute("color",attr,iAttrCount);
		sscanf(parameter,"%x",&color);
		parameter = getAttribute("color2",attr,iAttrCount);
		sscanf(parameter,"%x",&color2);
		//pixels = (unsigned int *)malloc(sizeof(unsigned int) * (width+10) * (width+10));
		//TextureGenerator::generateDualCircle(10,4,pixels,width+10,innerwidth,color,color2);
		//currentRenderable->sprite = new Sprite(new Texture(width+10,width+10,(unsigned char*)pixels),width+10,width+10,0,0,1,1);
		pixels = (unsigned int *)malloc(sizeof(unsigned int) * (width) * (width));
		TextureGenerator::generateDualCircle(pixels,width,innerwidth,color,color2);
		currentRenderable->animationType = animationType;
		currentRenderable->sprite = new Sprite(new Texture(width,width,(unsigned char*)pixels),width,width,0,0,1,1);
		break;
	case GENERATOR :
		parameter = getAttribute("width",attr,iAttrCount);
		width = atoi(parameter);
		parameter = getAttribute("height",attr,iAttrCount);
		height = atoi(parameter);
		parameter = getAttribute("iteration",attr,iAttrCount);
		iteration = atoi(parameter);
		pixels = (unsigned int *)malloc(sizeof(unsigned int) * width * height);
		TextureGenerator::generateShape(pixels,width,height,0,iteration);
		currentRenderable->sprite = new Sprite(new Texture(width,height,(unsigned char*)pixels),width,height,0,0,1,1);
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
	
void OGLRenderableFactory::setFactoryCallbacks(XML_Parser p, XML_StartElementHandler start, XML_EndElementHandler end) {
	oldStart = start;
	oldEnd = end;
	oldUserData = XML_GetUserData(p);
	currentParer = p;

	currentRenderable = new OGLRenderable();

	//Put hostile factory handler
	XML_SetUserData(currentParer, currentRenderable);
	XML_SetElementHandler(currentParer, sdlrenderable_xmlstart, sdlrenderable_xmlend);
}

IRenderable * OGLRenderableFactory::getRenderable() {
	return (IRenderable *)currentRenderable;
}