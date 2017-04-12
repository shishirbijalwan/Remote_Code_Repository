#include "TestExecutive.h"
/////////////////////////////////////////////////////////////////////
//  TestExecutive.cpp - Testing Comeplete application              //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////

#include <exception>

//////////////////////////////////////////////////
///The construtor creates an object of Client Class
TestExecutive::TestExecutive()
{
	client = new StringClient(8080, 8081);
}

//////////////////////////////////////////////////
///The wrapper function to get file list from server
void TestExecutive::getFileList()
{
	client->getFileList();
}

////////////////////////////////////////////////////
///This is a wrapper function to get file from server
void TestExecutive::getFileFromServer(std::string filename)
{
	client->getFileFromServer(filename);
}

////////////////////////////////////////////////////
///This is a wrapper function to connect to server
void TestExecutive::connect()
{
	client->connect();
	client->startCommunication();
}

////////////////////////////////////////////////////
///This is a wrapper function to send file to server
void TestExecutive::FileSender(std::string FilePath, std::string ListOfDependency)
{
	client->FileSender(FilePath, ListOfDependency);
}


/////////////////////////////////////////////////////
///Function to print the requirement
void TestExecutive::Requirement1()
{
	std::cout << "==========================================================================================================" << std::endl;
	std::cout << "Requirement1. Used Visual Studio 2015 and its C++ Windows console projects, as provided in the ECS computer labs. I have also used Windows Presentation Foundation (WPF) to provide a required client Graphical User Interface (GUI)." << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement2. I have used the C++ standard library's streams for all console I/O and new and delete for all heap-based memory management " << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement3. Repository program that provides a  checkin process.Metadata contains a description of the module/package and a list of other modules/packages on which it depends. " << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement4. Required 4 is optional and not been taken care " << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement5. Each File checkin  creates a new directory with a unique name based on the module or package name and checkin-date. Each module directory contains module metadata containing a description of all packages and package metadata that contains a list of all the files in the package" << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement6. Each checkin directory has a boolean closed property(Frozen is the name of xml tag in my case)" << std::endl;
	std::cout << "==========================================================================================================" << std::endl;
	std::cout << " To demonstrate above i have checked in  a File in Repository folder which is in Project Folder. Please do check it for folder name ActionsAndRules.h_V1_<timestamp>" << std::endl;

}

/////////////////////////////////////////////////////
///Function to print the requirement
void TestExecutive::Requirement2()
{
	std::cout << "\n==========================================================================================================" << std::endl;
	std::cout << "Requirement7. Provide an extraction process for modules or packages with or without dependencies. GUI has 2 buttons to get file with and without dependency" << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement8. Provide a message-passing communication system used to access the Repository's functionality from another process or machine. Please see the client sent msg and server recieved msg in individual console." << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement9. Communication system  provides support for passing HTTP style messages using either synchronous request/response or asynchronous one-way messaging.As shown in console." << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement10.  The communication system  also supports sending and receiving streams of bytes6. Streams will be established with an initial exchange of messages. Filesender function is for this purpose in both server and client. The above file transfer was done with help of this function only" << std::endl;
	std::cout << "-----------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "Requirement11.  This is an automated unit test suite that demonstrates i meet all the requirements of this project7 including the transmission of files" << std::endl;
	std::cout << "==========================================================================================================" << std::endl;
	std::cout << "Sending Request to get the List of files in Server" << std::endl;
}
///////////////////////////////////////////////////////////////////////
////It dequeue the message and send back the list of files in Repository
std::string TestExecutive::getMessage()
{
	std::string httpMsg = client->dequeueMsg();
	std::string StringListOfFile = breakMsg(httpMsg);
	return StringListOfFile;
}
/////////////////////////////////////////////////////////////////////////
///This function breaks incoming message and give File list to us as string
std::string TestExecutive::breakMsg(std::string message)
{
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

#ifdef TEST_EXECUTIVE
int main() {
	try{
	TestExecutive test;
	test.connect();
	std::string listOfDependentFiles2 = "No_Dependent_File";
	test.Requirement1();
	test.FileSender("..\\File\\ActionsAndRules.h", listOfDependentFiles2);
	test.Requirement2();
	test.getFileList();
	std::string StringFileList = test.getMessage();
	std::vector<std::string>   result;
	std::stringstream  data(StringFileList);
	std::string line;
	while (std::getline(data, line, ';'))
	{
		result.push_back(line); 
		}
	test.getFileFromServer(result[0]);
	::Sleep(200);
	std::cout << "\nPoints to note from above-" << std::endl;
	std::cout << "\n==========================================================================================================" << std::endl;

	std::cout << "1.Please see the messages used by server and client to communicate" << std::endl;
	std::cout << "2.Please check the download folder in the Project, where the client has received Requested file." << std::endl;
	std::cout << "3.We have tested the application for multiple clients.Currently one client is running in GUI and other is here in Test Executive." << std::endl;
	std::cout << "4.Please do read the user manual and readme file in ReadMe folder in project before testing GUI." << std::endl;

	std::cin.get();
	}
	catch (std::exception& e) {
		std::cout << "Standard exception: " << e.what() << std::endl;
	}
	return 0;
}
#endif