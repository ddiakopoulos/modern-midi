#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_MIDI_PORT_MGR_H
#define MODERNMIDI_MIDI_PORT_MGR_H

#include "modernmidi.h"
#include "rtmidi/RtMidi.h"
#include <assert.h>

namespace mm
{

class PortManager
{
	static std::shared_ptr<RtMidiOut> outputDevice;
	static std::shared_ptr<RtMidiIn> inputDevice;

	template<typename T>
	static std::shared_ptr<T> fetch(std::shared_ptr<T> device)
	{
		if (device) return device;
		else
		{
			device = std::make_shared<T>();
			return device;
		}
	}

	template<typename T>
	static void listPorts(std::shared_ptr<T> device)
	{
		std::cout << device->getPortCount() << " ports available: \n";
		for (uint32_t i = 0; i < device->getPortCount(); ++i)
		{
			std::cout << "## " << i << " : " << device->getPortName(i) << std::endl;
		}
		std::cout << std::endl;
	}

	template<typename T>
	static std::vector<std::string> getPortList(std::shared_ptr<T> device)
	{
		std::vector<std::string> portList;
		for(uint32_t i = 0; i < device->getPortCount(); ++i) 
		{
			portList.emplace_back(device->getPortName(i));
		}
		return portList;
	}

	template<typename T>
	static int getNumPorts(std::shared_ptr<T> device)
	{
		return device->getPortCount();
	}

	template<typename T>
	static std::string getPortName(std::shared_ptr<T> device, uint32_t portNumber)
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
	static std::vector<std::string> GetPortList(mm::PortType t);
	static int GetPortCount(mm::PortType t);
	static std::string GetPortName(mm::PortType t, uint32_t portNumber);

};

}

#endif