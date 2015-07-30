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

#include "metamidi.h"
#include "port_manager.h"
#include "midi_output.h"

int main(int argc, char *argv[], char *envp[])
{
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

	/*
	if (transpose)
	{
			progressionState = (progressionState + 1) % 12; 
			baseNote = centerNote + progression[progressionState];
	}
	*/

	int transposedNote = 0;
	int scaleIndexToUse = transposedNote % 12;
	int modifier = scales[5][scaleIndexToUse];


	MidiOutput hiduino("hiduino device");
	hiduino.openPort(1);

	for (int i = 0; i < 8; i++)
	{
		hiduino.sendMessage({144, uint8_t(i), 1});
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	return 0;
}