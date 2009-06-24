#include "Skills.h"

namespace Skills
{
	static const char* EnumStr[] = {STRME(Arrows),
								STRME(Slow),
								STRME(Fireball)};

	Enum FromString(std::string _skill)
	{
		for(int i = 0; i < sizeof(EnumStr) / sizeof(EnumStr[0]); i++)
		{
			if(EnumStr[i] == _skill)
				return Enum(i);
		}
	}

	std::string ToString(Enum _skill)
	{
		return EnumStr[_skill];
	}
}