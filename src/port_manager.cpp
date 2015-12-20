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