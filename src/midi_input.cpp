#pragma comment(user, "license")

#include "midi_input.h"

using namespace mm;

// Static
void MidiInput::_callback(double delta, std::vector<uint8_t> * message, void * userData) 
{
	auto myInput = static_cast<MidiInput*>(userData);
	myInput->handleMessage(delta * 1000, message); // convert s to ms
}

MidiInput::MidiInput(const std::string & name)
{
	inputDevice.reset(new RtMidiIn(RtMidi::UNSPECIFIED, name));
}

MidiInput::~MidiInput() 
{
	closePort();
}

void MidiInput::handleMessage(double delta, std::vector<uint8_t> * message)
{
	MidiMessage incomingMsg (message->at(0),message->at(1), message->at(2));

	if (messageCallback)
		messageCallback(incomingMsg);
}

bool MidiInput::openPort(int32_t portNumber) 
{	
	if (attached) throw std::runtime_error("device is already attached to a port");
	try 
	{
		inputDevice->setCallback(&_callback, this);
		inputDevice->openPort(portNumber, std::to_string(portNumber));
		attached = true;
	}
	catch (RtMidiError& e) 
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	info = {portNumber, false, inputDevice->getPortName(portNumber)};
	return true;
}

bool MidiInput::openPort(std::string deviceName) 
{
	int port = -1;
	for (uint32_t i = 0; i < inputDevice->getPortCount(); ++i) 
	{
		if (inputDevice->getPortName(i) == deviceName) 
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

bool MidiInput::openVirtualPort(std::string portName) 
{
    
#if defined (MM_PLATFORM_WINDOWS)
    throw std::runtime_error("virtual ports are unsupported on windows");
#endif
    
	if (attached) throw std::runtime_error("device is already attached to a port");
	try 
	{
		inputDevice->setCallback(&_callback, this);
		inputDevice->openVirtualPort(portName);
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

void MidiInput::closePort() 
{
	inputDevice->closePort();
	if (attached) inputDevice->cancelCallback();
	info = {-1, false, ""};
	attached = false;
}

void MidiInput::ignoreTypes(bool midiSysex, bool midiTiming, bool midiSense) 
{
	inputDevice->ignoreTypes(midiSysex, midiTiming, midiSense);
}
