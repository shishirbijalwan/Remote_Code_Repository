#ifndef STRINGCLIENT_H
#define STRINGCLIENT_H
/////////////////////////////////////////////////////////////////////
//  StringClient.h -This has the Client and the listener helper    //
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
This package has both the client definition as well the listener class to help the client to 
recieve the incoming messages and file. The client class has two blocking queue present in it
which it uses to store the incoming and outgoing messages. While creating the object of the client
we pass the client port information to it. It has a thread to send the messages in the send blocking 
queue and the dequeuing part of the received msg is done by the GUI.

Public Interface:
=================
enqueueMsg                       // This is to enqueue msg in the sending queue.
dequeueMsg			             // This is to enqueue msg in the receive queue.
StringClient		             // The class constructor assigns server and client port number.
FileSender                      // The helper function to send file.
getRecMsg           			// This function helps in decipher meaning of Http msg.
getRecieveMsgQueue			    // Returns Recieve queue by reference
breakMsg						// Breaks Http msg to Unordered_map
getFileList						// Created Http msg to get file list
startCommunication				// Starts sender thread
getFileFromServer				// Extract file from server
ReceiveMsg						// To initialize blocking queue of Receive msg class with client blocking queue
readFile						// Function to read incoming files from server
fileTrasnferHelper				// Helps in file transfer

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
#include <string>
#include <iostream>
#include <thread>
#include "../Project_4/HTTPMessage.h"
#include "../FileSystem/FileSystem.h"
#include <experimental/filesystem>

using Show = StaticLogger<1>;
using namespace Utilities;
using Message = std::string;
using BQueue = BlockingQueue < Message >;

//////////////////////////////////////////////////////////////////////
//Receiver_Helper acts as listener
class ReceiverHelper {
public:
	ReceiverHelper(BQueue& bqueue) :ReceiveMsg(bqueue) {
	}
	void operator()(Socket& socket_);	
	void RemoveLine(std::string& source, const std::string& to_remove);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	void fileTrasnferHelper(std::vector<std::string> info, std::string matter);
	std::unordered_map<std::string, std::string> breakMsg(std::string message);
private:
	BQueue& ReceiveMsg;
	std::string TempFolderPath = "..\\Download\\";
};


/////////////////////////////////////////////////////////////////////
// StringClient class

class StringClient
{
public:
	void enqueueMsg(Message Msg) { SendMsg.enQ(Msg); }
	std::string dequeueMsg() {return RecieveMsg.deQ(); }
	StringClient(int portNumber, int clientport);
	int get_file_size(std::string filename);
	void FileSender(std::string FilePath, std::string ListOfDependency);
	void getRecMsg();	
	BQueue& getRecieveMsgQueue() { return RecieveMsg; }
	std::unordered_map<std::string, std::string> breakMsg(std::string message);
	void getFileListHelper(std::unordered_map<std::string, std::string> httpmsg);
	void startCommunication();
	void connect();
	void getFileList();
	void getFileFromServer(std::string filename);
	void getSingleFile(std::string filename);
private:
	int serverPortNumber;
	int clientPortNumber;
	BQueue RecieveMsg;
	std::thread thread_;
	std::vector<std::string> ServerFileList;
	BQueue SendMsg;
	std::thread Senderthread_;
	std::thread RecieverThread_;
	bool stop_ = false;
	SocketSystem ss;
	SocketConnecter si;
	SocketListener sl;
	ReceiverHelper rHelper;
};

#endif



