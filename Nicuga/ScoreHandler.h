#pragma once

class IScoreHandler 
{
public :
	virtual void resetScore() = 0;
	virtual void updateScore(int hostileScore, char * outMessage) = 0;
	virtual int getScore() = 0;
};

class SimpleScoreHandler : public IScoreHandler {
private :
	int score;
public :
	virtual void resetScore();
	virtual void updateScore(int hostileScore, char * outMessage);
	virtual int getScore();
};

