1)Please do read the user manual before you start Testing the UI.
2) When you will run the compil.bat and run.bat file.It will generate 3 console and one UI-
 2.1)The TestExecutive console demostarte that all the requirements of the projet are met.
 2.2) The server console will show all the messages received by server
 2.3) This GUI console demonstrate all the Http messages sent and received at console.
3) Please note i have shown the buffer read and sent for file transfer as it will eat up all the space in console.
4)In my design i am creating XML as the part of requirement only. I haven't used it in any processing. I am creating 
  as table of dependency in unordered map. Using it for getting n level dependency using recursion and files present in Repository
  Below is the reason for that-
  Extacting data from XML  and then processing it is much slower in comparison to iterating the unorderedmap.
5) Please find below the folders where you need to check for files transfered in client and server-
 Client side file recieve folder- Project_4\Download  
 Server Repository Location -Project_4\Repository
 server Temporary folder is -Project_4\Temp  (Please note i am removing file from here once checked in so you wont find any file)

6) Please note i have created or made changes in below files only. Files apart from them socket,Blocking queue etc have not been modified by me.

 a) HttpMessage.h and cpp
 b) StringClient.h and StringClient.cpp	
 c) StringServer.h and StringServer.cpp
 d)Channel.h and Channel.cpp
 e)TestExecutive.h and cpp
 f) Windows.h and cpp

			
 