/////////////////////////////////////////////////////////////////////
//  Channel.cpp - GUI native code channel                          //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////
#define IN_DLL
#include "Channel.h"
#include "../StringClient/StringClient.h"
#include <string>
#include <thread>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////
// Channel class
class Channel : public IChannel
{
public:
	Channel(int serverPortNumber, int clientPortNumber);
	void start();
	void stop();
	void postMessage(std::string msg) { Client.enqueueMsg(msg); }
	std::string getMessage();
	void SendAFile(std::string FileName, std::string Dependency) { Client.FileSender(FileName, Dependency); }
	void getFileList() { Client.getFileList(); }
	void getFile(std::string FileName) { Client.getFileFromServer(FileName); }
	std::string breakMsg(std::string message);
	void getSingleFile(std::string FileName) { Client.getSingleFile(FileName); }

private:
	std::thread thread_;
	 StringClient Client;
	bool stop_ = false;

};

/////////////////////////////////////////////////////////////////
////Passing client and server port number to the client
Channel::Channel(int serverPortNumber, int clientPortNumber) : Client(serverPortNumber, clientPortNumber) {
	
}
///////////////////////////////////////////////////////////////////////
////It dequeue the message and send back the list of files in Repository
std::string Channel::getMessage() { 
	std::string httpMsg=Client.dequeueMsg();
	std::string StringListOfFile=breakMsg(httpMsg);	
	return StringListOfFile; }


///////////////////////////////////////////////////////////////////////////
///This function breaks the Http message and helps to create file list of string
std::string Channel::breakMsg(std::string message) {
	std::vector<std::string> messagepart;
	std::string part;

	for (size_t i = 0; i < message.length(); i++) {
		if (message[i] != '\n')
			part += message[i];

		if (message[i] == '\n') {
			messagepart.push_back(part);
			std::string temp;
			part = temp;
		}

	}
	std::string StringListOfFile;
	for (size_t i = 0; i < messagepart.size(); i++) {
		std::string cmd = messagepart[i];
		std::string arg;
		std::string::size_type pos = cmd.find(':');
		if (cmd.npos != pos) {
			arg = cmd.substr(pos + 2);
			cmd = cmd.substr(0, pos);
			if (cmd == "Content") {
				StringListOfFile = arg;
			}
		}
	}

	return StringListOfFile;
}
/////////////////////////////////////////////////////////////////////////////
///This method starts the client and connects it with the server
void Channel::start()
{
	Client.connect();
	Client.startCommunication();
	//std::string listOfDependentFiles2 = "No_Dependent_File";
	//Client.FileSender("..\\File\\ActionsAndRules.h", listOfDependentFiles2);
}
/////////////////////////////////////////////////////////////////////////
////This function is to stop the client(Now GUI handles it so not in use)
void Channel::stop()
{
	//client_.stop();
}

//////////////////////////////////////////////////////////////////////////////
///This is object factory which generates object of Channel class
IChannel* ObjectFactory::createChannel(int serverPortNumber, int clientPortNumber)
{
	return new Channel(serverPortNumber, clientPortNumber);
}

#ifdef TEST_CHANNEL

//----< test stub >----------------------------------------------------------
int main()
{
	ObjectFactory objFact;
	IChannel* pMockChannel = objFact.createChannel(8080, 8081);
	pMockChannel->start();
	pMockChannel->getFileList();
	std::string Msg = pMockChannel->getMessage();
	std::cout << "Msg Dequeued at channel is -" << Msg<<std::endl;
	
	std::string listOfDependentFiles = "..\\..\\Repository\\StringClient.cpp_V_1\\StringClient.cpp";
	std::string listOfDependentFiles2 = "No_Dependent_File";
	std::string listOfDependentFiles3 = "..\\..\\Repository\\StringServer.cpp_V_1\\StringServer.cpp";

	pMockChannel->SendAFile("C:\\Users\\Megha\\Desktop\\Project_4\\File\\StringClient.cpp", listOfDependentFiles3);
	pMockChannel->SendAFile("C:\\Users\\Megha\\Desktop\\Project_4\\File\\ActionsAndRules.h", listOfDependentFiles);
	pMockChannel->SendAFile("C:\\Users\\Megha\\Desktop\\Project_4\\File\\StringServer.cpp", listOfDependentFiles2);

	pMockChannel->getFileList();
	 Msg = pMockChannel->getMessage();
	std::cout << "Msg Dequeued at channel is -" << Msg << std::endl;

	pMockChannel->getFile("..\\..\\Repository\\ActionsAndRules.h_V_1\\ActionsAndRules.h");
	std::cin.get();
}
#endif