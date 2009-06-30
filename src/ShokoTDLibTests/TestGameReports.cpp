#include "stdafx.h"
#include <GameReport.h>

TEST(CheckReportBasics)
{
	GameReport gr;
	AlignmentVector av(100, 25);
	gr.SetAlignment(av);
	CHECK_EQUAL(av.GetGood(), gr.GetAlignment().GetGood());
}