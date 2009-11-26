#include "AlignmentVector.h"
#include <cmath>

AlignmentVector::AlignmentVector(unsigned int _good, unsigned int _evil) :
	good_(_good), evil_(_evil)
{

}

unsigned int AlignmentVector::GetGoodScore()
{
	double mag = sqrt(static_cast<double>(good_ * good_ + evil_ * evil_));
	double sum = good_ + evil_;
	double good = (double)good_ / mag;
	good = good * good;

	return static_cast<unsigned int>(good * sum);
}

unsigned int AlignmentVector::GetEvilScore()
{
	double mag = sqrt(static_cast<double>(good_ * good_ + evil_ * evil_));
	double sum = good_ + evil_;
	double evil = (double)evil_ / mag;
	evil = evil * evil;

	return static_cast<unsigned int>(evil * sum);
}
