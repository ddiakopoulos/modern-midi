#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_MESSAGE_H
#define MODERNMIDI_MESSAGE_H

#include "modernmidi.h"
#include <stdint.h>
#include <vector>
#include <memory>
#include <assert.h>

namespace mm
{
	// Channel info (MSN=command LSN=channel)
	#define MIDI_NOTE_OFF           0x80
	#define MIDI_NOTE_ON            0x90
	#define MIDI_POLY_PRESSURE      0xA0
	#define MIDI_CONTROL_CHANGE     0xB0
	#define MIDI_PROGRAM_CHANGE     0xC0
	#define MIDI_AFTERTOUCH		    0xD0
	#define MIDI_PITCH_BEND         0xE0
	
	// System Common Messages
	#define MIDI_SYSTEM_EXCLUSIVE   0xF0
	#define MIDI_TIME_CODE          0xF1
	#define MIDI_SONG_POS_POINTER   0xF2
	#define MIDI_SONG_SELECT        0xF3
	#define MIDI_RESERVED1          0xF4
	#define MIDI_RESERVED2          0xF5
	#define MIDI_TUNE_REQUEST       0xF6
	#define MIDI_EOX                0xF7
	
	// System Realtime Messages
	#define MIDI_TIME_CLOCK         0xF8
	#define MIDI_RESERVED3          0xF9
	#define MIDI_START              0xFA
	#define MIDI_CONTINUE           0xFB
	#define MIDI_STOP               0xFC
	#define MIDI_RESERVED4          0xFD
	#define MIDI_ACTIVE_SENSING     0xFE
	#define MIDI_SYSTEM_RESET       0xFF

	inline std::string getStatusString(uint8_t status) 
	{
		switch(status) 
		{ 
			case MIDI_NOTE_OFF: return "Note Off";
			case MIDI_NOTE_ON: return "Note On";
			case MIDI_CONTROL_CHANGE: return "Control Change";
			case MIDI_PROGRAM_CHANGE: return "Program Change";
			case MIDI_PITCH_BEND: return "Pitch Bend";
			case MIDI_POLY_PRESSURE: return "Poly Aftertouch";
			case MIDI_AFTERTOUCH: return "Aftertouch";
			case MIDI_SYSTEM_EXCLUSIVE: return "Sysex";
			case MIDI_TIME_CODE: return "Time Code";
			case MIDI_SONG_POS_POINTER: return "Song Pos";
			case MIDI_SONG_SELECT: return "Song Select";
			case MIDI_TUNE_REQUEST: return "Tune Request";
			case MIDI_EOX: return "Sysex End";
			case MIDI_TIME_CLOCK: return "Time Clock";
			case MIDI_START: return "Start";
			case MIDI_CONTINUE: return "Continue";
			case MIDI_STOP: return "Stop";
			case MIDI_ACTIVE_SENSING: return "Active Sensing";
			case MIDI_SYSTEM_RESET: return "System Reset";
			default: return "Unknown";
		}
	}

	///////////////////
	// MIDI Command  //
	///////////////////

	// Channels are indexed @ 1 to 16 (not 0-15)
	struct MidiMessage 
	{
		MidiMessage() {}
		MidiMessage(uint8_t b1, uint8_t b2, uint8_t b3, double ts = 0) : byte1(b1), byte2(b2), byte3(b3), timestamp(ts) {}
		MidiMessage(uint8_t b1, uint8_t b2, double ts = 0) : byte1(b1), byte2(b2), timestamp(ts) {}
		MidiMessage(const MidiMessage & rhs) 
		{
			*this = rhs;
		}

		MidiMessage & operator = (const MidiMessage & rhs)
		{
			byte1 = rhs.byte1;
			byte2 = rhs.byte2;
			byte3 = rhs.byte3;
			timestamp = rhs.timestamp;
			return *this;
		}

		bool usesChannel(const int channel) const 
		{
			assert(channel > 0 && channel <= 16);
			return ((byte1 & 0xF) == channel - 1) && ((byte1 & 0xF0) != 0xF0);
		}

		int getChannel() const
		{
			if ((byte1 & 0xF0) != 0xF0)
				return (byte1 & 0xF) + 1;
			return 0;
		}

		double timestamp;

		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
	};

	///////////////////////
	// Message Factories //
	///////////////////////

	inline uint8_t MakeCommand(const int type, const int channel)
    {
        return (uint8_t) (type | mm::clamp<uint8_t> (channel, 0, channel - 1));
    }

	inline MidiMessage MakeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
	{
		return MidiMessage(MakeCommand(MIDI_NOTE_ON, channel), note, velocity);
	}

	inline MidiMessage MakeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
	{
		return MidiMessage(MakeCommand(MIDI_NOTE_OFF, channel), note, velocity);
	}

	inline MidiMessage MakeControlChange(uint8_t channel, uint8_t control, uint8_t value)
	{
		return MidiMessage(MakeCommand(MIDI_CONTROL_CHANGE, channel), control, value);
	}

	inline MidiMessage MakeProgramChange(uint8_t channel, uint8_t value)
	{
		return MidiMessage(MakeCommand(MIDI_PROGRAM_CHANGE, channel), value);
	}

	inline MidiMessage MakePitchBend(uint8_t channel, int value)
	{
		return MidiMessage(MakeCommand(MIDI_PITCH_BEND, channel), value & 0x7F, (uint8_t)((value >> 7) & 0x7F)); 
	}

	inline MidiMessage MakePitchBend(uint8_t channel, uint8_t lsb, uint8_t msb)
	{
		return MidiMessage(MakeCommand(MIDI_PITCH_BEND, channel), lsb, msb);
	}

	inline MidiMessage MakePolyPressure(uint8_t channel, uint8_t note, uint8_t value)
	{
		return MidiMessage(MakeCommand(MIDI_POLY_PRESSURE, channel), note, value);
	}

	inline MidiMessage MakeAftertouch(uint8_t channel, uint8_t value)
	{
		return MidiMessage(MakeCommand(MIDI_AFTERTOUCH, channel), value);
	}

	//////////////////////////
	// MIDI Real-Time Event //
	//////////////////////////

	struct MidiPlayerEvent
	{
		MidiPlayerEvent() {};

		MidiPlayerEvent(double t, uint8_t b1, uint8_t b2, uint8_t b3, int track) : timestamp(t), trackIdx(track)
		{
			msg.byte1 = b1;
			msg.byte2 = b2;
			msg.byte3 = b3;
		}
		
		MidiPlayerEvent & operator= (const MidiPlayerEvent & rhs)
		{
			timestamp = rhs.timestamp;
			msg = rhs.msg;
			trackIdx = rhs.trackIdx;
			return *this;
		}
		
		double timestamp;
		int trackIdx;
		MidiMessage msg;
	};

	template<class C, class R> std::basic_ostream<C, R> & operator << (std::basic_ostream<C, R> & ss, const MidiPlayerEvent & v) 
	{
		return ss << "[" << int(v.msg.byte1) << ", " << int(v.msg.byte2) << ", " << int(v.msg.byte3) << "]";
	}

	///////////////////////////////
	// MIDI Events (for Parsing) //
	///////////////////////////////

	enum MIDI_EventType 
	{
		MIDI_EventSequenceNumber, 
		MIDI_EventText, 
		MIDI_EventCopyrightNotice,
		MIDI_EventTrackName, 
		MIDI_EventInstrumentName,
		MIDI_EventLyrics,
		MIDI_EventMarker,
		MIDI_EventCuePoint,
		MIDI_EventMidiChannelPrefix,
		MIDI_EventEndOfTrack,
		MIDI_EventSetTempo, 
		MIDI_EventSmpteOffset,
		MIDI_EventTimeSignature, 
		MIDI_EventKeySignature, 
		MIDI_EventSequencerSpecific,
		MIDI_EventUnknown,
		MIDI_EventSysEx,
		MIDI_EventDividedSysEx,
		MIDI_EventChannel
	};
	
	struct MidiTrackEvent 
	{
		MidiTrackEvent(MIDI_EventType s) : eventType(s) { }
		virtual ~MidiTrackEvent() { }
		MIDI_EventType eventType;
		int deltatime = 0;
	};

	////////////////
	// MIDI Track //
	////////////////

	struct MidiTrack
	{
		std::vector<std::shared_ptr<MidiTrackEvent>> events;
	};
	
} // mm

#endif