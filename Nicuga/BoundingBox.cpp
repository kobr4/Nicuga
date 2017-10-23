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

#include "BoundingBox.h"


BoundingBox::BoundingBox(void)
{
}

BoundingBox::BoundingBox(short topx,short topy, short bottomx,short bottomy)
{
	topLeft.set(topx,topy);
	bottomRight.set(bottomx,bottomy);
}

BoundingBox::~BoundingBox(void)
{
}

bool BoundingBox::isBound(Vector2D vector)
{
	if (vector.getX() < topLeft.getX())
		if (vector.getX() > bottomRight.getX())
			if (vector.getY() < topLeft.getY())
				if (vector.getY() > bottomRight.getY())
					return true;
	return false;
}

Vector2D BoundingBox::getTopLeft()
{
	return this->topLeft;
}

Vector2D BoundingBox::getBottomRight()
{
	return this->bottomRight;
}

void BoundingBox::setTopLeft(short topx,short topy)
{
	topLeft.set(topx,topy);
}

void BoundingBox::setBottomRight(short bottomx,short bottomy)
{
	bottomRight.set(bottomx,bottomy);
}