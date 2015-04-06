#include <Looper.h>
#include "MouseDragOrPopupWatcher.h"


int32 MouseDragOrPopupWatcher(void* data);


thread_id StartMouseDragOrPopupWatcher(BView* TargetView)
{
	thread_id MouseWatcherThread = spawn_thread(MouseDragOrPopupWatcher,"MouseDragOrPopupWatcher",
		B_NORMAL_PRIORITY,new BMessenger(TargetView));
	if(MouseWatcherThread != B_NO_MORE_THREADS && MouseWatcherThread != B_NO_MEMORY)
		resume_thread(MouseWatcherThread);
	return MouseWatcherThread;
}


int32 MouseDragOrPopupWatcher(void* data)
{
	BMessenger* TheMessenger = (BMessenger*)data;
	BPoint StartPos;
	uint32 StartButtons;
	bigtime_t StartTime;
	bool FirstCheck = true;
	BMessage MessageToSend;
	MessageToSend.AddPoint("where",BPoint(0,0));
	MessageToSend.AddInt32("buttons",0);
	MessageToSend.AddInt32("modifiers",0);
	bigtime_t PopupTime;
	if(get_click_speed(&PopupTime) != B_OK)
		return 0;
	PopupTime *= 2;
	while(true)
	{
		if (!TheMessenger->LockTarget())
		{
			delete TheMessenger;
			return 0;			// window is dead so exit
		}
		BLooper *TheLooper;
		BView* TheView = (BView*)TheMessenger->Target(&TheLooper);
		BPoint Where;
		uint32 Buttons;
		bigtime_t Time = system_time();
		TheView->GetMouse(&Where,&Buttons,false);
		if(FirstCheck)
		{
			StartPos = Where;
			StartButtons = Buttons;
			StartTime = Time;
			FirstCheck = false;
		}
		bool Send = false;
		if(Buttons == 0 || Buttons != StartButtons)
		{
			//Changed or released
			MessageToSend.what = MW_MOUSE_CLICK;
			Send = true;
		}
		else if(Where.x < StartPos.x-1.0 || Where.x > StartPos.x+1.0 ||
			Where.y < StartPos.y-1.0 || Where.y > StartPos.y+1.0)
		{
			MessageToSend.what = MW_MOUSE_DRAG;
			Send = true;
		}
		else if(Time >= StartTime + PopupTime)
		{
			MessageToSend.what = MW_MOUSE_POPUP;
			Send = true;
		}
		TheLooper->Unlock();
		if(Send)
		{
			MessageToSend.ReplacePoint("where",StartPos);
			MessageToSend.ReplaceInt32("buttons",Buttons);
			MessageToSend.ReplaceInt32("modifiers",modifiers());
			TheMessenger->SendMessage(&MessageToSend);
			delete TheMessenger;
			return 0;
		}
		snooze(50000);
	}
}
