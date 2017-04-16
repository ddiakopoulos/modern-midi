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

#include "modernmidi.h"
#include "sequence_player.h"
#include "midi_message.h"
#include "midi_utils.h"
#include "timer.h"

using namespace mm;

MidiSequencePlayer::MidiSequencePlayer(MidiOutput & output) : shouldSequence(false), output(output)
{

}
    
MidiSequencePlayer::~MidiSequencePlayer()
{
    shouldSequence = false;

    if (sequencerThread.joinable())
        sequencerThread.join();
}

double MidiSequencePlayer::ticksToSeconds(int ticks)
{
    double beats = (double) ticks / ticksPerBeat;
    double seconds = beats / (beatsPerMinute / 60.0);
    return seconds;
}

int MidiSequencePlayer::secondsToTicks(float seconds)
{
    double ticksPerSecond = (beatsPerMinute * ticksPerBeat) / 60.0;
    double ticks = ticksPerSecond * seconds;
    return (int) ticks;
}

void MidiSequencePlayer::loadSingleTrack(const MidiTrack & track, double ticksPerBeat, double beatsPerMinute)
{
    reset();

    this->ticksPerBeat = ticksPerBeat;
    this->beatsPerMinute = float(beatsPerMinute);
    msPerTick = 60000.0 / beatsPerMinute / ticksPerBeat;

    double localElapsedTicks = 0;

    // Events in track
    for (auto m : track)
    {
        localElapsedTicks += m->tick;
        double deltaTimestampInSeconds = ticksToSeconds( int(localElapsedTicks) );
        if (m->m->getMessageType() == MessageType::NOTE_ON) addTimestampedEvent(0, deltaTimestampInSeconds, m); // already checks if non-meta message
    }
}

void MidiSequencePlayer::loadMultipleTracks(const std::vector<MidiTrack> & tracks, double ticksPerBeat, double beatsPerMinute)
{
    // Unimplemented
}

void MidiSequencePlayer::start()
{
    if (sequencerThread.joinable()) 
        sequencerThread.join();

    shouldSequence = true;
    sequencerThread = std::thread(&MidiSequencePlayer::run, this);

#if defined(MM_PLATFORM_WINDOWS)
    HANDLE threadHandle = sequencerThread.native_handle();
    int err = (int)SetThreadPriority(threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
    if (err == 0)
    {
        std::cerr << "SetThreadPriority() failed: " << GetLastError() << std::endl;
    }

    err = (int)SetThreadAffinityMask(threadHandle, 1);
    if (err == 0)
    {
        std::cerr<< "SetThreadAffinityMask() failed: " << GetLastError() << std::endl;
    }
#endif
    
    //sequencerThread.detach();

    if (startedEvent)
        startedEvent();
}
    
void MidiSequencePlayer::run()
{
    double lastTime = 0;
    size_t eventCursor = 0;

    PlatformTimer timer;
    timer.start();

    while (eventCursor < eventList.size())
    {
        auto outputMsg = eventList[eventCursor];
        
        //std::cout << "Delta: " << lastTime - outputMsg.timestamp << std::endl;

        while((timer.running_time_s()) <= (outputMsg.timestamp))
        {
            continue;
        }

        output.send(*outputMsg.msg);

        if (shouldSequence == false) 
            break;

        lastTime = outputMsg.timestamp;
        eventCursor++;
    }

    timer.stop(); 

    if (loop && shouldSequence == true) 
        run();
        
    if (stoppedEvent)
        stoppedEvent();
}

void MidiSequencePlayer::stop()
{
    shouldSequence = false;
}
    
void MidiSequencePlayer::addTimestampedEvent(int track, double when, std::shared_ptr<TrackEvent> ev)
{
    if (ev->m->isMetaEvent() == false)
    {
        eventList.push_back(MidiPlayerEvent(when, ev->m, track));
    }
}

float MidiSequencePlayer::length() const
{
    return playTimeSeconds;
}

void MidiSequencePlayer::setLooping(bool newState)
{
    loop = newState;
}

void MidiSequencePlayer::reset()
{
    eventList.clear();
    eventCursor = 0;
    startTime = 0;
    playTimeSeconds = 0;
}
