#include "stdafx.h"
#include <AlignmentVector.h>

TEST(CheckAlignmentBasics)
{
	AlignmentVector v(50, 30, 10);
	CHECK_EQUAL(50, v.GetGood());
	CHECK_EQUAL(30, v.GetEvil());
	CHECK_EQUAL(10, v.GetNeutral());
	CHECK_EQUAL(64, v.GetGoodScore());
	CHECK_EQUAL(23, v.GetEvilScore());
	CHECK_EQUAL(2, v.GetNeutralScore());
}