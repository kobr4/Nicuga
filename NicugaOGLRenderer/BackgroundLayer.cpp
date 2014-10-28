#include "BackgroundLayer.h"
#include "Sprite.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
void BackgroundLayer::draw(Shader * shader) {
	update(0);
	int count = 0;
	glm::mat4 myMatrix = glm::mat4();
	float wround = floor(this->currentPosition[0] / w) * w - this->currentPosition[0];
	float hround = floor(this->currentPosition[1] / h) * h - this->currentPosition[1];
	for (float x = wround;x < screenWidth;x += w) {
		for (float y = hround;y < screenHeight;y += h) {
			sprite->setPosition(x,y);
			myMatrix = glm::translate(glm::mat4(), glm::vec3(sprite->getX(), sprite->getY(), 0.0f));
			memcpy(shader->getModelViewMatrix(), glm::value_ptr(myMatrix),sizeof(float)*16);
			shader->bind_attributes();
			sprite->draw();
			count++;
		}
	}

	
}

void BackgroundLayer::update(int dt) {
	this->currentPosition[0] +=  this->scrollSpeed[0];
	this->currentPosition[1] +=  this->scrollSpeed[1];
}