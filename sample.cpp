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
    
}

void ExampleOpenMIDIInput()
{
    
}

void ExampleOpenMIDIOutput()
{
    
}

void ExampleReadWriteFile()
{
    
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