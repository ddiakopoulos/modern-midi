#pragma comment(user, "license")

#pragma once

#ifndef METAMIDI_SEQUENCE_PLAYER_H
#define METAMIDI_SEQUENCE_PLAYER_H

#include "metamidi.h"
#include "concurrent_queue.h"
#include "midi_message.h"
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

		ConcurrentQueue<MidiPlayerEvent> eventQueue;

		std::vector<MidiPlayerEvent> eventList; // indexed by track

	private:
		
		void preprocessSequence();

		void run();

		void addTimestampedEvent(std::vector<MidiPlayerEvent> & list, int track, double now, MidiTrackEvent * ev);

		float beatsPerMinute;
		double ticksPerBeat;
		double msPerTick;
		
		float playTimeSeconds = 0;
		float startTime = 0;
		int eventCursor = 0;

		std::thread sequencerThread;
		std::atomic<bool> shouldSequence;
		bool loop = false;

		MidiSequence internalSequence;
	};

} // mm

#endif
