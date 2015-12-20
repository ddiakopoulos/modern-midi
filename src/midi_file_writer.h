#pragma comment(user, "license")

#pragma once

#ifndef MODERNMIDI_FILE_IO_H
#define MODERNMIDI_FILE_IO_H

#include "modernmidi.h"
#include "midi_message.h"
#include "midi_event.h"
#include <stdint.h>

namespace mm
{
  
class MidiFileWriter
{
	std::vector<MidiTrack> tracks;
	int ticksPerQuarterNote = 120;

public:

	MidiFileWriter();
	~MidiFileWriter();

	size_t getNumTracks() { return tracks.size(); }
	int getTicksPerQuarterNote() { return ticksPerQuarterNote; }

	void addEvent(int tick, int track, std::shared_ptr<MidiMessage> m);
    void addEvent(int track, std::shared_ptr<TrackEvent> m);
    
    void setTicksPerQuarterNote(int tpqn) { ticksPerQuarterNote = tpqn; }
    
	void addTrack(); 

	void write(std::ostream & out); 
};
    
} // mm

#endif