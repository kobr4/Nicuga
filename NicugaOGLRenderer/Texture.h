#pragma once;

class Texture {
private :
	unsigned int width;
	unsigned int height;
	unsigned int textureName;
	unsigned char * pixels;
public :
	Texture(int width, int height,unsigned char * pixels) {
		this->width = width;
		this->height = height;
		this->pixels = pixels;
		this->textureName = 0;
	}

	Texture(int width, int height,int textureName) {
		this->width = width;
		this->height = height;
		this->textureName = textureName;
	}
	void bind();
	void unbind();
	void do_register();
	void update();
};