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

#include "GameFactory.h"
#include "Game.h"
#include "expat.h"

#include "SaxTagsUtils.h"


#define BUFFSIZE	8192
static char Buff[BUFFSIZE];
static XML_Parser p;
//Prototypes 
void end(void *data, const char *el);
void start(void *data, const char *el, const char **attr);



static void start(void *data, const char *el, const char **attr) {
	//printf("TAG : %s\n",el);
	Game * game = (Game*)data;
	const char * livesstr;
	int lives = 0;
	int iAttrCount = XML_GetSpecifiedAttributeCount(p);
	switch (getTagType(el))
	{
	case LEVEL :
		game->addLevel(getAttribute("file",attr,iAttrCount));
		break;
	case PLAYERLIVES :
		livesstr = getAttribute("value",attr,iAttrCount);
		if (livesstr != NULL)
		{
			lives = atoi(livesstr);
		}
		game->setPlayerLives(lives);
		break;
	default :
		break;
	}
}

static void end(void *data, const char *el){
	Game * game = (Game*)data;
	/*
	switch (getTagType(el))
	{
	//default :
	//	break;
	}
	*/
}


Game * GameFactory::loadGame(const char * filename)
{
	Game * game = new Game();

	p = XML_ParserCreate(NULL);

	if (! p) {
		fprintf(stderr, "Couldn't allocate memory for parser\n");
		exit(-1);
	}

	XML_SetUserData(p, game);
	XML_SetElementHandler(p, start, end);

	FILE * f;
	fopen_s(&f,filename,"rb"); 
	if (f == NULL)
		printf("error opening file\n");

	int done = 0;
	while (!done) {
		
		int len;
		len = fread(Buff, 1, BUFFSIZE, f);
		if (ferror(stdin)) {
			fprintf(stderr, "Read error\n");
			exit(-1);
		}
		done = feof(f);
		if (! XML_Parse(p, Buff, len, done)) {
			fprintf(stderr, "Parse error at line %d:\n%s\n",
				XML_GetCurrentLineNumber(p),
				XML_ErrorString(XML_GetErrorCode(p)));
			exit(-1);
		}
	}
	XML_ParserFree(p);

	return game;
}