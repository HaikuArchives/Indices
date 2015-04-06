/*
	IndexListItem.h
	
	Copyright Will Dyson
	
	Created: 01/07/99 17:34:34
	
	Represents a multi-feild item in the list. Largely cribbed from the demo
	code supplied with ColumnListView. Many thanks to Brian Tietz for providing
	Santa's Gift Bag.
	
*/
#ifndef INDEX_ITEMS_H
#define INDEX_ITEMS_H

#include "CLVEasyItem.h"

class IndexListItem : public CLVEasyItem
{
	public:
		IndexListItem(uint32 level, char* AttrName, index_info* info, bool superitem = false, bool expanded = false);

	private:
		const char* GetTypeString(uint32 type);
		const char* GetSizeString(off_t size);
		const char* GetTimeString(time_t time);
		const char* GetUIDString(uid_t uid);
		const char* GetGIDString(gid_t gid);
		
		char buf[32];
};

#endif //INDEX_ITEMS_H
