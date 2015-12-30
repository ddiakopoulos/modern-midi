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

#ifndef MODERNMIDI_UTIL_H
#define MODERNMIDI_UTIL_H

#include "modernmidi.h"
#include "midi_event.h"

namespace mm 
{
    // Convert a MIDI command byte into a descriptive string
    const char * commandName(uint8_t command);

    // Convert a note number to frequency
    // The default value for A is 440.0. A different tuning value
    // can be provided.
    float noteToFrequency(uint8_t note);
    float noteToFrequency(uint8_t, float A);
    
    // Convert a frequency to a note number
    // The default value for A is 440.0. A different tuning value
    // can be provided.
    uint8_t frequencyToNote(float freq);
    uint8_t frequencyToNote(float freq, float A);
    
    // In-place conversion of tracks in absolute tick format
    // to a delta tick format
    inline void ConvertToDeltaTicks(std::vector<MidiTrack> & tracks)
    {
        for (int i = 0; i < tracks.size(); i++)
        {
            for (auto & event_list : tracks)
            {
                int lastTickValue = 0;
                int tmpTick = 0;
                for (auto & event : event_list)
                {
                    const auto msg = event->m;
                    tmpTick = event->tick;
                    event->tick -= lastTickValue;
                    lastTickValue = tmpTick;
                }
            }
        }
    }
    
    // In-place conversion of tracks in delta tick format
    // to an absolute tick format
    inline void ConvertToAbsoluteTicks(std::vector<MidiTrack> & tracks)
    {
        for (int i = 0; i < tracks.size(); i++)
        {
            for (auto & event_list : tracks)
            {
                int runningTickCounter = 0;
                int tmpTick = 0;
                for (auto & event : event_list)
                {
                    const auto msg = event->m;
                    tmpTick = event->tick;
                    event->tick = runningTickCounter;
                    runningTickCounter += tmpTick;
                }
            }
        }
    }
    

} // mm

#endif