/////////////////////////////////////////////////////////////////////
//  StringServer.cpp -This has the Server and the listener helper  //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////
#include "../Sockets/Sockets.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../FileSystem/FileSystem.h"
#include "../XmlElement/XmlElement.h"
#include "../Project_4/HTTPMessage.h"
#include "StringServer.h"
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


//////////////////////////////////////////////////////////////////////////////////////////////////////
///This function dequeues the receive queue and based on the Http msg calls different helper methods
void ServerRepos::RecReply() {	
		BQueue& recvQ = RecMsg;
		while (flag)
		{
			std::string msg = recvQ.deQ();  // will block here so send quit message when stopping
			if (msg == "quit")
				break;
			std::unordered_map<std::string, std::string> messagepart = breakMsg(msg);
			std::unordered_map<std::string, std::string>::iterator it;
			it = messagepart.find("command");
			if (it != messagepart.end()) {
				if (it->second == "CHECKIN") {
					CheckInHelper(messagepart);
				}					
				else if (it->second == "Get_the_file")
					GetFiles(messagepart);
				else if (it->second == "Get_File_List")
					getFileListHelper(messagepart);
				else if (it->second == "Get_Single_File")
					GetSingleFile(messagepart);
				else
					std::cout << "Unknown Command found";			
			}			
		}
		std::cout << "\n  Server stopping\n\n";
}

/////////////////////////////////////////////////////////////////////////////////
////Break HTTP Msg based on new line and semicolom.Return an unordered map of the HTTP msg
std::unordered_map<std::string, std::string> ServerRepos::breakMsg(std::string message)
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
	std::unordered_map<std::string, std::string> tempMap;
	for (size_t i = 0; i < messagepart.size(); i++) {
		std::string cmd = messagepart[i];
		std::string arg;

		std::string::size_type pos = cmd.find(':');
		if (cmd.npos != pos) {
			arg = cmd.substr(pos + 2);
			cmd = cmd.substr(0, pos);
			//	std::cout  << cmd << "   " << arg <<std::endl;
			std::pair<std::string, std::string> p(cmd, arg);
			tempMap.insert(p);
		}
	}
	return tempMap;
}
////////////////////////////////////////////////////////////////////////////
////Break HTTP Msg based on new line and semicolom.Return an unordered map of the HTTP msg
std::unordered_map<std::string, std::string> ClientHandler::breakMsg(std::string message)
{
	std::vector<std::string> messagepart;
	std::string part;

	for (size_t i = 0; i < message.length(); i++) {
	if(message[i] != '\n')
		part += message[i];

		if (message[i] == '\n') {
			messagepart.push_back(part);
			std::string temp;
			part = temp;
		}

	}
	std::unordered_map<std::string, std::string> tempMap;
	for (size_t i = 0; i < messagepart.size(); i++) {
		std::string cmd = messagepart[i];
		std::string arg;

		std::string::size_type pos = cmd.find(':');
		if (cmd.npos != pos) {
			arg = cmd.substr(pos + 2);
			cmd = cmd.substr(0, pos);
			std::pair<std::string, std::string> p(cmd, arg);
			tempMap.insert(p);
		}
	}
	return tempMap;
}
////////////////////////////////////////////////////////////
////Operator overloading of the Client Handler class which helps the server to enqueue the received
//msg to the recieve queue of the client
void ClientHandler::operator()(Socket& socket_)
{
  while (true)
  {		  
	  std::string msg = socket_.recvString();
	  if (msg.size() == 0)
		  continue;
	if (msg == "quit") {
		break;
	}
	std::cout << "\nMessage Received at Server\n" << msg << std::endl;
	std::size_t found = msg.find("command:");
	if (found != std::string::npos) {
	found = msg.find("File_Coming");
		if (found != std::string::npos) {
		std::unordered_map<std::string,std::string> msgParts=	breakMsg(msg);
		std::unordered_map<std::string, std::string>::iterator it;
		it = msgParts.find("Content");
		std::string Filename = it->second;
		it = msgParts.find("Dependency");
		std::string Dep = it->second;
		it = msgParts.find("ContentLength");
		int length = atoi(it->second.c_str());
		it = msgParts.find("SenderAddress");
		int portNumberOwner = atoi(it->second.c_str());
		std::cout << "Msg in Socket" << std::endl;
		std::cout << msg << std::endl;		
		readFile(Filename, length, socket_);
		}else{
		RecMsg.enQ(msg);		
		}
	}  
  }
}
//////////////////////////////////////////////////////////////////////////
////This function helps in creating incoming file in the tem directory
bool ClientHandler::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	std::string fqname = TempFolderPath + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{		
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);
		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close(); 
	return true;
}
///////////////////////////////////////////////////////
//Remove a line from string
void ServerRepos::RemoveLine(std::string& source, const std::string& to_remove)
{
	size_t m = source.find(to_remove);
	size_t n = source.find_first_of("\n", m + to_remove.length()); 
	source.erase(m, n - m + 1);
}
/////////////////////////////////////////////////////////////////////////////////
////This function helps in creating file in Temporary Folder(Not getting used now)
void ServerRepos::fileTrasnferHelper(std::vector<std::string> info, std::string matter)
{
	std::string p = TempFolderPath + "File.txt"; /// hardcoded here
	std::ofstream logging;
	logging.open(p, std::ios_base::app);
	logging << matter;
}
/////////////////////////////////////////////////////////
///Checking Temp File  was created or not(Not Used now)
bool ServerRepos::checkTempFiles(std::string filename)
{
	for (size_t i = 0; i < tempFiles.size(); i++) {
	
		if (tempFiles[i] == filename) {
			return true;
		}
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////
/////This method helps in checkin the file inside the repository
void ServerRepos::CheckInHelper(std::unordered_map<std::string, std::string> httpmsg)
{
	std::string fileName;
	int length;
	std::string dependentFiles;
	int portNumberOwner;	
	std::unordered_map<std::string, std::string>::iterator it;
	it = httpmsg.find("Content");
	fileName = it->second;
	std::unordered_map<std::string, std::string>::iterator it1;
	it1 = httpmsg.find("Dependency");
	dependentFiles = it1->second;
	std::unordered_map<std::string, std::string>::iterator it2;
	it2 = httpmsg.find("ContentLength");
	length = atoi(it->second.c_str());
	it = httpmsg.find("SenderAddress");
	portNumberOwner = atoi(it->second.c_str());
	int versionNumber= versionHelper(fileName) ;
	std::string tempFile = TempFolderPath + fileName;
	std::string timestamp = getTimeStampString(); // Calls function to get timestamp in string
	std::string XmlFileName = fileName.substr(0, fileName.find(".", 0));
	std::string RepositoryDirectory = RepositoryPath;
	std::string folder = fileName + "_V_" + std::to_string(versionNumber);
	std::string Fullpath = RepositoryDirectory + folder+ timestamp;
	std::string destination = Fullpath + "\\" + fileName;
	FileSystem::Directory::create(Fullpath);
	FileSystem::File::copy(tempFile, destination);
	FileSystem::File::remove(tempFile);
	std::pair<std::string, std::string> tempEntry(destination, dependentFiles);
	dependencyTable.insert(tempEntry);
	XmlFileName = Fullpath +"\\"+XmlFileName + ".xml";
	XMLHelper(fileName, portNumberOwner, dependentFiles, XmlFileName);	// call XML generation function
}
///////////////////////////////////////////////////////////////////////////////////////////////
////This method helps in getting file from repository and sending it to the client
void ServerRepos::getFileListHelper(std::unordered_map<std::string, std::string> httpmsg)
{	std::string stringList;
	std::unordered_map<std::string, std::string>::iterator it;
	it = httpmsg.find("ToAddress");
	int serverport = atoi(it->second.c_str());
	it = httpmsg.find("SenderAddress");
	int clientPort = atoi(it->second.c_str());
	for (std::unordered_map<std::string, std::string>::iterator it = dependencyTable.begin(); it != dependencyTable.end(); ++it) {
		if(stringList.size()!=0)
		stringList = stringList + ";" + it->first;
		else
			stringList = it->first;	
	}

	while (!si.connect("localhost", clientPort))
	{
		Show::write("\n server waiting to connect to client");
		::Sleep(100);
	}
	HTTPMessage* httpMessage = new HTTPMessage;
	httpMessage->addAttributeToMsg("command:", "Get_File_List");
	httpMessage->addAttributeToMsg("ToAddress:", std::to_string(clientPort));
	httpMessage->addAttributeToMsg("SenderAddress:", std::to_string(serverport));
	httpMessage->addAttributeToMsg("Content:", stringList);
	si.sendString(httpMessage->getHttpMessage());
	si.sendString("quit");
	si.close();
}
////////////////////////////////////////////////////////////////////////////////////////////
///This method helps in sending the file  to client
void ServerRepos::GetFiles(std::unordered_map<std::string, std::string> httpmsg)
{
	std::string FileName;
	std::unordered_map<std::string, std::string>::iterator it;
	it = httpmsg.find("Content");
	FileName = it->second;
	it = httpmsg.find("ToAddress");
	int serverport = atoi(it->second.c_str());
	it = httpmsg.find("SenderAddress");
	int clientPort = atoi(it->second.c_str());
	std::unordered_map<std::string, std::string>::iterator it2;
	it2 = dependencyTable.find(FileName);
	std::string dependencyListString = it2->second;
	std::vector<std::string>   result;
	result.push_back(FileName);

	Dependencygetter(result, FileName); // Calss function to get the dependency array list
	
	for (size_t i = 0; i < result.size(); i++) {

		FileSender(result[i], serverport, clientPort); // sending files one by one
	}		
		si.close();
		si.sendString("quit");
}

///////////////////////////////////////////////////////////////////////////
///Helper Method to send the file from server to client
void ServerRepos::FileSender(std::string FilePath,int ServerPortNumber,int ClientPortNumber) {
	std::string ReadFilePath = FilePath;
	std::string FileName = FilePath.substr(FilePath.find_last_of("/\\") + 1);
	FileSystem::FileInfo fi(ReadFilePath);
	size_t filesize = fi.size();
	std::string  sizeString = std::to_string(filesize);
	if (!FileSystem::File::exists(ReadFilePath)) {
		std::cout << "Input doesn't exist" << std::endl;
		return;
	}
	FileSystem::File ReadFile(ReadFilePath);
	ReadFile.open(FileSystem::File::in, FileSystem::File::binary);
	if (!ReadFile.isGood())
	{
		std::cout << "Error while opening input file" << std::endl;
		return;
	}
	std::string ListOfDependency;
	while (!si.connect("localhost", ClientPortNumber))
	{
		Show::write("\n server waiting to connect");
		::Sleep(100);
	}
	HTTPMessage* httpMessage1 = new HTTPMessage;
	httpMessage1->addAttributeToMsg("command:", "File_Coming");
	httpMessage1->addAttributeToMsg("ToAddress:", std::to_string(ClientPortNumber));
	httpMessage1->addAttributeToMsg("SenderAddress:", std::to_string(ServerPortNumber));
	httpMessage1->addAttributeToMsg("ContentLength:", sizeString);
	httpMessage1->addAttributeToMsg("Content:", FileName);
	si.sendString(httpMessage1->getHttpMessage());
	std::cout << "\nMessage sent from server\n" << httpMessage1->getHttpMessage() << std::endl;
	const int BufferLength = 2048;
	Socket::byte buffer[BufferLength];
	while (true)
	{
		FileSystem::Block blk = ReadFile.getBlock(BufferLength);
		if (blk.size() == 0)
			break;
		for (size_t i = 0; i < blk.size(); i++)
			buffer[i] = blk[i];
		si.send(blk.size(), buffer);
		if (!ReadFile.isGood())
			break;		
	}
	ReadFile.close();		
}



////////////////////////////////////////////////////////////////////////////
/////This method hels in maintaining the versionNumber in the Repository
int ServerRepos::versionHelper(std::string tablename)
{
	std::unordered_map<std::string, int>::iterator it;
	int versionNumber;
	it = versionTable.find(tablename);
	if (it != versionTable.end()) {
		int temp = it->second;
		versionNumber = temp + 1;
		it->second = versionNumber;
	}
	else {
		std::pair<std::string, int> p(tablename, 1);
		versionTable.insert(p);
		versionNumber = 1;
	}
	return versionNumber;
}
///////////////////////////////////////////////////////////////////////
///This function returns the file size
int ServerRepos::get_file_size(std::string filename) // path to file
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}
/////////////////////////////////////////////////////////////////////////
///Helper For creating Xml File
std::string ServerRepos::XMLHelper(std::string nameOfFile, int portNumber, std::string DependencyList, std::string XMLFileName)
{	using sPtr = std::shared_ptr < AbstractXmlElement >;
	///Creating Main Root XML
	sPtr root = makeTaggedElement("MetaData");
	///////Adding name of the File as name element
	sPtr name = makeTaggedElement("Name");
	name->addChild(makeTextElement(nameOfFile));
	root->addChild(name);
	//////////////////////////////////////////
	/////Adding owner
	sPtr owner = makeTaggedElement("Owner");
	owner->addChild(makeTextElement(std::to_string(portNumber)));
	root->addChild(owner);
	///////////////////////////////
	///Creating sub Root Under XML to hold Dependency
	sPtr Deps = makeTaggedElement("Deps");
	root->addChild(Deps);

	std::vector<std::string>   result;
	std::stringstream  data(DependencyList);
	std::string line;
	while (std::getline(data, line, ';'))
	{
		result.push_back(line); // Note: You may get a couple of blank lines
	}
	for (size_t i = 0; i < result.size(); i++) {
		sPtr child1 = makeTaggedElement("Dep");
		child1->addChild(makeTextElement(result[i]));
		Deps->addChild(child1);
	}
	///// Adding Frozen to the XML document
	sPtr Frozen = makeTaggedElement("Frozen");
	Frozen->addChild(makeTextElement("Yes"));
	root->addChild(Frozen);
	sPtr docEl = makeDocElement(root);
	std::string xmlString = docEl->toString();
	std::string p = XMLFileName;
	std::ofstream logging;
	logging.open(p, std::ios_base::app);
	logging << docEl->toString();
	return std::string();
}

//////////////////////////////////////////////////////////////////////////////////////////
/////With the help of this function we are able to get the dependency of a file recursively
void ServerRepos::Dependencygetter(std::vector<std::string>& dependencyVector, std::string currentFile)
{
	std::vector<std::string> TempDependencyVector;
	std::string DependecyString;
	std::unordered_map<std::string, std::string>::iterator it;
	it = dependencyTable.find(currentFile);
	DependecyString = it->second;
	std::stringstream  data(DependecyString);
	std::string line;
	while (std::getline(data, line, ';'))
	{
		TempDependencyVector.push_back(line); 
	}

	for (size_t i = 0; i < TempDependencyVector.size(); i++) {
		if (TempDependencyVector[i] != "No_Dependent_File") {
			if (std::find(dependencyVector.begin(), dependencyVector.end(), TempDependencyVector[i]) == dependencyVector.end()) {
				dependencyVector.push_back(TempDependencyVector[i]);
				Dependencygetter(dependencyVector, TempDependencyVector[i]);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////
////This function give us the timestamp in string
std::string ServerRepos::getTimeStampString()
{
	time_t now = time(0);
	char str[26];
	ctime_s(str, sizeof str, &now);
	std::string timestamp;
	for (auto chr : str) {
		if (chr == ' ') {
			timestamp.push_back('_');
		}
		else if (chr == ':') {
			timestamp.push_back('-');
		}
		else if (chr == '\n')
			;
		else
			timestamp.push_back(chr);
	}
	timestamp.pop_back();

	return timestamp;
}
////////////////////////////////////////////////////////////////////////////////
///This function is to get a single File (Not the dependent file)
void ServerRepos::GetSingleFile(std::unordered_map<std::string, std::string> httpmsg)
{
	std::string FileName;
	std::unordered_map<std::string, std::string>::iterator it;
	it = httpmsg.find("Content");
	FileName = it->second;
	it = httpmsg.find("ToAddress");
	int serverport = atoi(it->second.c_str());
	it = httpmsg.find("SenderAddress");
	int clientPort = atoi(it->second.c_str());
	std::unordered_map<std::string, std::string>::iterator it2;
	it2 = dependencyTable.find(FileName);
	std::string dependencyListString = it2->second;
	std::vector<std::string>   result;
	result.push_back(FileName);

	for (size_t i = 0; i < result.size(); i++) {

		FileSender(result[i], serverport, clientPort); // sending files one by one
	}
	si.close();
	si.sendString("quit");
}


//----< test stub >--------------------------------------------------
#ifdef TEST_SERVER

int main()
{
  Show::attach(&std::cout);
  Show::start();
  Show::title("\n  String Server started");
  try
  {
	  SocketSystem ss;
	  SocketListener sl(8080, Socket::IP6);
	  ServerRepos serverRepo;

	  ClientHandler* clander = new ClientHandler(serverRepo.getRecMsg());
	  sl.start(*clander);

	 serverRepo.RecReply();
	
    Show::write("\n --------------------\n  press key to exit: \n --------------------");
	std::cout << "Server is ending" << std::endl;
    std::cout.flush();
    std::cin.get();
  }
  catch (std::exception& exc)
  {
    Show::write("\n  Exeception caught: ");
    std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
    Show::write(exMsg);
  }
}
#endif
