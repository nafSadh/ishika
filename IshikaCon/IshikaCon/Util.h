#ifndef ISHIKA_UTIL_H
#define ISHIKA_UTIL_H

int getrand(int min,int max, int x=0, int y=0);
int getRandNZ(int min,int max, int x=0, int y=0);
float sqr(float x);
float ptDistance(float x1, float y1, float x2, float y2);

namespace ishika{
	enum  BrushType
	{
		Simple,
		WetOnDry,
		WetOnWet,
		Blobby,
		Crunchy,
		Blobby2
	};
}

#endif ISHIKA_UTIL_H