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

#include "sequence_player.h"

std::random_device rd;
std::mt19937 gen(rd());

std::vector<uint8_t> read_file_binary(std::string pathToFile)
{
	std::cout << "[Debug] Open: " << pathToFile << std::endl;
    
    FILE * file = fopen(pathToFile.c_str(), "rb");
    
    if (!file)
    {
        throw std::runtime_error("file not found");
    }
    
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


std::vector<int> generate_string_track_indicies(const std::string & name)
{
	std::vector<int> indices;

	for (auto & c : name) 
	{
		if (c == 'a') indices.push_back(0);
		if (c == 'b') indices.push_back(1);
		if (c == 'c') indices.push_back(2);
		if (c == 'd') indices.push_back(3);
		if (c == 'e') indices.push_back(4);
		if (c == 'f') indices.push_back(5);
		if (c == 'g') indices.push_back(6);
		if (c == 'h') indices.push_back(7);
		if (c == 'i') indices.push_back(8);
		if (c == 'j') indices.push_back(9);
		if (c == 'k') indices.push_back(10);
		if (c == 'l') indices.push_back(11);
		if (c == 'm') indices.push_back(12);
		if (c == 'n') indices.push_back(13);
		if (c == 'o') indices.push_back(14);
		if (c == 'p') indices.push_back(15);
		if (c == 'q') indices.push_back(16);
		if (c == 'r') indices.push_back(17);
		if (c == 's') indices.push_back(18);
		if (c == 't') indices.push_back(19);
		if (c == 'u') indices.push_back(20);
		if (c == 'v') indices.push_back(21);
		if (c == 'w') indices.push_back(22);
		if (c == 'x') indices.push_back(23);
		if (c == 'y') indices.push_back(24);
		if (c == 'z') indices.push_back(25);
	}

	return indices;
}

int main(int argc, char *argv[], char *envp[])
{
    MidiFileReader reader;
    reader.useAbsoluteTicks = false;
    reader.parse(read_file_binary("midifonts.mid"));

	std::vector<MidiTrack> letters;

	// Map 0-26 = A through Z
	for (int i = 1; i < 27; i++)
	{
		letters.push_back(reader.tracks[i]);
	}

	auto letterIndicies = generate_string_track_indicies("juliusrosenberg");

	std::vector<MidiTrack> dimitriName;
	for (auto idx : letterIndicies)
	{
		dimitriName.push_back(letters[idx]);
	}

	PortManager::PrintPortList(TYPE_OUTPUT);
		
	MidiOutput dripper("mio");
	MidiSequencePlayer player(dripper);

	bool success = dripper.openPort(1);

	if (success)
	{
		player.loadSequence(dimitriName);
		player.start();
	}

	while(true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}