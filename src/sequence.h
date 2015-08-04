#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_SEQUENCE_H
#define MODERNMIDI_SEQUENCE_H

#include "modernmidi.h"
#include "midi_message.h"
#include "midi_event.h"

namespace mm 
{
	class MidiSequence 
	{
		void parseInternal(const std::vector<uint8_t> & buffer);

	public:

		MidiSequence();
		~MidiSequence();
		
		void parse(const std::vector<uint8_t> & buffer);

		double getEndTime();

		void clearTracks();
		
		float ticksPerBeat; // precision (number of ticks distinguishable per second)
		float startingTempo;

		std::vector<MidiTrack> tracks;
	};

} // mm

#endif
