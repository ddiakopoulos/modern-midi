#pragma comment(user, "license")

#pragma once

#ifndef METAMIDI_STRUCTS_H
#define METAMIDI_STRUCTS_H

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
	#define MIDI_CHANNEL_PRESSURE   0xD0
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

	///////////////////
	// MIDI Command  //
	///////////////////

	struct MidiMessage 
	{
		MidiMessage() {}
		MidiMessage(uint8_t b1, uint8_t b2, uint8_t b3) : byte1(b1), byte2(b2), byte3(b3) {}
		MidiMessage(const MidiMessage & rhs) 
		{
			*this = rhs;
		}

		MidiMessage & operator= (const MidiMessage & rhs)
		{
			byte1 = rhs.byte1;
			byte2 = rhs.byte2;
			byte3 = rhs.byte3;
			return *this;
		}

		bool usesChannel(const int channel) const 
		{
			assert(channel > 0 && channel <= 16);
			return ((byte1 & 0xf) == channel - 1) && ((byte1 & 0xf0) != 0xf0);
		}

		uint8_t byte1;
		uint8_t byte2;
		uint8_t byte3;
	};
	
	//////////////////////////
	// MIDI Real-Time Event //
	//////////////////////////

	struct MidiRtEvent
	{
		MidiRtEvent() {};

		MidiRtEvent(double t, uint8_t b1, uint8_t b2, uint8_t b3, int track) : timestamp(t), trackIdx(track)
		{
			msg.byte1 = b1;
			msg.byte2 = b2;
			msg.byte3 = b3;
		}
		
		MidiRtEvent & operator= (const MidiRtEvent & rhs)
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

	template<class C, class R> std::basic_ostream<C, R> & operator <<(std::basic_ostream<C, R> & ss, const MidiRtEvent & v) 
	{
		return ss << "[" << int(v.msg.byte1) << ", " << int(v.msg.byte2) << ", " << int(v.msg.byte3) << "]";
	}

	///////////////////////////////
	// MIDI Events (for Parsing) //
	///////////////////////////////

	enum MIDI_EventType 
	{
		MIDI_EventSequenceNumber, MIDI_EventText, MIDI_EventCopyrightNotice,
		MIDI_EventTrackName, MIDI_EventInstrumentName, MIDI_EventLyrics,
		MIDI_EventMarker, MIDI_EventCuePoint, MIDI_EventMidiChannelPrefix,
		MIDI_EventEndOfTrack, MIDI_EventSetTempo, MIDI_EventSmpteOffset,
		MIDI_EventTimeSignature, MIDI_EventKeySignature, MIDI_EventSequencerSpecific,
		MIDI_EventUnknown, MIDI_EventSysEx, MIDI_EventDividedSysEx, MIDI_EventChannel
	};
	
	struct MidiEvent 
	{
		MidiEvent(MIDI_EventType s) : eventType(s) { }
		virtual ~MidiEvent() { }
		MIDI_EventType eventType;
		int deltatime = 0;
	};

	////////////////
	// MIDI Track //
	////////////////

	struct MidiTrack
	{
		std::vector<std::shared_ptr<MidiEvent>> events;
	};
	
} // mm

#endif