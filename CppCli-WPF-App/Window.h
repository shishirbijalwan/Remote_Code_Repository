#ifndef WINDOW_H
#define WINDOW_H
/////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application            //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It 
*  provides one class, RepositoryGUI, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class Channel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.

*Public Interface:
=================
setUpStatusBar                       // Function for setting up statuc bar.
setUpTabControl			             // Sets up tab control.
setUpExtractView		             // Sets up extract view.
setUpCheckInView                     // Set up Check in view.
checkIn				           		 // Function for checkin a file.
updateStatusBar					     //Function for updating status bar
getMessage							 // Gets message from client receive queue
browseForFolder						 // Function for browse the folder
getItemsFromList					 // Function for getting selected items from the list
extract								 // Function to get file from server
Unloading							 // Function helps in loading
Unloading							// Functios helps in unloading GUI
RepositoryGUI						// Constructor of the class starts thread
*  Required Files:
*  ---------------
*- StringServer.h,Channel.h,Windows.h,HTTPMessage.h,Filesystem.h,XmlElement.h,StringClient.h,Cpp11-BlockingQueue.h,Logger.h,Sockets.h,Utilities.h

*
*  Build Command:
*  --------------
* devenv Project_4.sln /rebuild debug
*

Maintenance History:
====================
ver 1.0
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;

#include "../Channel/Channel.h"
#include <iostream>
#include <vector>

namespace CppCliWindows
{
	ref class RepositoryGUI : Window
	{		
		IChannel* pChannel;

		// Controls for Window

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hExtractTab = gcnew TabItem();
		TabItem^ hCheckInTab = gcnew TabItem();
		TabItem^ hConnectTab = gcnew TabItem();
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for Extract View

		Grid^ hExtractGrid = gcnew Grid();
		Button^ hGetFileListButton = gcnew Button();
		Button^ hExtractButton = gcnew Button();
		Button^ hWithOutDepButton = gcnew Button();
		ListBox^ hRepPkgList = gcnew ListBox();
		ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
		StackPanel^ hStackPanel1 = gcnew StackPanel();

		String^ msgText
			= "Command:ShowMessage\n"   // command
			+ "Sendr:localhost@8080\n"  // send address
			+ "Recvr:localhost@8090\n"  // receive address
			+ "Content-length:44\n"     // body length attribute
			+ "\n"                      // end header
			+ "Hello World\nCSE687 - Object Oriented Design";  // message body

															   // Controls for Check In View
		Grid^ hCheckInGrid = gcnew Grid();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		ListBox^ hCheckInList = gcnew ListBox();
		ListBox^ hDependenciesList = gcnew ListBox();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hCheckInGetFileListButton = gcnew Button();
		Button^ hCheckInButton = gcnew Button();
		Grid^ hGrid2 = gcnew Grid();

		// receive thread

		Thread^ recvThread;

	public:
		RepositoryGUI();
		~RepositoryGUI();

		void setUpStatusBar();
		void setUpTabControl();
		void setUpExtractView();
		void setUpCheckInView();
		void setUpCheckInViewPart2();
		void setUpConnectionView();

		void getFileList(Object^ obj, RoutedEventArgs^ args);
		void checkIn(Object^ obj, RoutedEventArgs^ args);
		void addPackage(String^ msg);
		void updateStatusBar(String^ msg);
		void getMessage();
		void extract(Object^ sender, RoutedEventArgs^ args);
		void getSingleFile(Object^ sender, RoutedEventArgs^ args); //Function added
		void getItemsFromList(Object^ sender, RoutedEventArgs^ args);
		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
	private:
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		void setExtractListProperties();
		void setButtonsProperties();
	};
}


#endif
