//Conventions:
//    Global constants (declared with const) and #defines - all uppercase letters with words separated 
//        by underscores.
//        (E.G., #define MY_DEFINE 5).
//        (E.G., const int MY_CONSTANT = 5;).
//    New data types (classes, structs, typedefs, etc.) - begin with an uppercase letter followed by
//        lowercase words separated by uppercase letters.  Enumerated constants contain a prefix
//        associating them with a particular enumerated set.
//        (E.G., typedef int MyTypedef;).
//        (E.G., enum MyEnumConst {MEC_ONE, MEC_TWO};)
//    Global variables - begin with "g_" followed by lowercase words separated by underscores.
//        (E.G., int g_my_global;).
//    Argument and local variables - begin with a lowercase letter followed by
//        lowercase words separated by underscores.
//        (E.G., int my_local;).
//    Member variables - begin with "m_" followed by lowercase words separated by underscores.
//        (E.G., int m_my_member;).
//    Functions (member or global) - begin with an uppercase letter followed by lowercase words
//        separated by uppercase letters.
//        (E.G., void MyFunction(void);).


#ifndef _SGB_BETTER_SCROLL_VIEW_H_
#define _SGB_BETTER_SCROLL_VIEW_H_


//******************************************************************************************************
//**** PROJECT HEADER FILES
//******************************************************************************************************
#include <ScrollView.h>


//******************************************************************************************************
//**** PROJECT HEADER FILES
//******************************************************************************************************
#include "ScrollViewCorner.h"


//******************************************************************************************************
//**** CLASS DECLARATIONS
//******************************************************************************************************
class BetterScrollView : public BScrollView
{
	public:
		BetterScrollView(const char *name, BView *target, uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_FRAME_EVENTS | B_WILL_DRAW, bool horizontal = true, bool vertical = true,
			bool scroll_view_corner = true, border_style border = B_FANCY_BORDER);
		virtual ~BetterScrollView();
		virtual void SetDataRect(BRect data_rect, bool scrolling_allowed = true);
		BRect GetDataRect();
		virtual	void FrameResized(float new_width, float new_height);
		void UpdateScrollBars(bool scrolling_allowed);

	private:
		BRect m_data_rect;
		BScrollBar* m_h_scrollbar;
		BScrollBar* m_v_scrollbar;
		ScrollViewCorner* m_scroll_view_corner;
		BView* m_target;
};


#endif