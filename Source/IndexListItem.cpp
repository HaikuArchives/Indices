/*
	IndexListItem.cpp
	
	Copyright Will Dyson
	
	Created: 01/07/99 17:34:29
	
	This program is freely distributable under the terms of
	the Gnu Public License. If you have a patch or new feature,
	please let me know. mailto:will@cs.earlham.edu
*/

#include <View.h>
#include <Region.h>
#include <Font.h>

#include <fs_index.h>
#include <TypeConstants.h>

//posix
#include <time.h>

//c++
#include <stdio.h>

#include "Colors.h"

#include "CLVEasyItem.h"

#include "IndexListItem.h"

IndexListItem::IndexListItem(uint32 level, char* AttrName, index_info* info, bool superitem, bool expanded)
: CLVEasyItem(level, superitem, expanded, 20.0)
{
	SetColumnContent(0, AttrName);
	SetColumnContent(1, GetTypeString(info->type));
	SetColumnContent(2, GetSizeString(info->size));
	SetColumnContent(3, GetTimeString(info->modification_time)); 
	SetColumnContent(4, GetTimeString(info->creation_time));
	SetColumnContent(5, GetUIDString(info->uid));
	SetColumnContent(6, GetGIDString(info->gid));
}


const char* IndexListItem::GetTypeString(uint32 type)
{
		switch (type)
		{
			case B_STRING_TYPE:
			{
				return ("String");
			}
			break;
			
			case B_INT64_TYPE:
			{
				return ("Int64");
			}
			break;
			
			case B_INT32_TYPE:
			{
				return ("Int32");
			}
			break;
			
			case B_FLOAT_TYPE:
			{
				return ("Float");
			}
			break;
			
			case B_DOUBLE_TYPE:
			{
				return ("Double");
			}
			break;
			
			default:
			{
				return ("Unknown");
			}
		}
}

const char* IndexListItem::GetSizeString(off_t size)
{
	int kbytes;
	
	kbytes = size / 1000;
	
	sprintf(buf, "%d K", kbytes);
	return (buf);
}

const char* IndexListItem::GetTimeString(time_t time)
{
	struct tm* mytime;
	
	mytime = localtime(&time);
	sprintf(buf, "%d/%d/%d %d:%d:%d", mytime->tm_mon+1, mytime->tm_mday, mytime->tm_year+1900, mytime->tm_hour, mytime->tm_min, mytime->tm_sec);
	
	return (buf);
}

const char* IndexListItem::GetUIDString(uid_t uid)
{
	// BeOS doesn't use UID or GID anyway
	return ("not implemented");
}

const char* IndexListItem::GetGIDString(gid_t gid)
{
	return ("not implemented");
}
