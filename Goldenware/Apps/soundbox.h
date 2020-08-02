// Serial debugging interface
//
// License: MIT
// Author: konstantint

#pragma once
#include <cstdio>

#include "../Sound/melodies.h"
#include "../Sound/music.h"

template<typename Motors, typename Clock> struct SoundBoxApp {
	SoundMaker<Motors> sound;
	Clock clock;

	inline SoundBoxApp(Motors& motors, Clock& clock_) :
			sound(motors), clock(clock_) {
	}

	inline int main() {
		uint8_t ch;
		printf(
				"Welcome to the music app.\nPress keys to make sounds. Capital Q to quit.\n");

		while (true) {
			scanf("%c", &ch);

			switch (ch) {
			case 'Q':
				printf("Leaving SoundBox...\n");
				return 0;
			case 'z':
				cadence();
				break;
			case 'a':
				note(C5);
				break;
			case 'w':
				note(C5_SHARP);
				break;
			case 's':
				note(D5);
				break;
			case 'e':
				note(D5_SHARP);
				break;
			case 'd':
				note(E5);
				break;
			case 'f':
				note(F5);
				break;
			case 't':
				note(F5_SHARP);
				break;
			case 'g':
				note(G5);
				break;
			case 'y':
				note(G5_SHARP);
				break;
			case 'h':
				note(A6);
				break;
			case 'u':
				note(A6_SHARP);
				break;
			case 'j':
				note(B6);
				break;
			case 'k':
				note(C6);
				break;
			case 'o':
				note(C6_SHARP);
				break;
			case 'l':
				note(D6);
				break;
			case 'm':
				mario();
				break;
			case 'n':
				i_believe_i_can_fly();
				break;
			case 'p':
				note(D6_SHARP);
				break;
			case ';':
				note(E6);
				break;
			default:
				unknown();
			};
		}
		return 0;
	}

	void unknown() {
		printf("This key is not mapped to a sound.\n");
	}

	inline void cadence() {
		Melodies::cadence(sound, clock);
	}

	void note(NoteFrequency note) {
		sound.sound(note);
		clock.delay_us(200000);
		sound.sound(SILENCE);
	}

	inline void mario() {
		Melodies::mario(sound, clock);
	}

	inline void i_believe_i_can_fly() {
		Melodies::i_believe_i_can_fly(sound, clock);
	}

};

