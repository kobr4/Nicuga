#pragma once
class Texture;
class FrameBuffer {
private :
	unsigned int fb;
	unsigned int depthRb;
	unsigned int renderTex;
	int width;
	int height;
	unsigned char * pixels;
public :
	FrameBuffer(int width,int height) {
		this->width = width;
		this->height = height;
		this->fb = 0;
		this->depthRb = 0;
		this->renderTex = 0;
	}
	void bind();
	void unbind(int screenWidth, int screenHeight);
	void do_register();
	void draw(int screenWidth, int screenHeight);
	Texture * getTexture();
};
