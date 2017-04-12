#ifndef CHANNEL_H
#define CHANNEL_H

/////////////////////////////////////////////////////////////////////
//  Channel.h - GUI native code channel                            //
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
The main aim for creation of this package is to create the link between the naitive code
that is the client class and the managed code that is the GUI. In this package we have created
a Interface Ichannel and the object factory of it. The class Channel acts as the wrapper class
in case of this application for the client class. This packages has been compiled as the dll.

Public Interface:
=================
getMessage							// This is to dequeue client recieve queue.
postMessage							// This enqueues msg to client send queue.
start							    // This function starts the client.
SendAFile							// Wrapper of send file function of Client.
getFileList							// Wrapper of get file list function of Client.
getFile								//  Wrapper of get file  function of Client.
createChannel						// Object factory of the Ichannel.

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

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>

struct IChannel
{
public:
	virtual std::string getMessage() = 0;
	virtual void postMessage(std::string msg) = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void SendAFile(std::string FileName, std::string Dependency = "No_Dependent_File") = 0;
	virtual void  getFileList() = 0;
	virtual void getFile(std::string FileName) = 0;
	virtual void getSingleFile(std::string FileName) = 0;
};

extern "C" {
	struct ObjectFactory
	{
		DLL_DECL IChannel* createChannel(int serverPortNumber, int clientPortNumber);
	};
}

#endif