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
	//@tofix - set these params from midi track
	ticksPerBeat = 480.0; //internalTracks.ticksPerBeat > 0 ? internalSequence.ticksPerBeat : 100.0f;
	beatsPerMinute = 100; //internalSequence.startingTempo > 0 ? internalSequence.startingTempo : 120.0f;
	msPerTick = 60000.0 / beatsPerMinute / ticksPerBeat;

	// Not used right now 
	//double totalSequenceTicks = internalSequence.getEndTime();
	//std::cout << "Total Sequence Ticks: " << totalSequenceTicks << std::endl;
	//playTimeSeconds = float((totalSequenceTicks * msPerTick) / 1000.0);
	//std::cout << "Play time in seconds " << playTimeSeconds << std::endl;

	//@todo: sanity check tracks
	
	/*
	// Look for tempo event, assume we only have one
	for (const auto track : internalSequence.tracks)
	{
		for (const auto mEvt : track)
		{
			if (mEvt.eventType == MIDI_EventSetTempo)
			{
				SetTempoEvent * ste = (SetTempoEvent *) mEvt.get();
				beatsPerMinute = float(60000000.0 / double (ste->microsecondsPerBeat));
				msPerTick = 60000.0 / beatsPerMinute /  ticksPerBeat;
			}
			else if (mEvt->eventType == MIDI_EventTimeSignature)
			{
				TimeSignatureEvent * tse = (TimeSignatureEvent *) mEvt.get();
				std::cout << "Numerator: " << tse->numerator; 
			}
		}
	}
	*/

	// Preprocess tracks into flat vector with timestamps
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

		std::cout << "Track Idx: " << trackIdx << std::endl;
		std::cout << "Local Elapsed Ticks " << localElapsedTicks << std::endl;
		trackIdx++;
	}

	std::cout << "Event List Size: " << eventList.size() << std::endl;
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
	sequencerThread.detach();

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

		//std::vector<unsigned char> msg;
		//msg = { (unsigned char) outputMsg.msg.get()->data[0], (unsigned char) outputMsg.msg.get()->data[1], (unsigned char) outputMsg.msg.get()->data[2]};
		//output.outputDevice->sendMessage(&msg);

		output.send(*outputMsg.msg);
		//std::cout << "Cursor: " << eventCursor << "\tTimer: " << timer.running_time_s() << "\t\tTS: " << outputMsg.timestamp << std::endl;

		//

		//if (eventCallback) 
		//	eventCallback(outputMsg);

		if (shouldSequence == false) 
			break;

		//eventQueue.push(outputMsg);

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
