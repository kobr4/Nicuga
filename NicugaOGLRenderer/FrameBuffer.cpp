#include "FrameBuffer.h"
#include <stdlib.h>
#include <GL/glew.h>
#include "Texture.h"

#include <stdio.h>

void FrameBuffer::bind(){
	if (this->fb == 0) {
		do_register();
	}

	glViewport(0, 0, width, height);
	//glViewport(0, 0, 640, 480);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRb);	

	// check status
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		exit(0);
	}
}

void FrameBuffer::unbind(int screenWidth, int screenHeight){
	glViewport(0, 0, screenWidth, screenHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::do_register(){
		glewInit();
		glGenFramebuffers(1, &this->fb);
		glGenRenderbuffers(1, &this->depthRb);
		glBindRenderbuffer(GL_RENDERBUFFER, this->depthRb);
		glGenTextures(1, &this->renderTex);
		glBindTexture(GL_TEXTURE_2D, this->renderTex);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		pixels = (unsigned char *)calloc(sizeof(char),4 * width * height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// create render buffer and bind 16-bit depth buffer
		glBindRenderbuffer(GL_RENDERBUFFER, this->depthRb);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, this->width, this->height);	

		// check status
		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			exit(0);
		}
}

Texture * FrameBuffer::getTexture() {
	return new Texture(this->width,this->height,this->renderTex);
}