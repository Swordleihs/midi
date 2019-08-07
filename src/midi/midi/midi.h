#pragma once
#include <cstdint>
#include "io/read.h"
#include "io/endianness.h"
#include <string>
#include "primitives.h"
#include "io/vli.h"
#include <iostream>

namespace midi {

	struct CHUNK_HEADER {
		char id[4];
		uint32_t size;
	};

	void read_chunk_header(std::istream& s, CHUNK_HEADER* c);
	std::string header_id(const CHUNK_HEADER c);

#pragma pack(push, 1)
	struct MTHD {
		CHUNK_HEADER header;
		uint16_t type;
		uint16_t ntracks;
		uint16_t division;
	};
#pragma pack(pop)

	void read_mthd(std::istream& s, MTHD* m);

	bool is_sysex_event(uint8_t byte);
	bool is_meta_event(uint8_t byte);
	bool is_midi_event(uint8_t byte);
	bool is_running_status(uint8_t byte);

	uint8_t extract_midi_event_type(uint8_t status);
	Channel extract_midi_event_channel(uint8_t status);

	bool is_note_off(uint8_t status);
	bool is_note_on(uint8_t status);
	bool is_polyphonic_key_pressure(uint8_t status);
	bool is_control_change(uint8_t status);
	bool is_program_change(uint8_t status);
	bool is_channel_pressure(uint8_t status);
	bool is_pitch_wheel_change(uint8_t status);

	class EventReceiver {
	public:
		virtual void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) = 0;
		virtual void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) = 0;
		virtual void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) = 0;
		virtual void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) = 0;
		virtual void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) = 0;
		virtual void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) = 0;
		virtual void program_change(Duration dt, Channel channel, Instrument program) = 0;
		virtual void channel_pressure(Duration dt, Channel channel, uint8_t pressure) = 0;
		virtual void pitch_wheel_change(Duration dt, Channel channel, uint16_t wheel_position) = 0;
	};

	void read_mtrk(std::istream& s, EventReceiver& e);

	struct NOTE {
		NoteNumber note_number;
		Time start;
		Duration duration;
		uint8_t velocity;
		Instrument instrument;

		NOTE (NoteNumber n, Time s, Duration d, uint8_t v, Instrument i){
			note_number = n;
			start = s;
			duration = d;
			velocity = v;
			instrument = i;
		}
	};

	bool operator == (NOTE note0, NOTE note1);
	bool operator != (NOTE note0, NOTE note1);
	std::ostream& operator << (std::ostream& ostream, NOTE note);

	struct ChannelNoteCollector : EventReceiver {
	public:
		Channel channel;
		std::function<void(const NOTE&)> note_receiver;
		Time current_time = Time(0);
		Time starttime_notes[128];
		uint16_t velocity_notes[128];
		Instrument instrument;

		ChannelNoteCollector(Channel channel0, std::function<void(const NOTE&)> note_receiver0) {
			channel = channel0;
			note_receiver = note_receiver0;

			for (uint16_t& velocity : velocity_notes) {
				velocity = 0;
			}
		}

		virtual void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
		virtual void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
		virtual void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
		virtual void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
		virtual void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) override;
		virtual void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) override;
		virtual void program_change(Duration dt, Channel channel, Instrument program) override;
		virtual void channel_pressure(Duration dt, Channel channel, uint8_t pressure) override;
		virtual void pitch_wheel_change(Duration dt, Channel channel, uint16_t wheel_position) override;
	};

	class EventMulticaster : EventReceiver {
	public:
		std::vector<std::shared_ptr<EventReceiver>> channel_caster;
		Time currentTime = Time(0);
		Time startTimeNotes[128];
		uint8_t velocityNotes[128];
		Instrument instrument;

		EventMulticaster(){}

		EventMulticaster(std::vector<std::shared_ptr<EventReceiver>> c) {
			channel_caster = c;
		}

		virtual void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
		virtual void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
		virtual void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
		virtual void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
		virtual void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) override;
		virtual void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) override;
		virtual void program_change(Duration dt, Channel channel, Instrument program) override;
		virtual void channel_pressure(Duration dt, Channel channel, uint8_t pressure) override;
		virtual void pitch_wheel_change(Duration dt, Channel channel, uint16_t wheel_position) override;
	};

	std::unique_ptr<uint8_t[]> copy(std::unique_ptr<uint8_t[]>& to_copy, uint64_t data_size);

	class NoteCollector : public EventReceiver {
	public:
		EventMulticaster event_multicaster;
		std::function<void(const NOTE&)> note_receiver;

		static std::vector<std::shared_ptr<EventReceiver>> create_list(std::function<void(const NOTE&)> receiver) {
			std::vector<std::shared_ptr<EventReceiver>> receivers;
			for (int channel = 0; channel < 16; channel++) {
				auto ptr = std::make_shared<ChannelNoteCollector> (Channel(channel), receiver);
				receivers.push_back(ptr);
			}
			return receivers;
		}
		
		NoteCollector(std::function<void(const NOTE&)> r) {
			note_receiver = r;
			event_multicaster = create_list(note_receiver);
		}

		virtual void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
		virtual void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
		virtual void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
		virtual void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
		virtual void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) override;
		virtual void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) override;
		virtual void program_change(Duration dt, Channel channel, Instrument program) override;
		virtual void channel_pressure(Duration dt, Channel channel, uint8_t pressure) override;
		virtual void pitch_wheel_change(Duration dt, Channel channel, uint16_t wheel_position) override;
	};

	std::vector<NOTE> read_notes(std::istream& s);
}