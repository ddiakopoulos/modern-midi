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

#include <time.h>
#include <chrono>
#include <functional>
#include <algorithm>
#include <functional>
#include <random>
#include <vector>
#include <thread>
#include <iostream>
#include <fstream>
#include <mutex>
#include <cassert>
#include <map>
#include <type_traits>
#include <array>
#include <fstream>

#include "modernmidi.h"
#include "port_manager.h"
#include "midi_input.h"
#include "midi_output.h"
#include "midi_message.h"
#include "midi_event.h"
#include "midi_file_reader.h"
#include "midi_file_writer.h"
#include "sequence_player.h"

std::random_device rd;
std::mt19937 gen(rd());

std::vector<uint8_t> readFile(std::string pathToFile)
{
    std::cout << "[Debug] Open: " << pathToFile << std::endl;
    
    FILE * file = fopen(pathToFile.c_str(), "rb");
    
    if (!file)
        throw std::runtime_error("file not found");
    
    fseek(file, 0, SEEK_END);
    size_t lengthInBytes = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    std::vector<uint8_t> fileBuffer(lengthInBytes);
    
    size_t elementsRead = fread(fileBuffer.data(), 1, lengthInBytes, file);
    
    if (elementsRead == 0 || fileBuffer.size() < 64)
        throw std::runtime_error("error reading file or file too small");
    
    fclose(file);
    
    return fileBuffer;
}

using namespace mm;

void ExampleConstructMessages()
{
    // The library implements factory functions to construct common message types:
    MidiMessage msg0 = MakeNoteOn(1, 60, 127);
    MidiMessage msg1 = MakeNoteOff(1, 60, 127);
    MidiMessage msg2 = MakeTextMetaEvent(MetaEventType::LYRIC, "ModernMIDI");
    
    // MidiMessage will directly take bytes, but doesn't ensure that you've created a valid message...
    MidiMessage invalidMessage = MidiMessage(0, 255, 128, 0.0);
    
    // Use the array operator to access MIDI bytes directly
    if (invalidMessage[1] == 255)
    {
        // True!
    }
    
    if (msg0.isNoteOnOrOff())
    {
        // True!
    }
    
    if (msg1.getMessageType() == (uint8_t) MessageType::NOTE_OFF)
    {
        // True!
    }
    
    if (msg2.isMetaEvent())
    {
        // True!
    }
    
    if (msg0.getChannel() == 1)
    {
        // True! ModernMIDI 1-indexes MIDI channels (1-16, not 0-15)
    }
}

void ExampleQueryMIDIDevices()
{
    // Use the static functions to pretty-print a list of available devices
    PortManager::PrintPortList(PortType::TYPE_OUTPUT);
    PortManager::PrintPortList(PortType::TYPE_INPUT);
    
    // Both functions come with parsable counterparts
    std::vector<std::string> outputList = PortManager::GetPortList(PortType::TYPE_OUTPUT);
    std::vector<std::string> inputList = PortManager::GetPortList(PortType::TYPE_INPUT);
    
    // Also query the name of a port number
    std::string outputZeroPortName = PortManager::GetPortName(PortType::TYPE_OUTPUT, 0);
    std::string inputZeroPortName = PortManager::GetPortName(PortType::TYPE_INPUT, 0);
}

void ExampleOpenMIDIInput()
{
    // Initialize an input device. The string identifer is for human-readability
    // and does not find or open a device by that handle.
    MidiInput exampleInput("example");
    
    // Attempt to open an input device by port number. This method may throw internally
    // and will pretty-print the exception message on failure.
    bool success = exampleInput.openPort(0);
    
    // Attempt to open an input device by port name. This function is particularly
    // useful because most operating systems do not guarantee that port numbers
    // remain static across reboots:
    // bool success = exampleInput.openPort("midi-device-name");
    
    if (success)
    {
        // Attach an input callback to the device to accept incoming messages
        exampleInput.messageCallback = [&](const mm::MidiMessage msg)
        {
            std::cout << "Data: " << (int) msg.data[0] << ", " << (int) msg.data[1] << ", " <<(int) msg.data[2] << std::endl;
        };
    }
    
    // exampleInput will automatically close itself when it leaves this scope, so the following line is not needed:
    // exampleInput.closePort()
}

void ExampleOpenMIDIOutput()
{
    // Initialize an output device. The string identifer is for human-readability
    // and does not find or open a device by that handle.
    MidiOutput exampleOutput("example");
    
    // Attempt to open an output device by port number. This method may throw internally
    // and will pretty-print the exception message on failure.
    bool success = exampleOutput.openPort(0);
    
    if (success)
    {
        // The MidiOutput API is simple and either takes a vector of bytes
        // or a ModernMIDI message type:
        exampleOutput.send(MakeProgramChange(1, 88));
    }
    
    exampleOutput.closePort();
}

void ExampleReadWriteFile()
{
    MidiFileReader reader;
    
    // Midi track events are always encoded with a delta value (time in ticks between
    // events), so the user of this library has the ability to treat the value
    // as a true delta or a running counter of elapsed ticks.
    reader.useAbsoluteTicks = false;
    
    try
    {
        // One of the asset files is a font file that encodes the letters A through Z
        // as note on + note off events for use in an installation.
        reader.parse(readFile("assets/midifonts.mid"));
    }
    catch (const std::exception & e)
    {
        std::cout << "Parsing Error: " << e.what() << std::endl;
    }
    
    // Track 0 = meta; Track 1 = A; ... ; Track 26 = Z; Track 27 = debug
    
    // Look for tempo event, assume we only have one
    // Note: this is a little broken at the moment...
    for (const auto track : reader.tracks)
    {
        for (const auto e : track)
        {
            auto msg = *e->m.get();
            
            if (!msg.isMetaEvent()) continue;
            
            if (msg.getMetaEventSubtype() == (uint8_t) MetaEventType::TEMPO_CHANGE)
            {
                double microsecondsPerBeat = msg[0];
                double beatsPerMinute = float(60000000.0 / double (microsecondsPerBeat));
                double msPerTick = 60000.0 / beatsPerMinute / reader.ticksPerBeat;
            }
            else if (msg.getMetaEventSubtype() == (uint8_t) MetaEventType::TIME_SIGNATURE)
            {
                std::cout << "Time Signature: " << (int) msg[1] << " / " << (int) msg[2] << std::endl; // Numerator / Denominator
            }
        }
    }
    
    // We're going to take track 1 from midifonts.mid and write it back out
    // as a separate file. The file will contain the letter A.
    MidiFileWriter theLetterA;
    
    // We can use a value that we found in the file (see the tempo loop above), or supply our own
    theLetterA.setTicksPerQuarterNote(480);
    
    // Add an initial empty track (indexed at location 0)
    theLetterA.addTrack();
    
    // It's useful to know the underlying types when parsing data,
    // but a helper typedefs are provided
    MidiTrack aTrack = reader.tracks[1]; // Where MidiTrack is actually std::vector<std::shared_ptr<TrackEvent>>
    
    for (int i = 0; i < aTrack.size(); i++)
    {
        std::shared_ptr<TrackEvent> event = aTrack[i];
        if (event->m->isNoteOnOrOff())
            theLetterA.addEvent(0, event);
    }
    
    // Write back to disk
    std::fstream output("assets/loopback.mid", std::ios::out);
    theLetterA.write(output);
    output.close();
}

void ExampleSequencePlayer()
{
    
}

int main(int argc, char *argv[], char *envp[])
{
    ExampleConstructMessages();
    
    ExampleQueryMIDIDevices();
    
    ExampleOpenMIDIInput();
    
    ExampleOpenMIDIOutput();
    
    ExampleReadWriteFile();
    
    ExampleSequencePlayer();
    
    return 0;
}