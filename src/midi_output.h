#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_MIDI_OUTPUT_H
#define MODERNMIDI_MIDI_OUTPUT_H

#include "modernmidi.h"
#include "midi_message.h"
#include "rtmidi/RtMidi.h"

namespace mm
{

class MidiOutput
{
	bool attached = false;

	bool sendRaw(std::vector<unsigned char> msg);

public:

	MidiOutput(const std::string & name);
	~MidiOutput();

	bool openPort(int32_t portNumber);
	bool openPort(std::string deviceName);
	bool openVirtualPort(std::string portName);
	void closePort();
	
	bool send(const std::vector<uint8_t> & msg);
	bool send(const mm::MidiMessage & msg);

	RtMidiOut * getOutputDevice() { return outputDevice.get(); }

	mm::MidiDeviceInfo info;

	std::unique_ptr<RtMidiOut> outputDevice;
};

}

#endif
