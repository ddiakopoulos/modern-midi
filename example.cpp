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

#include "metamidi.h"
#include "port_manager.h"
#include "midi_output.h"

int main(int argc, char *argv[], char *envp[])
{
	PortManager::PrintPortList(mm::PortType::TYPE_OUTPUT);
	//auto name = PortManager::GetPortName(mm::PortType::TYPE_OUTPUT, 1);

	MidiOutput hiduino("hiduino device");
	hiduino.openPort(1);

	for (int i = 0; i < 8; i++)
	{
		hiduino.sendMessage({144, uint8_t(i), 1});
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	return 0;
}