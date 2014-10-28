#include "ScoreHandler.h"

void SimpleScoreHandler::resetScore() {
	this->score = 0;
}

void SimpleScoreHandler::updateScore(int hostileScore, char * outMessage) {
	this->score += hostileScore;
}

int SimpleScoreHandler::getScore() {
	return this->score;
}