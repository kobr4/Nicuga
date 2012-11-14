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

#include "SaxTagsUtils.h"
#include <string.h>

tTag getTagType(const char * el)
{
	if (!strcmp(el,"hostile"))
		return HOSTILE;
	else if (!strcmp(el,"ship"))
		return SHIP;
	else if (!strcmp(el,"game"))
		return GAME;
	else if (!strcmp(el,"boundingbox"))
		return BOUNDINGBOX;
	else if (!strcmp(el,"level"))
		return LEVEL;
	else if (!strcmp(el,"barrage"))
		return BARRAGE;
	else if (!strcmp(el,"sequence"))
		return LEVELSEQUENCE;
	else if (!strcmp(el,"life"))
		return LIFE;
	else if (!strcmp(el,"score"))
		return SCORE;
	else if (!strcmp(el,"playerlives"))
		return PLAYERLIVES;
	else if (!strcmp(el,"renderable"))
		return RENDERABLE;
	return UNKNOWN;
}

const char * getAttribute(char * attrname, const char ** attrlist, int attrcount)
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