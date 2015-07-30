#pragma comment(user, "license")

#pragma once

#ifndef METAMIDI_MIDI_INPUT_H
#define METAMIDI_MIDI_INPUT_H

#include "metamidi.h"
#include "midi_message.h"
#include "rtmidi/RtMidi.h"
#include <functional>

class MidiInput
{
	std::unique_ptr<RtMidiIn> inputDevice;

	static void _callback(double delta, std::vector<uint8_t> * message, void * userData);

	void handleMessage(double delta, std::vector<uint8_t> * message);

	bool attached = false;
	
public:

	MidiInput(const std::string & name);
	~MidiInput();
	
	bool openPort(int32_t portNumber);
	bool openPort(std::string deviceName);
	bool openVirtualPort(std::string portName);
	void closePort();
	
	void ignoreTypes(bool midiSysex, bool midiTiming, bool midiSense);

	RtMidiIn * getInputDevice() { return inputDevice.get(); }

	mm::MidiDeviceInfo info;

	std::function<void (const mm::MidiMessage msg)> messageCallback;
};

#endif