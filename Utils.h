#ifndef UTILS_H
#define UTILS_H

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

namespace Utils
{
	// Safe release for pointers
	template<typename T> inline void safeRelease(T*& pointer)
	{
		if (pointer)
		{
			delete pointer;
			pointer = NULL;
		}
	}
}

#endif // UTILS_H
