#pragma comment(user, "license")

#include "midi_output.h"

using namespace mm;

MidiOutput::MidiOutput(const std::string & name) 
{
	outputDevice.reset(new RtMidiOut(RtMidi::UNSPECIFIED, name));
}

MidiOutput::~MidiOutput() 
{
	closePort();
}

bool MidiOutput::openPort(int32_t portNumber) 
{	
	if (attached) throw std::runtime_error("device is already attached to a port");
	try 
	{
		closePort();
		outputDevice->openPort(portNumber, std::to_string(portNumber));
		attached = true;
	}
	catch(RtMidiError & e) 
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	info = {portNumber, false, outputDevice->getPortName(portNumber)};
	return true;
}

bool MidiOutput::openPort(std::string deviceName) 
{
	int port = -1;
	for (uint32_t i = 0; i < outputDevice->getPortCount(); ++i) 
	{
		std::string name = outputDevice->getPortName(i);
		if(name == deviceName) 
		{
			port = i;
			break;
		}
	}
	if (port == -1) 
	{
		std::cerr << "Port not available" << std::endl;
		return false;
	} 
	return openPort(port);
}

bool MidiOutput::openVirtualPort(std::string portName) 
{
#if defined (MM_PLATFORM_WINDOWS)
    throw std::runtime_error("virtual ports are unsupported on windows");
#endif
    
	if (attached) throw std::runtime_error("device is already attached to a port");
	try 
	{
		closePort();
		outputDevice->openVirtualPort(portName);
		attached = true;
	}
	catch(RtMidiError & e) 
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	info = {-1, true, portName};
	return true;
}

void MidiOutput::closePort() 
{
	outputDevice->closePort();
	info = {-1, false, ""};
	attached = false;
}

bool MidiOutput::sendRaw(std::vector<unsigned char> msg)
{
	if (!outputDevice) throw std::runtime_error("output device not initialized");
	if (!attached) throw std::runtime_error("interface not bound to a port");
	try 
	{
		outputDevice->sendMessage(&msg);
	}
	catch(RtMidiError & e) 
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

bool MidiOutput::send(const std::vector<uint8_t> & msg)
{
	return sendRaw(static_cast<std::vector<unsigned char>>(msg));
}

bool MidiOutput::send(const mm::MidiMessage & msg)
{
	return sendRaw(static_cast<std::vector<unsigned char>>(msg.data));
}