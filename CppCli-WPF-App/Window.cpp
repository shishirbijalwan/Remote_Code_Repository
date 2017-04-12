/////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application            //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP Pallvilion, Windows 7                        //
//  Application:   Remote Repository, Sp16				           //
//  Author:        Shishir Bijalwan, Syracuse University           //
//                 (979) 587-6340, sbijalwa@syr.edu                //
/////////////////////////////////////////////////////////////////////
#include "Window.h"
using namespace CppCliWindows;
#include <exception>
////////////////////////////////////////////////////
//The class constructor is usedto set up the channel
RepositoryGUI::RepositoryGUI(){
	// set up channel
	ObjectFactory* pObjFact = new ObjectFactory;	
	pChannel = pObjFact->createChannel(8080, 8082);
	pChannel->start();
	delete pObjFact;
	// client's receive thread
	recvThread = gcnew Thread(gcnew ThreadStart(this, &RepositoryGUI::getMessage));
	recvThread->Start();
	// set event handlers

	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &RepositoryGUI::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &RepositoryGUI::Unloading);
	hGetFileListButton->Click += gcnew RoutedEventHandler(this, &RepositoryGUI::getFileList);
	hExtractButton->Click += gcnew RoutedEventHandler(this, &RepositoryGUI::extract);
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &RepositoryGUI::browseForFolder);
	hCheckInGetFileListButton->Click += gcnew RoutedEventHandler(this, &RepositoryGUI::getFileList);
	hCheckInButton->Click += gcnew RoutedEventHandler(this, &RepositoryGUI::checkIn);
	hWithOutDepButton->Click += gcnew RoutedEventHandler(this, &RepositoryGUI::getSingleFile);

	// set Window properties
	this->Title = "Repository";
	this->Width = 800;
	this->Height = 600;

	// attach dock panel to Window

	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// setup Window controls and views
	setUpTabControl();
	setUpStatusBar();
	setUpExtractView();
	setUpCheckInView();
	setUpConnectionView();
}
////////////////////////////////////////////
///Distructor to delet pChannel object
RepositoryGUI::~RepositoryGUI()
{
	delete pChannel;
	
}
///////////////////////////////////////////
//This fucntions setups the status bar
void RepositoryGUI::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "very important messages will appear here";
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}
///////////////////////////////////////
/// This is to set up the tab
void RepositoryGUI::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hCheckInTab->Header = "Upload";
	hExtractTab->Header = "Download";
	hConnectTab->Header = "Connect";
	hTabControl->Items->Add(hCheckInTab);
	hTabControl->Items->Add(hExtractTab);
//	hTabControl->Items->Add(hConnectTab);
}
///////////////////////////////////////
/// This  sets Extract list properties
void RepositoryGUI::setExtractListProperties()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hExtractGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hRepPkgList;
	hRepPkgList->Items->Add("");
	hRepPkgList->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hExtractGrid->SetRow(hScrollViewer1, 0);
	hExtractGrid->Children->Add(hScrollViewer1);
}
///////////////////////////////////////////////
///This method is to set the button properties
void RepositoryGUI::setButtonsProperties()
{
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hExtractGrid->RowDefinitions->Add(hRow2Def);
	hGetFileListButton->Content = "Get File List";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hExtractButton->Content = "Download";
	hBorder2->Child = hGetFileListButton;
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hExtractButton;
	hStackPanel1->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hStackPanel1->Children->Add(hSpacer);
	hStackPanel1->Children->Add(hBorder3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 10;
	hWithOutDepButton->Content = "Download Without Dependencies";
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 190;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hWithOutDepButton;
	hStackPanel1->Children->Add(hSpacer2);
	hStackPanel1->Children->Add(hBorder4);
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hExtractGrid->SetRow(hStackPanel1, 1);
	hExtractGrid->Children->Add(hStackPanel1);
}
//////////////////////////////////////////
///This function is to  set up extract
void RepositoryGUI::setUpExtractView()
{
	Console::Write("\n  setting up sendMessage view");
	hExtractGrid->Margin = Thickness(20);
	hExtractTab->Content = hExtractGrid;

	setExtractListProperties();
	setButtonsProperties();
}
///////////////////////////////////////////////////////////////
///This function is to convert system string to standard string
std::string RepositoryGUI::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}
///////////////////////////////////////////////////////////////
///This function is to get the file list in Repository
void RepositoryGUI::getFileList(Object^ obj, RoutedEventArgs^ args)
{
	
	pChannel->getFileList();
	Console::Write("\n  sent package list request");
	hStatus->Text = "Sent Package List Request";
}
/////////////////////////////////////////////////////////////
///This function is to call the check in process
void RepositoryGUI::checkIn(Object^ obj, RoutedEventArgs^ args)
{
	hStatus->Text = "Sending Checkin Request";

	if (!(hCheckInList->SelectedItem==nullptr))
	{
		std::string Filename = toStdString(hCheckInList->SelectedItem->ToString());

	std::vector<std::string> files_;
	std::string DependencyList;

	int index = 0;
	int count = hDependenciesList->SelectedItems->Count;
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hDependenciesList->SelectedItems)
		{
			items[index++] = item;
		}
	}
	std::vector<std::string> dep;
	if (count > 0) {
		for each (String^ item in items)
		{
			if (DependencyList.length() == 0) {
				DependencyList = toStdString(item);
			}
			else {
				DependencyList = DependencyList + ";"+ toStdString(item);
			}
		}
	}
	std::cout << "Length of file name is = " << Filename.length() << std::endl;
	pChannel->SendAFile(Filename, DependencyList);
	}
	hStatus->Text = "Please check in done check Repository";

}

////////////////////////////////////////////////////////
///To convert std string to system string
String^ RepositoryGUI::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}
//////////////////////////////////////////////////////////
///This is to add file name and directory name to the list
void RepositoryGUI::addPackage(String^ msg)
{
	array<String^>^ List = msg->Split(';');
	hRepPkgList->Items->Clear();
	hDependenciesList->Items->Clear();
	for (int i = 0; i < List->Length; i++)
	{
		hRepPkgList->Items->Add(List[i]);
		hDependenciesList->Items->Add(List[i]);
	}
}
////////////////////////////////////////////////
///This function is to update the status bar
void RepositoryGUI::updateStatusBar(String^ msg)
{
	hStatus->Text == msg;
}

///////////////////////////////////////////////////////
//This function gets the msg through client channel
void RepositoryGUI::getMessage()
{
	// recvThread runs this function

	while (true)
	{
		std::cout << "\n  receive thread calling getMessage()";
		std::string msg = pChannel->getMessage();
		String^ sMsg = toSystemString(msg);
					
			array<String^>^ args = gcnew array<String^>(1);
			args[0] = sMsg;

			Action<String^>^ act = gcnew Action<String^>(this, &RepositoryGUI::addPackage);
			Dispatcher->Invoke(act, args);
		}
	
}
/////////////////////////////////////////////////////////////////
///This methods helps to download file along with dependent files
void RepositoryGUI::extract(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  cleared message text");
	if(!(hRepPkgList->SelectedItem==nullptr)){
	hStatus->Text = "Sending Extract Request";	
	std::string Filename = toStdString(hRepPkgList->SelectedItem->ToString());	
	pChannel->getFile(Filename);
	hStatus->Text = "Please check Download Folder";
	}

}
///////////////////////////////////////////////////////////////////////////////////////
///This function is to get a single file without any dependency
void CppCliWindows::RepositoryGUI::getSingleFile(Object ^ sender, RoutedEventArgs ^ args)
{
	Console::Write("\n  cleared message text");
	hStatus->Text = "Sending Extract Request";
	if(!(hRepPkgList->SelectedItem==nullptr)){
	std::string Filename = toStdString(hRepPkgList->SelectedItem->ToString());
	pChannel->getSingleFile(Filename);
	hStatus->Text = "Please check Download Folder";
	}
}

//////////////////////////////////////////////////////////////////////
////This functions gets the selected items in the browers window of UI
void RepositoryGUI::getItemsFromList(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hCheckInList->SelectedItems->Count;
	hStatus->Text = "Show Selected Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hCheckInList->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hCheckInList->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hCheckInList->Items->Add(item);
		}
	}
}


////////////////////////////////////////////////////////
///This function is to set UP the check in view
void RepositoryGUI::setUpCheckInView()
{
	Console::Write("\n  setting up Check In view");
	hCheckInGrid->Margin = Thickness(20);
	hCheckInTab->Content = hCheckInGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(200);
	hCheckInGrid->RowDefinitions->Add(hRow1Def);
	RowDefinition^ spacer = gcnew RowDefinition();
	spacer->Height = GridLength(20);
	hCheckInGrid->RowDefinitions->Add(spacer);
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(200);
	RowDefinition^ hRow2Def2 = gcnew RowDefinition();
	//hRow2Def2->Height = GridLength(75);
	hCheckInGrid->RowDefinitions->Add(hRow2Def);
	hCheckInGrid->RowDefinitions->Add(hRow2Def2);
	ColumnDefinition^ hColumn1Def = gcnew ColumnDefinition();
	hColumn1Def->Width = GridLength(500);
	hCheckInGrid->ColumnDefinitions->Add(hColumn1Def);
	ColumnDefinition^ hColumn2Def = gcnew ColumnDefinition();
	hCheckInGrid->ColumnDefinitions->Add(hColumn2Def);

	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hCheckInList->SelectionMode = SelectionMode::Single;
	hBorder1->Child = hCheckInList;
	hCheckInGrid->SetRow(hBorder1, 0);
	hCheckInGrid->SetColumn(hBorder1, 0);
	hCheckInGrid->Children->Add(hBorder1);

	Border^ hBorder2 = gcnew Border();
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hDependenciesList->SelectionMode = SelectionMode::Multiple;
	hBorder2->Child = hDependenciesList;
	hCheckInGrid->SetRow(hBorder2, 2);
	hCheckInGrid->SetColumn(hBorder2, 0);
	hCheckInGrid->Children->Add(hBorder2);
	setUpCheckInViewPart2();
}


///////////////////////////////////////////////////////
////this is to setup check In view helper
void  RepositoryGUI::setUpCheckInViewPart2() {

	hFolderBrowseButton->Content = "Select Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	hCheckInGrid->SetRow(hFolderBrowseButton, 0);
	hCheckInGrid->SetColumn(hFolderBrowseButton, 1);
	hCheckInGrid->Children->Add(hFolderBrowseButton);

	// Show selected items button.
	hCheckInGetFileListButton->Content = "Get File List";
	hCheckInGetFileListButton->Height = 30;
	hCheckInGetFileListButton->Width = 120;
	hCheckInGetFileListButton->BorderThickness = Thickness(2);
	hCheckInGetFileListButton->BorderBrush = Brushes::Black;
	hCheckInGrid->SetRow(hCheckInGetFileListButton, 2);
	hCheckInGrid->SetColumn(hCheckInGetFileListButton, 1);
	hCheckInGrid->Children->Add(hCheckInGetFileListButton);

	// Check In button
	hCheckInButton->Content = "Check In";
	hCheckInButton->Height = 30;
	hCheckInButton->Width = 120;
	hCheckInButton->BorderThickness = Thickness(2);
	hCheckInButton->BorderBrush = Brushes::Black;
	hCheckInGrid->SetRow(hCheckInButton, 3);
	hCheckInGrid->SetColumn(hCheckInButton, 1);
	hCheckInGrid->Children->Add(hCheckInButton);

	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}
////////////////////////////////////////////////////////////////////////
/////This is to setup the browser button of the UI
void RepositoryGUI::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hCheckInList->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
		hCheckInList->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		
	}
}
///////////////////////////////////////////////////
///This is write on console one  setting up connection view
void RepositoryGUI::setUpConnectionView()
{
	Console::Write("\n  setting up Connection view");
}
///////////////////////////////////////////////////
///This is to write on console on load process
void RepositoryGUI::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}
///////////////////////////////////////////////////
///This is to write the unload process
void RepositoryGUI::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
}

#ifdef TEST_REPOSIRORYGUI

[STAThread]
//int _stdcall WinMain()

int main(array<System::String^>^ args)
{
	try{
	Console::WriteLine(L"\n Starting RepositoryGUI");

	Application^ app = gcnew Application();
	app->Run(gcnew RepositoryGUI());
	Console::WriteLine(L"\n\n");
	}
	catch (Exception^ e) {
		Console::Write(e);
	}
}
#endif