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

#ifndef MODERNMIDI_EVENT_H
#define MODERNMIDI_EVENT_H

#include "modernmidi.h"
#include "midi_message.h"
#include <stdint.h>

namespace mm
{
    ///////////////////////
    // MIDI Player Event //
    ///////////////////////

    struct MidiPlayerEvent
    {
        MidiPlayerEvent(double t,std::shared_ptr<MidiMessage> m, int track) : timestamp(t), trackIdx(track), msg(m) {}
        double timestamp;
        int trackIdx;
        std::shared_ptr<MidiMessage> msg;
    };

    template<class C, class R> std::basic_ostream<C, R> & operator << (std::basic_ostream<C, R> & ss, const MidiPlayerEvent & v) 
    {
        return ss << "[" << int(v.msg->data[0]) << ", " << int(v.msg->data[1]) << ", " << int(v.msg->data[2]) << "]";
    }

    ///////////////////////
    // MIDI Track Events //
    ///////////////////////

    struct TrackEvent
    {
        int tick = 0;
        int track = 0;
        std::shared_ptr<MidiMessage> m;
        TrackEvent(int tick, int track, std::shared_ptr<MidiMessage> m) : tick(tick), track(track), m(m) { }
        TrackEvent(TrackEvent && r) { *this = std::move(r); }
        TrackEvent & operator = (TrackEvent && r) { tick = r.tick; track = r.track; m = std::move(r.m); return *this; }
        virtual ~TrackEvent() {};
    };

    ////////////////
    // MIDI Track //
    ////////////////

    typedef std::vector<std::shared_ptr<TrackEvent>> MidiTrack;
    
} // mm

#endif