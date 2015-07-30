#include "port_manager.h"

std::shared_ptr<RtMidiOut> PortManager::outputDevice;
std::shared_ptr<RtMidiIn> PortManager::inputDevice;

using namespace mm;

void PortManager::PrintPortList(PortType t) 
{
	if (t == TYPE_INPUT) listPorts(fetch<RtMidiIn>(inputDevice));
	else listPorts(fetch<RtMidiOut>(outputDevice));
}

std::vector<std::string> PortManager::GetPortList(PortType t) 
{
	if (t == TYPE_INPUT) return getPortList(fetch<RtMidiIn>(inputDevice));
	else return getPortList(fetch<RtMidiOut>(outputDevice));
}

int PortManager::GetPortCount(PortType t) 
{
	if (t == TYPE_INPUT) return getNumPorts(fetch<RtMidiIn>(inputDevice));
	else return getNumPorts(fetch<RtMidiOut>(outputDevice));
}

std::string PortManager::GetPortName(PortType t, uint32_t portNumber) 
{
	if (t == TYPE_INPUT) return getPortName(fetch<RtMidiIn>(inputDevice), portNumber);
	else return getPortName(fetch<RtMidiOut>(outputDevice), portNumber);
}