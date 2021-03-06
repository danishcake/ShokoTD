#include "Skills.h"

namespace Skills
{
	static const char* EnumStr[] = {STRME(Arrows),
								STRME(Slow),
								STRME(Burn),
								STRME(Craze), 
								STRME(SpawnPause), 
								STRME(PermaSlow)};

	Enum FromString(std::string _skill)
	{
		for(int i = 0; i < sizeof(EnumStr) / sizeof(EnumStr[0]); i++)
		{
			if(EnumStr[i] == _skill)
				return Enum(i);
		}
		return Skills::Arrows;
	}

	std::string ToString(Enum _skill)
	{
		return EnumStr[_skill];
	}
}