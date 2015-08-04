#include "midi_file_io.h"

using namespace mm;

MidiFile::MidiFile()
{

}

MidiFile::~MidiFile()
{

}

void MidiFile::addTrack()
{
	tracks.emplace_back(std::vector<TrackEvent>());
}

void MidiFile::addEvent(int tick, int track, std::shared_ptr<MidiMessage> m)
{
	if (track > tracks.size()) 
		throw std::invalid_argument("track exceeds availble tracks");

	tracks[track].emplace_back(tick, track, m);
}

void MidiFile::write(std::ostream & out)
{
    // 1. The characters "MThd"
    out << 'M';
    out << 'T';
    out << 'h';
    out << 'd';
            
    // 2. Header size is always 6
    write_uint32_be(out, 6);
            
    // 3. MIDI file format, type 0 or 1
    write_uint16_be(out, (getNumTracks() == 1) ? 0 : 1);
            
    // 4. write out the number of tracks.
    write_uint16_be(out, getNumTracks());
            
    // 5. write out the number of ticks per quarternote. (avoiding SMTPE for now)
    write_uint16_be(out, getTicksPerQuarterNote());
            
    std::vector<uint8_t> trackRawData;

    auto eot = MakeEndOfTrackMetaEvent();
            
    // Debugging:
    uint64_t elapsedTicks = 0;
            
    for (auto & event_list : tracks)
    {
        for (auto & event : event_list)
        {
            const auto msg = event.m;
                    
            // Suppress end-of-track meta messages (one will be added
            // automatically after all track data has been written).
            if (msg->getMetaEventSubtype() == uint8_t(MetaEventType::END_OF_TRACK)) continue;
                    
            // Debugging
            std::cout << "[Tick] # " << int (event.tick) << std::endl;
            elapsedTicks += event.tick;
            std::cout << "[Elapsed] # " <<  elapsedTicks << std::endl;
                    
            write_variable_length(event.tick, trackRawData);
                    
            if ((msg->getMessageType() == uint8_t(MessageType::SYSTEM_EXCLUSIVE)) || (event.m->getMessageType() == uint8_t(MessageType::EOX)))
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
                    std::cout << "[Sysex] Write: " << int ((*msg)[k]) << std::endl;
                    trackRawData.emplace_back((*msg)[k]);
                }
            }
                    
            else
            {
                // Non-sysex type of message, so just output the bytes of the message:
                for (size_t k = 0; k < msg->messageSize(); k++)
                {
                    std::cout << "[Msg] Write: " << int ((*msg)[k]) << std::endl;
                    trackRawData.emplace_back((*msg)[k]);
                }
            }
        }
    }
                
    auto size = trackRawData.size();
            
    if ((size < 3) || !((trackRawData[size - 3] == 0xFF) && (trackRawData[size - 2] == 0x2F)))
    {
		trackRawData.emplace_back(0x0); // tick
        trackRawData.emplace_back(eot[0]);
        trackRawData.emplace_back(eot[1]);
        trackRawData.emplace_back(eot[2]);
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
}