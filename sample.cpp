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
#include "midi_output.h"
#include "midi_message.h"
#include "midi_event.h"

#include "midi_file_reader.h"
#include "midi_file_writer.h"

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

int main(int argc, char *argv[], char *envp[])
{
    
    MidiFileReader reader;
    reader.useAbsoluteTicks = false;
    reader.parse(readFile("assets/midifonts.mid"));
    
    // Track = 0 meta, Track 27 = debug
    
    MidiFileWriter theLetterA;
    theLetterA.setTicksPerQuarterNote(480);
    
    theLetterA.addTrack();

    auto aTrack = reader.tracks[1];
    
    for (int i = 0; i < aTrack.size(); i++)
    {
        auto event = aTrack[i];
        if (event->m->isNoteOnOrOff())
        {
            theLetterA.addEvent(0, event);
        }
    }
    
    std::fstream output("loopback_a.mid", std::ios::out);
    theLetterA.write(output);
    
    output.close();
    
	return 0;
}