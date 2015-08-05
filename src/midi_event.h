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

	///////////////////////
	// MIDI Track Events //
	///////////////////////

	struct TrackEvent
	{
		int tick = 0;
		int track = 0;
		std::shared_ptr<MidiMessage> m;
		TrackEvent(int tick, int track, std::shared_ptr<MidiMessage> m) : tick(tick), track(track), m(m) { }
		TrackEvent(TrackEvent && r) { *this = std::move(r); }
		TrackEvent & operator = (TrackEvent && r) { tick = r.tick; track = r.track; m = std::move(r.m); return *this; }
		virtual ~TrackEvent() {};
	};

	////////////////
	// MIDI Track //
	////////////////

	typedef std::vector<std::shared_ptr<TrackEvent>> MidiTrack;
	
} // mm

#endif