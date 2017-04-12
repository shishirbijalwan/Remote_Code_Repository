#ifndef TESTEXECUTIVE_H
#define TESTEXECUTIVE_H
/////////////////////////////////////////////////////////////////////
//  TestExecutive.h - Testing complete application                 //
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
This package is to test the client server interaction. In this package we
have demostarted that the requirements of the projects 4 are getting full
Filled. We have tried taking a requirement at a time and demonstrate that
Its been taken care but sometimes we have clubed few together and then shown
result.

Public Interface:
=================
getMessage							// This is to dequeue client recieve queue.
postMessage							// This enqueues msg to client send queue.
start							    // This function starts the client.
SendAFile							// Wrapper of send file function of Client.
getFileList							// Wrapper of get file list function of Client.
getFile								//  Wrapper of get file  function of Client.
Requirement1						// Displaying Requirement
Requirement2						// Displaying Requirement
breakMsg							// Breaks the Http message and give File list

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
#include "../StringClient/StringClient.h"

class TestExecutive {
public:
	TestExecutive();
	void getFileList();
	void getFileFromServer(std::string filename);
	void connect();
	void FileSender(std::string FilePath, std::string ListOfDependency);
	void Requirement1();
	void Requirement2();
	std::string getMessage();
	std::string breakMsg(std::string message);
private:
	StringClient* client;
};


#endif




