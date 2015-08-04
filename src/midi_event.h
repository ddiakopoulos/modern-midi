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
		return ss << "[" << int(v.msg.data[0]) << ", " << int(v.msg.data[1]) << ", " << int(v.msg.data[2]) << "]";
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
    
    #define DECLARE_MIDI_EVENT(ev) struct ev ## Event : public MidiTrackEvent { ev ## Event() : MidiTrackEvent(MIDI_Event ## ev) { }
        
    DECLARE_MIDI_EVENT(SequenceNumber) int number; };
    DECLARE_MIDI_EVENT(Text) std::string text; };
    DECLARE_MIDI_EVENT(CopyrightNotice) std::string text; };
    DECLARE_MIDI_EVENT(TrackName) std::string text; };
    DECLARE_MIDI_EVENT(InstrumentName) std::string text; };
    DECLARE_MIDI_EVENT(Lyrics) std::string text; };
    DECLARE_MIDI_EVENT(Marker) std::string text; };
    DECLARE_MIDI_EVENT(CuePoint) std::string text; };
    DECLARE_MIDI_EVENT(MidiChannelPrefix) int channel; };
    DECLARE_MIDI_EVENT(EndOfTrack) };
    DECLARE_MIDI_EVENT(SetTempo) int microsecondsPerBeat; }; // Otherwise known as microsecondsPerQuaterNote
    DECLARE_MIDI_EVENT(SmpteOffset) int framerate; int hour; int min; int sec; int frame; int subframe; };
    DECLARE_MIDI_EVENT(TimeSignature) int numerator; int denominator; int metronome; int thirtyseconds; };
    DECLARE_MIDI_EVENT(KeySignature) int key; int scale; };
    DECLARE_MIDI_EVENT(SequencerSpecific) ~SequencerSpecificEvent() { delete[] data; } uint8_t * data; };
    DECLARE_MIDI_EVENT(Unknown) ~UnknownEvent() { delete[] data; } uint8_t * data; };
    DECLARE_MIDI_EVENT(SysEx) ~SysExEvent() { delete[] data; } uint8_t * data; };
    DECLARE_MIDI_EVENT(DividedSysEx) ~DividedSysExEvent() { delete[] data; } uint8_t * data; };
    DECLARE_MIDI_EVENT(Channel) uint8_t midiCommand; uint8_t param1; uint8_t param2; };

	////////////////
	// MIDI Track //
	////////////////

	struct MidiTrack
	{
		std::vector<std::shared_ptr<MidiTrackEvent>> events;
	};
	
} // mm

#endif