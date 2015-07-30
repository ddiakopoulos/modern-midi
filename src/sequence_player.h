#pragma once

#ifndef METAMIDI_SEQUENCE_PLAYER_H
#define METAMIDI_SEQUENCE_PLAYER_H

#include "metamidi.h"
#include "concurrent_queue.h"
#include "midi_structs.h"
#include "sequence.h"

#include <functional>
#include <thread>
#include <chrono>
#include <atomic>

namespace mm 
{
    class MidiSequencePlayer 
    {

	public:

        MidiSequencePlayer();
        ~MidiSequencePlayer();
        
		void loadSequence(MidiSequence sequence);
        void start();
		void stop();

		void setLooping(bool newState)
		{
			loop = newState;
		}

        float length() const; // length of the contained song in seconds
        
        double ticksToSeconds(double ticks)
        {
            double beats = ticks / ticksPerBeat;
            double seconds = beats / (beatsPerMinute / 60.0);
            return seconds;
        }
        
        int secondsToTicks(float seconds)
        {
            double ticksPerSecond = (beatsPerMinute * ticksPerBeat) / 60.0;
            double ticks = ticksPerSecond * seconds;
            return (int) ticks;
        }

        std::function<void ()> startedEvent;
		std::function<void ()> stoppedEvent;

		ConcurrentQueue<MidiRtEvent> eventQueue;
		std::vector<MidiRtEvent> eventList; // indexed by track

	private:
		
		void preprocessSequence();

		void run();

        void addTimestampedEvent(std::vector<MidiRtEvent> & list, int track, double now, MidiEvent * ev);

        
        float beatsPerMinute;
        double ticksPerBeat;
		double msPerTick;
        
		float playTimeSeconds = 0;
        float startTime = 0;
        int eventCursor = 0;

		std::thread sequencerThread;
		std::atomic<bool> shouldSequence = false;
		bool loop = false;

		MidiSequence internalSequence;
    };

} // mm

#endif
