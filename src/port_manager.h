#pragma once

#ifndef METAMIDI_MIDI_PORT_MGR_H
#define METAMIDI_MIDI_PORT_MGR_H

#include "metamidi.h"
#include "rtmidi/RtMidi.h"

#if defined(_MSC_VER)
	#pragma comment(lib, "winmm.lib")
#endif

class PortManager
{
	static std::shared_ptr<RtMidiOut> outputDevice;
	static std::shared_ptr<RtMidiIn> inputDevice;

	template<typename T>
	static T * fetch(std::shared_ptr<T> port)
	{
		if (port.get()) 
			return port.get();
		else
		{
			port.reset(new T());
			return port.get();
		}
	}

	template<typename T>
	static void listPorts(T * device)
	{
		std::cout << device->getPortCount() << " ports available: ";
		for (uint32_t i = 0; i < device->getPortCount(); ++i)
		{
			std::cout << "\t" << i << " : " << device->getPortName(i);
		}
		std::cout << std::endl;
	}

	template<typename T>
	static std::vector<std::string> getPortList(T * device)
	{
		std::vector<std::string> portList;
		for(uint32_t i = 0; i < device->getPortCount(); ++i) 
		{
			portList.emplace_back(device->getPortName(i));
		}
		return portList;
	}

	template<typename T>
	static int getNumPorts(T * device)
	{
		return device->getPortCount();
	}

	template<typename T>
	static std::string getPortName(T * device, uint32_t portNumber)
	{
		try 
		{
			return device->getPortName(portNumber);
		}
		catch(RtMidiError & e) 
		{
			std::cerr << e.what() << std::endl;
		}
		return "";
	}

public:

	static void PrintPortList(mm::PortType t);
	static std::vector<std::string>  GetPortList(mm::PortType t);
	static int GetPortCount(mm::PortType t);
	static std::string GetPortName(mm::PortType t, uint32_t portNumber);

};

#endif