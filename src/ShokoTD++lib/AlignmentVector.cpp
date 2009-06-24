#include "AlignmentVector.h"
#include <cmath>

AlignmentVector::AlignmentVector(unsigned int _good, unsigned int _evil, unsigned int _neutral) :
	good_(_good), evil_(_evil), neutral_(_neutral)
{

}

unsigned int AlignmentVector::GetGoodScore()
{
	double mag = sqrtf(good_ * good_ + evil_ * evil_+ neutral_ * neutral_);
	double sum = good_ + evil_ + neutral_;
	double good = (double)good_ / mag;
	good = good * good;

	return good * sum;
}

unsigned int AlignmentVector::GetEvilScore()
{
	double mag = sqrtf(good_ * good_ + evil_ * evil_+ neutral_ * neutral_);
	double sum = good_ + evil_ + neutral_;
	double evil = (double)evil_ / mag;
	evil = evil * evil;

	return evil * sum;
}

unsigned int AlignmentVector::GetNeutralScore()
{
	double mag = sqrtf(good_ * good_ + evil_ * evil_+ neutral_ * neutral_);
	double sum = good_ + evil_ + neutral_;
	double neutral = (double)neutral_ / mag;
	neutral = neutral * neutral;

	return neutral * sum;
}
