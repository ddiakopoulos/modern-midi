/*
Copyright (c) 2015, Dimitri Diakopoulos All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "modernmidi.h"
#include "midi_file_reader.h"
#include "midi_message.h"
#include <algorithm>

// File Parsing Validation Todo:
// ==============================
// [] Bad file name
// [] Bad header
// [] Unknown header type
// [] Bad header size
// [] Bad type
// [] Bad tmecode
// [] Header too short
// [] Track too short
// [] Event too short
// ==============================

namespace mm 
{

TrackEvent * parseEvent(int tick, int track, uint8_t const *& dataStart, MessageType lastEventTypeByte)
{
    MessageType type = (MessageType) *dataStart++;
        
    auto m = std::make_shared<MidiMessage>();
    TrackEvent * event = new TrackEvent(tick, track, m);

    if (((uint8_t) type & 0xF) == 0xF) 
    {
        // Meta event 
        if ((uint8_t) type == 0xFF) 
        {
            MetaEventType subtype = (MetaEventType) *dataStart++;
            int length = read_variable_length(dataStart); 

            event->m->data = std::vector<unsigned char>(std::max(length, 3), 0);
            event->m->data[0] = (uint8_t) type;
            event->m->data[1] = (uint8_t) subtype;
            event->m->data[2] = length;

            switch(subtype) 
            {
                case MetaEventType::SEQUENCE_NUMBER: 
                {
                    if (length != 2) throw std::invalid_argument("Expected length for SEQUENCE_NUMBER event is 2");
                    read_bytes(event->m->data, dataStart, 2);
                    return event;
                }
                case MetaEventType::TEXT: 
                case MetaEventType::COPYRIGHT: 
                case MetaEventType::TRACK_NAME: 
                case MetaEventType::INSTRUMENT: 
                case MetaEventType::LYRIC: 
                case MetaEventType::MARKER: 
                case MetaEventType::CUE:
                case MetaEventType::PATCH_NAME:
                case MetaEventType::DEVICE_NAME:
                {
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
                    
                case MetaEventType::END_OF_TRACK: 
                {
                    if (length != 0) throw std::invalid_argument("Expected length for END_OF_TRACK event is 0");
                    return event;
                }
                case MetaEventType::TEMPO_CHANGE: 
                {
                    if (length != 3) throw std::invalid_argument("Expected length for TEMPO_CHANGE event is 3");
                    //event->m->data[3] = read_uint24_be(dataStart); // @dimitri TOFIX
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
                case MetaEventType::SMPTE_OFFSET: 
                {
                    if (length != 5) throw std::invalid_argument("Expected length for SMPTE_OFFSET event is 5");
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
                case MetaEventType::TIME_SIGNATURE: 
                {
                    if (length != 4) throw std::invalid_argument("Expected length for TIME_SIGNATURE event is 4");
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
                case MetaEventType::KEY_SIGNATURE: 
                {
                    if (length != 2) throw std::invalid_argument("Expected length for KEY_SIGNATURE event is 2");
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
                case MetaEventType::PROPRIETARY: 
                {
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
                case MetaEventType::UNKNOWN:
                default:
                {
                    // Unknown events?
                    read_bytes(event->m->data, dataStart, length);
                    return event;
                }
            }
        }

        else if (type == MessageType::SYSTEM_EXCLUSIVE) 
        {
            int length = read_variable_length(dataStart);
            read_bytes(event->m->data, dataStart, length);
            return event;
        }

        else if (type == MessageType::EOX)
        {
            int length = read_variable_length(dataStart);
            read_bytes(event->m->data, dataStart, length);
            return event;
        }
        else 
        {
            throw std::runtime_error("Unrecognised MIDI event type byte");
        }
    }

    // Channel events
    else 
    {
        event->m->data = std::vector<unsigned char>(3, 0);
        event->m->data[0] = (uint8_t) type;

        // Running status... 
        if (((uint8_t) type & 0x80) == 0) 
        {
            // Reuse lastEventTypeByte as the event type.
            // eventTypeByte is actually the first parameter
            event->m->data[0] = (uint8_t) type;
            type = lastEventTypeByte;
        }
        else 
        {
            event->m->data[1] = uint8_t(*dataStart++);
            lastEventTypeByte = type;
        }

        // Just in case
        event->m->data[2] = 0xFF;

        switch (MessageType((uint8_t) type & 0xF0))
        {
            case MessageType::NOTE_OFF:
                event->m->data[2] = uint8_t(*dataStart++);
                return event;
            case MessageType::NOTE_ON:
                event->m->data[2] = uint8_t(*dataStart++);
                return event;
            case MessageType::POLY_PRESSURE:
                event->m->data[2] = uint8_t(*dataStart++);
                return event;
            case MessageType::CONTROL_CHANGE:
                event->m->data[2] = uint8_t(*dataStart++);
                return event;
            case MessageType::PROGRAM_CHANGE:
                return event;
            case MessageType::AFTERTOUCH:
                return event;
            case MessageType::PITCH_BEND: 
                event->m->data[2] = uint8_t(*dataStart++);
                return event;
            default:
                throw std::runtime_error("Unrecognised MIDI event type");
        }
    }
}

MidiFileReader::MidiFileReader() : tracks(0), ticksPerBeat(480), startingTempo(120)
{
        
}

MidiFileReader::~MidiFileReader()
{

}
    
void MidiFileReader::parseInternal(const std::vector<uint8_t> & buffer)
{
    const uint8_t * dataPtr = buffer.data();
        
    int headerId = read_uint32_be(dataPtr);
    int headerLength = read_uint32_be(dataPtr);

    if (headerId != 'MThd' || headerLength != 6) 
    {
        std::cerr << "Bad .mid file - couldn't parse header" << std::endl;
        return;
    }
        
    read_uint16_be(dataPtr); //@tofix format type -> save for later eventually 

    int trackCount = read_uint16_be(dataPtr);
    int timeDivision = read_uint16_be(dataPtr);
        
    // CBB: deal with the SMPTE style time coding
    // timeDivision is described here http://www.sonicspot.com/guide/midifiles.html
    if (timeDivision & 0x8000) 
    {
        std::cerr << "Found SMPTE time frames" << std::endl;
        //int fps = (timeDivision >> 16) & 0x7f;
        //int ticksPerFrame = timeDivision & 0xff;
        // given beats per second, timeDivision should be derivable.
        return;
    }
        
    startingTempo = 120.0f; // midi default 
    ticksPerBeat = float(timeDivision); // ticks per beat (a beat is defined as a quarter note)
        
    for (int i = 0; i < trackCount; ++i) 
    {
        MidiTrack track;

        headerId = read_uint32_be(dataPtr);
        headerLength = read_uint32_be(dataPtr);

        if (headerId != 'MTrk') 
        {
            throw std::runtime_error("Bad .mid file - couldn't find track header");
        }

        uint8_t const * dataEnd = dataPtr + headerLength;

        MessageType runningEvent = MessageType::INVALID;

        int tickCount = 0;

        while (dataPtr < dataEnd) 
        {
            auto tick = read_variable_length(dataPtr);
            
            if (useAbsoluteTicks)
            {
                tickCount += tick;
            }
            else
            {
                tickCount = tick;
            }

            auto ev = std::shared_ptr<TrackEvent>(parseEvent(tickCount, i, dataPtr, runningEvent));

            if (ev->m->isMetaEvent() == false)
            {
                runningEvent = MessageType(ev->m->data[0]);
            }
                
            track.push_back(ev);
        }

        tracks.push_back(track);
    }

}

// In ticks
double MidiFileReader::getEndTime()
{
    double totalLength = 0;
    for (const auto t : tracks)
    {
        double localLength = 0;
        for (const auto e : t)
            localLength += e->tick;

        if (localLength > totalLength)
            totalLength = localLength;
    }
    return totalLength;
}

void MidiFileReader::parse(const std::vector<uint8_t> & buffer)
{
    tracks.clear();
    parseInternal(buffer);
}

} // mm
