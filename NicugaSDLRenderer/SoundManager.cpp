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

void SoundManager::init()
{
	this->currentMusic = NULL;
	this->explosionSound = NULL;
	int ret = Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
	if (ret != 0)
	{
		printf("ERROR : unable to initialize audio device (%s)",SDL_GetError());
		exit(1);
	}

}

void SoundManager::loadMusic(const char * filename)
{
	this->currentMusic = Mix_LoadMUS(filename);
	if (this->currentMusic == NULL)
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
	Mix_PlayMusic(this->currentMusic, 1);
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