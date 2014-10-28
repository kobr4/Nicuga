#pragma once 
#include <vector>
class Sprite;
class Shader;
class BackgroundLayer {
private :
	int screenWidth;
	int screenHeight;
	float scrollSpeed[3];
	float currentPosition[3];

	Sprite * sprite;
	float w;
	float h;
public :
	BackgroundLayer(int screenWidth, int screenHeight, float scrollSpeedX = 0.0, float scrollSpeedY = -2.0) {
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
		scrollSpeed[0] = scrollSpeedX;
		scrollSpeed[1] = scrollSpeedY;
		scrollSpeed[2] = 0.0;
		currentPosition[0] = 0.0;
		currentPosition[1] = 0.0;
		currentPosition[2] = 0.0;
	}
	void update(int dt);
	void draw(Shader * shade);
	void addElement(Sprite * sprite,int width,int height) {
		this->sprite = sprite;
		w = (float)width;
		h = (float)height;
	}
};

class BackgroundManager {
private :
	std::vector<BackgroundLayer*>  listLayer;
public :
	void addLayer(BackgroundLayer * backgroundLayer){
		listLayer.push_back(backgroundLayer);
	}

	void update(int dt) {
		for(int i = 0;i < listLayer.size();i++) {
			listLayer[i]->update(dt);
		}
	}

	void draw(Shader * shader) {
		for(int i = 0;i < listLayer.size();i++) {
			listLayer[i]->draw(shader);
		}
	}

};