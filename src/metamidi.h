#pragma once

#ifndef METAMIDI_H
#define METAMIDI_H

#include <memory>
#include <vector>
#include <string>
#include <stdint.h>
#include <iostream>
#include <exception>

///////////////
// Platforms //
///////////////

#if defined(WIN32) || defined(_WIN32)
	#define MM_PLATFORM_WINDOWS 1
	//#define __WINDOWS_MM__
#endif

#ifdef __APPLE__
    #define MM_PLATFORM_OSX 1
	//#define __MACOSX_CORE__
#endif

#if defined(__linux) || defined(__unix) || defined (__posix)
	#define MM_PLATFORM_POSIX
	//#define __LINUX_ALSA__
#endif

#if defined(MM_PLATFORM_WINDOWS)
	#define INLINE __forceinline
#else
	#define INLINE inline
#endif

#define MM_PI double(3.1415926535897932384626433832795)
#define MM_EULER double(2.7182818284590452353602874713527)
#define MM_SQRT2 double(1.4142135623730950488016887242097)
#define MM_ONE_OVER_SQRT2 double(0.70710678118654752440084436210485)
#define MM_LN10 double(2.3025850929940456840179914546844)
#define MM_ONE_OVER_LN10 double(0.43429448190325182765112891891661)
#define MM_LN2 double(0.69314718055994530941723212145818)
#define MM_ONE_OVER_LN2 double(1.4426950408889634073599246810019)
#define MM_SEMITONE_FACTOR double(1.0594630943592952645618252949463)

namespace mm
{
	enum PortType
	{
		TYPE_INPUT,
		TYPE_OUTPUT
	};

	struct MidiDeviceInfo
	{
		int32_t idx = -1; 
		bool emulated = false;
		std::string name = "";
		MidiDeviceInfo() {};
		MidiDeviceInfo(int32_t idx, bool e, std::string n) : idx(idx), emulated(e), name(n) {};
	};

}

#endif