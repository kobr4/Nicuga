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
#include "SoundManager.h"
#include <stdio.h>
#include <math.h>



// make a passthru processor function that does nothing...
void noEffect(int chan, void *stream, int len, void *udata)
{
    // you could work with stream here...
	/*
	SoundManager * manager = ((SoundManager*)udata);
	if (manager->bassDetect > 0) {
		
		//printf("Sample played chan=%d len=%d\n",chan,len);
		if (manager->tab == NULL) {
			manager->tab = (double*)malloc(sizeof(double)*( (len/4)*2+1 ));
		}
		for (int i = 0;i < len/4;i++) {
			short int * t = (short*)stream;
			manager->tab[i*2+1] = (double)t[i*2];
			manager->tab[i*2+2] = 0;
		}

		manager->bassDetect = 0;
	}
	*/
	/*
	double d = 0.f;
	four1(tab,1024,1);
	
	for (int i = 0;i < 200;i++) {
		d += sqrt(tab[i*2+1]*tab[i*2+1] + tab[i*2+2]*tab[i*2+2]);
	}
	
	((SoundManager*)udata)->bassDetect = (unsigned int)d / 200;
	*/
	//printf("f=%f fi=%f ech=%f\n",tab[1],tab[2],d/200);
}

void SoundManager::init()
{
	this->tab = 0;
	this->bassDetect = 1;
	this->currentMusic = NULL;
	this->explosionSound = NULL;
	int ret = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	if (ret != 0)
	{
		printf("ERROR : unable to initialize audio device (%s)",SDL_GetError());
		exit(1);
	}

}

void SoundManager::loadMusic(const char * filename)
{
	//this->currentMusic = Mix_LoadMUS(filename);
	this->currentMusicChunk = Mix_LoadWAV(filename);
	if (this->currentMusicChunk == NULL)
	{
		printf("ERROR : unable to load audio file (%s)",SDL_GetError());
		exit(1);
	}
}

void SoundManager::loadExplosionSound(const char * filename)
{
	this->explosionSound = Mix_LoadWAV(filename);
}

void SoundManager::playCurrentMusic()
{
	//Mix_PlayMusic(this->currentMusic, 1);
	Mix_PlayChannel(0,currentMusicChunk,-1);

	// register noEffect as a postmix processor
	/*
	if(!Mix_RegisterEffect(0, noEffect, NULL, this)) {
		printf("Mix_RegisterEffect: %s\n", Mix_GetError());
	}
	*/
}

void SoundManager::playExplosionSound()
{
	int channel;
	channel = Mix_PlayChannel(-1, this->explosionSound, 0);
	if(channel == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}
}

void SoundManager::close()
{
	/* La musique est terminée, on la libère */
	if (this->currentMusic)
		Mix_FreeMusic(this->currentMusic);

    /* On libère le matériel */
    Mix_CloseAudio();
}



void SoundManager::processAudio() {

}