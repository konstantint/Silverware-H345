// Note & music-generation utilities.
//
// License: MIT
// Author: konstantint

#pragma once
#include "../Common/motors.h"

enum NoteFrequency
	: uint16_t {
		SILENCE = 0,
	C0 = 16,
	C0_SHARP = 17,
	D0 = 18,
	D0_SHARP = 19,
	E0 = 21,
	F0 = 22,
	F0_SHARP = 23,
	G0 = 24,
	G0_SHARP = 26,
	A0 = 28,
	A0_SHARP = 29,
	B0 = 31,
	C1 = 33,
	C1_SHARP = 35,
	D1 = 37,
	D1_SHARP = 39,
	E1 = 41,
	F1 = 44,
	F1_SHARP = 46,
	G1 = 49,
	G1_SHARP = 52,
	A1 = 55,
	A1_SHARP = 58,
	B1 = 62,
	C2 = 65,
	C2_SHARP = 69,
	D2 = 73,
	D2_SHARP = 78,
	E2 = 82,
	F2 = 87,
	F2_SHARP = 92,
	G2 = 98,
	G2_SHARP = 104,
	A2 = 110,
	A2_SHARP = 117,
	B2 = 123,
	C3 = 131,
	C3_SHARP = 139,
	D3 = 147,
	D3_SHARP = 156,
	E3 = 165,
	F3 = 175,
	F3_SHARP = 185,
	G3 = 196,
	G3_SHARP = 208,
	A3 = 220,
	A3_SHARP = 233,
	B3 = 247,
	C4 = 262,
	C4_SHARP = 277,
	D4 = 294,
	D4_SHARP = 311,
	E4 = 330,
	F4 = 349,
	F4_SHARP = 370,
	G4 = 392,
	G4_SHARP = 415,
	A4 = 440,
	A4_SHARP = 466,
	B4 = 494,
	C5 = 523,
	C5_SHARP = 554,
	D5 = 587,
	D5_SHARP = 622,
	E5 = 659,
	F5 = 698,
	F5_SHARP = 740,
	G5 = 784,
	G5_SHARP = 831,
	A5 = 880,
	A5_SHARP = 932,
	B5 = 988,
	C6 = 1046,
	C6_SHARP = 1109,
	D6 = 1175,
	D6_SHARP = 1245,
	E6 = 1319,
	F6 = 1397,
	F6_SHARP = 1480,
	G6 = 1568,
	G6_SHARP = 1661,
	A6 = 1760,
	A6_SHARP = 1865,
	B6 = 1976,
	C7 = 2093,
	C7_SHARP = 2217,
	D7 = 2349,
	D7_SHARP = 2489,
	E7 = 2637,
	F7 = 2794,
	F7_SHARP = 2960,
	G7 = 3136,
	G7_SHARP = 3322,
	A7 = 3520,
	A7_SHARP = 3729,
	B7 = 3951,
	C8 = 4186,
	C8_SHARP = 4435,
	D8 = 4699,
	D8_SHARP = 4978,
	E8 = 5274,
	F8 = 5588,
	F8_SHARP = 5920,
	G8 = 6272,
	G8_SHARP = 6645,
	A8 = 7040,
	A8_SHARP = 7459,
	B8 = 7902
};

enum NoteLength
	: char {
		WHOLE = 16 * 3,
	HALF = 8 * 3,
	QUARTER = 4 * 3,
	QUARTER_DOT = 6 * 3,
	EIGHTH = 2 * 3,
	EIGHTH_DOT = 3 * 3,
	DOUBLETRIPLET = 8,
	TRIPLET = 4,
	SIXTEENTH = 1 * 3
};

enum NoteAction
	: char {
		SIMPLE, // Sound + a 10ms pause afterwards.
	LEGATO,	// Sound for the full duration
	LEGATO_1, // Legato for the lowest note, everything else simple
	LEGATO_2, // Legato for the two lowest notes, everything else simple
	LEGATO_3,
};

enum Tempo
	: char {
		GRAVE = 40,
	LARGO = 50,
	LENTO = 60,
	ADAGIO = 70,
	ADAGIETTO = 80,
	ANDANTE = 90,
	MODERATO = 100,
	ALLEGRETTO = 110,
	ALLEGRO_MODERATO = 120,
	ALLEGRO = 130,
	VIVACE = 160,
	PRESTO = 180
};

// Extra value to denote all motors
constexpr static QuadcopterMotorId ALL_FOUR = (QuadcopterMotorId) (-1);

template<typename Motors> class SoundMaker {
	Motors& _motors;

public:
	SoundMaker(Motors& motors) :
			_motors(motors) {
	}
	;

	void sound(NoteFrequency note, QuadcopterMotorId motor_id = ALL_FOUR,
			float power = 0.01) {
		if (note == SILENCE) {
			if (motor_id == ALL_FOUR)
				for (auto i : ALL_MOTORS)
					_motors.set_power(i, 0.0);
			else
				_motors.set_power(motor_id, 0.0);
		} else {
			if (motor_id == ALL_FOUR)
				for (auto i : ALL_MOTORS)
					_motors.set_power(i, note, power);
			else
				_motors.set_power(motor_id, note, power);
		}
	}

	void chord(NoteFrequency note1, NoteFrequency note2, NoteFrequency note3,
			NoteFrequency note4, float power = 0.01) {
		sound(note1, BACK_RIGHT, power);
		sound(note2, FRONT_RIGHT, power);
		sound(note3, BACK_LEFT, power);
		sound(note4, FRONT_LEFT, power);
	}
};

template<typename Sound, typename Clock> class MusicMaker {
private:
	Sound _snd;
	Clock _clock;
	uint32_t _coef;
public:
	MusicMaker(Sound& sound, Clock& clock, Tempo tempo = MODERATO) :
			_snd(sound), _clock(clock), _coef(60000000 / (tempo * QUARTER)) // duration_in_ms = duration_coef * (1 minute / bpm / coef_per_beat)
	{
	}

	// Keep the sound for the duration - 10ms, then 10ms silence.
	void play(NoteLength duration, NoteFrequency note1, NoteFrequency note2,
			NoteFrequency note3, NoteFrequency note4,
			NoteAction action = SIMPLE, float power = 0.01) {
		_snd.chord(note1, note2, note3, note4, power);
		_clock.delay_us(duration * _coef - 30000);
		if (action == SIMPLE)
			_snd.sound(SILENCE);
		else if (action == LEGATO_1)
			_snd.chord(note1, SILENCE, SILENCE, SILENCE, power);
		else if (action == LEGATO_2)
			_snd.chord(note1, note2, SILENCE, SILENCE, power);
		else if (action == LEGATO_3)
			_snd.chord(note1, note2, note3, SILENCE, power);
		_clock.delay_us(30000);
		_snd.sound(SILENCE);
	}

	inline void play(NoteLength duration, NoteFrequency note1,
			NoteFrequency note2, NoteAction action = SIMPLE,
			float power = 0.1) {
		play(duration, note1, note1, note2, note2);
	}

	inline void play(NoteLength duration, NoteFrequency note1,
			NoteAction action = SIMPLE, float power = 0.1) {
		play(duration, note1, note1, note1, note1);
	}

	// No notes = silence
	inline void play(NoteLength duration) {
		_snd.sound(SILENCE);
		_clock.delay_us(duration * _coef);
	}
};
