#ifndef TEST_BUILD

#include <iostream>
#include "imaging/bitmap.h"
#include "imaging/bmp-format.h"
#include "imaging/color.h"
#include <cstdint>
#include <fstream>
#include <vector>
#include "midi/midi.h"
#include "shell/command-line-parser.h"
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace imaging;
using namespace colors;
using namespace midi;
using namespace std;
using namespace shell;

void draw_rectangle(Bitmap& bitmap, const Position& top_left, const uint32_t& width, const uint32_t& height, const Color& color) {
	for (uint32_t i = top_left.x; i < top_left.x + width; i++) {
		for (uint32_t j = top_left.y; j < top_left.y + height; j++) {
			bitmap[Position(i, j)] = color;
		}
	}
}

int get_width(const vector<NOTE> notes) {
	int width = 0;
	for (NOTE note : notes) {
		if (value(note.start + note.duration) > width) {
			width = value(note.start + note.duration);
		}
	}
	return width;
}

int get_lowest_note(vector<NOTE> notes) {
	int lowest = 127;
	for (NOTE note : notes) {
		if (value(note.note_number) < lowest) {
			lowest = value(note.note_number);
		}
	}
	return lowest;
}

int get_highest_note(vector<NOTE> notes) {
	int highest = 0;
	for (NOTE note : notes) {
		if (value(note.note_number) > highest) {
			highest = value(note.note_number);
		}
	}
	return highest;
}

int get_note_height_difference(vector<NOTE> notes) {
	return (get_highest_note(notes) - get_lowest_note(notes) + 1);
}

int main(int argn, char* argv[]) {
	/*
	//dot
	Bitmap bitmap(500, 500);
	bitmap[Position(250, 250)] = Color(1, 0, 0);
	save_as_bmp("C:\\Users\\Ruben Claes\\Desktop\\bmp\\bitmap.bmp", bitmap);

	//rectangle
	Bitmap bitmap0(500, 500);
	draw_rectangle(bitmap0, Position(225, 275), Position(275, 225), Color(1, 1, 1));
	save_as_bmp("C:\\Users\\Ruben Claes\\Desktop\\bmp\\bitmap0.bmp", bitmap0);*/

	//full file
	uint32_t frame_width = 0;
	uint32_t step = 1;
	uint32_t scale = 2;
	uint32_t note_height = 16;
	string input_file = "C:\\Users\\Ruben Claes\\Desktop\\Ucll\\2_S2\\Programmeren voor Multimedia\\Opdracht\\midi-files\\bohemian.mid";
	string output_file = "C:\\Users\\Ruben Claes\\Desktop\\Ucll\\2_S2\\Programmeren voor Multimedia\\Opdracht\\output\\f%d.bmp";

	//command definition
	CommandLineParser cmd_parser;
	cmd_parser.add_argument(string("-w"), &frame_width);
	cmd_parser.add_argument(string("-d"), &step);
	cmd_parser.add_argument(string("-s"), &scale);
	cmd_parser.add_argument(string("-h"), &note_height);
	cmd_parser.process(vector<string>(argv + 1, argv + argn));

	/*
	cout << "positional arguments:" << std::endl;
	cout << "---" << std::endl;

	for (auto positional : cmd_parser.positional_arguments()) {
		cout << positional << endl;
	}
	cout << "---" << endl;
	*/

	vector<string> positional_args = cmd_parser.positional_arguments();

	if (positional_args.size() >= 1) {
		input_file = positional_args[0];

		if (positional_args.size() >= 2) {
			output_file = positional_args[1];
		}
	}

	ifstream stream(input_file, ifstream::binary);
	vector<NOTE> notes = read_notes(stream);
	uint32_t width = get_width(notes) * (scale / 100.0);
	uint32_t height = get_note_height_difference(notes) * note_height;

	if (frame_width == 0) {
		frame_width = width;
	}

	int low = get_lowest_note(notes);
	int high = get_highest_note(notes);
	cout << "bitmap size: " << width << " x " << get_note_height_difference(notes) * note_height << endl;

	//draw frames
	Bitmap bitmap1(width, 128 * note_height);
	for (int i = 0; i <= 127; i++) {
		for (NOTE note : notes) {
			if (note.note_number == NoteNumber(i)) {
				draw_rectangle(
					bitmap1,
					Position(value(note.start) * (scale / 100.0), (127 - i) * note_height),
					value(note.duration) * (scale / 100.0),
					note_height,
					Color((value(note.instrument) % 7) / 7.0, (value(note.instrument) % 17) / 17.0, (value(note.instrument) % 37) / 37.0));
			}
		}
	}

	//cropping
	bitmap1 = *bitmap1.slice(0, note_height * (128 - high), width, get_note_height_difference(notes) * note_height).get();
	
	// save
	for (int i = 0; i <= (width - frame_width); i += step) {
		Bitmap temp = *bitmap1.slice(i, 0, frame_width, (high - low + 1) * note_height).get();
		stringstream frame_nr;
		frame_nr << setfill('0') << setw(5) << (i / step);

		string out = output_file;
		save_as_bmp(out.replace(out.find("%d"), 2, frame_nr.str()), temp);
		cout << "frame " << (i / step) << " created" << endl;
	}
	cout << "finished" << endl;
}

#endif
