#pragma once

class AlignmentVector
{
private:
	unsigned int good_;
	unsigned int evil_;
	unsigned int neutral_;
public:
	AlignmentVector(unsigned int _good, unsigned int _evil, unsigned int _neutral);

	unsigned int GetGood(){return good_;}
	unsigned int GetEvil(){return evil_;}
	unsigned int GetNeutral(){return neutral_;}

	unsigned int GetGoodScore();
	unsigned int GetEvilScore();
	unsigned int GetNeutralScore();

	unsigned int GetSum(){return good_ + evil_ + neutral_;}
};