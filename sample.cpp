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

#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <random>
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
#include "midi_utils.h"

std::random_device rd;
std::mt19937 gen(rd());

inline int32_t Pack(uint8_t a, uint8_t b, uint8_t c)
{
    uint32_t tmp = ((c & 0x80) ? (0xFF << 24) : 0x00 << 24) | (c << 16) | (b << 8) | (a << 0);
    return tmp;
}

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
    
    // MidiMessage will directly take bytes, but doesn’t ensure that you’ve created a valid message…
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
    
    if (msg1.getMessageType() == MessageType::NOTE_OFF)
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
    
    // Both functions come with parseable counterparts
    std::vector<std::string> outputList = PortManager::GetPortList(PortType::TYPE_OUTPUT);
    std::vector<std::string> inputList = PortManager::GetPortList(PortType::TYPE_INPUT);
    
    // Also query the name of a port number
    std::string outputZeroPortName = PortManager::GetPortName(PortType::TYPE_OUTPUT, 0);
    std::string inputZeroPortName = PortManager::GetPortName(PortType::TYPE_INPUT, 0);
}

void ExampleOpenMIDIInput()
{
    // Initialize an input device. The string identifier is for human-readability
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
    // Initialize an output device. The string identifier is for human-readability
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
    
    // midifonts.mid track layout:
    // [Track 0 = meta // Track 1 = A //  …  // Track 26 = Z // Track 27 = debug]
    
    // Look for tempo event, assume we only have one
    for (const auto track : reader.tracks)
    {
        for (const auto e : track)
        {
            auto msg = *e->m.get();
            
            if (!msg.isMetaEvent()) continue;
            
            if (msg.getMetaEventSubtype() == MetaEventType::TEMPO_CHANGE)
            {
                double microsecondsPerBeat = Pack(msg[5], msg[4], msg[3]);
                double beatsPerMinute = float(60000000.0 / double (microsecondsPerBeat));
                double msPerTick = 60000.0 / beatsPerMinute / reader.ticksPerBeat;
                std::cout << "Microseconds Per Beat: " << microsecondsPerBeat << std::endl;
                std::cout << "Beats Per Minute: " << beatsPerMinute << std::endl;
                std::cout << "Milliseconds per Tick: " << msPerTick << std::endl;
            }
            else if (msg.getMetaEventSubtype() == MetaEventType::TIME_SIGNATURE)
            {
                std::cout << "Time Signature: " << (int) msg[4] << " / " << (int) powf(2.0f, msg[5]) << std::endl; // Numerator / Denominator
            }
        }
    }
    
    // We’re going to take track 1 from midifonts.mid and write it back out
    // as a separate file. The file will contain the letter A.
    MidiFileWriter theLetterA;
    
    // We can use a value that we found in the file (see the tempo loop above), or supply our own
    theLetterA.setTicksPerQuarterNote(480);
    
    // Add an initial empty track (indexed at location 0)
    theLetterA.addTrack();
    
    // It’s useful to know the underlying types when parsing data,
    // but a helper typedefs are provided
    MidiTrack aTrack = reader.tracks[1]; // Where MidiTrack is actually std::vector<std::shared_ptr<TrackEvent>>
    
    for (int i = 0; i < aTrack.size(); i++)
    {
        std::shared_ptr<TrackEvent> event = aTrack[i];
        if (event->m->isNoteOnOrOff())
            theLetterA.addEvent(0, event);
    }
    
    // Write back to disk
    std::fstream output("assets/output/loopback.mid", std::ios::out);
    theLetterA.write(output);
    output.close();
}

void ExampleSequencePlayer()
{
    // ModernMIDI’s sequence player is always in flux.
    // In this implementation, the constructor takes a MidiOutput reference so it can directly play events
    // on a device. It also contains a concurrent queue so it can be polled from a separate thread,
    // (for instance to drive an animation system). In other incarnations, sequence_player also included an std::function
    // callback from the inner thread.
    
    MidiOutput sequenceOutput("sequence");
    
    bool success = sequenceOutput.openPort(0);
    
    if (success)
    {
        MidiSequencePlayer player(sequenceOutput);
        
        {
            MidiTrack programmaticTrack;
            
            auto noteOnMsg = std::make_shared<MidiMessage>(MakeNoteOn(1, 60, 96));
            programmaticTrack.push_back(std::make_shared<TrackEvent>(120, 0, noteOnMsg));
            
            auto noteOffMsg = std::make_shared<MidiMessage>(MakeNoteOn(1, 60, 0));
            programmaticTrack.push_back(std::make_shared<TrackEvent>(249, 0,  noteOffMsg));
            
            player.loadSingleTrack(programmaticTrack);
        }
        
        // Started Event Callback
        player.startedEvent = [&]()
        {
            std::cout << "Notification that the MidiSequencePlayer has started…" << std::endl;
        };
        
        // Stopped Event Callback
        player.stoppedEvent = [&]()
        {
            std::cout << "Notification that the MidiSequencePlayer has stopped…" << std::endl;
        };
        
        player.start();
    }
}

void ExampleMusicTheory()
{
    // Fixme
    std::vector<std::vector<int>> scales =
    {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 },
        { 0, 1, 0, 0, -1, 0, 1, 0, 0, -1, 1, 0 },
        { 0, 1, 0, 0, -1, 0, 1, 0, 0, -1, 0, -1 },
        { 0, 1, 0, 1, 0, -1, 1, 0, 1, 0, -1, 1 },
        { 0, -1, 1, 0, -1, 0, 1, 0, -1, 1, 0, -1 },
        { 0, 1, 0, 0, -1, 0, 1, 0, 1, 0, 0, -1 },
        { 0, 0, -1, 0, -1, 0, 1, 0, 0, -1, 0, -1 },
        { 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0 },
        { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, -1 },
        { 0, 0, -1, 0, -1, 0, 0, -1, 0, -1, 0, -1 },
        { 0, 0, -1, 1, 0, 0, 1, 0, 0, -1, 1, 0 },
        { 0, 1, 0, 0, -1, 0, 0, -1, 0, -1, 0, -1}
    };
    std::vector<int> progression = {0, -5, 2, -3, 4, -1, 6, 1, -4, 3, -2, 5};
    
    auto randomScaleDegree = std::uniform_int_distribution<int>(0, 11);
    
    int transposedNote = 0;
    int scaleIndexToUse = transposedNote % 12;
    int modifier = scales[5][scaleIndexToUse];
    
    MidiFileWriter scaleFile;
    scaleFile.setTicksPerQuarterNote(245);
    scaleFile.addTrack();
    
    scaleFile.addEvent(0, 0, std::make_shared<MidiMessage>(MakeTextMetaEvent(MetaEventType::TRACK_NAME, "scales!")));
    scaleFile.addEvent(0, 0, std::make_shared<MidiMessage>(MakeTempoMetaEvent(600000))); // 100 bpm
    scaleFile.addEvent(0, 0, std::make_shared<MidiMessage>(MakeTimeSignatureMetaEvent(3, 8)));
    scaleFile.addEvent(0, 0, std::make_shared<MidiMessage>(MakeKeySignatureMetaEvent(-5, true))); // B flat minor
    
    // Generate note on/off pair
    for (int i = 0; i < progression.size(); ++i)
    {
        int note = 24 + progression[i];
        scaleFile.addEvent(0, 0, std::make_shared<MidiMessage>(MakeNoteOn(1, note, 96))); // Delta tick 0
        scaleFile.addEvent(240, 0, std::make_shared<MidiMessage>(MakeNoteOff(1, note, 0))); // Delta tick + 240
    }
    
    // By the standard, MIDI files can only be encoded with delta ticks (the 14 bit variable length value
    // is not sufficient for long files). Some people prefer to author content in absolute format and convert.
    //ConvertToDeltaTicks(scaleFile.getTracks()); // Useful when .addEvent consists of absolute ticks
    //ConvertToAbsoluteTicks(scaleFile.getTracks());
  
    // Write back to disk
    std::fstream output("assets/output/scales.mid", std::ios::out);
    scaleFile.write(output);
    output.close();
    
}

int main(int argc, char *argv[], char *envp[])
{
    ExampleConstructMessages();
    
    ExampleQueryMIDIDevices();
    
    ExampleOpenMIDIInput();
    
    ExampleOpenMIDIOutput();
    
    ExampleReadWriteFile();
    
    ExampleSequencePlayer();
    
    ExampleMusicTheory();
    
    return 0;
}