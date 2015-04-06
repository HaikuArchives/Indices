#include "PrefilledBitmap.h"

PrefilledBitmap::PrefilledBitmap(BRect bounds, color_space space, const void *data, int32 length,
	bool acceptsViews, bool needsContiguousMemory)
: BBitmap(bounds, space, acceptsViews, needsContiguousMemory)
{
	if(length == 0)
	{
		if(space == B_CMAP8)
			length = ((int32(bounds.right-bounds.left+1)+3)&0xFFFFFFFC)*int32(bounds.bottom-bounds.top+1);
		else if(space == B_RGB32)
			length = int32(bounds.right-bounds.left+1)*int32(bounds.bottom-bounds.top+1)*3;
	}
	SetBits(data, length, 0, space);
}


PrefilledBitmap::~PrefilledBitmap()
{ }