#include "Shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

int Shader::vertexPositionHandle = 1;
int Shader::texCoordHandle = 2;

void load_string(const char * filename,int &length,char **&string, int *&stringlength) {
	char s[2048];
	FILE * f;
	f = fopen(filename,"r");
	if (f == NULL) {
		printf("Couldn't load file %s\n",filename);
		exit(0); 
	}

	length = 0;
	while (fgets (s, 2048, f)!=NULL)
	length++;
	fseek(f,0,SEEK_SET); 
	string = (char **)malloc(sizeof(char*)*length);
	stringlength = (int *)malloc(sizeof(int)*length);
	length = 0;
	while (fgets (s, 2048, f)!=NULL){
		int l = strlen(s) + 1;
		string[length] = (char*)malloc(sizeof(char)*l);
		strcpy(string[length],s);
		stringlength[length] = strlen(string[length]);
		length++;
	}

	fclose(f);

	for(int i = 0;i < length;i++) {
		printf("%s",string[i]);
	}
}

void Shader::load_fragment(const char * filename) {
	load_string(filename,this->f_length,this->f_string,this->f_string_length);
}

void Shader::load_vertex(const char * filename) {
	load_string(filename,this->v_length,this->v_string,this->v_string_length);
}

void Shader::bind_attributes() {
	glUniformMatrix4fv(this->modelViewMatrixHandle, 1, 0,this->modelViewMatrix);
	glUniformMatrix4fv(this->projectionMatrixHandle, 1, 0,this->projectionMatrix);
	glUniform4fv(this->colorVectorHandle,1,this->colorVector);
}

void Shader::bind()
{
	if (programHandle == 0) {
		do_register();
	}
    glUseProgram(programHandle);

	this->bind_attributes();

}

void Shader::unbind()
{
    glUseProgram(0);			
}

void Shader::do_register() {

	GLint status;
	char buffer[2048];
	int length;
	
	glewExperimental=GL_TRUE;
	glewInit();

	this->vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(this->vertexShaderHandle, this->v_length,(const GLchar **)this->v_string,this->v_string_length);
	glCompileShader(this->vertexShaderHandle);

	glGetShaderiv(this->vertexShaderHandle,GL_COMPILE_STATUS,&status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(this->vertexShaderHandle,2048,&length,buffer);
		puts("Fatal error on vertex shader compilation.");
		puts(buffer);
		exit(0);
	}

	this->fragmentShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragmentShaderHandle, this->f_length,(const GLchar **)this->f_string,this->f_string_length);
	glCompileShader(this->fragmentShaderHandle);

	glGetShaderiv(this->vertexShaderHandle,GL_COMPILE_STATUS,&status);
	if (status == GL_FALSE) {
		glGetShaderInfoLog(this->vertexShaderHandle,2048,&length,buffer);
		puts("Fatal error on fragment shader compilation.");
		puts(buffer);
		exit(0);
	}

	this->programHandle = glCreateProgram();
	glAttachShader(this->programHandle, this->vertexShaderHandle);
	glAttachShader(this->programHandle, this->fragmentShaderHandle);

	glBindAttribLocation(this->programHandle, Shader::vertexPositionHandle, "a_Position");
	glBindAttribLocation(this->programHandle, Shader::texCoordHandle, "a_TexCoord");

	glLinkProgram(this->programHandle);

	this->modelViewMatrixHandle = glGetUniformLocation(this->programHandle, "u_MVMatrix");
	this->projectionMatrixHandle = glGetUniformLocation(this->programHandle, "u_PMatrix");
	this->colorVectorHandle = glGetUniformLocation(this->programHandle, "u_CVector");
}
