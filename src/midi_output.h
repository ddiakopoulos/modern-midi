#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_MIDI_OUTPUT_H
#define MODERNMIDI_MIDI_OUTPUT_H

#include "modernmidi.h"
#include "rtmidi/RtMidi.h"

namespace mm
{

class MidiOutput
{
	std::unique_ptr<RtMidiOut> outputDevice;
	bool attached = false;
public:

	MidiOutput(const std::string & name);
	~MidiOutput();

	bool openPort(int32_t portNumber);
	bool openPort(std::string deviceName);
	bool openVirtualPort(std::string portName);
	void closePort();
	
	bool sendMessage(const std::vector<uint8_t> & msg);

	RtMidiOut * getOutputDevice() { return outputDevice.get(); }

	mm::MidiDeviceInfo info;
};

}

#endif
