#pragma once
#include <string>
#define STRME(x) \
(#x)

namespace Skills
{
	enum Enum
	{
		Arrows, Slow, Fireball,

		None
	};

	Enum FromString(std::string _skill);

	std::string ToString(Enum _skill);
}