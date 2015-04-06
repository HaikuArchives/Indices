/*
	IndVolItem.cpp
	
	Copyright Will Dyson
	
	Created: 01/09/99 23:28:21
	
	This program is freely distributable under the terms of
	the Gnu Public License. If you have a patch or new feature,
	please let me know. mailto:will@cs.earlham.edu
*/
#include <ListItem.h>
#include <Volume.h>

#include "IndVolItem.h"

IndVolItem::IndVolItem(BVolume* vol, char* string)
: BStringItem(string)
{
	Volume = new BVolume(*vol); // store a copy of the BVolume passed in.
}
