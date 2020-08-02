// A debugging app for firmware development
//
// License: MIT
// Author: konstantint

#pragma once
#include <cstdio>
#include "soundbox.h"
#include "../Common/blink.h"
#include "../Common/gyro.h"
#include "../Common/loop.h"
#include "../Common/printf.h"
#include "../Sound/music.h"

template<typename QuadcopterBoard, typename Controller>
struct SerialDebugApp {
	QuadcopterBoard& board;
	Controller& controller;

	float pwr = 0.0;
	int motor = 0;
	int adch = 7;

	inline SerialDebugApp(QuadcopterBoard& board_, Controller& controller_) :
			board(board_), controller(controller_) {

	}

	inline int main() {
		// Welcome blink
		blink(board, 5, 100000);

		uint8_t ch;
		printf(
				"Welcome to the serial debug app.\nPress h for a list of commands\n");

		while (true) {
			printf("Command: \n");
			scanf("%c", &ch);
			printf("%c\n", ch);

			switch (ch) {
			case 'a':
				a();
				break;
			case 'b':
				b();
				break;
			case 'c':
				c();
				break;
			case 'd':
				d();
				break;
			case 'f':
				f();
				break;
			case 'g':
				g();
				break;
			case 'h':
				h();
				break;
			case 'l':
				l();
				break;
			case 'm':
				m();
				break;
			case 'n':
				n();
				break;
			case 'p':
				p();
				break;
			case 'q':
				return 0;
			case 'r':
				r();
				break;
			case 's':
				s();
				break;
			case 't':
				t();
				break;
			case 'x':
				x();
				break;
			case 'y':
				y();
				break;
			default:
				unknown();
			};
		}
		return 0;
	}

	void unknown() {
		printf("Unknown command. Press h for help.\n");
	}

	void h() {
		printf("Supported commands: \n"
				" h: Help\n"
				" a: Accelerometer debug\n"
				" b: Read battery voltage\n"
				" c: Get PLL clock frequency\n"
				" d: Delay 5 seconds\n"
				" f: Gyro calibration\n"
				" g: Gyro readout\n"
				" t: Elapsed time (micros())\n"
				" l: Toggle LED\n"
				" m: Select next motor\n"
				" n: Stop all motors\n"
				" p: Select next power\n"
				" q: Quit\n"
				" r: Receive packet\n"
				" s: Start the soundbox app\n"
				" x: Display PID parameters\n"
				" y: Set current motor to current power\n");
	}

	void a() {
		printf("Accelerometer\n");
		bool res = board.mpu6050.read_all();
		printf("Result: %i\n", res);
		printf("A:");
		printf(board.mpu6050.data().acc);
		printf("\n");
		printf("A(g):");
		printf(board.mpu6050.data().acc_g());
		printf("\n");
		printf("G:");
		printf(board.mpu6050.data().gyro);
		printf("\n");
		printf("G(dps):");
		printf(board.mpu6050.data().gyro_dps());
		printf("\n");
#ifdef MPU6050_USE_TEMPERATURE_SENSOR
		printf("T: %i\n", board.sixaxis.data().temp);
		printf("T(C):"); printf(board.sixaxis.data().temp_c()); printf("\n");
#endif
	}

	void b() {
		printf("Battery voltage: ");
		printf(board.battery.get_voltage());
		printf("\n");
	}

	void c() {
		printf("%i\n", board.clock.get_pll_clock_frequency());
	}

	void d() {
		t();
		board.clock.delay_us(5000000);
		printf("Delay done\n");
		t();
	}

	void f() {
		bool result = controller.gyro.calibrate();
		printf("Calibration success: %i\n", result);
		printf("Saved gyro offset: "), printf(controller.gyro.gyro_offset); printf("\n");
	}

	void g() {
		printf("Calibrated gyro readout: ");
		printf(controller.gyro.read());
		printf("\n");
		printf("Saved gyro offset: "), printf(controller.gyro.gyro_offset); printf("\n");
	}

	void t() {
		printf("Time: %i\n", board.clock.micros());
	}

	void l() {
		board.led.toggle();
	}

	void m() {
		motor = (motor + 1) % 4;
		printf("Current motor: %i", motor);
	}

	void n() {
		for (auto i : ALL_MOTORS)
			board.motors.set_power(i, 0.0);
	}

	void p() {
		pwr += 0.2f;
		if (pwr > 1.0)
			pwr = 0.0;
		printf("Current power: %i", (int) (pwr * 1000));
	}

	void r() {
		board.rx.recv();
		printf(board.rx.data());
	}

	void s() {
		printf("Starting SoundBox...\n");
		SoundBoxApp<decltype(board.motors), decltype(board.clock)> app(
				board.motors, board.clock);
		app.main();
	}

	void x() {
		printf("PID parameters\n");
		for (auto ch : ROLL_PITCH_YAW) {
			printf("Ch %i: Kp=", ch);
			printf(controller.pids[ch].K_p);
			printf(" Ki*dt=");
			printf(controller.pids[ch].K_i_dt);
			printf(" Kd/dt=");
			printf(controller.pids[ch].K_d_div_dt);
			printf("\n");
		}
	}

	void y() {
		board.motors.set_power((QuadcopterMotorId) motor, pwr);
	}
};

