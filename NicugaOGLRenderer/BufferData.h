#pragma once

class BufferData {
	unsigned int bufferName;
	char * bufferData;
	int bufferSize;
public :
	BufferData(char * bufferData,int bufferSize);
	void updateBuffer();
	void do_register();
	void bind();
	void unbind();
};