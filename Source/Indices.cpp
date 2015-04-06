/*
	Indices.cpp
	
	Copyright Will Dyson
	
	Created: 01/06/99 19:05:21
	
	This program is freely distributable under the terms of
	the Gnu Public License. If you have a patch or new feature,
	please let me know. mailto:will@cs.earlham.edu
*/

//AppKit
#include <Application.h>

//InterfaceKit
#include <Window.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Alert.h>

//StorageKit
#include <VolumeRoster.h>
#include <Volume.h>
#include <fs_index.h>

//posix
#include <dirent.h>

//C++
#include <string.h>
#include <string>

//ColumnListView
#include "ColumnListView.h"
#include "CLVColumn.h"

//SGB
#include "BetterScrollView.h"

//local
#include "IndexListItem.h"
#include "Indices.h"
#include "IndicesVolWin.h"

//our message->what constant:
const uint32 MENU_UPDATE = 'MeUp';

const char* APP_SIG = "application/x-vnd.WD-Indices";

IndexApp::IndexApp()
: BApplication(APP_SIG)
{
	new IndicesVolWin();
}

IndexApp::~IndexApp()
{
}

IndexWin::IndexWin(BVolume* Volume)
: BWindow(BRect(25, 75, 550, 300), "Indices for: ", B_DOCUMENT_WINDOW, 0)
{
	char NameBuff[B_FILE_NAME_LENGTH];
	
	
	BRect framerect = Bounds();
	
	TheVolume = new BVolume(*Volume);
	
	TheVolume->GetName(NameBuff);
	string TitleBuff = "Indices for: ";
	TitleBuff += NameBuff;
	
	SetTitle(TitleBuff.c_str());
	
	_SetupMenus(framerect);
	_SetupView(framerect);
	_UpdateList();
	
	Show();
}

void IndexWin::_SetupMenus(BRect frame)
{
	BMenu* amenu;
	
	menubar = new BMenuBar(frame, "menubar");
	amenu = new BMenu("Indices");
	amenu->AddItem(new BMenuItem("Update List", new BMessage(MENU_UPDATE), 'U'));
	
	menubar->AddItem(amenu);
	AddChild(menubar);
}

void IndexWin::_SetupView(BRect frame)
{
	BetterScrollView* containerview;
	
	//make room for scrollbar
	frame.right -= B_V_SCROLL_BAR_WIDTH;
	frame.bottom -= B_H_SCROLL_BAR_HEIGHT;
	
	//make room for menubar
	frame.top = menubar->Bounds().bottom + 1.0;
	
	displayview = new ColumnListView(frame, &containerview, "displayview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE, B_SINGLE_SELECTION_LIST, true, true, true, true, B_NO_BORDER, be_plain_font);


	//info columns
	displayview->AddColumn(new CLVColumn("Name",115.0,CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH,50.0));
	displayview->AddColumn(new CLVColumn("Type",100.0, CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH,50.0));
	displayview->AddColumn(new CLVColumn("Size",50.0,CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH));
	displayview->AddColumn(new CLVColumn("ModTime",108.0,CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH));
	displayview->AddColumn(new CLVColumn("CreateTime",108.0,CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH));
	displayview->AddColumn(new CLVColumn("UID",50.0,CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH|CLV_HIDDEN));
	displayview->AddColumn(new CLVColumn("GID",50.0,CLV_SORT_KEYABLE|CLV_HEADER_TRUNCATE|CLV_TELL_ITEMS_WIDTH|CLV_HIDDEN));
	
	
	//this scrollview encapsulates the displayview
	AddChild(containerview);
}

// get the list of indices and display them
void IndexWin::_UpdateList()
{
	index_info info; // used for getting info on indices
	dev_t device;
	
	BListItem* listitem;
	DIR *indexdir;
	struct dirent* ent;
	
	if (displayview->IsEmpty() == false)
	{
		// empty list
		while (true)
		{
			listitem = displayview->RemoveItem(NULL);
			if (listitem == NULL)
			{
				break;
			}
			delete listitem;
		}
	}
	
	
	device = TheVolume->Device(); //DevideID
	
	indexdir = fs_open_index_dir(device);
	if (indexdir == NULL)
	{
		char NameBuff[B_FILE_NAME_LENGTH];
		
		TheVolume->GetName(NameBuff);
		
		string AlertBuff = "Couldn't open index dir for volume: ";
		AlertBuff += NameBuff;
		
		BAlert* alert = new BAlert("erroralert", AlertBuff.c_str(), "Rats");
		alert->Go();
		
		//Close();
		return;
	}
	
	// get and pack the items
	while (true)
	{
		ent = fs_read_index_dir(indexdir);
		if (ent == NULL)
		{
			break;
		}
		fs_stat_index(device, ent->d_name, &info);
		displayview->AddItem(new IndexListItem(1, ent->d_name, &info));
	}
	
	fs_close_index_dir(indexdir); 
}

bool IndexWin::QuitRequested()
{
	return true;
}

void IndexWin::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case MENU_UPDATE:
		{
			_UpdateList();
		}
		break;
		
		default:
		{
			BWindow::MessageReceived(message);
		}
	}
}

int main()
{
	IndexApp myapp;
	myapp.Run();
	return 0;
}
