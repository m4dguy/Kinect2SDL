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

	// Convert uint16 depth image to RGBquad for display.
	// Compresses color value range, but still allows wrap-around if compressed value exceeds 255.
	inline void compress(UINT16* depth, void* wrapped, const uint depthWidth, const unsigned int depthHeight, const unsigned int cutoff = 10){
		BYTE value;
		BYTE* ptr = (BYTE*)wrapped;
		const unsigned int length = depthWidth * depthHeight;
		for (uint i = 0; i < length; ++i){
			unsigned int index = i * sizeof(RGBQUAD);
			value = (BYTE)(depth[i] / cutoff);
			ptr[index + 0] = value;
			ptr[index + 1] = value;
			ptr[index + 2] = value;
		}
	}   

	// Convert uint16 depth image to RGBquad for display.
	// Clamps values to cutoff value, then rescales them to range [0, 255].
	inline void cutoffRescale(UINT16* depth, void* wrapped, const uint depthWidth, const unsigned int depthHeight, const unsigned int cutoff = 2000){
		BYTE value;
		BYTE* ptr = (BYTE*)wrapped;
		const float factor = 255.f / cutoff;
		const unsigned int length = depthWidth * depthHeight;
		for (uint i = 0; i < length; ++i){
			unsigned int index = i * sizeof(RGBQUAD);
			value = (BYTE)(min(depth[i], cutoff) * factor);
			ptr[index + 0] = value;
			ptr[index + 1] = value;
			ptr[index + 2] = value;
		}
	}
}

#endif // UTILSKINECT_H
