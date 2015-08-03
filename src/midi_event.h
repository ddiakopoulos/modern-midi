#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_EVENT_H
#define MODERNMIDI_EVENT_H

#include "modernmidi.h"
#include "midi_message.h"
#include <stdint.h>

namespace mm
{
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