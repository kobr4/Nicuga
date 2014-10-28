#include "Texture.h"
#include <GL/glew.h>

void Texture::bind() {
	if (this->textureName == 0) {
		do_register();
	}
	glBindTexture(GL_TEXTURE_2D, this->textureName); 
}

void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0); 
}

void Texture::update() {
	if (this->textureName != 0) {
		glBindTexture(GL_TEXTURE_2D, this->textureName); 
				
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	}
}

void Texture::do_register() {
	glGenTextures(1, &this->textureName); 
	this->update();
}