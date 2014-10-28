#pragma once;

class Shader{
	char ** f_string;
	int * f_string_length;
	int f_length;
	char ** v_string;
	int * v_string_length;
	int v_length;
	int vertexShaderHandle;
	int fragmentShaderHandle;
	unsigned int programHandle;
	
	float modelViewMatrix[16];	
	int modelViewMatrixHandle;
	float projectionMatrix[16];
	int projectionMatrixHandle;
	float  colorVector[4];
	int colorVectorHandle;
	
	
public :
	static int vertexPositionHandle;
	static int texCoordHandle;
	Shader() {
		programHandle = 0;
		colorVector[0] = 1.0f;
		colorVector[1] = 1.0f;
		colorVector[2] = 1.0f;
		colorVector[3] = 1.0f;
	};
	void load_fragment(const char * filename);
	void load_vertex(const char * filename);
	void bind();
	void unbind();
	void do_register();
	void bind_attributes();
	float * getModelViewMatrix() { return modelViewMatrix;};
	float * getProjectionMatrix() { return projectionMatrix;};
	float * getColorVector() {return colorVector;};
};