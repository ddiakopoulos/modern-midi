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

#ifndef MODERNMIDI_MIDI_INPUT_H
#define MODERNMIDI_MIDI_INPUT_H

#include "modernmidi.h"
#include "midi_message.h"
#include <functional>

namespace mm
{

class MidiInput
{
    std::unique_ptr<RtMidiIn> inputDevice;
    static void _callback(double delta, std::vector<uint8_t> * message, void * userData);
    void handleMessage(double delta, std::vector<uint8_t> * message);
    bool attached = false;
public:

    MidiInput(const std::string & name);
    ~MidiInput();
    
    bool openPort(int32_t portNumber);
    bool openPort(std::string deviceName);
    bool openVirtualPort(std::string portName);
    void closePort();
    
    void ignoreTypes(bool midiSysex, bool midiTiming, bool midiSense);
    RtMidiIn * getInputDevice() { return inputDevice.get(); }

    mm::MidiDeviceInfo info;

    std::function<void (const mm::MidiMessage msg)> messageCallback;
};

}

#endif