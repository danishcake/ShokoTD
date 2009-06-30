#pragma once
#include "AlignmentVector.h"

class GameReport
{
private:
	AlignmentVector av_;
	bool flawless_;
public:
	GameReport();
	void SetAlignment(AlignmentVector _av){av_ = _av;}
	AlignmentVector GetAlignment(){return av_;}

	void SetFlawless(bool _flawless){flawless_ = _flawless;}
	bool GetFlawless(){return flawless_;}
};
