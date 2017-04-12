/////////////////////////////////////////////////////////////////////
//  StringClient.cpp -This has the Client and the listener helper  //
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
#include <string>
#include <iostream>
#include <thread>
#include "../Project_4/HTTPMessage.h"
#include "../FileSystem/FileSystem.h"
#include <experimental/filesystem>
#include "../StringClient/StringClient.h"

using Show = StaticLogger<1>;
using namespace Utilities;
using BQueue = BlockingQueue < Message >;

////////////////////////////////////////////////////////////////////
///Operator overload to help the client listen from the socket
void ReceiverHelper::operator()(Socket& socket_) {
	while (true) {
		std::string msg = socket_.recvString();
		if (msg == "quit") {
			break;
		}
		if (msg.length() == 0) {
			continue;
		}
		std::cout << "Msg received at client" << std::endl;
		std::cout << msg << std::endl;
		std::size_t found = msg.find("File_Coming");
		if (found == std::string::npos) {
			ReceiveMsg.enQ(msg);
		}
		else {
			std::unordered_map<std::string, std::string> msgParts = breakMsg(msg);
			std::unordered_map<std::string, std::string>::iterator it;
			it = msgParts.find("Content");
			std::string Filename = it->second;
			it = msgParts.find("ContentLength");
			int length = atoi(it->second.c_str());
			std::cout << "Msg in Socket" << std::endl;
			std::cout << msg << std::endl;
			readFile(Filename, length, socket_);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
///String client contructor to initialize the server,client and socket listener
StringClient::StringClient(int portNumber, int clientport) :sl(clientport), rHelper(RecieveMsg) {
	serverPortNumber = portNumber;
	clientPortNumber = clientport;
}


/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
  ClientCounter() { ++clientCount; }
  size_t count() { return clientCount; }
private:
  static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;


///////////////////////////////////////////////////////////////////////
////Read Incoming file and create the file in a temporary folder
bool ReceiverHelper::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];
	size_t bytesToRead;
	std::string fqname = TempFolderPath + filename;
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}
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
		fileSize -= BlockSize; // Reduce the block size received from total file size to get left amount of file
	}
	file.close();
	return true;
}
/////////////////////////////////////////////////////////////////////
////Break HTTP Msg based on new line and semicolom.Return an unordered map of the HTTP msg
std::unordered_map<std::string, std::string> ReceiverHelper::breakMsg(std::string message)
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
			std::pair<std::string, std::string> p(cmd, arg);
			tempMap.insert(p);
		}
	}
	return tempMap;
}


////////////////////////////////////////////////////////////
//Function to send the file to Server
void StringClient::FileSender(std::string FilePath, std::string ListOfDependency) {
	std::string ReadFilePath = FilePath;
	std::string FileName = FilePath.substr(FilePath.find_last_of("/\\") + 1);
	FileSystem::FileInfo fi(ReadFilePath);
	size_t filesize = fi.size();
	std::string  sizeString = Converter<size_t>::toString(filesize);
	if (!FileSystem::File::exists(ReadFilePath)) {
		std::cout << "Input doesn't exist" << std::endl;return;
	}
	FileSystem::File ReadFile(ReadFilePath);
	ReadFile.open(FileSystem::File::in, FileSystem::File::binary);
	if (!ReadFile.isGood())
	{std::cout << "Error while opening input file" << std::endl;
		return;	}	
	HTTPMessage* httpMessage1 = new HTTPMessage;
	httpMessage1->addAttributeToMsg("command:", "File_Coming");
	httpMessage1->addAttributeToMsg("ToAddress:", std::to_string(serverPortNumber));
	httpMessage1->addAttributeToMsg("SenderAddress:", std::to_string(clientPortNumber));
	httpMessage1->addAttributeToMsg("ContentLength:", sizeString);
	httpMessage1->addAttributeToMsg("Content:", FileName);
	httpMessage1->addAttributeToMsg("Dependency:", ListOfDependency);
	si.sendString(httpMessage1->getHttpMessage());
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
	HTTPMessage* httpMessage2 = new HTTPMessage;
	httpMessage2->addAttributeToMsg("command:", "CHECKIN");
	httpMessage2->addAttributeToMsg("ToAddress:", std::to_string(serverPortNumber));
	httpMessage2->addAttributeToMsg("SenderAddress:", std::to_string(clientPortNumber));
	httpMessage2->addAttributeToMsg("ContentLength:", std::to_string(get_file_size(FilePath)));
	httpMessage2->addAttributeToMsg("Content:", FileName);
	httpMessage2->addAttributeToMsg("Dependency:", ListOfDependency);
	si.sendString(httpMessage2->getHttpMessage());
	std::cout <<"Msg sent from client \n"  <<httpMessage2->getHttpMessage() << std::endl;
}
/////////////////////////////////////////////////////////////////////////////
//This function was thread to dequeue the receive msg and act on it when GUI was not there
//But as now we have the GUI this method is not getting used.
void StringClient::getRecMsg()
{
	RecieverThread_ = std::thread(
		[this] {
		BQueue& recvQ = RecieveMsg;

		while (true)
		{
			std::string msg = recvQ.deQ();  // will block here so send quit message when stopping
			if (msg == "quit")
				break;
			if (msg.length() == 0)
				continue;
			std::unordered_map<std::string, std::string> messagepart = breakMsg(msg);
			std::unordered_map<std::string, std::string>::iterator it;
			it = messagepart.find("command");
			if (it != messagepart.end()) {				
				 if (it->second == "Get_File_List")
					getFileListHelper(messagepart);
				else
					std::cout << "Unknown Command found";
			}
		}
	});
	RecieverThread_.detach();
}
////////////////////////////////////////////////////////////////////////////////////
////Break HTTP Msg based on new line and semicolon.Return an unordered map of the HTTP msg
std::unordered_map<std::string, std::string> StringClient::breakMsg(std::string message)
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
			std::pair<std::string, std::string> p(cmd, arg);
			tempMap.insert(p);
		}
	}
	return tempMap;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
///This method was getting used when we didn't had GUI. It used to act on a HTTP msg when it had file list
void StringClient::getFileListHelper(std::unordered_map<std::string, std::string> httpmsg)
{
	std::string stringList;
	std::unordered_map<std::string, std::string>::iterator it;

	it = httpmsg.find("Content");
	stringList = it->second;
	std::stringstream test(stringList);
	std::string segment;

	while (std::getline(test, segment, ';'))
	{
		ServerFileList.push_back(segment);
	}

	if (ServerFileList.size() == 0)
		std::cout << "Repository is empty for now" << std::endl;
	for (size_t i = 0; i < ServerFileList.size(); i++) {
	
		std::cout << "File " << i << " " << ServerFileList[i] << std::endl;
	}
}
///////////////////////////////////////////////////////////////////////
///This functions returns the file size.
int StringClient::get_file_size(std::string filename) // path to file
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

////////////////////////////////////////////////////////////////////////////
//This function creates a thread which dequeues the msg and send it to server
void StringClient::startCommunication()
{	
	sl.start(rHelper);

	Senderthread_ = std::thread(
		[this] {
	while (!stop_)
	{
		Message msg = SendMsg.deQ();  // will block here so send quit message when stopping
std::cout << "Msg sent By client- \n" << msg << std::endl;			
		si.sendString(msg);
	}
	});
	Senderthread_.detach();
}
///////////////////////////////////////////////////////////////////////////
//// This function connects client with the server
void StringClient::connect()
{

	while (!si.connect("localhost", serverPortNumber))
	{
		Show::write("\n client waiting to connect");
		::Sleep(100);
	}
	std::cout << "Server Connected " << std::endl;
}
/////////////////////////////////////////////////////////////////////////////////////////
//This function creates the Http Message and put it in the send queue to get the file list
void StringClient::getFileList()
{
	HTTPMessage* httpMessage5 = new HTTPMessage;
	httpMessage5->addAttributeToMsg("command:", "Get_File_List");
	httpMessage5->addAttributeToMsg("ToAddress:", std::to_string(serverPortNumber));
	httpMessage5->addAttributeToMsg("SenderAddress:", std::to_string(clientPortNumber));
	SendMsg.enQ(httpMessage5->getHttpMessage());
}
/////////////////////////////////////////////////////////
///This function creates the Http Message and put it in the send queue to get the file
void StringClient::getFileFromServer(std::string filename)
{
	HTTPMessage* httpMessage1 = new HTTPMessage;
	httpMessage1->addAttributeToMsg("command:", "Get_the_file");
	httpMessage1->addAttributeToMsg("ToAddress:", std::to_string(serverPortNumber));
	httpMessage1->addAttributeToMsg("SenderAddress:", std::to_string(clientPortNumber));
	httpMessage1->addAttributeToMsg("Content:", filename);
	SendMsg.enQ(httpMessage1->getHttpMessage());
}
/////////////////////////////////////////////////////////////////
///This function is to get a single file without dependent files
void StringClient::getSingleFile(std::string filename)
{
	HTTPMessage* httpMessage1 = new HTTPMessage;
	httpMessage1->addAttributeToMsg("command:", "Get_Single_File");
	httpMessage1->addAttributeToMsg("ToAddress:", std::to_string(serverPortNumber));
	httpMessage1->addAttributeToMsg("SenderAddress:", std::to_string(clientPortNumber));
	httpMessage1->addAttributeToMsg("Content:", filename);
	SendMsg.enQ(httpMessage1->getHttpMessage());
}

//////////////////////////////////////////////////////////////////////////////
////This function removes the First fine of a file
void ReceiverHelper::RemoveLine(std::string & source, const std::string & to_remove)
{
	size_t m = source.find(to_remove);
	size_t n = source.find_first_of("\n", m + to_remove.length());
	source.erase(m, n - m + 1);
}
//////////////////////////////////////////////////////////////////////
///This File transfer helper is when we receive file as string(not in use now)
void ReceiverHelper::fileTrasnferHelper(std::vector<std::string> info, std::string matter)
{
	std::string p = TempFolderPath + info[2];
	std::ofstream logging;
	logging.open(p, std::ios_base::app);
	logging << matter;
}

#ifdef TEST_STRINGCLIENT

int main()
{
  Show::title("Demonstrating two String Clients each running on a child thread");

  StringClient c1(8080,8081);

 /* HTTPMessage* httpMessage5 = new HTTPMessage;
  httpMessage5->addAttributeToMsg("command:", "Get_File_List");
  httpMessage5->addAttributeToMsg("ToAddress:", "8080");
  httpMessage5->addAttributeToMsg("SenderAddress:", "8081");
  c1.enqueueMsg(httpMessage5->getHttpMessage());*/

 // c1.getFileList();
 /* ReceiverHelper* rHelper = new ReceiverHelper(c1.getRecieveMsgQueue());

  SocketSystem ss;
  SocketListener sl(8081, Socket::IP6);
  sl.start(*rHelper);*/
  c1.connect();
 // c1.getRecMsg();
  c1.startCommunication();

 



  /*std::string msgCommand = "command:" + command;
  std::string msgToAddress = "ToAddress:" + std::to_string(toPortNumber);
  std::string msgMyAddress= "SenderAddress:" + std::to_string(myPortNumber);
  std::string msgContentLength = "ContentLength:" + std::to_string(contentLength);
  std::string msgContent = "Content:" + content;*/

  ////////////////////////////////////////////////////////////
  ///Get File List 
 


////////////////////////////////////////////////////////////////////
 ///// Check in File

	 // std::vector<std::string> listDependency = { "File2.txt","file3.txt" };
  //std::cout << "File sender function called" << std::endl;
  std::string listOfDependentFiles = "..\\..\\Repository\\StringClient.cpp_V_1\\StringClient.cpp";
  std::string listOfDependentFiles2 = "No_Dependent_File";
  std::string listOfDependentFiles3 = "..\\..\\Repository\\StringServer.cpp_V_1\\StringServer.cpp";

// c1.FileSender("C:\\Users\\Megha\\Desktop\\Project_4\\File\\File.txt", listDependency);
 c1.FileSender("C:\\Users\\Megha\\Desktop\\Project_4\\File\\StringClient.cpp", listOfDependentFiles3);
 c1.FileSender("C:\\Users\\Megha\\Desktop\\Project_4\\File\\ActionsAndRules.h", listOfDependentFiles);
 c1.FileSender("C:\\Users\\Megha\\Desktop\\Project_4\\File\\StringServer.cpp", listOfDependentFiles2);

// c1.FileSender("C:\\Users\\Megha\\Desktop\\Project_4\\File\\ActionsAndRules.h", listDependency);
 c1.getFileList();
 //c1.enqueueMsg(httpMessage5->getHttpMessage());

  ////////////////////////////////////////////////////////////
  ///Get File List 
 /* HTTPMessage* httpMessage = new HTTPMessage;
  httpMessage->addAttributeToMsg("command:", "Get_File_List");
  httpMessage->addAttributeToMsg("ToAddress:", "8080");
  httpMessage->addAttributeToMsg("SenderAddress:", "8081");
  c1.enqueueMsg(httpMessage->getHttpMessage());*/

  ////////////////////////////////////////////////////
  //Get File Test

 c1.getFileFromServer("..\\..\\Repository\\ActionsAndRules.h_V_1\\ActionsAndRules.h");

 // std::cout << "File sender function ENNND" << std::endl;

 // c1.enqueueMsg("quit");
 // c1.joinSenderThread();
 // c1.endCommunication();
  
  
 

 // std::cout << "======Client main is ending============" << std::endl;
  std::cin.get();

  //StringClient c1;
  //std::thread t1(
  //  [&]() { c1.execute(100, 50); } // 50 messages 100 millisec apart
  //);

  //StringClient c2;
  //std::thread t2(
  //  [&]() { c2.execute(120, 50); } // 50 messages 120 millisec apart
  //);
  //t1.join();
  //t2.join();
}
#endif
