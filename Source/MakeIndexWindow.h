

#ifndef MAKE_INDEX_WINDOW_H
#define MAKE_INDEX_WINDOW_H

class BApplication;
class BColumnListView;
class BWindow;
class BMessage;
class BVolume;
class BRect;
class BMenuBar;
class BPopUpMenu;

class MakeIndexWindow : public BWindow
{
	public:
	
	MakeIndexWindow(BVolume* Volume, BWindow* indexWindow);
	
	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage* message);
	
	private:
	
	void _UpdateCreateButton();

	BWindow* fIndexWindow;
	
	BColumnListView* displayview;
	BMenuBar* menubar;
	BVolume* TheVolume;
	BButton* fCreateButton;
	BTextControl* fIndexName;
	BPopUpMenu* fTypeMenu;
	int32 fIndexType;
};

#endif // INDEX_H
