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

#ifndef MODERNMIDI_SEQUENCE_PLAYER_H
#define MODERNMIDI_SEQUENCE_PLAYER_H

#include "modernmidi.h"
#include "concurrent_queue.h"
#include "midi_message.h"
#include "midi_file_reader.h"

#include <functional>
#include <thread>
#include <chrono>
#include <atomic>

#include "midi_output.h"

namespace mm 
{

// This class is always a work in progress, and does not currently handle things like
// mid-track tempo changes.
class MidiSequencePlayer 
{
    MidiOutput & output;
    
    void run();
    
    // Default behavior of this function is to reject playing any metadata events
    void addTimestampedEvent(int track, double when, std::shared_ptr<TrackEvent> ev);
    
    double ticksToSeconds(int ticks);
    
    int secondsToTicks(float seconds);

    float beatsPerMinute;
    double ticksPerBeat;
    double msPerTick;
    
    float playTimeSeconds = 0;
    float startTime = 0;
    int eventCursor = 0;
    
    std::thread sequencerThread;
    std::atomic<bool> shouldSequence;
    bool loop = false;
    
public:

    MidiSequencePlayer(MidiOutput & output);
    ~MidiSequencePlayer();
        
    void loadSingleTrack(const MidiTrack & track, double ticksPerBeat = 480, double beatsPerMinute = 120);
    void loadMultipleTracks(const std::vector<MidiTrack> & tracks, double ticksPerBeat = 480, double beatsPerMinute = 120);

    void start();
    void stop();

    void setLooping(bool newState);

    float length() const;
    
    void reset();

    std::function<void ()> startedEvent;
    std::function<void ()> stoppedEvent;

    std::function<void(const MidiPlayerEvent ev)> eventCallback;

    ConcurrentQueue<MidiPlayerEvent> eventQueue;

    std::vector<MidiPlayerEvent> eventList; // indexed by track
};

} // mm

#endif
