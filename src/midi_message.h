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
	enum class MessageType : uint8_t
	{
		// Standard Message
		MIDI_NOTE_OFF           = 0x80,
		MIDI_NOTE_ON            = 0x90,
		MIDI_POLY_PRESSURE      = 0xA0,
		MIDI_CONTROL_CHANGE     = 0xB0,
		MIDI_PROGRAM_CHANGE     = 0xC0,
		MIDI_AFTERTOUCH		    = 0xD0,
		MIDI_PITCH_BEND         = 0xE0,
	
		// System Common Messages
		MIDI_SYSTEM_EXCLUSIVE   = 0xF0,
		MIDI_TIME_CODE          = 0xF1,
		MIDI_SONG_POS_POINTER   = 0xF2,
		MIDI_SONG_SELECT        = 0xF3,
		MIDI_RESERVED1          = 0xF4,
		MIDI_RESERVED2          = 0xF5,
		MIDI_TUNE_REQUEST       = 0xF6,
		MIDI_EOX                = 0xF7,
	
			// System Realtime Messages
		MIDI_TIME_CLOCK         = 0xF8,
		MIDI_RESERVED3          = 0xF9,
		MIDI_START              = 0xFA,
		MIDI_CONTINUE           = 0xFB,
		MIDI_STOP               = 0xFC,
		MIDI_RESERVED4          = 0xFD,
		MIDI_ACTIVE_SENSING     = 0xFE,
		MIDI_SYSTEM_RESET       = 0xFF
	};

	inline std::string getStatusString(MessageType status) 
	{
		switch(status) 
		{ 
			case MessageType::MIDI_NOTE_OFF: return "Note Off";
			case MessageType::MIDI_NOTE_ON: return "Note On";
			case MessageType::MIDI_CONTROL_CHANGE: return "Control Change";
			case MessageType::MIDI_PROGRAM_CHANGE: return "Program Change";
			case MessageType::MIDI_PITCH_BEND: return "Pitch Bend";
			case MessageType::MIDI_POLY_PRESSURE: return "Poly Aftertouch";
			case MessageType::MIDI_AFTERTOUCH: return "Aftertouch";
			case MessageType::MIDI_SYSTEM_EXCLUSIVE: return "Sysex";
			case MessageType::MIDI_TIME_CODE: return "Time Code";
			case MessageType::MIDI_SONG_POS_POINTER: return "Song Pos";
			case MessageType::MIDI_SONG_SELECT: return "Song Select";
			case MessageType::MIDI_TUNE_REQUEST: return "Tune Request";
			case MessageType::MIDI_EOX: return "Sysex End";
			case MessageType::MIDI_TIME_CLOCK: return "Time Clock";
			case MessageType::MIDI_START: return "Start";
			case MessageType::MIDI_CONTINUE: return "Continue";
			case MessageType::MIDI_STOP: return "Stop";
			case MessageType::MIDI_ACTIVE_SENSING: return "Active Sensing";
			case MessageType::MIDI_SYSTEM_RESET: return "System Reset";
			default: return "Unknown";
		}
	}

	///////////////////
	// MIDI Message  //
	///////////////////

	// Channels are indexed @ 1 to 16 (not 0-15)
	struct MidiMessage 
	{

		MidiMessage() 
		{
			data = {0, 0, 0};
		}

		MidiMessage(const uint8_t b1, const uint8_t b2, const uint8_t b3, const double ts = 0) : timestamp(ts) 
		{
			data = {b1, b2, b3};
		}

		MidiMessage(const uint8_t b1, const  uint8_t b2, const double ts = 0) : timestamp(ts) 
		{
			data = {b1, b2, 0};
		}

		MidiMessage(const MidiMessage & rhs) 
		{
			*this = rhs;
		}

		MidiMessage & operator = (const MidiMessage & rhs)
		{
			data = rhs.data;
			timestamp = rhs.timestamp;
			return *this;
		}

		bool usesChannel(const int channel) const 
		{
			assert(channel > 0 && channel <= 16);
			return ((data[0] & 0xF) == channel - 1) && ((data[0] & 0xF0) != 0xF0);
		}

		int getChannel() const
		{
			if ((data[0] & 0xF0) != 0xF0)
				return (data[0] & 0xF) + 1;
			return 0;
		}

		unsigned char operator [] (int i) 
		{
			//@tofix -- add assert
			return data[i];
		}

		MessageType getStatus()
		{
			if (MessageType(data[0]) >= MessageType::MIDI_SYSTEM_EXCLUSIVE)
			{
				return MessageType(data[0] & 0xFF);
			}
			else
			{
				return MessageType(data[0] & 0xF0);
			}
		}

		double timestamp = 0;
		std::vector<unsigned char> data;
	};

	///////////////////////
	// Message Factories //
	///////////////////////

	inline uint8_t MakeCommand(const MessageType type, const int channel)
    {
        return (uint8_t) ((uint8_t) type | mm::clamp<uint8_t> (channel, 0, channel - 1));
    }

	inline MidiMessage MakeNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_NOTE_ON, channel), note, velocity);
	}

	inline MidiMessage MakeNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_NOTE_OFF, channel), note, velocity);
	}

	inline MidiMessage MakeControlChange(uint8_t channel, uint8_t control, uint8_t value)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_CONTROL_CHANGE, channel), control, value);
	}

	inline MidiMessage MakeProgramChange(uint8_t channel, uint8_t value)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_PROGRAM_CHANGE, channel), value);
	}

	inline MidiMessage MakePitchBend(uint8_t channel, int value)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_PITCH_BEND, channel), value & 0x7F, (uint8_t)((value >> 7) & 0x7F)); 
	}

	inline MidiMessage MakePitchBend(uint8_t channel, uint8_t lsb, uint8_t msb)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_PITCH_BEND, channel), lsb, msb);
	}

	inline MidiMessage MakePolyPressure(uint8_t channel, uint8_t note, uint8_t value)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_POLY_PRESSURE, channel), note, value);
	}

	inline MidiMessage MakeAftertouch(uint8_t channel, uint8_t value)
	{
		return MidiMessage(MakeCommand(MessageType::MIDI_AFTERTOUCH, channel), value);
	}

	///////////////////////
	// MIDI Player Event //
	///////////////////////

	struct MidiPlayerEvent
	{
		MidiPlayerEvent() {};

		MidiPlayerEvent(double t, uint8_t b1, uint8_t b2, uint8_t b3, int track) : timestamp(t), trackIdx(track)
		{
			msg.data = {b1, b2, b3};
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

	//////////////////////////////////////
	// MIDI Track Events (file parsing) //
	//////////////////////////////////////

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