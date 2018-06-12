/*
	Indices.h
	
	Copyright Will Dyson
	
	Created: 01/06/99 19:05:34
	
	This program is freely distributable under the terms of
	the Gnu Public License. If you have a patch or new feature,
	please let me know. mailto:will@cs.earlham.edu
*/

#ifndef INDEX_H
#define INDEX_H

class BApplication;
class BColumnListView;
class BWindow;
class BMessage;
class BVolume;
class BRect;
class BMenuBar;
class BClipboard;

class IndexApp : public BApplication
{
	public:
	
	IndexApp();
	~IndexApp();
};

class IndexWin : public BWindow
{
	public:
	
	IndexWin(BVolume* Volume);
	
	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage* message);
	virtual void MenusBeginning();
	private:
	
	BMenuBar* _SetupMenus();
	BColumnListView* _SetupView();
	void _UpdateList();
	
	BColumnListView* fDisplayView;
	BMenuBar* fMenuBar;
	BVolume* fVolume;
	BClipboard* fClipboard;
	BMenuItem* fRemoveIndexItem;
	BMenuItem* fCopyIndicesItem;
	BMenuItem* fPasteIndicesItem;
};

#endif // INDEX_H
