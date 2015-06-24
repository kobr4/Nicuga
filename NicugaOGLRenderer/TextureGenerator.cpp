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
#include "TextureGenerator.h"
#include "Vector2D.h"
#include <stdlib.h>
#include <string.h>

void TextureGenerator::overdraw(unsigned int * pixels, int w,int h, unsigned int * srcpixels, int wsource, int hsource) {
	int wstart = (w - wsource) / 2;
	int hstart = (h - hsource) / 2;
	for (int i = 0; i < w*h;i++) {
		pixels[i] = 0;
	}


	for (int i = 0;i < wsource; i++) {
		for (int j = 0;j < hsource;j++) {
			pixels[i+wstart + (j+hstart)*w] = srcpixels[i+j*wsource];
		}
	}
}


void TextureGenerator::generateCircle(unsigned int * pixels,int w, unsigned int color)
{
	Vector2D center = Vector2D(w / 2.0f,w / 2.0f);
	float radius = w / 2.0f;
	for (int i = 0;i < w;i++)
	  for(int j = 0;j < w;j++)
	  {
		  if (center.distance((float)i,(float)j) < radius)
			  pixels[i + j * w] = color;
		  else pixels[i + j * w] = 0;
	  }

}

void TextureGenerator::generateDualCircle(unsigned int * pixels,int w,int innerw,unsigned int color,unsigned int color2)
{
	Vector2D center = Vector2D(w / 2.0f,w / 2.0f);
	float radius = w / 2.0f;

	
	float radius2 = innerw / 2.0f;

	for (int i = 0;i < w;i++)
	  for(int j = 0;j < w;j++)
	  {
		  float d = center.distance((float)i,(float)j);
		  if (d < radius2)
		  {
			  float coef = (1.0f-(float)d/radius2 )*0.5f+0.5f;
			  //if (coef < 0.6f)
			  //  coef = 0.6f;
			  int r = (int)((color2 & 0xff) * coef);
			  int g = (int)(((color2 & 0xff00) >> 8)* coef);
			  int b = (int)(((color2 & 0xff0000) >> 16) * coef);
			  pixels[i + j * w] = r | g << 8 | b << 16;
		  }
		  else if (d < radius)
			  pixels[i + j * w] = color;
		  else pixels[i + j * w] = 0;
	  }
}

void TextureGenerator::generateTriangle(unsigned int * pixels,int w, unsigned int color)
{
	Vector2D sommet = Vector2D((float)w/2.f,0.f);
	Vector2D bottomleft = Vector2D(0,(float)w);
	Vector2D bottomright = Vector2D((float)w,(float)w);

	Vector2D s1 = Vector2D(sommet.getX() - bottomleft.getX(),sommet.getY() - bottomleft.getY());
	Vector2D s2 = Vector2D(bottomright.getX() - sommet.getX(),bottomright.getY() - sommet.getY());

	for (int i = 0;i < w;i++)
	  for(int j = 0;j < w;j++)
	  {
		  float x = (float)i - bottomleft.getX();
		  float y = (float)j - bottomleft.getY();

		  float dot = s1.getX() * y - s1.getY() * x;

		  if (dot > 0)
		  {

			  float x = bottomright.getX() - (float)i;
			  float y = bottomright.getY() - (float)j;

			  float dot = s2.getX() * y - s2.getY() * x;
			  if (dot < 0)
				pixels[i + j * w] = color;
			  else
				  pixels[i + j * w] = 0;
		  }
		  else
			  pixels[i + j * w] = 0;
	  }
}

void TextureGenerator::generateDualTriangle(unsigned int * pixels,int w, int innerw, unsigned int color,unsigned int color2)
{
	Vector2D sommet = Vector2D((float)w/2.f,0.f);
	Vector2D bottomleft = Vector2D(0,(float)w);
	Vector2D bottomright = Vector2D((float)w,(float)w);

	Vector2D s1 = Vector2D(sommet.getX() - bottomleft.getX(),sommet.getY() - bottomleft.getY());
	Vector2D s2 = Vector2D(bottomright.getX() - sommet.getX(),bottomright.getY() - sommet.getY());

	for (int i = 0;i < w;i++)
	  for(int j = 0;j < w;j++)
	  {
		  float x = (float)i - bottomleft.getX();
		  float y = (float)j - bottomleft.getY();

		  float dot = s1.getX() * y - s1.getY() * x;

		  if (dot > 0)
		  {

			  float x = bottomright.getX() - (float)i;
			  float y = bottomright.getY() - (float)j;

			  float dot = s2.getX() * y - s2.getY() * x;
			  if (dot < 0)
				pixels[i + j * w] = color;
			  //else
			//	  pixels[i + j * w] = 0;
		  }
		  //else
		//	  pixels[i + j * w] = 0;
	  }

	sommet = Vector2D((float)w/2.f,(float)w - innerw);
	bottomleft = Vector2D((float)w - (float)innerw,(float)innerw);
	bottomright = Vector2D((float)innerw,(float)innerw);

	s1 = Vector2D(sommet.getX() - bottomleft.getX(),sommet.getY() - bottomleft.getY());
	s2 = Vector2D(bottomright.getX() - sommet.getX(),bottomright.getY() - sommet.getY());

	for (int i = 0;i < w;i++)
	  for(int j = 0;j < w;j++)
	  {
		  float x = (float)i - bottomleft.getX();
		  float y = (float)j - bottomleft.getY();

		  float dot = s1.getX() * y - s1.getY() * x;

		  if (dot > 0)
		  {

			  float x = bottomright.getX() - (float)i;
			  float y = bottomright.getY() - (float)j;

			  float dot = s2.getX() * y - s2.getY() * x;
			  if (dot < 0)
				pixels[i + j * w] = color2;
			  //else
			//	  pixels[i + j * w] = 0;
		  }
		  //else
		//	  pixels[i + j * w] = 0;
	  }
}

void TextureGenerator::generateDualRectangle(unsigned int * pixels,int w,int h, unsigned int color,unsigned int color2) {
	for (int i = 0;i < w;i++) {
		for (int j = 0;j < h;j++) {
			if ((i == 0)||(i== w-1)||(j == 0)||(j==h-1)) {
				pixels[i + j * w] = color; 
				//spritedata[i + j * width] = 0xff0909ff; 
				//spritedata[i + j * width] = 0xff020211; 
				//spritedata[i + j * width] = 0xff070707;
			} else {
				pixels[i + j * w] = color2;
			}
		}
	}

}

void TextureGenerator::generateGrid(unsigned int * pixels,int w,int h,int space,unsigned int color) {
	for (int i = 0;i < w;i++) {
		for(int j= 0;j < h;j++) {
			if ((i%space == 0) || (j%space ==0)){
				pixels[i + j * w] = color;
			} else {
				pixels[i + j * w] = 0x0;
			}
		}
	}
}

void TextureGenerator::generateStarfield(unsigned int * pixels,int w,int h,int nbStars) {
	for(int i = 0;i < nbStars;i++) {
		int x = rand()%w;
		int y = rand()%h;
		pixels[x + y * w] = 0xffffffff;
	}
}

unsigned int dither_pixel(unsigned int &pixel, int factor) {
	unsigned int a = ((pixel & 0xff000000) >> 24) / factor;
	unsigned int r = ((pixel & 0xff0000) >> 16) / factor;
	unsigned int g = ((pixel & 0x00ff00) >> 8) / factor;
	unsigned int b = (pixel & 0x0000ff) / factor;
	return (a<<24) + (r << 16) + (g << 8) + b;
}

void merge_pixels(unsigned int * dest, unsigned int * src, int w,int h) {
	for (int i = 0;i < w * h;i++) {
		if (src[i]!=0) {
			dest[i] = src[i];
		} 
	
	}
}

void TextureGenerator::blurTexture(unsigned int * pixels,int w,int h, int blurFactor) {
	unsigned int * tmp_pixels = (unsigned int *)malloc(sizeof(unsigned int) * w * h);
	unsigned int * tmp_pixels2 = (unsigned int *)malloc(sizeof(unsigned int) * w * h);
	memset(tmp_pixels,0,sizeof(unsigned int) * w * h);
	
	merge_pixels(tmp_pixels,pixels,w,h);
	for (int j = 0; j < blurFactor;j++) {
		//merge_pixels(tmp_pixels,pixels,w,h);
		for (int i = 0;i < w*h;i++) {
			unsigned int comp1 = (i - w)<0? 0 : dither_pixel(tmp_pixels[i-w],4);
			unsigned int comp2 = (i + w)>(w*h)? 0 : dither_pixel(tmp_pixels[i+w],4);
			unsigned int comp3 = (i - 1)<0? 0 : dither_pixel(tmp_pixels[i-1],4);
			unsigned int comp4 = ((i + 1)%w)==0? 0 : dither_pixel(tmp_pixels[i+1],4);
			tmp_pixels2[i] = comp1 + comp2 + comp3 + comp4;
		}
		memcpy(tmp_pixels,tmp_pixels2,sizeof(unsigned int) * w * h);
	}
	//merge_pixels(tmp_pixels,pixels,w,h);
	memcpy(pixels,tmp_pixels,sizeof(unsigned int) * w * h);
	free(tmp_pixels);
	free(tmp_pixels2);
}

void TextureGenerator::generateCircle(int overdraw,int blurFactor,unsigned int * pixels,int w,unsigned int color) {
	unsigned int shapeWidth = w - overdraw;
	unsigned int * tmp_pixels = new unsigned int[shapeWidth*shapeWidth];
	generateCircle(tmp_pixels, shapeWidth, color);
	TextureGenerator::overdraw(pixels,w,w,tmp_pixels,shapeWidth,shapeWidth);
	//TextureGenerator::blurTexture(pixels,w,w,blurFactor);
	delete tmp_pixels;
}

void TextureGenerator::generateDualCircle(int overdraw,int blurFactor,unsigned int * pixels,int w,int innerw,unsigned int color,unsigned int color2) {
	unsigned int shapeWidth = w - overdraw;
	unsigned int * tmp_pixels = new unsigned int[shapeWidth*shapeWidth];
	generateDualCircle(tmp_pixels, shapeWidth,innerw, color,color2);
	TextureGenerator::overdraw(pixels,w,w,tmp_pixels,shapeWidth,shapeWidth);
	TextureGenerator::blurTexture(pixels,w,w,blurFactor);
	delete tmp_pixels;
}

void TextureGenerator::generateTriangle(int overdraw, int blurFactor,unsigned int * pixels,int w, unsigned int color) {
	unsigned int shapeWidth = w - overdraw;
	unsigned int * tmp_pixels = new unsigned int[shapeWidth*shapeWidth];
	generateTriangle(tmp_pixels, shapeWidth, color);
	TextureGenerator::overdraw(pixels,w,w,tmp_pixels,shapeWidth,shapeWidth);
	TextureGenerator::blurTexture(pixels,w,w,blurFactor);
	delete tmp_pixels;
}


void TextureGenerator::generateDualTriangle(int overdraw,int blurFactor,unsigned int * pixels,int w, int innerw, unsigned int color,unsigned int color2) {
}

void TextureGenerator::generateDualRectangle(int overdraw,int blurFactor,unsigned int * pixels,int w,int h, unsigned int color,unsigned int color2) {
}