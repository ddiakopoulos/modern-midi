#pragma once

#ifndef METAMIDI_SEQUENCE_H
#define METAMIDI_SEQUENCE_H

#include "metamidi.h"
#include "midi_structs.h"

namespace mm 
{
	#define DECLARE_MIDI_EVENT(ev) struct ev ## Event : public MidiEvent { ev ## Event() : MidiEvent(MIDI_Event ## ev) { }

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
    DECLARE_MIDI_EVENT(SequencerSpecific) ~SequencerSpecificEvent() { delete[] data; } uint8_t* data; };
    DECLARE_MIDI_EVENT(Unknown) ~UnknownEvent() { delete[] data; } uint8_t* data; };
    DECLARE_MIDI_EVENT(SysEx) ~SysExEvent() { delete[] data; } uint8_t* data; };
    DECLARE_MIDI_EVENT(DividedSysEx) ~DividedSysExEvent() { delete[] data; } uint8_t* data; };
    DECLARE_MIDI_EVENT(Channel) uint8_t midiCommand; uint8_t param1; uint8_t param2; };

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
