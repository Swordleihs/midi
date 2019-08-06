#include "midi.h"

namespace midi {

	void read_chunk_header(std::istream& s, CHUNK_HEADER* c) {
		io::read_to(s, c);
		io::switch_endianness(&((*c).size));
	}

	std::string header_id(const CHUNK_HEADER c) {
		std::string s = "";
		for (char c : c.id) {
			s += c;
		}
		return s;
	}

	void read_mthd(std::istream& s, MTHD* m) {
		io::read_to(s, m);
		io::switch_endianness(&(((*m).header).size));
		io::switch_endianness(&((*m).type));
		io::switch_endianness(&((*m).ntracks));
		io::switch_endianness(&((*m).division));
	}

	bool is_sysex_event(uint8_t byte) {
		return (byte == 0xF0) || (byte == 0xF7);
	}

	bool is_meta_event(uint8_t byte) {
		return byte == 0xFF;
	}

	bool is_midi_event(uint8_t byte) {
		return (byte >= 0x80) && (byte <= 0xEF);
	}

	bool is_running_status(uint8_t byte) {
		return byte < 0x80;
	}

	uint8_t extract_midi_event_type(uint8_t status) {
		return status >> 4;
	}

	Channel extract_midi_event_channel(uint8_t status) {
		status &= 0x0F;
		return Channel(status);
	}

	bool is_note_off(uint8_t status) {
		return status == 0x08;
	}

	bool is_note_on(uint8_t status) {
		return status == 0x09;
	}

	bool is_polyphonic_key_pressure(uint8_t status) {
		return status == 0x0A;
	}

	bool is_control_change(uint8_t status) {
		return status == 0x0B;
	}

	bool is_program_change(uint8_t status) {
		return status == 0x0C;
	}

	bool is_channel_pressure(uint8_t status) {
		return status == 0x0D;
	}

	bool is_pitch_wheel_change(uint8_t status) {
		return status == 0x0E;
	}

	void read_mtrk(std::istream& s, EventReceiver& event_receiver) {
		CHUNK_HEADER header;
		read_chunk_header(s, &header);

		bool has_next = true;
		uint8_t running_identifier;

		while (has_next) {
			Duration duration(io::read_variable_length_integer(s));
			uint8_t identifier = io::read<uint8_t>(s);

			if (is_running_status(identifier)){
				s.putback(identifier);
				identifier = running_identifier;
			}
			else {
				running_identifier = identifier;
			}

			if (is_meta_event(identifier)) {
				uint8_t type = io::read<uint8_t>(s);

				if (type == 0x2F) {
					has_next = false;
				}

				uint64_t data_size = io::read_variable_length_integer(s);
				std::unique_ptr<uint8_t[]> data = io::read_array<uint8_t>(s, data_size);

				event_receiver.meta(duration, type, std::move(data), data_size);
			}

			else if (is_sysex_event(identifier)) {
				uint64_t data_size = io::read_variable_length_integer(s);
				std::unique_ptr<uint8_t[]> data = io::read_array<uint8_t>(s, data_size);

				event_receiver.sysex(duration, std::move(data), data_size);
			}

			else if (is_midi_event(identifier)) {
				uint8_t type = extract_midi_event_type(identifier);
				Channel channel(extract_midi_event_channel(identifier));

				if (is_note_off(type)) {
					NoteNumber note(io::read<uint8_t>(s));
					uint8_t velocity = io::read<uint8_t>(s);

					event_receiver.note_off(duration, channel, note, velocity);
				}

				else if (is_note_on(type)) {
					NoteNumber note(io::read<uint8_t>(s));
					uint8_t velocity = io::read<uint8_t>(s);

					event_receiver.note_on(duration, channel, note, velocity);
				}

				else if (is_polyphonic_key_pressure(type)) {
					NoteNumber note(io::read<uint8_t>(s));
					uint8_t pressure = io::read<uint8_t>(s);

					event_receiver.polyphonic_key_pressure(duration, channel, note, pressure);
				}

				else if (is_control_change(type)) {
					uint8_t controller = io::read<uint8_t>(s);
					uint8_t pressure = io::read<uint8_t>(s);

					event_receiver.control_change(duration, channel, controller, pressure);
				}

				else if (is_program_change(type)) {
					Instrument program(io::read<uint8_t>(s));

					event_receiver.program_change(duration, channel, program);
				}

				else if (is_channel_pressure(type)) {
					uint8_t pressure = io::read<uint8_t>(s);

					event_receiver.channel_pressure(duration, channel, pressure);
				}

				else if (is_pitch_wheel_change(type)) {
					uint16_t lower = io::read<uint8_t>(s);
					uint16_t upper = (io::read<uint8_t>(s) << 7);
					uint16_t position = (upper | lower);

					event_receiver.pitch_wheel_change(duration, channel, position);
				}

			}
		}
	}

	bool operator == (NOTE note0, NOTE note1) {
		return (note0.note_number == note1.note_number)
			&& (note0.start == note1.start)
			&& (note0.duration == note1.duration)
			&& (note0.velocity == note1.velocity)
			&& (note0.instrument == note1.instrument);
	}

	bool operator != (NOTE note0, NOTE note1) {
		return !(note0 == note1);
	}

	std::ostream& operator << (std::ostream& ostream, NOTE note) {
		ostream << "Note(number=" << note.note_number
			<< ",start=" << note.start
			<< ",duration=" << note.duration
			<< ",instrument=" << note.instrument
			<< ")";

		return ostream;
	}

}