#ifndef STRINGSERVER_H
#define STRINGSERVER_H
/////////////////////////////////////////////////////////////////////
//  StringServer.h -This has the Server and the listener helper    //
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
This package has both the server definition as well the listener class to help the server to
recieve the incoming messages and file. The server class has one blocking queue present in it
which it uses to store the incoming messages. While creating the object of the serverwe pass the
server port information to it. It has a thread to work on the messages in the receive blocking
queue.The receive msg queue get enqueued by the listener class which has the reference of the server
blocking queue.

Public Interface:
=================
getRecMsg                       // This functions returns reference of recieve blocking queue.
ReplyMsg			             // This starts the thread which works on receive msg queue.
breakMsg						// Breaks Http msg to Unordered_map
fileTrasnferHelper              // The helper function in recieving file.
CheckInHelper           	    // This function helps in checkin the file.
getFileListHelper			    // This function helps in getting the file list.
GetFiles						// This function helps in getting the file.
getFileList						// Created Http msg to get file list.
FileSender						// Helps in sending file to client.
versionHelper					// Helps in maintaining version.
XMLHelper						// Helps in creating xml file.
Dependencygetter				// Helps in getting dependency.
getTimeStampString				// Gives timestamp in string.
readFile						// Reads the incoming file and create it in temp.
ClientHandler					// Listener class constructor to get refernce of blocking queue.

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

#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../FileSystem/FileSystem.h"
#include "../XmlElement/XmlElement.h"
#include "../Project_4/HTTPMessage.h"

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <experimental/filesystem>
#include "../FileSystem/FileSystem.h"

using Show = StaticLogger<1>;
using BQueue = BlockingQueue < std::string >;
using namespace FileSystem;
using namespace XmlProcessing;

//////////////////////////////////////////////////////////////////////////////
////This acts as a listener class for server
class ClientHandler
{
public:
	void operator()(Socket& socket_);
		std::unordered_map<std::string, std::string> breakMsg(std::string message);
		bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	
	ClientHandler(BQueue& bqueue) :RecMsg(bqueue) {	}

private:
	BQueue&  RecMsg;
	std::string TempFolderPath = "..\\Temp\\";
	std::string RepositoryPath = "..\\Repository\\";


};

/////////////////////////////////////////////////////////////////
///Sever Class
class ServerRepos {

public:
	BQueue& getRecMsg() { return RecMsg; }
	void  RecReply();
	std::unordered_map<std::string, std::string> breakMsg(std::string message);
	void RemoveLine(std::string& source, const std::string& to_remove);
	void fileTrasnferHelper(std::vector<std::string> info, std::string matter);
	bool checkTempFiles(std::string filename);
	void CheckInHelper(std::unordered_map<std::string, std::string> httpmsg);
	void getFileListHelper(std::unordered_map<std::string, std::string> httpmsg);
	void GetFiles(std::unordered_map<std::string, std::string> httpmsg);
	void FileSender(std::string FilePath, int ServerPortNumber, int ClientPortNumber);
	int versionHelper(std::string filename);
	int get_file_size(std::string filename);
	std::string XMLHelper(std::string nameOfFile, int portNumber, std::string DependencyList, std::string XMLFileName);
	void Dependencygetter(std::vector<std::string>& dependencyVector, std::string currentFile);
	std::string getTimeStampString();
	void GetSingleFile(std::unordered_map<std::string, std::string> httpmsg);


private:
	BQueue  RecMsg;
	bool flag = true;
	std::thread thread_;
	std::thread Replythread_;
	std::vector<std::string> ListOfFile;
	std::vector<std::string> tempFiles;
	BQueue SendMsg;
	bool stop_ = false;
	std::unordered_map<std::string, int> versionTable;
	std::unordered_map<std::string, std::string> dependencyTable;
	std::string TempFolderPath = "..\\Temp\\";
	std::string RepositoryPath = "..\\Repository\\";
	ClientHandler* pClientHandler;
	SocketSystem ss;
	SocketConnecter si;

};

#endif



