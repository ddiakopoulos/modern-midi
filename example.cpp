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

	std::this_thread::sleep_for(std::chrono::milliseconds(100000));
	return 0;
}