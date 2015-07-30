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
	#define __WINDOWS_MM__
#endif

#ifdef __APPLE__
    #define MM_PLATFORM_OSX 1
	#define __MACOSX_CORE__
#endif

#if defined(__linux) || defined(__unix) || defined (__posix)
	#define MM_PLATFORM_POSIX
	#define __LINUX_ALSA__
#endif

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
		bool open = false;
		std::string name = "";
		MidiDeviceInfo() {};
		MidiDeviceInfo(int32_t idx, bool e, bool o, std::string n) : idx(idx), emulated(e), open(o), name(n) {};
	};
}

#endif