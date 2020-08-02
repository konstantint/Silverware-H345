// Main entry point
//
// License: MIT
// Author: konstantint
// Based on the work of silverx and other contributors to the Silverware project.

#include "Apps/goldenware.h"
#include "Control/basic_acro.h"
#include "Hardware/Platforms/JJRC_H345/board.h"

JjrcH345 board;
GoldenwareApp<JjrcH345, BasicAcro<JjrcH345>> app(board);

int main() {
	return app.main();
}
