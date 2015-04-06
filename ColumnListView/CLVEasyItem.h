//CLVEasyItem header file


#ifndef _CLV_EASY_ITEM_H_
#define _CLV_EASY_ITEM_H_


//******************************************************************************************************
//**** SYSTEM HEADER FILES
//******************************************************************************************************
#include <List.h>


//******************************************************************************************************
//**** PROJECT HEADER FILES AND CLASS NAME DECLARATIONS
//******************************************************************************************************
#include "CLVListItem.h"


//******************************************************************************************************
//**** CLVEasyItem CLASS DECLARATION
//******************************************************************************************************
class CLVEasyItem : public CLVListItem
{
	public:
		//Constructor and destructor
		CLVEasyItem(uint32 level = 0, bool superitem = false, bool expanded = false, float minheight = 0.0);
		virtual ~CLVEasyItem();

		virtual void SetColumnContent(int column_index, const char *text, bool truncate = true);
		virtual void SetColumnContent(int column_index, const BBitmap *bitmap, float horizontal_offset = 2.0, bool copy = true);
		virtual void DrawItemColumn(BView* owner, BRect item_column_rect, int32 column_index, bool complete);
		virtual void Update(BView *owner, const BFont *font);
		static int CompareItems(const CLVListItem* a_Item1, const CLVListItem* a_Item2, int32 KeyColumn);
		BRect TruncateText(int32 column_index, float column_width);
			//Returns the area that needs to be redrawn, or BRect(-1,-1,-1,-1) if nothing
		virtual void ColumnWidthChanged(int32 column_index, float column_width, ColumnListView* the_view);
		virtual void FrameChanged(int32 column_index, BRect new_frame, ColumnListView* the_view);

	private:
		void PrepListsForSet(int column_index);
		
		BList m_column_types;	//List of int32's converted from CLVColumnTypes
		BList m_column_content;	//List of char* (full content) or BBitmap*
		BList m_aux_content;	//List of char* (truncated content) or int32 for bitmap horizontal offset
		BList m_cached_rects;	//List of BRect for truncated text
		float m_text_offset;
};


#endif
