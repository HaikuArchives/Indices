/*
	IndexListItem.cpp
	
	Copyright Will Dyson
	
	Created: 01/07/99 17:34:29
	
	This program is freely distributable under the terms of
	the Gnu Public License. If you have a patch or new feature,
	please let me know. mailto:will@cs.earlham.edu
*/
#include <ColumnTypes.h>
#include <View.h>
#include <Region.h>
#include <Font.h>

#include <fs_index.h>
#include <TypeConstants.h>

//posix
#include <time.h>

//c++
#include <stdio.h>

#include "IndexListItem.h"

IndexListItem::IndexListItem(uint32 level, char* AttrName, index_info* info, bool superitem, bool expanded)
: BRow()
{
	SetField(new BStringField(AttrName), 0);
	SetField(new BStringField(GetTypeString(info->type)), 1);
	SetField(new BSizeField(info->size), 2);
	SetField(new BDateField(&info->modification_time), 3);
	SetField(new BDateField(&info->creation_time), 4);
	SetField(new BStringField(GetUIDString(info->uid)), 5);
	SetField(new BStringField(GetGIDString(info->gid)), 6);
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
