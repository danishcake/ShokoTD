#pragma once
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>


namespace TextAlignment
{
	enum Enum
	{
		TopLeft, Top, TopRight, Left, Centre, Right, BottomLeft, Bottom, BottomRight
	};
}

struct WidgetText
{
	std::string text_;
	std::vector<std::string> text_lines_;
public:
	void SetText(std::string _text)
	{
		text_ = _text;
		boost::split(text_lines_, _text, boost::is_any_of("\n"));

	}
	std::string GetText(){return text_;}
	std::vector<std::string> GetTextLines(){return text_lines_;}
	TextAlignment::Enum alignment;
	WidgetText(){text_=""; alignment=TextAlignment::Centre;}
};