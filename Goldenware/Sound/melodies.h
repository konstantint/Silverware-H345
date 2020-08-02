// Melody-producing scripts
//
// License: MIT
// Author: konstantint

#pragma once
#include "music.h"

namespace Melodies {

template<typename Sound, typename Clock>
void mario(Sound& sound, Clock& clock) {
	// https://musescore.com/user/27687306/scores/4913846
	MusicMaker<Sound, Clock> m(sound, clock, PRESTO);
	// -- 1 -- intro
	auto intro = [&]() {
		m.play(EIGHTH, D2, E4);
		m.play(EIGHTH, D2, E4);
		m.play(EIGHTH);
		m.play(EIGHTH, D2, E4);
		m.play(EIGHTH);
		m.play(EIGHTH, C2, C4);
		m.play(EIGHTH, D2, E4);
		m.play(EIGHTH);

		// -- 2 --
		m.play(QUARTER, G2, G4);
		m.play(QUARTER);
		m.play(QUARTER, G1, G3);
		m.play(QUARTER);
	};
	intro();
	// -- 3 -- first phrase
	m.play(QUARTER, G2, C4);
	m.play(EIGHTH);
	m.play(QUARTER, E2, G3);
	m.play(EIGHTH);
	m.play(QUARTER, C2, E3);
	// -- 4
	m.play(EIGHTH);
	m.play(QUARTER, F2, A4);
	m.play(QUARTER, G2, B4);
	m.play(EIGHTH, F2_SHARP, A4_SHARP);
	m.play(QUARTER, F2, A4);
	// -- 5
	m.play(DOUBLETRIPLET, E2, G3);
	m.play(DOUBLETRIPLET, C3, E4);
	m.play(DOUBLETRIPLET, E3, G4);
	m.play(QUARTER, F3, A5);
	m.play(EIGHTH, D3, F4);
	m.play(EIGHTH, E3, G4);
	// -- 6
	m.play(EIGHTH);
	m.play(QUARTER, C3, E4);
	m.play(EIGHTH, A3, C4);
	m.play(EIGHTH, B3, D4);
	m.play(QUARTER, G2, B4);
	m.play(EIGHTH);
	// == 7 -- second phrase, first version
	auto bar7 = [&]() {
		m.play(QUARTER, C2);
		m.play(EIGHTH, SILENCE, G4);
		m.play(EIGHTH, G2, F4_SHARP);
		m.play(EIGHTH, SILENCE, F4);
		m.play(EIGHTH, SILENCE, D4_SHARP, LEGATO);
		m.play(EIGHTH, C3, D4_SHARP);
		m.play(EIGHTH, C3, E4);
	};
	bar7();
	// -- 8
	auto bar8 = [&]() {
		m.play(EIGHTH, F2, SILENCE, LEGATO);
		m.play(EIGHTH, F2, G3_SHARP);
		m.play(EIGHTH, SILENCE, A4);
		m.play(EIGHTH, C3, C4);
		m.play(EIGHTH, C3, SILENCE, LEGATO);
		m.play(EIGHTH, C3, A4);
		m.play(EIGHTH, F2, C4, LEGATO);
		m.play(EIGHTH, F2, D4);
	};
	bar8();
	// -- 9 -- very similar to bar7
	m.play(QUARTER, C2);
	m.play(EIGHTH, SILENCE, G4);
	m.play(EIGHTH, G2, F4_SHARP);
	m.play(EIGHTH, SILENCE, F4);
	m.play(EIGHTH, SILENCE, D4_SHARP, LEGATO);
	m.play(EIGHTH, G2, D4_SHARP);
	m.play(EIGHTH, C3, E4);
	// -- 10
	m.play(EIGHTH);
	m.play(QUARTER, G3, C5);
	m.play(EIGHTH, G3, C5);
	m.play(QUARTER, G3, C5);
	m.play(QUARTER, G2);
	// -- 11,12 -- second phrase, second version repeat of 7 & 8
	bar7();
	bar8();
	// -- 13
	m.play(QUARTER, C2);
	m.play(QUARTER, G2_SHARP, D4_SHARP);
	m.play(EIGHTH);
	m.play(QUARTER, A3_SHARP, D4);
	m.play(EIGHTH);
	// -- 14
	m.play(QUARTER, C3, C4);
	m.play(EIGHTH);
	m.play(EIGHTH, G2);
	m.play(QUARTER, G2);
	m.play(QUARTER, C2);
	// == 15 - third phrase
	auto bar15 = [&]() {
		m.play(EIGHTH, G1_SHARP, C4, LEGATO_2);
		m.play(EIGHTH, G1_SHARP, C4);
		m.play(EIGHTH);
		m.play(EIGHTH, D2_SHARP, C4);
		m.play(EIGHTH);
		m.play(EIGHTH, SILENCE, C4);
		m.play(QUARTER, G2_SHARP, D4);
	};
	bar15();
	// -- 16
	auto bar16 = [&]() {
		m.play(EIGHTH, G2, E4, LEGATO_2);
		m.play(EIGHTH, G2, C4);
		m.play(EIGHTH);
		m.play(EIGHTH, C2, A4);
		m.play(QUARTER, SILENCE, G3, LEGATO);
		m.play(QUARTER, G1, G3);
	};
	bar16();
	// -- 17 (begins like 15)
	m.play(EIGHTH, G1_SHARP, C4, LEGATO_2);
	m.play(EIGHTH, G1_SHARP, C4);
	m.play(EIGHTH);
	m.play(EIGHTH, D2_SHARP, C4);
	m.play(EIGHTH);
	m.play(EIGHTH, SILENCE, C4);
	m.play(EIGHTH, G2_SHARP, D4, LEGATO);
	m.play(EIGHTH, G2_SHARP, E4);
	// -- 18
	m.play(QUARTER, G2);
	m.play(EIGHTH);
	m.play(EIGHTH, C2);
	m.play(QUARTER);
	m.play(QUARTER, G1);
	// -- 19,20 repeat bar 15&16
	bar15();
	bar16();
	// -- end of phrase = intro, go back to phrase 1
	intro(); // We could also simply loop to beginning here.
}

template<typename Sound, typename Clock>
void i_believe_i_can_fly(Sound& sound, Clock& clock) {
	// https://sheetmusic-free.com/download/9794/
	MusicMaker<Sound, Clock> m(sound, clock, LARGO);
//
//		// 1
//		m.play(HALF, G1, D3, E3, C4, LEGATO_1);
//		m.play(QUARTER, G1, SILENCE, E3, E3, LEGATO_1);
//		m.play(QUARTER, G1, SILENCE, G3, G3, LEGATO_1);
//		// 2
//		m.play(QUARTER_DOT, G1, C3, D3, G3_SHARP, LEGATO_1);
//		m.play(EIGHTH, G1, SILENCE, E3, G3, LEGATO_1);
//		m.play(QUARTER, G1, D3, F3, A4, LEGATO_1);
//		m.play(EIGHTH_DOT, G1, SILENCE, C4, C4, LEGATO_1);
//		m.play(SIXTEENTH, G1, SILENCE, C4, C4);
//		// 3
//		m.play(EIGHTH, G1, E3, G3, SILENCE, LEGATO_2);
//		m.play(SIXTEENTH, G1, E3, C4, SILENCE, LEGATO_2);
//		m.play(SIXTEENTH, G1, E3, C4, SILENCE, LEGATO_3);
//		m.play(EIGHTH, G1, E3, C4, SILENCE, LEGATO_2);
//		m.play(SIXTEENTH, G1, E3, D4, SILENCE, LEGATO_2);
//		m.play(SIXTEENTH, G1, D4, E3, SILENCE, LEGATO_2);
//		m.play(EIGHTH, G1, D4, SILENCE, SILENCE, LEGATO_1);
//		m.play(SIXTEENTH, G1, E4, SILENCE, SILENCE, LEGATO_1);
//		m.play(SIXTEENTH, G1, E4, SILENCE, SILENCE, LEGATO_2);
//		m.play(EIGHTH_DOT, G1, E4, SILENCE, SILENCE, LEGATO_1);
//		m.play(SIXTEENTH, G1, C4, SILENCE, SILENCE, LEGATO_1);

	// .. I believe I can
	m.play(EIGHTH, G1, SILENCE, LEGATO_2);
	m.play(SIXTEENTH, G1, E4, LEGATO_2);
	m.play(SIXTEENTH, G1, E4);
	m.play(EIGHTH, E4);
	m.play(SIXTEENTH, D4);
	m.play(SIXTEENTH, C4);
	// .. fly, I believe I can
	m.play(QUARTER_DOT, E3, G3, C4, C2, LEGATO_3);
	m.play(EIGHTH, E3, G3, C4, C2);
	m.play(EIGHTH, C2, LEGATO_1);
	m.play(SIXTEENTH, C2, SILENCE, E4, E4, LEGATO_1);
	m.play(SIXTEENTH, C2, SILENCE, E4, E4, LEGATO_1);
	m.play(EIGHTH, C2, G3, E4, E4, LEGATO_1);
	m.play(SIXTEENTH, C2, SILENCE, D4, D4, LEGATO_1);
	m.play(SIXTEENTH, C2, SILENCE, C4, C4);
	// .. touch the sky. I think about it every
	m.play(EIGHTH, E3, G3, A2, D4, LEGATO_3);
	m.play(SIXTEENTH, E3, G3, A2, C4, LEGATO_3);
	m.play(SIXTEENTH, E3, G3, A2, C4, LEGATO);
	m.play(EIGHTH, E3, G3, C4, A2, LEGATO_3);
	m.play(EIGHTH, A2, G3, C4, E3, LEGATO_1);
	m.play(SIXTEENTH, A2, SILENCE, SILENCE, SILENCE, LEGATO_1);

	m.play(SIXTEENTH, A2, G3, G3, G3, LEGATO_1);
	m.play(SIXTEENTH, A2, E4, E4, E4, LEGATO_1);
	m.play(SIXTEENTH, A2, E4, E4, E4, LEGATO_1);
	m.play(SIXTEENTH, A2, G3, C4, E4, LEGATO_1);
	m.play(SIXTEENTH, A2, E4, E4, E4);
	m.play(SIXTEENTH, G1, E4, E4, E4);
	m.play(SIXTEENTH, A2, E4, E4, E4);

	// .. night and day, spread my wings and
	m.play(EIGHTH, D1, A4, C4, E4, LEGATO_1);
	m.play(SIXTEENTH, D1, F4, F4, F4, LEGATO_1);
	m.play(SIXTEENTH, D1, A4, C4, F4, LEGATO);

	m.play(EIGHTH, A4, C4, F4, D1, LEGATO_3);
	m.play(EIGHTH, D1, A4, C4, F4, LEGATO_1);

	m.play(EIGHTH, D1, SILENCE, SILENCE, SILENCE, LEGATO_1);
	m.play(SIXTEENTH, D1, E4, E4, E4, LEGATO_1);
	m.play(SIXTEENTH, D1, E4, E4, E4, LEGATO_1);
	m.play(EIGHTH, D1, F3, A4, E4, LEGATO_1);
	m.play(EIGHTH, D1, E4, E4, E4);
	// .. fly away
	m.play(EIGHTH, F3, A4, G1, E4, LEGATO_3);
	m.play(SIXTEENTH, F3, A4, G1, D4, LEGATO_3);
	m.play(SIXTEENTH, F3, A4, G1, D4, LEGATO);
	m.play(EIGHTH, F3, A4, D4, G1, LEGATO_3);
	m.play(EIGHTH, G1, A4, D4, F3, LEGATO_1);
	// m.play(SIXTEENTH, G1, SILENCE, SILENCE, SILENCE, LEGATO_1); // Repeat intro note
}

template<typename Sound, typename Clock>
void cadence(Sound& sound, Clock& clock) {
	MusicMaker<Sound, Clock> m(sound, clock, ANDANTE);
	m.play(QUARTER, B4, D5, G5, B5);
	m.play(QUARTER, C5, E5, G5, C6);
}

// Choose a song by number
template<typename Sound, typename Clock>
void song(Sound& sound, Clock& clock, int number) {
	switch (number % 3) {
	case 0: cadence(sound, clock); break;
	case 1: i_believe_i_can_fly(sound, clock); break;
	case 2: mario(sound, clock); break;
	}
}

} // namespace Songs
