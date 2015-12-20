#pragma comment(user, "license")

#include "port_manager.h"
using namespace mm;

// Statics
std::shared_ptr<RtMidiOut> PortManager::outputDevice;
std::shared_ptr<RtMidiIn> PortManager::inputDevice;

void PortManager::PrintPortList(PortType t) 
{
    assert(t <= TYPE_INVALID);
    if (t == TYPE_INPUT) listPorts(fetch<RtMidiIn>(inputDevice));
    else listPorts(fetch<RtMidiOut>(outputDevice));
}

std::vector<std::string> PortManager::GetPortList(PortType t) 
{
    assert(t <= TYPE_INVALID);
    if (t == TYPE_INPUT) return getPortList(fetch<RtMidiIn>(inputDevice));
    else return getPortList(fetch<RtMidiOut>(outputDevice));
}

int PortManager::GetPortCount(PortType t) 
{
    assert(t <= TYPE_INVALID);
    if (t == TYPE_INPUT) return getNumPorts(fetch<RtMidiIn>(inputDevice));
    else return getNumPorts(fetch<RtMidiOut>(outputDevice));
}

std::string PortManager::GetPortName(PortType t, uint32_t portNumber) 
{
    assert(t <= TYPE_INVALID);
    if (t == TYPE_INPUT) return getPortName(fetch<RtMidiIn>(inputDevice), portNumber);
    else return getPortName(fetch<RtMidiOut>(outputDevice), portNumber);
}