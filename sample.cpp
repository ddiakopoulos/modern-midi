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
#include "midi_file_io.h"

std::random_device rd;
std::mt19937 gen(rd());

using namespace mm;

int main(int argc, char *argv[], char *envp[])
{
    MidiFile myFile;

    myFile.addTrack();
	myFile.addEvent(0, 0, std::make_shared<MidiMessage>(MakeTextMetaEvent(MetaEventType::TRACK_NAME, "test")));
    myFile.addEvent(120, 0, std::make_shared<MidiMessage>(MakeNoteOn(1, 60, 96)));
	myFile.addEvent(360, 0, std::make_shared<MidiMessage>(MakeNoteOff(1, 60, 0)));
   
    std::fstream output("test.mid", std::ios::out);
    
    if (!output.is_open())
    {
        std::cerr << "couldn't open... " << std::endl;
    }
    
    myFile.write(output);
    
    output.close();
    
    std::cout << "Done!" << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(10));
    /*
	PortManager::PrintPortList(mm::PortType::TYPE_OUTPUT);
	//auto name = PortManager::GetPortName(mm::PortType::TYPE_OUTPUT, 1);

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

	int progressionState = 0; 
	int baseNote = 0;
	int centerNote = 0;
	int mode = 5;

	//if (transpose)
	//{
	//		progressionState = (progressionState + 1) % 12;
	//		baseNote = centerNote + progression[progressionState];
	//}

	int transposedNote = 0;
	int scaleIndexToUse = transposedNote % 12;
	int modifier = scales[5][scaleIndexToUse];
    
    auto msg = mm::MakeNoteOn(2, 60, 127);

	MidiOutput hiduino("hiduino device");
	bool success = hiduino.openPort(1);

    
	uint8_t scaleIdx = 0;

	auto randomScaleDegree = std::uniform_int_distribution<int>(1, 7);

	for (int i = 0; i < 128; i++)
	{
		if (i % 4 == 0)
		{
			hiduino.send(MakeNoteOn(1, scaleIdx, 1));
		}

		if (i % 8 == 0)
		{
			hiduino.send(MakeNoteOn(1, uint8_t(randomScaleDegree(gen)), 1));
		}

		if (i % 16 == 0)
		{
			scaleIdx++;
		}

		std::this_thread::sleep_for(std::chrono::microseconds(16 * 8333)); // 120 bpm
	}
     
    */
    
	return 0;
}