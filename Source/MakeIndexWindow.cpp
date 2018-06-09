
//AppKit
#include <Application.h>

//InterfaceKit
#include <Alert.h>
#include <ColumnTypes.h>
#include <LayoutBuilder.h>
#include <Button.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Window.h>
#include <PopUpMenu.h>
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
#include "MakeIndexWindow.h"

//our message->what constant:
const uint32 MSG_CHOOSE_TYPE = 'MTyp';
const uint32 MSG_CREATE_INDEX = 'Mkin';
const uint32 MSG_CHANGE_NAME = 'Mchn';
#define B_TRANSLATE(x)	x


MakeIndexWindow::MakeIndexWindow(BVolume* Volume)
: BWindow(BRect(25, 75, 350, 400), B_EMPTY_STRING, B_TITLED_WINDOW,  B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fIndexType(-1)
{
	char NameBuff[B_FILE_NAME_LENGTH];
	
	
	BRect framerect = Bounds();
	
	TheVolume = new BVolume(*Volume);
	
	TheVolume->GetName(NameBuff);
	std::string TitleBuff = "Create new index on: ";
	TitleBuff += NameBuff;
	
	SetTitle(TitleBuff.c_str());
	
	fIndexName = new BTextControl("Index name:", "", new BMessage(MSG_CHANGE_NAME));
	fIndexName->SetModificationMessage(new BMessage(MSG_CHANGE_NAME));
	fTypeMenu = new BPopUpMenu(B_TRANSLATE("Choose a type"));
	
	BMessage* message = new BMessage(MSG_CHOOSE_TYPE);
	message->AddInt32("IndexType", B_INT32_TYPE);
	fTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Int32"), message));
	message = new BMessage(MSG_CHOOSE_TYPE);
	message->AddInt32("IndexType", B_INT64_TYPE);
	fTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Int64"), message));
	message = new BMessage(MSG_CHOOSE_TYPE);
	message->AddInt32("IndexType", B_STRING_TYPE);
	fTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("String"), message));
	message = new BMessage(MSG_CHOOSE_TYPE);
	message->AddInt32("IndexType", B_FLOAT_TYPE);
	fTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Float"), message));
	message = new BMessage(MSG_CHOOSE_TYPE);
	message->AddInt32("IndexType", B_DOUBLE_TYPE);
	fTypeMenu->AddItem(new BMenuItem(B_TRANSLATE("Double"), message));

	
	//myMenu->ItemAt(0)->SetMarked(true);

/*
	if (strncmp("int", optarg, 3) == 0)
					indexType = B_INT32_TYPE;
				else if (strncmp("llong", optarg, 5) == 0)
					indexType = B_INT64_TYPE;
				else if (strncmp("string", optarg, 6) == 0)
					indexType = B_STRING_TYPE;
				else if (strncmp("float", optarg, 5) == 0)
					indexType = B_FLOAT_TYPE;
				else if (strncmp("double", optarg, 6) == 0)
					indexType = B_DOUBLE_TYPE;
				else
					usage(1);
*/	
	BMenuField* menuField = new BMenuField(NULL, B_TRANSLATE("Type:"), fTypeMenu);
	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.SetInsets(B_USE_WINDOW_SPACING, B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
		.AddGrid(B_USE_HALF_ITEM_SPACING, B_USE_HALF_ITEM_SPACING)
			.Add(fIndexName->CreateLabelLayoutItem(), 0, 0)
			.Add(fIndexName->CreateTextViewLayoutItem(), 1, 0)
			.Add(menuField->CreateLabelLayoutItem(), 0, 1)
			.Add(menuField->CreateMenuBarLayoutItem(), 1, 1)
		.End()
		.AddGroup(B_HORIZONTAL)
			.AddGlue()
			.Add(fCreateButton = new BButton("createButton", "Create", new  BMessage('Mkin')))
		.End();
	
	fCreateButton->SetEnabled(false);

	Show();
}

void MakeIndexWindow::_UpdateCreateButton()
{
	fCreateButton->SetEnabled((fIndexType != -1) && strcmp(fIndexName->Text(),"") != 0);
}


bool MakeIndexWindow::QuitRequested()
{
	return true;
}

void MakeIndexWindow::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case MSG_CHOOSE_TYPE:
		{
			fIndexType = message->FindInt32("IndexType");
			_UpdateCreateButton();
		}
		break;
		case MSG_CHANGE_NAME:
		{
			_UpdateCreateButton();
		}
		break;
		case MSG_CREATE_INDEX:
		{
			if (fs_create_index(TheVolume->Device(), fIndexName->Text(), fIndexType, 0) != 0)
				//fprintf(stderr, "%s: Could not create index: %s\n", "x", strerror(errno));
				fprintf(stderr, "ERROR\n");
				// TODO Alert
			Close();
		}
		break;
		
		
		
		default:
		{
			BWindow::MessageReceived(message);
		}
	}
}
