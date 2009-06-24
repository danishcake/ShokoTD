#pragma once
#include "AlignmentVector.h"

class GameReport
{
private:
	AlignmentVector av_;
public:
	GameReport();
	void SetAlignment(AlignmentVector _av){av_ = _av;}
	AlignmentVector GetAlignment(){return av_;}
};
