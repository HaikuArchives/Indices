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
#include <Clipboard.h>
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
#include "MakeIndexWindow.h"

//our message->what constant:
const uint32 MENU_UPDATE = 'MeUp';
const uint32 MENU_MKINDEX = 'MeMk';
const uint32 MENU_RMINDEX = 'Mrmi';
const uint32 MENU_COPYINDICES = 'MCop';
const uint32 MENU_PASTEINDICES = 'MPas';
const uint32 MENU_SELECTALL = 'MSeA';

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

IndexWin::IndexWin(BVolume* volume)
: BWindow(BRect(25, 75, 650, 400), "Indices for: ", B_DOCUMENT_WINDOW, 0)
{
	char NameBuff[B_FILE_NAME_LENGTH];
	
	
	BRect framerect = Bounds();
	
	fVolume = new BVolume(*volume);
	
	fVolume->GetName(NameBuff);
	std::string TitleBuff = "Indices for: ";
	TitleBuff += NameBuff;
	
	SetTitle(TitleBuff.c_str());
	
	fMenuBar = _SetupMenus();
	fDisplayView = _SetupView();

	BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
		.Add(fMenuBar)
		.Add(fDisplayView);
	_UpdateList();

	fClipboard = new BClipboard("Indices");

	Show();
}

BMenuBar* IndexWin::_SetupMenus()
{
	BMenu* amenu;
	BMenuBar* menuBar = new BMenuBar("menubar");
	amenu = new BMenu(B_TRANSLATE("Indices"));
	amenu->AddItem(new BMenuItem(B_TRANSLATE("New index"), new BMessage(MENU_MKINDEX), 'N'));
	amenu->AddItem(fRemoveIndexItem = new BMenuItem(B_TRANSLATE("Remove indices"), new BMessage(MENU_RMINDEX), 'R'));
	amenu->AddSeparatorItem();
	amenu->AddItem(new BMenuItem(B_TRANSLATE("Update List"), new BMessage(MENU_UPDATE), 'U'));
	menuBar->AddItem(amenu);
	amenu = new BMenu(B_TRANSLATE("Edit"));
	amenu->AddItem(fCopyIndicesItem = new BMenuItem(B_TRANSLATE("Copy"), new BMessage(MENU_COPYINDICES), 'C'));
	amenu->AddItem(fPasteIndicesItem = new BMenuItem(B_TRANSLATE("Paste"), new BMessage(MENU_PASTEINDICES), 'V'));
	amenu->AddSeparatorItem();
	amenu->AddItem(new BMenuItem(B_TRANSLATE("Select all"), new BMessage(MENU_SELECTALL), 'A'));
	menuBar->AddItem(amenu);
	return menuBar;
}

BColumnListView* IndexWin::_SetupView()
{
	BColumnListView* displayView = new BColumnListView("DisplayView", B_FRAME_EVENTS|B_NAVIGABLE, B_NO_BORDER);

	//info columns
	int32 i = 0;
	displayView->AddColumn(new BStringColumn(B_TRANSLATE("Name"), 115, 10, 600, B_TRUNCATE_END), i++);
	displayView->AddColumn(new BStringColumn(B_TRANSLATE("Type"), 100, 10, 600, 0), i++);
	displayView->AddColumn(new BSizeColumn(B_TRANSLATE("Size"), 50, 10, 600), i++);
	displayView->AddColumn(new BDateColumn(B_TRANSLATE("ModTime"), 108, 10, 600), i++);
	displayView->AddColumn(new BDateColumn(B_TRANSLATE("CreateTime"), 108, 10, 600), i++);
	displayView->AddColumn(new BStringColumn(B_TRANSLATE("UID"), 50, 10, 600, 0), i++);
	displayView->AddColumn(new BStringColumn(B_TRANSLATE("GID"), 50, 10, 600, 0), i++);

	return displayView;
}

// get the list of indices and display them
void IndexWin::_UpdateList()
{
	index_info info; // used for getting info on indices
	dev_t device;
	
	BListItem* listitem;
	DIR *indexdir;
	struct dirent* ent;
	
	fDisplayView->Clear();
	
	device = fVolume->Device(); //DevideID
	
	indexdir = fs_open_index_dir(device);
	if (indexdir == NULL)
	{
		char NameBuff[B_FILE_NAME_LENGTH];
		
		fVolume->GetName(NameBuff);
		
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
		fDisplayView->AddRow(new IndexListItem(1, ent->d_name, &info));
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
		case MENU_MKINDEX:
		{
			(new MakeIndexWindow(fVolume, this))->Show();
			break;
		}
		case MENU_RMINDEX:
		{
			if (fDisplayView->CurrentSelection(NULL) == NULL)
				break;
			BString alertText;
			/*
			int32 count = 0;
			BRow* countRow = NULL;
			while((countRow = fDisplayView->CurrentSelection(countRow)))
				++count;

			static BMessageFormat formatText(B_TRANSLATE("{0, plural,"
					"=1{Deleting this index cannot be undone.\nAre you sure?}"
					"other{Deleting these # indices cannot be undone.\nAre you sure?}}"));
			formatText.Format(alertText, count);
			*/
			alertText = "Removing these indices cannot be undone.\nAre you sure?";
			if ((new BAlert("", alertText, B_TRANSLATE("Remove"),
				B_TRANSLATE("Cancel"), NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT))->Go() == 1)
				break;
			BRow* row = NULL;
			while((row = fDisplayView->CurrentSelection(row))) {
				BStringField *intNameField = (BStringField *)row->GetField(0);
				if (fs_remove_index(fVolume->Device(), intNameField->String()) != 0)
					fprintf(stderr, "ERROR"); // TODO Alert
			}
			_UpdateList();
			break;
		}
		case MENU_SELECTALL:
		{
			int32 numberOfRows = fDisplayView->CountRows();
			for (int32 i = 0; i < numberOfRows; i++)
			{
				BRow* row = fDisplayView->RowAt(i);
				fDisplayView->AddToSelection(row);
			}
			break;
		}
		case MENU_COPYINDICES:
		{
			BMessage* clip = (BMessage *)NULL;
			if (fClipboard->Lock()) {
				fClipboard->Clear();
			if (clip = fClipboard->Data()) {
				BRow* row = NULL;
				while((row = fDisplayView->CurrentSelection(row))) {
					BStringField *intNameField = (BStringField *)row->GetField(0);
					clip->AddString("IndexName", intNameField->String());
					clip->AddInt32("IndexType", dynamic_cast<IndexListItem*>(row)->GetIndexType());
					printf("archive %s %d\n", intNameField->String(), dynamic_cast<IndexListItem*>(row)->GetIndexType());
				}
				fClipboard->Commit();
			}
			fClipboard->Unlock();
			}
			break;
		}
		case MENU_PASTEINDICES:
		{
			BMessage* clip = (BMessage *)NULL;
			if (fClipboard->Lock()) {
				if (clip = fClipboard->Data()) {
					type_code typeCode;
					int32 countFound;
					clip->GetInfo("IndexName", &typeCode, &countFound);
					for (int32 i = 0; i < countFound; i++)
					{
						BString indexName;
						int32 indexType;
						clip->FindString("IndexName", i, &indexName);
						clip->FindInt32("IndexType", i, &indexType);
						printf("create %s %d\n", indexName.String(), indexType);
						if (fs_create_index(fVolume->Device(), indexName.String(), indexType, 0) != 0)
							fprintf(stderr, "ERROR\n"); // TODO Alert
					}
				}
				fClipboard->Unlock();
			}
			_UpdateList();
			break;
		}
		default:
		{
			BWindow::MessageReceived(message);
		}
	}
}

void IndexWin::MenusBeginning()
{
	if (fDisplayView->CurrentSelection(NULL) != NULL) {
		fRemoveIndexItem->SetEnabled(true);
		fCopyIndicesItem->SetEnabled(true);
	} else {
		fRemoveIndexItem->SetEnabled(false);
		fCopyIndicesItem->SetEnabled(false);
	}
	BMessage* clip = (BMessage *)NULL;
	if (fClipboard->Lock()) {
		int32 countFound = 0;
		if (clip = fClipboard->Data()) {
			type_code typeCode;
			clip->GetInfo("IndexName", &typeCode, &countFound);
		}
		fPasteIndicesItem->SetEnabled(countFound > 0);
		fClipboard->Unlock();
	}
}

int main()
{
	IndexApp myapp;
	myapp.Run();
	return 0;
}
