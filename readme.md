# ModernMIDI

It's got templates! And c++11! 

### ToDo
- [X] build message factories
- [X] rename MidiRtMessage + fix
- [X] cross platform thread priority 
- [X] cross-platform timer
- [X] Flesh out MidiFile struct
- [X] Convert Nick's midi parster to new message type
- [X] Nuke events
- [X] Initial file writing support
- [ ] MidiFile sanity checks before writing (1 track, enough events, etc)
- [ ] Sanity checks while reading (see todo list embedded in file)
- [ ] Test multiple tracks (reading + writing)
- [ ] Midi timing utilities
- [ ] Sort tracks (out of order timed events)
- [ ] Write all common meta events
- [ ] Link note pairs (on/off)
- [ ] Refactor scales/chords
- [ ] Tempo/BPM/ticks/timing (absolute vs relative)
- [ ] Take a look at mem mgmt
- [ ] Time map => ticks relative to overall file with several tempo events

### Longer Term
- [ ] Other helpful concepts, like tuning systems
- [ ] Controller remapping (keyboards, other midi devices)
- [ ] Virtual midi driver for Windows
- [ ] MIDI clock source/generator
- [ ] SMPTE support
- [ ] Better SysEx test 