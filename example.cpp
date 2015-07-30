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

int main(int argc, char *argv[], char *envp[])
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100000));
	return 0;
}