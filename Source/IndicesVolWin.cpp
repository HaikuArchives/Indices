/*
	IndicesVolWin.cpp
	
	Copyright Will Dyson
	
	Created: 01/09/99 17:10:59
*/

// Interface Kit
#include <Window.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Menu.h>
#include <Messenger.h>
#include <ScrollView.h>
#include <ListView.h>
#include <Rect.h>
#include <Alert.h>

// App Kit
#include <Message.h>

// Storage kit
#include <fs_index.h>
#include <VolumeRoster.h>
#include <Volume.h>
#include <NodeMonitor.h>

// c++
#include <string.h>
// local
#include "IndVolItem.h"
#include "Indices.h"
#include "IndicesVolWin.h"

const uint32 OPEN_INDEX_LIST = 'OpIL';
const uint32 MENU_OPEN = 'Meop';


IndicesVolWin::IndicesVolWin()
: BWindow(BRect(50, 75, 300, 300), "Volumes",B_DOCUMENT_WINDOW, 0)
{
	volroster = new BVolumeRoster();
	BRect frame = Bounds(); //Bounds() is slow - perform once
	BMessenger messenger(this); // send messages to this object
	
	_SetupMenu(frame);
	_SetupView(frame);
	_SetupVolList();
	
	
/*
	Watch for changes in volume list. Change messages
	will be sent with the specified BMessenger, which targets
	our window object.
*/

	volroster->StartWatching(messenger);
	
	Show();
}

IndicesVolWin::~IndicesVolWin()
{
	volroster->StopWatching();
	delete volroster;
	be_app->PostMessage(B_QUIT_REQUESTED); // kill app
}

/*
	Add menus to window
*/
void IndicesVolWin::_SetupMenu(BRect frame)
{
	BMenu* amenu;
	
	menubar = new BMenuBar(frame, "menubar");
	amenu = new BMenu("Volumes");
	
	amenu->AddItem(new BMenuItem("Open", new BMessage(MENU_OPEN), 'O'));
	amenu->AddSeparatorItem();
	amenu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
	
	menubar->AddItem(amenu);
	AddChild(menubar);
}

/*
	Create BListView and a scrollview to contain it.
*/
void IndicesVolWin::_SetupView(BRect frame)
{
	BScrollView* scrollview;
	
	//make room for scrollbars
	frame.right -= B_V_SCROLL_BAR_WIDTH;
	frame.bottom -= B_H_SCROLL_BAR_HEIGHT;
	
	//make room for menubar
	frame.top = menubar->Bounds().bottom + 1.0;
	
	displayview = new BListView(frame, "VolList", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL_SIDES);
	
	scrollview = new BScrollView("scrollview", displayview, B_FOLLOW_ALL_SIDES, 0, true, true, B_NO_BORDER);
	
	displayview->SetInvocationMessage(new BMessage(OPEN_INDEX_LIST));
	
	AddChild(scrollview);
}


/*
	Get the list of volumes and put them into
	our list.
*/
void IndicesVolWin::_SetupVolList(void)
{
	BVolume myvol;
	BListItem* listitem;
	
	
	for (int32 i = 0; i < displayview->CountItems(); i++)
			delete displayview->ItemAt(i);

	displayview->MakeEmpty();

	// make list
	volroster->Rewind();
	while (volroster->GetNextVolume(&myvol)!= B_BAD_VALUE)
	{
		// remove volumes without indices
		if (HasIndices(myvol) == true)
		{
			AddVol(myvol);
		}
	}
}

// does this volume respond to fs index calls?
bool IndicesVolWin::HasIndices(BVolume myvol)
{
	if(fs_open_index_dir(myvol.Device()) != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// adds volume to the volume list
void IndicesVolWin::AddVol(BVolume myvol)
{
	char NameBuff[B_FILE_NAME_LENGTH];
	
	myvol.GetName(NameBuff);
	// Skip packageManager directories
	if (strcmp(NameBuff, "system") == 0 || strcmp(NameBuff, "config") == 0)
		return;
	
	displayview->AddItem(new IndVolItem(&myvol, NameBuff));
}

bool IndicesVolWin::QuitRequested()
{
	return true;
}

void IndicesVolWin::MessageReceived(BMessage* message)
{
	switch (message->what)
	{
		case OPEN_INDEX_LIST:
		{
			int32 myindex;
			status_t result;
			BListItem* anitem;
			
			// find the index of the item selected
			// we have a single selection list now, but this
			// will have to change if we want a multiple selection list
			result = message->FindInt32("index", &myindex);
			if (result != B_OK)
			{
				break;
			}
			
			// use the index to get the item
			anitem = displayview->ItemAt(myindex);
			if (anitem == NULL)
			{
				break;
			}
			
			// use the item to get the BVolume
			// use the BVolume to init the Index window
			new IndexWin(dynamic_cast<IndVolItem*>(anitem)->Volume);
		}
		break;
		
		case MENU_OPEN:
		{
			// call invoke using the Invocation Message we set
			// in _SetupView. See BListView docs.
			displayview->Invoke(displayview->InvocationMessage());
		}
		break;
		
		// add a new volume to the list
		case B_NODE_MONITOR:
		{
			int32 opcode;
			
			if (message->FindInt32("opcode", &opcode) != B_OK)
			{
				break;
			}
			switch (opcode)
			{
				case B_DEVICE_MOUNTED:
				{
					status_t result;
					int32 devID;
					BVolume volume;
					
					result = message->FindInt32("new device", &devID);
					if (result != B_OK)
					{
						break;
					}
					
					volume.SetTo(devID);
					
					if (HasIndices(volume) == true)
					{
						AddVol(volume);
					}
				}
				break;
				
				case B_DEVICE_UNMOUNTED:
				{
					// just remake the list in this case
					_SetupVolList();
				}
				break;
			}
		}
		break;
		
		
		default:
		{
			BWindow::MessageReceived(message);
		}	
	}
}
