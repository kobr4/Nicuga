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
#include <vector>

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


typedef struct {
	Vector2D p1;
	Vector2D p2;
	Vector2D p3;
} T_T; 

typedef struct {
	Vector2D p1;
	Vector2D p2;
	Vector2D normal;
} T_AXIS;


Vector2D maxInbound(Vector2D p1, Vector2D p2, int w, int h,float step) {
	bool inbound = true;
	Vector2D p;
	Vector2D res = Vector2D(0.0,0.0);
	float current = 0;
	while (inbound) {
		current += step;
		p = p1 + (p2 - p1) * current;
		if ((p.getX() < 0.0)||(p.getY() < 0.0)||(p.getX() > (float)w)||(p.getY() > (float)h)) {
			inbound = false;
		}

		if (inbound) {
			res = p;
		}
	}

	return res;
}

Vector2D getAxisNormal(Vector2D p1, Vector2D p2, Vector2D p3) {
	Vector2D p2p1 = p2 - p1;
	Vector2D p3p1 = p3 - p1;
	Vector2D p3p1n = p3p1;
	p3p1n.normalize();
	Vector2D k = p1 + p3p1n * (p2p1.dot(p3p1) / p3p1.distance(0,0));
	Vector2D n = p2 - k;
	n.normalize();
	return n;
}

int max(int a,int b) {
	int m;
	(a > b) ? m = a : m = b;
	return m;
}

int min(int a,int b) {
	int m;
	(a < b) ? m = a : m = b;
	return m;
}

void rasterTriangle(unsigned int * pixels,int w,int h, unsigned int color, T_T * tri) {
	//printf("Raster triangle [%f %f][%f %f][%f %f]\n",tri->p1.getX(),tri->p1.getY(),tri->p2.getX(),tri->p2.getY(),tri->p3.getX(),tri->p3.getY());

	/* get the bounding box of the triangle */
	int maxX = max(tri->p1.getX(), max(tri->p2.getX(), tri->p3.getX()));
	int minX = min(tri->p1.getX(), min(tri->p2.getX(), tri->p3.getX()));
	int maxY = max(tri->p1.getY(), max(tri->p2.getY(), tri->p3.getY()));
	int minY = min(tri->p1.getY(), min(tri->p2.getY(), tri->p3.getY()));
	//printf("minX=%d maxX=%d minY=%d maxY=%d\n",minX,maxX,minY,maxY);

	/* spanning vectors of edge (v1,v2) and (v1,v3) */
	Vector2D vs1 = Vector2D(tri->p2.getX() - tri->p1.getX(), tri->p2.getY() - tri->p1.getY());
	Vector2D vs2 = Vector2D(tri->p3.getX() - tri->p1.getX(), tri->p3.getY() - tri->p1.getY());

	for (int x = minX; x <= maxX; x++)
	{
	  for (int y = minY; y <= maxY; y++)
	  {
		Vector2D q = Vector2D(x - tri->p1.getX(), y - tri->p1.getY());

		float s = (float)q.cross(vs2) / vs1.cross(vs2);
		float t = (float)vs1.cross(q) / vs1.cross(vs2);

		if ( (s >= 0) && (t >= 0) && (s + t <= 1))
		{ /* inside triangle */
		  pixels[x + y * w] = color;
		}
	  }
	}
}



int addTriangle(T_AXIS * axis, std::vector<T_AXIS> * axisList, std::vector<T_T> * triList,int w,int h) {
	T_T newT;
	T_AXIS newA;
	int retryCounter = 0; 
	float ratio = (float)(rand()%90 + 10) / 100.f;

	//printf("R1 = %f\n",ratio);
	newT.p1 = axis->p1 + (axis->p2 - axis->p1) * ratio ;
	//printf("P1 : [%f %f]",newT.p1.getX(),newT.p1.getY());

	int sign = (rand()%100)&1==1 ? 0 : 1;
	Vector2D max1 = maxInbound(newT.p1,axis->p1,w,h,0.1);
	Vector2D max2 = maxInbound(newT.p1,axis->p2,w,h,0.1);
	Vector2D max = sign == 0 ? max1 : max2;
	ratio = (float)(rand()%90 + 10) / 100.f;

	//printf("R2 = %f\n",ratio);
	newT.p2 = newT.p1 + (max - newT.p1) * ratio ;


	float length = rand()%10 + 10.0f;
	bool done = false;
	
	while (done == false) {
		ratio = (float)(rand()%90 + 10) / 100.f;
		Vector2D pT =  newT.p1 + (newT.p2 - newT.p1) * ratio ;
		newT.p3 = pT + axis->normal * length;
		if ((newT.p3.getX() > 0.0)&&(newT.p3.getY() > 0.0)&&(newT.p3.getX() < (float)w)&&(newT.p3.getY() < (float)h)) {
			done = true;
		} else {
			//printf("P3 not inbound retry. [%f %f]",newT.p3.getX(),newT.p3.getY());
			retryCounter++;
			if (retryCounter > 100) {
				return -1;
			}
		}
	}

	printf("Adding triangle [%f %f][%f %f][%f %f]\n",newT.p1.getX(),newT.p1.getY(),newT.p2.getX(),newT.p2.getY(),newT.p3.getX(),newT.p3.getY());
	newA.p1 = newT.p1;
	newA.p2 = newT.p3;
	newA.normal = getAxisNormal(newT.p1,newT.p2,newT.p3) * -1;
	//printf("Adding axis [%f %f][%f %f][%f %f]\n",newA.p1.getX(),newA.p1.getY(),newA.p2.getX(),newA.p2.getY(),newA.normal.getX(),newA.normal.getY());
	axisList->push_back(newA);
	

	newA.p1 = newT.p2;
	newA.p2 = newT.p3;
	newA.normal = getAxisNormal(newT.p2,newT.p1,newT.p3) * -1;
	//printf("Adding axis [%f %f][%f %f][%f %f]\n",newA.p1.getX(),newA.p1.getY(),newA.p2.getX(),newA.p2.getY(),newA.normal.getX(),newA.normal.getY());
	axisList->push_back(newA);

	newA.p1 = newT.p1;
	newA.p2 = newT.p2;
	newA.normal = getAxisNormal(newT.p1,newT.p3,newT.p2) * -1;
	//printf("Adding axis [%f %f][%f %f][%f %f]\n",newA.p1.getX(),newA.p1.getY(),newA.p2.getX(),newA.p2.getY(),newA.normal.getX(),newA.normal.getY());
	axisList->push_back(newA);

	triList->push_back(newT);

	return 0;
}

void hsymetrize(unsigned int * pixels,int w,int h) {
	for (int i = w/2;i < w;i++)
		for(int j = 0;j < h;j++) 	
		{
			pixels[i + j * w] = pixels[w-i + j * w];
		}
}


void TextureGenerator::generateShape(unsigned int * pixels,int w,int h, unsigned int color,unsigned int iteration) {
	for (int i = 0;i < w;i++)
		for(int j = 0;j < h;j++) 	
		{
			pixels[i + j * w] = 0;
		}	
	
	std::vector<T_T> triList;
	std::vector<T_AXIS> axisList;
	T_AXIS iniAxis;
	int gw = w/2; 
	iniAxis.p1 = Vector2D(gw,0);
	iniAxis.p2 = Vector2D(gw,h-1);
	iniAxis.normal = Vector2D(-1,0);
	axisList.push_back(iniAxis);
	
	for (int i = 0;i < iteration;i++) {
		T_AXIS axis = axisList[rand()%axisList.size()];
		addTriangle(&axis,&axisList,&triList,gw,h);
	}

	for (int i = 0;i < triList.size();i++) {
		unsigned int c = rand()%50*4+56 << 16;
		unsigned int r = 0x1f;
		unsigned int g = 0x1f << 8;
		unsigned int b = 0x1f << 16;
		
		rasterTriangle(pixels,gw,h, 0xff000000 | c | r | g | b, &(triList[i]));
	}

	hsymetrize(pixels,w,h);
}

void TextureGenerator::convert24to32(unsigned char * src, unsigned char * dst, int w, int h) {
	int dstoffset = 0;
	int srcoffset = 0;
	for (int i = 0; i < w * h; i++) {
		dst[dstoffset] = src[srcoffset];
		dst[dstoffset+1] = src[srcoffset+1];
		dst[dstoffset+2] = src[srcoffset+2];
		dst[dstoffset+3] = 0xff;
		dstoffset += 4;
		srcoffset += 3;
	}
}