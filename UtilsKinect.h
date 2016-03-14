#ifndef UTILSKINECT_H
#define UTILSKINECT_H

namespace UtilsKinect
{
	// Safe release for interfaces
	template<class Interface> inline void safeRelease(Interface *& pInterfaceToRelease){
		if (pInterfaceToRelease != NULL){
			pInterfaceToRelease->Release();
			pInterfaceToRelease = NULL;
		}
	}

	// convert uint16 depth image to RGBquad for display
	inline void compressDepth(UINT16* depth, void* wrapped, const uint depthWidth, const unsigned int depthHeight, const unsigned int cutoff = 10){
		BYTE* ptr = (BYTE*)wrapped;
		const unsigned int length = depthWidth * depthHeight;
		for (uint i = 0; i < length; ++i){
			unsigned int index = i * sizeof(RGBQUAD);
			ptr[index + 0] = (BYTE)(depth[i] / cutoff);
			ptr[index + 1] = (BYTE)(depth[i] / cutoff);
			ptr[index + 2] = (BYTE)(depth[i] / cutoff);
		}
	}
}

#endif // UTILSKINECT_H
