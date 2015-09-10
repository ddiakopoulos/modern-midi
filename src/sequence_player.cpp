#pragma comment(user, "license")

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


void MidiSequencePlayer::loadSequence(const std::vector<MidiTrack> & tracks)
{
	reset();

	ticksPerBeat = 480.0; //internalTracks.ticksPerBeat > 0 ? internalSequence.ticksPerBeat : 100.0f;
	beatsPerMinute = 100; //internalSequence.startingTempo > 0 ? internalSequence.startingTempo : 120.0f;
	msPerTick = 60000.0 / beatsPerMinute / ticksPerBeat;

	int trackIdx = 0;

	std::cout << "Track Size: " << tracks.size() << std::endl;

	for (const auto t : tracks)
	{
		double localElapsedTicks = 0;

		// Events in track
		for (auto m : t)
		{
			localElapsedTicks += m->tick;
			double deltaTimestampInSeconds = ticksToSeconds(localElapsedTicks);
			if (m->m->getMessageType() == (uint8_t) MessageType::NOTE_ON) addTimestampedEvent(trackIdx, deltaTimestampInSeconds, m); // already checks if non-meta message
		}

		//std::cout << "Track Idx: " << trackIdx << std::endl;
		//std::cout << "Local Elapsed Ticks " << localElapsedTicks << std::endl;
		trackIdx++;
	}

	std::cout << "[New Sequence] Event List Size: " << eventList.size() << std::endl;
}

void MidiSequencePlayer::start()
{
	if (sequencerThread.joinable()) 
		sequencerThread.join();

	shouldSequence = true;
	sequencerThread = std::thread(&MidiSequencePlayer::run, this);

#if defined(MM_PLATFORM_WINDOWS)
	HANDLE threadHandle = sequencerThread.native_handle();
	auto err = SetThreadPriority(threadHandle, THREAD_PRIORITY_TIME_CRITICAL);
	if (err == 0)
	{
		std::cerr << "SetThreadPriority() failed: " << GetLastError() << std::endl;
	}

	err = SetThreadAffinityMask(threadHandle, 1);
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

	int lastTrack = 0;
	while (eventCursor < eventList.size())
	{
		auto outputMsg = eventList[eventCursor];

		// Pause between letters (new idx)
		if (outputMsg.trackIdx != lastTrack)
		{
			lastTrack = outputMsg.trackIdx;
			std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			timer.start();
		}

		//std::cout << "Delta: " << lastTime - outputMsg.timestamp << std::endl;

		while((timer.running_time_s()) <= (outputMsg.timestamp))
		{
			continue;
			// Spinny spin spin.
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
	
void MidiSequencePlayer::addTimestampedEvent(int track, double now, std::shared_ptr<TrackEvent> ev)
{
	if (ev->m->isMetaEvent() == false)
	{
		eventList.push_back(MidiPlayerEvent(now, ev->m, track));
	}
}

float MidiSequencePlayer::length() const
{
	return playTimeSeconds;
}
