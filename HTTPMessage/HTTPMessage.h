#ifndef HTTPMESSAGE_H
#define HTTPMESSAGE_H
/////////////////////////////////////////////////////////////////////
//  HTTPMessage.h - Http messages                                  //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////

/*
Module Operations:
==================
This package is created to help the client and the server to create the HTTP messages
which helps the client and the server to conversate between them. In this design we have
given the user to add different attributes to the HTTP message based on his requirement.
The get functions append the semicolon between the attributes and collect all atributes
to form a string and give to the requestor.

Public Interface:
=================
addAttributeToMsg					// This adds different attributes to for HTTP msg.
getHttpMessage						// This function returns back HTTP msg.

Build Process:
==============
Required files
- StringServer.h,Channel.h,Windows.h,HTTPMessage.h,Filesystem.h,XmlElement.h,StringClient.h,Cpp11-BlockingQueue.h,Logger.h,Sockets.h,Utilities.h
*  Build Command:
*  --------------
* devenv Project_4.sln /rebuild debug

Maintenance History:
====================
ver 1.0

*/
#include <iostream>
#include <unordered_map>

class HTTPMessage {
public:
	std::string getHttpMessage();
	void addAttributeToMsg(std::string attributeName,std::string attributeValue);

private:
	std::string command;
	int toPortNumber;
	int myPortNumber;
	int contentLength;
	std::string content;
	std::unordered_map<std::string, std::string> httpmsg;

};


#endif



