#include "SpriteBatch.h"
#include "BufferData.h"
#include <GL/glew.h>
#include <stdlib.h>
#include "Texture.h"
#include "Shader.h"

SpriteBatch::SpriteBatch(Texture * texture,int bufferSize) {
    this->bufferSize = bufferSize;
    this->buffer = (float *)malloc(sizeof(float) * bufferSize);
    this->bufferCount = 0;
    this->bufferData = new BufferData((char*)this->buffer,sizeof(float) * bufferSize);
}

bool SpriteBatch::addElement(float * element, int size) {
    for(int i = 0;i < size;i++) {
        if (this->bufferCount == bufferSize) return false;
        this->buffer[this->bufferCount++] = element[i];
    }
    return true;
}

void SpriteBatch::reset() {
    this->bufferCount = 0;
}

void SpriteBatch::draw() {
	glEnable(GL_TEXTURE_2D);
	
	this->texture->bind();

    this->bufferData->updateBuffer(this->bufferCount * sizeof(float));
	this->bufferData->bind();	

	glVertexPointer (3,GL_FLOAT,5*sizeof(float),0);
	glTexCoordPointer (2,GL_FLOAT,5*sizeof(float),0);
	
	glVertexAttribPointer(Shader::vertexPositionHandle,3,GL_FLOAT,GL_FALSE,5*sizeof(float),0);
	glEnableVertexAttribArray(Shader::vertexPositionHandle);

	glVertexAttribPointer(Shader::texCoordHandle,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(Shader::texCoordHandle);

	glDrawArrays (GL_TRIANGLES, 0, bufferCount/6);

	this->bufferData->unbind();

	this->texture->unbind();

	glDisable(GL_TEXTURE_2D);
}