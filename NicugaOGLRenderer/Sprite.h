#pragma once 
#include <vector>

class Texture;
class BufferData;
class Sprite {
public :
	Sprite(Texture * texture,float width,float height,int u,int v, int u2, int v2);
	virtual void draw();
	void updateTexture();
	void updateQuad(float x1, float y1, float x2, float y2);
	void updateQuad(float x1, float y1, float x2, float y2,float x3, float y3, float x4, float y4);
	void setPosition(float x, float y) {
		this->x = x;
		this->y = y;
	}
	float getX() { return this->x;}
	float getY() { return this->y;}
	float getHeight() { return this->height;}
	float getWidth() { return this->width;}
protected :
	Texture * texture;
	BufferData * bufferData;
	int u;
	int v;
	int u2;
	int v2;
	float x;
	float y;
	float width;
	float height;
	float * vb;
};

class SpriteTrail : public Sprite{
public : 
	virtual void draw();
	void updateCoordinates(float * coordinates);
private :
	float * coordinates;
};


class SpriteLoop {
private :
	SpriteLoop(int frameLength) {
		this->frameLength = frameLength;
	}
public :
	int current;
	int frameLength;
	std::vector<Sprite *> spriteList;
	void draw();
};