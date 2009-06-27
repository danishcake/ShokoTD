#include "stdafx.h"
#include <tinyxml.h>
#include <string>

TEST(XML_template_specialisation)
{
	TiXmlDocument doc("xml_doc.xml");
	TiXmlElement el("Root");
	el.SetAttribute("attr", "something with spaces");
	
	doc.InsertEndChild(el);
	doc.SaveFile();
	

	TiXmlDocument doc2("xml_doc.xml");
	doc2.LoadFile();
	TiXmlElement* el2 = doc2.FirstChildElement("Root");
	std::string str;
	//el2->QueryValueAttribute("attr", &str);
	el2->QueryValueAttribute("attr", &str);

	CHECK_EQUAL("something with spaces", str);
	
}