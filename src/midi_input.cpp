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

#include "midi_input.h"

using namespace mm;

// Static
void MidiInput::_callback(double delta, std::vector<uint8_t> * message, void * userData) 
{
    auto myInput = static_cast<MidiInput*>(userData);
    myInput->handleMessage(delta * 1000, message); // convert s to ms
}

MidiInput::MidiInput(const std::string & name)
{
    inputDevice.reset(new RtMidiIn(RtMidi::UNSPECIFIED, name));
}

MidiInput::~MidiInput() 
{
    closePort();
}

void MidiInput::handleMessage(double delta, std::vector<uint8_t> * message)
{
    MidiMessage incomingMsg (message->at(0),message->at(1), message->at(2));

    if (messageCallback)
        messageCallback(incomingMsg);
}

bool MidiInput::openPort(int32_t portNumber) 
{   
    if (attached) throw std::runtime_error("device is already attached to a port");
    try 
    {
        inputDevice->setCallback(&_callback, this);
        inputDevice->openPort(portNumber, std::to_string(portNumber));
        attached = true;
    }
    catch (RtMidiError& e) 
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
    info = {portNumber, false, inputDevice->getPortName(portNumber)};
    return true;
}

bool MidiInput::openPort(std::string deviceName) 
{
    int port = -1;
    for (uint32_t i = 0; i < inputDevice->getPortCount(); ++i) 
    {
        if (inputDevice->getPortName(i) == deviceName) 
        {
            port = i;
            break;
        }
    }
    if (port == -1) 
    {
        std::cerr << "Port not available" << std::endl;
        return false;
    } 
    return openPort(port);
}

bool MidiInput::openVirtualPort(std::string portName) 
{
    
#if defined (MM_PLATFORM_WINDOWS)
    throw std::runtime_error("virtual ports are unsupported on windows");
#endif
    
    if (attached) throw std::runtime_error("device is already attached to a port");
    try 
    {
        inputDevice->setCallback(&_callback, this);
        inputDevice->openVirtualPort(portName);
        attached = true;
    }
    catch(RtMidiError & e) 
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    info = {-1, true, portName};
    return true;
}

void MidiInput::closePort() 
{
    inputDevice->closePort();
    if (attached) inputDevice->cancelCallback();
    info = {-1, false, ""};
    attached = false;
}

void MidiInput::ignoreTypes(bool midiSysex, bool midiTiming, bool midiSense) 
{
    inputDevice->ignoreTypes(midiSysex, midiTiming, midiSense);
}
