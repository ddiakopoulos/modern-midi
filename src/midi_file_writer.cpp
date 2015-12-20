#include "midi_file_writer.h"

using namespace mm;

std::ostream & write_uint16_be(std::ostream & out, uint16_t value)
{
    union { uint8_t bytes[2]; uint16_t v; } data;
    data.v = value;
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}

std::ostream & write_int16_be(std::ostream & out, int16_t value)
{
    union { uint8_t bytes[2]; int16_t v; } data;
    data.v = value;
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}

std::ostream & write_uint32_be(std::ostream & out, uint32_t value)
{
    union { uint8_t bytes[4]; uint32_t v; } data;
    data.v = value;
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}

std::ostream & write_int32_be(std::ostream & out, int32_t value)
{
    union { uint8_t bytes[4]; int32_t v; } data;
    data.v = value;
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}

std::ostream & write_float_be(std::ostream & out, float value)
{
    union { uint8_t bytes[4]; float v; } data;
    data.v = value;
    out << data.bytes[3]; out << data.bytes[2];
    out << data.bytes[1]; out << data.bytes[0];
    return out;
}

std::ostream & write_double_be(std::ostream & out, double value)
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
void write_variable_length(uint32_t aValue, std::vector<uint8_t> & outdata)
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

MidiFileWriter::MidiFileWriter() { }

MidiFileWriter::~MidiFileWriter() { }

void MidiFileWriter::addTrack()
{
    tracks.emplace_back(MidiTrack());
}

void MidiFileWriter::addEvent(int tick, int track, std::shared_ptr<MidiMessage> m)
{
    if (track > tracks.size()) 
        throw std::invalid_argument("track exceeds availble tracks");

    tracks[track].push_back(std::make_shared<TrackEvent>(tick, track, m));
}

void MidiFileWriter::addEvent(int track, std::shared_ptr<TrackEvent> m)
{
    if (track > tracks.size())
        throw std::invalid_argument("track exceeds availble tracks");
    
    tracks[track].push_back(m);
}

void MidiFileWriter::write(std::ostream & out)
{
    // MIDI File Header
    out << 'M'; out << 'T'; out << 'h'; out << 'd';
    write_uint32_be(out, 6);
    write_uint16_be(out, (getNumTracks() == 1) ? 0 : 1);
    write_uint16_be(out, getNumTracks());
    write_uint16_be(out, getTicksPerQuarterNote());
            
    std::vector<uint8_t> trackRawData;
            
    for (auto & event_list : tracks)
    {
        for (auto & event : event_list)
        {
            const auto msg = event->m;
                    
            // Suppress end-of-track meta messages (one will be added
            // automatically after all track data has been written).
            if (msg->getMetaEventSubtype() == uint8_t(MetaEventType::END_OF_TRACK)) continue;
                    
            write_variable_length(event->tick, trackRawData);
                    
            if ((msg->getMessageType() == uint8_t(MessageType::SYSTEM_EXCLUSIVE)) || (event->m->getMessageType() == uint8_t(MessageType::EOX)))
            {
                // 0xf0 == Complete sysex message (0xf0 is part of the raw MIDI).
                // 0xf7 == Raw byte message (0xf7 not part of the raw MIDI).
                // Print the first byte of the message (0xf0 or 0xf7), then
                // print a VLV length for the rest of the bytes in the message.
                // In other words, when creating a 0xf0 or 0xf7 MIDI message,
                // do not insert the VLV byte length yourself, as this code will
                // do it for you automatically.
                trackRawData.emplace_back(msg->data[0]); // 0xf0 or 0xf7;
                        
                write_variable_length(msg->messageSize() - 1, trackRawData);
                        
                for (size_t k = 1; k < msg->messageSize(); k++)
                {
                    trackRawData.emplace_back((*msg)[k]);
                }
            }
                    
            else
            {
                // Non-sysex type of message, so just output the bytes of the message:
                for (size_t k = 0; k < msg->messageSize(); k++)
                {
                    trackRawData.emplace_back((*msg)[k]);
                }
            }
        }
    }
                
    auto size = trackRawData.size();
    auto eot = MakeEndOfTrackMetaEvent();
    
    if ((size < 3) || !((trackRawData[size - 3] == 0xFF) && (trackRawData[size - 2] == 0x2F)))
    {
        trackRawData.emplace_back(0x0); // tick
        trackRawData.emplace_back(eot[0]);
        trackRawData.emplace_back(eot[1]);
        trackRawData.emplace_back(eot[2]);
    }
            
    // Write the track ID marker "MTrk":
    out << 'M'; out << 'T'; out << 'r'; out << 'k';
    write_uint32_be(out, trackRawData.size());
    out.write((char*) trackRawData.data(), trackRawData.size());
}