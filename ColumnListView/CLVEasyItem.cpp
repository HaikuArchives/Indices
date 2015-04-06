//CLVListItem source file


//******************************************************************************************************
//**** SYSTEM HEADER FILES
//******************************************************************************************************
#include <string.h>
#include <Region.h>


//******************************************************************************************************
//**** PROJECT HEADER FILES
//******************************************************************************************************
#include "CLVEasyItem.h"
#include "Colors.h"
#include "CLVColumn.h"
#include "ColumnListView.h"


//******************************************************************************************************
//**** TYPE DEFINITIONS AND CONSTANTS
//******************************************************************************************************
enum
{
	CLVColNone =				0x00000000,
	CLVColStaticText = 			0x00000001,
	CLVColTruncateText =		0x00000002,
	CLVColBitmap = 				0x00000003,

	CLVColTypesMask =			0x00000003,
	CLVColFlagBitmapIsCopy =	0x00000004,
	CLVColFlagNeedsTruncation =	0x00000008
};


//******************************************************************************************************
//**** CLVEasyItem CLASS DEFINITION
//******************************************************************************************************
CLVEasyItem::CLVEasyItem(uint32 level, bool superitem, bool expanded, float minheight)
: CLVListItem(level,superitem,expanded,minheight)
{
	m_text_offset = 0.0;
}


CLVEasyItem::~CLVEasyItem()
{
	int num_columns = m_column_types.CountItems();
	for(int column = 0; column < num_columns; column++)
	{
		int32 type = (int32)m_column_types.ItemAt(column);
		bool bitmap_is_copy = false;
		if(type & CLVColFlagBitmapIsCopy)
			bitmap_is_copy = true;
		type &= CLVColTypesMask;
		if(type == 	CLVColStaticText || type == CLVColTruncateText)
			delete[] ((char*)m_column_content.ItemAt(column));
		if(type == CLVColTruncateText)
			delete[] ((char*)m_aux_content.ItemAt(column));
		if(type == CLVColBitmap && bitmap_is_copy)
			delete ((BBitmap*)m_column_content.ItemAt(column));
		delete (BRect*)m_cached_rects.ItemAt(column);
	}
}


void CLVEasyItem::PrepListsForSet(int column_index)
{
	int cur_num_columns = m_column_types.CountItems();
	bool delete_old = (cur_num_columns >= column_index-1);
	while(cur_num_columns <= column_index)
	{
		m_column_types.AddItem((void*)CLVColNone);
		m_column_content.AddItem(NULL);
		m_aux_content.AddItem(NULL);
		m_cached_rects.AddItem(new BRect(-1,-1,-1,-1));
		cur_num_columns++;
	}
	if(delete_old)
	{
		//Column content exists already so delete the old entries
		int32 old_type = (int32)m_column_types.ItemAt(column_index);
		bool bitmap_is_copy = false;
		if(old_type & CLVColFlagBitmapIsCopy)
			bitmap_is_copy = true;
		old_type &= CLVColTypesMask;

		void* old_content = m_column_content.ItemAt(column_index);
		char* old_truncated = (char*)m_aux_content.ItemAt(column_index);
		BRect* old_cached_rect = (BRect*)m_cached_rects.ItemAt(column_index);
		if(old_type == CLVColStaticText || old_type == CLVColTruncateText)
			delete[] ((char*)old_content);
		if(old_type == CLVColTruncateText)
			delete[] old_truncated;
		if(old_type == CLVColBitmap && bitmap_is_copy)
			delete ((BBitmap*)old_content);
		((BRect**)m_cached_rects.Items())[column_index]->Set(-1,-1,-1,-1);
	}
}


void CLVEasyItem::SetColumnContent(int column_index, const char *text, bool truncate)
{
	PrepListsForSet(column_index);

	//Create the new entry
	((BRect**)m_cached_rects.Items())[column_index]->Set(-1,-1,-1,-1);
	if(text == NULL || text[0] == 0)
	{
		((int32*)m_column_types.Items())[column_index] = CLVColNone;
		((char**)m_column_content.Items())[column_index] = NULL;
		((char**)m_aux_content.Items())[column_index] = NULL;
	}
	else
	{
		char* copy = new char[strlen(text)+1];
		strcpy(copy,text);
		((char**)m_column_content.Items())[column_index] = copy;

		if(!truncate)
		{
			((int32*)m_column_types.Items())[column_index] = CLVColStaticText;
			((char**)m_aux_content.Items())[column_index] = NULL;
		}
		else
		{
			((int32*)m_column_types.Items())[column_index] = CLVColTruncateText|CLVColFlagNeedsTruncation;
			copy = new char[strlen(text)+3];
			strcpy(copy,text);
			((char**)m_aux_content.Items())[column_index] = copy;
		}
	}
}


void CLVEasyItem::SetColumnContent(int column_index, const BBitmap *bitmap, float horizontal_offset, bool copy)
{
	PrepListsForSet(column_index);

	//Create the new entry
	if(bitmap == NULL)
	{
		((int32*)m_column_types.Items())[column_index] = CLVColNone;
		((char**)m_column_content.Items())[column_index] = NULL;
		((char**)m_aux_content.Items())[column_index] = NULL;
	}
	else
	{
		if(copy)
			((int32*)m_column_types.Items())[column_index] = CLVColBitmap|CLVColFlagBitmapIsCopy;
		else
			((int32*)m_column_types.Items())[column_index] = CLVColBitmap;
		BBitmap* the_bitmap;
		if(copy)
		{
			the_bitmap = new BBitmap(bitmap->Bounds(),bitmap->ColorSpace());
			int32 copy_ints = bitmap->BitsLength()/4;
			int32* source = (int32*)bitmap->Bits();
			int32* dest = (int32*)the_bitmap->Bits();
			for(int32 i = 0; i < copy_ints; i++)
				dest[i] = source[i];
		}
		else
			the_bitmap = (BBitmap*)bitmap;
		((BBitmap**)m_column_content.Items())[column_index] = the_bitmap;
		((int32*)m_aux_content.Items())[column_index] = (int32)horizontal_offset;
	}
}


void CLVEasyItem::DrawItemColumn(BView *owner, BRect item_column_rect, int32 column_index, bool complete)
{
	rgb_color color;
	bool selected = IsSelected();
	if(selected)
		color = BeListSelectGrey;
	else
		color = White;
	owner->SetLowColor(color);
	owner->SetDrawingMode(B_OP_COPY);
	if(selected || complete)
	{
		owner->SetHighColor(color);
		owner->FillRect(item_column_rect);
	}
	if(column_index == -1)
		return;

	BRegion Region;
	Region.Include(item_column_rect);
	owner->ConstrainClippingRegion(&Region);

	*((BRect*)m_cached_rects.ItemAt(column_index)) = item_column_rect;

	int32 type = (int32)m_column_types.ItemAt(column_index);
	bool needs_truncation = false;
	if(type & CLVColFlagNeedsTruncation)
		needs_truncation = true;
	type &= CLVColTypesMask;
	if(type == CLVColStaticText || type == CLVColTruncateText)
	{
		char* text;

		owner->SetHighColor(Black);
		if(type == CLVColTruncateText)
		{
			if(needs_truncation)
			{
				TruncateText(column_index,item_column_rect.right-item_column_rect.left);
				((int32*)m_column_types.Items())[column_index] = CLVColTruncateText;
			}
			text = (char*)m_aux_content.ItemAt(column_index);			
		}
		else
			text = (char*)m_column_content.ItemAt(column_index);
		
		owner->DrawString(text,BPoint(item_column_rect.left+2.0,item_column_rect.top+m_text_offset));
	}
	else if(type == CLVColBitmap)
	{
		const BBitmap* bitmap = (BBitmap*)m_column_content.ItemAt(column_index);
		BRect bounds = bitmap->Bounds();
		float horizontal_offset = (float)((int32)m_aux_content.ItemAt(column_index));
		item_column_rect.left += horizontal_offset;
		item_column_rect.right = item_column_rect.left + (bounds.right-bounds.left);
		item_column_rect.top += ceil((item_column_rect.bottom-item_column_rect.top)-(bounds.bottom-bounds.top));
		item_column_rect.bottom = item_column_rect.top + (bounds.bottom-bounds.top);
		owner->SetDrawingMode(B_OP_OVER);
		owner->DrawBitmap(bitmap,item_column_rect);
		owner->SetDrawingMode(B_OP_COPY);
	}
	owner->ConstrainClippingRegion(NULL);
}


char tolower(char x);	//Kinda lame that I HAVE to have a prototype....
char tolower(char x)
{
	if(x >= 'A' && x <= 'Z')
		return x-'A'+'a';
	else
		return x;
}


void CLVEasyItem::Update(BView *owner, const BFont *font)
{
	CLVListItem::Update(owner,font);
	font_height FontAttributes;
	be_plain_font->GetHeight(&FontAttributes);
	float FontHeight = ceil(FontAttributes.ascent) + ceil(FontAttributes.descent);
	m_text_offset = ceil(FontAttributes.ascent) + (Height()-FontHeight)/2.0;
}


int CLVEasyItem::CompareItems(const CLVListItem *a_Item1, const CLVListItem *a_Item2, int32 KeyColumn)
{
	const CLVEasyItem* Item1 = cast_as(a_Item1,const CLVEasyItem);
	const CLVEasyItem* Item2 = cast_as(a_Item2,const CLVEasyItem);
	if(Item1 == NULL || Item2 == NULL || Item1->m_column_types.CountItems() <= KeyColumn ||
		Item2->m_column_types.CountItems() <= KeyColumn)
		return 0;
	
	int32 type1 = ((int32)Item1->m_column_types.ItemAt(KeyColumn)) & CLVColTypesMask;
	int32 type2 = ((int32)Item2->m_column_types.ItemAt(KeyColumn)) & CLVColTypesMask;

	if(!((type1 == CLVColStaticText || type1 == CLVColTruncateText) &&
		(type2 == CLVColStaticText || type2 == CLVColTruncateText)))
		return 0;

	char* Temp = (char*)Item1->m_column_content.ItemAt(KeyColumn);
	char* Text1 = new char[strlen(Temp)+1];
	for(int Counter = 0; Counter <= strlen(Temp); Counter++)
		Text1[Counter] = tolower(Temp[Counter]);	

	Temp = (char*)Item2->m_column_content.ItemAt(KeyColumn);
	char* Text2 = new char[strlen(Temp)+1];
	for(int Counter = 0; Counter <= strlen(Temp); Counter++)
		Text2[Counter] = tolower(Temp[Counter]);	

	int Result = strcmp(Text1,Text2);
	delete[] Text1;
	delete[] Text2;
	return Result;
}


BRect CLVEasyItem::TruncateText(int32 column_index, float column_width)
{
	column_width -= 4;
		//Because when I draw the text I start drawing 2 pixels to the right from the column's left edge, and want
		//to stop 2 pixels before the right edge
	BRect invalid(-1,-1,-1,-1);
	char* full_text = (char*)m_column_content.ItemAt(column_index);
	char* new_text = new char[strlen(full_text)+3];
	char* truncated_text = (char*)m_aux_content.ItemAt(column_index);
	GetTruncatedString(full_text,new_text,column_width);
	if(strcmp(truncated_text,new_text)!=0)
	{
		//The truncated text has changed
		invalid = *((BRect*)m_cached_rects.ItemAt(column_index));
		if(invalid != BRect(-1,-1,-1,-1))
		{
			//Figure out which region just got changed
			int32 cmppos;
			int32 cmplen = strlen(new_text);
			char remember;
			for(cmppos = 0; cmppos <= cmplen; cmppos++)
				if(new_text[cmppos] != truncated_text[cmppos])
				{
					remember = new_text[cmppos];
					new_text[cmppos] = 0;
					break;
				}
			invalid.left += 2 + be_plain_font->StringWidth(new_text);
			new_text[cmppos] = remember;
		}
		//Remember the new truncated text
		strcpy(truncated_text,new_text);
	}
	delete[] new_text;
	return invalid;
}


void CLVEasyItem::ColumnWidthChanged(int32 column_index, float column_width, ColumnListView *the_view)
{
	float usable_width = column_width - ExpanderShift(column_index,the_view);
	BRect* cached_rect = (BRect*)m_cached_rects.ItemAt(column_index);
	float width_delta = usable_width-(cached_rect->right-cached_rect->left);		
	cached_rect->right += width_delta;

	int num_columns = m_cached_rects.CountItems();
	for(int column = 0; column < num_columns; column++)
		if(column != column_index)
		{
			BRect* other_rect = (BRect*)m_cached_rects.ItemAt(column);
			if(other_rect->left > cached_rect->left)
				other_rect->OffsetBy(width_delta,0);
		}

	int32 type = ((int32)m_column_types.ItemAt(column_index)) & CLVColTypesMask;
	if(type == CLVColTruncateText && *cached_rect != BRect(-1,-1,-1,-1))
	{
		//If it's onscreen, truncate and invalidate the changed area
		if(cached_rect->Intersects(the_view->Bounds()))
		{
			BRect invalid = TruncateText(column_index, usable_width);
			((int32*)m_column_types.Items())[column_index] = CLVColTruncateText;
			if(invalid != BRect(-1.0,-1.0,-1.0,-1.0))
				the_view->Invalidate(invalid);
		}
		else
			//If it's not onscreen flag it for truncation the next time it's drawn
			((int32*)m_column_types.Items())[column_index] = CLVColTruncateText|CLVColFlagNeedsTruncation;
	}
}


void CLVEasyItem::FrameChanged(int32 column_index, BRect new_frame, ColumnListView *the_view)
{
	BRect* cached_rect = (BRect*)m_cached_rects.ItemAt(column_index);
	if(*cached_rect != new_frame)
	{
		*cached_rect = new_frame;

		//If it's onscreen, truncate and invalidate the changed area
		if(new_frame.Intersects(the_view->Bounds()))
		{
			BRect invalid = TruncateText(column_index, new_frame.right-new_frame.left);
			((int32*)m_column_types.Items())[column_index] = CLVColTruncateText;
			if(invalid != BRect(-1.0,-1.0,-1.0,-1.0))
				the_view->Invalidate(invalid);
		}
		else
			//If it's not onscreen flag it for truncation the next time it's drawn
			((int32*)m_column_types.Items())[column_index] = CLVColTruncateText|CLVColFlagNeedsTruncation;
	}
}

