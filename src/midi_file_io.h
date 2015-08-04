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
  
inline std::ostream & write_uint16_be(std::ostream & out, uint16_t value)
{
    union { uint8_t bytes[2]; uint16_t v; } data;
    data.v = value;
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}
    
inline std::ostream & write_int16_be(std::ostream & out, int16_t value)
{
    union { uint8_t bytes[2]; int16_t v; } data;
    data.v = value;
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}
    
inline std::ostream & write_uint32_be(std::ostream & out, uint32_t value)
{
    union { uint8_t bytes[4]; uint32_t v; } data;
    data.v = value;
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}
    
inline std::ostream & write_int32_be(std::ostream & out, int32_t value)
{
    union { uint8_t bytes[4]; int32_t v; } data;
    data.v = value;
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}
    
inline std::ostream & write_float_be(std::ostream & out, float value)
{
    union { uint8_t bytes[4]; float v; } data;
    data.v = value;
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}
    
inline std::ostream & write_double_be(std::ostream & out, double value)
{
    union { uint8_t bytes[8]; double v; } data;
    data.v = value;
    out << data.bytes[7]; out << data.bytes[6];
    out << data.bytes[5]; out << data.bytes[4];
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}
    
// Write a number to the midifile
// as a variable length value which segments a file into 7-bit
// values.  Maximum size of aValue is 0x7fffffff
inline void write_variable_length(uint32_t aValue, std::vector<uint8_t> & outdata)
{
    uint8_t bytes[5] = {0};
        
    bytes[0] = (uint8_t) (((uint32_t) aValue >> 28) & 0x7F);  // most significant 5 bits
    bytes[1] = (uint8_t) (((uint32_t) aValue >> 21) & 0x7F);  // next largest 7 bits
    bytes[2] = (uint8_t) (((uint32_t) aValue >> 14) & 0x7F);
    bytes[3] = (uint8_t) (((uint32_t) aValue >> 7)  & 0x7F);
    bytes[4] = (uint8_t) (((uint32_t) aValue)       & 0x7F);  // least significant 7 bits
        
    int start = 0;
    while (start < 5 && bytes[start] == 0)
        start++;
        
    for (int i = start; i < 4; i++)
    {
        bytes[i] = bytes[i] | 0x80;
        outdata.push_back(bytes[i]);
    }
    outdata.push_back(bytes[4]);
}
    
struct TrackEvent
{
    int tick = 0;
    int track = 0;
	std::shared_ptr<MidiMessage> m;
	TrackEvent(int tick, int track, std::shared_ptr<MidiMessage> m) : tick(tick), track(track), m(m) { }
};
 
class MidiFile
{
	std::vector<std::vector<TrackEvent>> tracks;
	int ticksPerQuarterNote = 120;

public:

	MidiFile();
	~MidiFile();

	int getNumTracks() { return tracks.size(); }
	int getTicksPerQuarterNote() { return ticksPerQuarterNote; }

	void addEvent(int tick, int track, std::shared_ptr<MidiMessage> m);
	void addTrack(); 

	void write(std::ostream & out); 
};
    
} // mm

#endif