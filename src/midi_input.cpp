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
	//@todo
}

bool MidiInput::openPort(int32_t portNumber) 
{	
	closePort();
	try 
	{
		inputDevice->setCallback(&_callback, this);
		inputDevice->openPort(portNumber, std::to_string(portNumber));
	}
	catch (RtMidiError& e) 
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	info = {portNumber, false, true, inputDevice->getPortName(portNumber)};
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
	closePort();
	try 
	{
		inputDevice->setCallback(&_callback, this);
		inputDevice->openVirtualPort(portName);
	}
	catch(RtMidiError & e) 
	{
		std::cerr << e.what() << std::endl;
		return false;
	}

	// Why 0? 
	info = {0, true, true, portName};
	return true;
}

void MidiInput::closePort() 
{
	inputDevice->closePort();
	if (info.open) inputDevice->cancelCallback();
	info = {-1, false, false, ""};
}

void MidiInput::ignoreTypes(bool midiSysex, bool midiTiming, bool midiSense) 
{
	inputDevice->ignoreTypes(midiSysex, midiTiming, midiSense);
}
