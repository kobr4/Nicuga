#include "BufferData.h"
#include <GL/glew.h>

BufferData::BufferData(char * bufferData,int bufferSize) {
	this->bufferData = bufferData;
	this->bufferSize = bufferSize;
	this->bufferName = 0;
}

void BufferData::do_register() {
	glGenBuffers(1,&this->bufferName);
	updateBuffer();
}

void BufferData::updateBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, this->bufferName);
	glBufferData(GL_ARRAY_BUFFER,this->bufferSize,this->bufferData,GL_DYNAMIC_DRAW);   
}

void BufferData::bind() {
	if (this->bufferName == 0) {
		do_register();
	}
	glBindBuffer(GL_ARRAY_BUFFER, this->bufferName);
}
	
void BufferData::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}