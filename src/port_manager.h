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

#pragma once

#ifndef MODERNMIDI_MIDI_PORT_MGR_H
#define MODERNMIDI_MIDI_PORT_MGR_H

#include "modernmidi.h"
#include <assert.h>

namespace mm
{

class PortManager
{
    static std::shared_ptr<RtMidiOut> outputDevice;
    static std::shared_ptr<RtMidiIn> inputDevice;

    template<typename T>
    static std::shared_ptr<T> fetch(std::shared_ptr<T> device)
    {
        if (device) return device;
        else
        {
            device = std::make_shared<T>();
            return device;
        }
    }

    template<typename T>
    static void listPorts(std::shared_ptr<T> device)
    {
        std::cout << device->getPortCount() << " ports available: \n";
        for (uint32_t i = 0; i < device->getPortCount(); ++i)
        {
            std::cout << "## " << i << " : " << device->getPortName(i) << std::endl;
        }
        std::cout << std::endl;
    }

    template<typename T>
    static std::vector<std::string> getPortList(std::shared_ptr<T> device)
    {
        std::vector<std::string> portList;
        for(uint32_t i = 0; i < device->getPortCount(); ++i) 
        {
            portList.emplace_back(device->getPortName(i));
        }
        return portList;
    }

    template<typename T>
    static int getNumPorts(std::shared_ptr<T> device)
    {
        return device->getPortCount();
    }

    template<typename T>
    static std::string getPortName(std::shared_ptr<T> device, uint32_t portNumber)
    {
        try 
        {
            return device->getPortName(portNumber);
        }
        catch(RtMidiError & e) 
        {
            std::cerr << e.what() << std::endl;
        }
        return "";
    }

public:

    static void PrintPortList(mm::PortType t);
    static std::vector<std::string> GetPortList(mm::PortType t);
    static int GetPortCount(mm::PortType t);
    static std::string GetPortName(mm::PortType t, uint32_t portNumber);

};

}

#endif