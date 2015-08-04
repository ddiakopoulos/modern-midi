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
        out << data.bytes[1];
        out << data.bytes[0];
        return out;
    }
    
    inline std::ostream & write_int16_be(std::ostream & out, int16_t value)
    {
        union { uint8_t bytes[2]; int16_t v; } data;
        data.v = value;
        out << data.bytes[1];
        out << data.bytes[0];
        return out;
    }
    
    inline std::ostream & write_uint32_be(std::ostream & out, uint32_t value)
    {
        union { uint8_t bytes[4]; uint32_t v; } data;
        data.v = value;
        out << data.bytes[3];
        out << data.bytes[2];
        out << data.bytes[1];
        out << data.bytes[0];
        return out;
    }
    
    inline std::ostream & write_int32_be(std::ostream & out, int32_t value)
    {
        union { uint8_t bytes[4]; int32_t v; } data;
        data.v = value;
        out << data.bytes[3];
        out << data.bytes[2];
        out << data.bytes[1];
        out << data.bytes[0];
        return out;
        
    }
    
    inline std::ostream & write_float_be(std::ostream& out, float value)
    {
        union { uint8_t bytes[4]; float v; } data;
        data.v = value;
        out << data.bytes[3];
        out << data.bytes[2];
        out << data.bytes[1];
        out << data.bytes[0];
        return out;
    }
    
    inline std::ostream & write_double_be(std::ostream& out, double value)
    {
        union { uint8_t bytes[8]; double v; } data;
        data.v = value;
        out << data.bytes[7];
        out << data.bytes[6];
        out << data.bytes[5];
        out << data.bytes[4];
        out << data.bytes[3];
        out << data.bytes[2];
        out << data.bytes[1];
        out << data.bytes[0];
        return out;
    }
    
    // Write a number to the midifile
    // as a variable length value which segments a file into 7-bit
    // values.  Maximum size of aValue is 0x7fffffff
    inline void write_variable_length(uint32_t aValue, std::vector<uint8_t> & outdata)
    {
        uint8_t bytes[5] = {0};
        
        bytes[0] = (uint8_t)(((uint32_t)aValue >> 28) & 0x7f);  // most significant 5 bits
        bytes[1] = (uint8_t)(((uint32_t)aValue >> 21) & 0x7f);  // next largest 7 bits
        bytes[2] = (uint8_t)(((uint32_t)aValue >> 14) & 0x7f);
        bytes[3] = (uint8_t)(((uint32_t)aValue >> 7)  & 0x7f);
        bytes[4] = (uint8_t)(((uint32_t)aValue)       & 0x7f);  // least significant 7 bits
        
        int start = 0;
        while (start < 5 && bytes[start] == 0)
            start++;
        
        for (int i=start; i<4; i++)
        {
            bytes[i] = bytes[i] | 0x80;
            outdata.push_back(bytes[i]);
        }
        outdata.push_back(bytes[4]);
    }
    
    struct TrackEvent
    {
        MidiMessage m;
        int tick = 0;
        int track = 0;
    };
    
    struct MidiFile
    {
        int numTracks = 0;
        int ticksPerQuarterNote = 240;
        
        std::vector<std::vector<TrackEvent>> tracks;
        
        int getNumTracks() { return numTracks; }
        int getTicksPerQuarterNote() { return ticksPerQuarterNote; }
        
        int write(std::ostream & out)
        {
            
            // 1. The characters "MThd"
            out << 'M';
            out << 'T';
            out << 'h';
            out << 'd';
            
            // 2. Header size is always 6
            write_uint32_be(out, 6);
            
            // 3. MIDI file format, type 0, 1, or 2
            write_uint16_be(out, (getNumTracks() == 1) ? 0 : 1);
            
            // 4. write out the number of tracks.
            write_uint16_be(out, getNumTracks());
            
            // 5. write out the number of ticks per quarternote. (avoiding SMTPE for now)
            write_uint16_be(out, getTicksPerQuarterNote());
            
            std::vector<uint8_t> trackRawData;
            
            uint8_t endoftrack[4] = {0x0, 0xFF, 0x2F, 0x00};
            
            for (auto & event_list : tracks)
            {
                
                for (auto & event : event_list)
                {
                    
                    const auto & msg = event.m;
                    
                    // Suppress end-of-track meta messages (one will be added
                    // automatically after all track data has been written).
                    if (msg.getMetaEventSubtype() == uint8_t(MetaEventType::END_OF_TRACK)) continue;
                    
                    write_variable_length(event.tick, trackRawData);
                    
                    if ((msg.getMessageType() == uint8_t(MessageType::SYSTEM_EXCLUSIVE)) || (event.m.getMessageType() == uint8_t(MessageType::EOX)))
                    {
                        // 0xf0 == Complete sysex message (0xf0 is part of the raw MIDI).
                        // 0xf7 == Raw byte message (0xf7 not part of the raw MIDI).
                        // Print the first byte of the message (0xf0 or 0xf7), then
                        // print a VLV length for the rest of the bytes in the message.
                        // In other words, when creating a 0xf0 or 0xf7 MIDI message,
                        // do not insert the VLV byte length yourself, as this code will
                        // do it for you automatically.
                        trackRawData.emplace_back(msg.data[0]); // 0xf0 or 0xf7;
                        
                        write_variable_length(msg.messageSize() - 1, trackRawData);
                        
                        for (int k = 1; k < msg.messageSize(); k++)
                        {
                            trackRawData.emplace_back(msg[k]);
                        }
                    }
                    
                    else
                    {
                        // Non-sysex type of message, so just output the bytes of the message:
                        for (int k = 0; k < msg.messageSize(); k++)
                        {
                            trackRawData.emplace_back(msg[k]);
                        }
                    }
                    
                }
            }
                
            auto size = trackRawData.size();
            
            if ((size < 3) || !((trackRawData[size - 3] == 0xFF) && (trackRawData[size - 2] == 0x2F)))
            {
                trackRawData.emplace_back(endoftrack[0]);
                trackRawData.emplace_back(endoftrack[1]);
                trackRawData.emplace_back(endoftrack[2]);
                trackRawData.emplace_back(endoftrack[3]);
            }
            
            // Write the track ID marker "MTrk":
            out << 'M';
            out << 'T';
            out << 'r';
            out << 'k';
            
            // Then size of the MIDI data to follow:
            write_uint32_be(out, trackRawData.size());
            
            // Now the data
            out.write((char*) trackRawData.data(), trackRawData.size());
            
            // ???
            return true;
        }
        
    };
    
} // mm

#endif