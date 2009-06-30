#include "stdafx.h"
#include <AlignmentVector.h>

TEST(CheckAlignmentBasics)
{
	AlignmentVector v(50, 30);
	CHECK_EQUAL(50, v.GetGood());
	CHECK_EQUAL(30, v.GetEvil());
	CHECK_EQUAL(58, v.GetGoodScore());
	CHECK_EQUAL(21, v.GetEvilScore());
}