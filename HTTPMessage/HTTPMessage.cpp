/////////////////////////////////////////////////////////////////////
//  HTTPMessage.cpp - Http messages                                //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include "HTTPMessage.h"


#ifdef TEST_HTTPMESSAGE
int main()
{
	HTTPMessage httpMessage("Get the file", 1001, 1002);
	httpMessage.setContentLength(10);
	httpMessage.setContent("This is the content ");
	
	std::cout << "This is main display" << std::endl;
	std::cout << "This is Message 1" << std::endl;

	std::cout << httpMessage.getHttpMessage();

	std::cout << "\n\n\n";
	std::cout << "This is Message 2" << std::endl;

	HTTPMessage httpMessage2("Checkin a file", 555, 312);
	httpMessage2.setContentLength(10);
	httpMessage2.setContent("This is the content ");

	std::cout << httpMessage2.getHttpMessage();

	return 0;
}

#endif



///////////////////////////////////////////////////////////////////////////////
///This function add all the attributes into one seperated by colon and newline
std::string HTTPMessage::getHttpMessage()
{
	std::string combineAttributes;

	for (std::unordered_map<std::string, std::string>::iterator it = httpmsg.begin(); it != httpmsg.end(); ++it) {
	
		std::string temp = it->first + ":" + it->second + "\n";
		combineAttributes = combineAttributes + temp;

	}

	return combineAttributes;
}
///////////////////////////////////////////////////////////////////////////////////
////This function adds the attribute to an unordered map
void HTTPMessage::addAttributeToMsg(std::string attributeName, std::string attributeValue)
{
	std::pair<std::string, std::string> temp(attributeName, attributeValue);
	httpmsg.insert(temp);
}
