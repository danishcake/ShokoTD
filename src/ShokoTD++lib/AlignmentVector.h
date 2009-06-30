#pragma once

class AlignmentVector
{
private:
	unsigned int good_;
	unsigned int evil_;
public:
	AlignmentVector(unsigned int _good, unsigned int _evil);

	unsigned int GetGood(){return good_;}
	unsigned int GetEvil(){return evil_;}

	unsigned int GetGoodScore();
	unsigned int GetEvilScore();

	unsigned int GetSum(){return good_ + evil_;}
};