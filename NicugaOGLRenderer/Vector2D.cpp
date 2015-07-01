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
#include "Vector2D.h"
#include <math.h>

Vector2D::Vector2D(void)
{
	v[0] = 0;
	v[1] = 0;
}

Vector2D::Vector2D(float x, float y)
{
	this->set(x,y);
}


Vector2D::~Vector2D(void)
{
}


void Vector2D::set(float x, float y)
{
	v[0] = x;
	v[1] = y;
}

float Vector2D::getX()
{
	return v[0];
}


float Vector2D::getY()
{
	return v[1];
}

float Vector2D::distance(float x, float y)
{
	return sqrt((v[0] - x) * (v[0] - x) + (v[1] - y) * (v[1] - y));
}

float Vector2D::dot(Vector2D vec) {
	return v[0] * vec.getX() + v[1] * vec.getY();
}

float Vector2D::cross(Vector2D vec) {
	return v[0] * vec.getY() - v[1] * vec.getX();
}

void Vector2D::normalize() {
	float d = this->distance(0.f,0.f);
	v[0] = v[0] / d;
	v[1] = v[1] / d;
}

Vector2D Vector2D::operator+(Vector2D arg) {
	return Vector2D(this->getX()+arg.getX(),this->getY()+arg.getY());
}

Vector2D Vector2D::operator-(Vector2D arg) {
	return Vector2D(this->getX()-arg.getX(),this->getY()-arg.getY());
}

Vector2D Vector2D::operator*(float arg) {
	return Vector2D(this->getX() * arg,this->getY() * arg);
}

