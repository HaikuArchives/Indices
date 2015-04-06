/*
	IndVolItem.h
	
	Copyright Will Dyson
	
	Created: 01/09/99 23:09:09
	
	This program is freely distributable under the terms of
	the Gnu Public License. If you have a patch or new feature,
	please let me know. mailto:will@cs.earlham.edu
*/
#ifndef INDVOLITEM_H
#define INDVOLITEM_H

class BStringItem;
class BVolume;

class IndVolItem : public BStringItem
{
	public:
	IndVolItem(BVolume* vol, char* string);
	
	BVolume* Volume;
};

#endif //INDVOLITEM_H
