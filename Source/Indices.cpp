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
#include <Alert.h>
#include <ColumnTypes.h>
#include <LayoutBuilder.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Window.h>
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
#include <ColumnListView.h>

//local
#include "IndexListItem.h"
#include "Indices.h"
#include "IndicesVolWin.h"

//our message->what constant:
const uint32 MENU_UPDATE = 'MeUp';

const char* APP_SIG = "application/x-vnd.WD-Indices";

#define B_TRANSLATE(x)	x

IndexApp::IndexApp()
: BApplication(APP_SIG)
{
	new IndicesVolWin();
}

IndexApp::~IndexApp()
{
}

IndexWin::IndexWin(BVolume* Volume)
: BWindow(BRect(25, 75, 650, 400), "Indices for: ", B_DOCUMENT_WINDOW, 0)
{
	char NameBuff[B_FILE_NAME_LENGTH];
	
	
	BRect framerect = Bounds();
	
	TheVolume = new BVolume(*Volume);
	
	TheVolume->GetName(NameBuff);
	std::string TitleBuff = "Indices for: ";
	TitleBuff += NameBuff;
	
	SetTitle(TitleBuff.c_str());
	
	_SetupMenus(framerect);
	_SetupView(framerect);

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(menubar)
		.Add(displayview);
	
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
	//AddChild(menubar);
}

void IndexWin::_SetupView(BRect frame)
{
	displayview = new BColumnListView("displayview", B_FRAME_EVENTS|B_NAVIGABLE, B_NO_BORDER);

	//info columns
	int32 i = 0;
	displayview->AddColumn(new BStringColumn(B_TRANSLATE("Name"), 115, 10, 600, B_TRUNCATE_END), i++);
	displayview->AddColumn(new BStringColumn(B_TRANSLATE("Type"), 100, 10, 600, 0), i++);
	displayview->AddColumn(new BSizeColumn(B_TRANSLATE("Size"), 50, 10, 600), i++);
	displayview->AddColumn(new BDateColumn(B_TRANSLATE("ModTime"), 108, 10, 600), i++);
	displayview->AddColumn(new BDateColumn(B_TRANSLATE("CreateTime"), 108, 10, 600), i++);
	displayview->AddColumn(new BStringColumn(B_TRANSLATE("UID"), 50, 10, 600, 0), i++);
	displayview->AddColumn(new BStringColumn(B_TRANSLATE("GID"), 50, 10, 600, 0), i++);

	//this scrollview encapsulates the displayview
	//AddChild(displayview);
}

// get the list of indices and display them
void IndexWin::_UpdateList()
{
	index_info info; // used for getting info on indices
	dev_t device;
	
	BListItem* listitem;
	DIR *indexdir;
	struct dirent* ent;
	
	displayview->Clear();
	/*
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
	*/
	
	device = TheVolume->Device(); //DevideID
	
	indexdir = fs_open_index_dir(device);
	if (indexdir == NULL)
	{
		char NameBuff[B_FILE_NAME_LENGTH];
		
		TheVolume->GetName(NameBuff);
		
		std::string AlertBuff = "Couldn't open index dir for volume: ";
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
		displayview->AddRow(new IndexListItem(1, ent->d_name, &info));
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
