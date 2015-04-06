#ifndef INDICESVOLWIN_H
#define INDICESVOLWIN_H
/*
	IndicesVolWin.h
	
	Copyright Will Dyson
	
	Created: 01/09/99 16:53:48
*/

class BWindow;
class BVolumeRoster;
class BMenuBar;
class BListView;

class IndicesVolWin : public BWindow
{
	public:
	
	IndicesVolWin();
	~IndicesVolWin();
	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage* message);
	
	private:
	
	void _SetupMenu(BRect frame);
	void _SetupView(BRect frame);
	void _SetupVolList(void);
	bool HasIndices(BVolume vol);
	void AddVol(BVolume vol);
	
	BVolumeRoster* volroster;
	BMenuBar* menubar;
	BListView* displayview;
};

#endif //INDICESVOLWIN_H
