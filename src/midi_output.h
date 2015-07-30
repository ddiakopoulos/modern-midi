#pragma once

#ifndef METAMIDI_MIDI_OUTPUT_H
#define METAMIDI_MIDI_OUTPUT_H

#include "metamidi.h"
#include "rtmidi/RtMidi.h"

class MidiOutput
{
	std::unique_ptr<RtMidiOut> outputDevice;
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

#endif
