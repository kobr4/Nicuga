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

class TextureGenerator
{
public :
	static void generateCircle(unsigned int * pixels,int w,unsigned int color);
	static void generateDualCircle(unsigned int * pixels,int w,int innerw,unsigned int color,unsigned int color2);
	static void generateTriangle(unsigned int * pixels,int w, unsigned int color);
	static void generateDualTriangle(unsigned int * pixels,int w, int innerw, unsigned int color,unsigned int color2);
	static void generateDualRectangle(unsigned int * pixels,int w,int h, unsigned int color,unsigned int color2);
	static void generateGrid(unsigned int * pixels,int w,int h,int space,unsigned int color);
	static void generateStarfield(unsigned int * pixels,int w,int h,int nbStars);
	

	static void generateCircle(int overdraw,int blurFactor,unsigned int * pixels,int w,unsigned int color);
	static void generateDualCircle(int overdraw,int blurFactor,unsigned int * pixels,int w,int innerw,unsigned int color,unsigned int color2);
	static void generateTriangle(int overdraw,int blurFactor,unsigned int * pixels,int w, unsigned int color);
	static void generateDualTriangle(int overdraw,int blurFactor,unsigned int * pixels,int w, int innerw, unsigned int color,unsigned int color2);
	static void generateDualRectangle(int overdraw,int blurFactor,unsigned int * pixels,int w,int h, unsigned int color,unsigned int color2);

	static void generateShape(unsigned int * pixels,int w,int h, unsigned int color,unsigned int iteration);

	static void blurTexture(unsigned int * pixels,int w,int h, int blurFactor);
	static void overdraw(unsigned int * pixels, int w,int h, unsigned int * srcpixels, int wsource, int hsource);

	static void convert24to32(unsigned char * sr , unsigned char * dst, int w, int h);
};