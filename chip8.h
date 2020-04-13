#pragma once
#include <string_view>
#include <array>
#include <bitset>
#include <thread>

//void timer_setter(chip8* chip);

class chip8 {
public:
	enum REGISTER : int {
		V0 = 0,
		V1, V2, V3, V4, V5, V6, V7, V8, V9, VA, VB, VC, VD, VE, VF
	};

	enum BUTTON : int {
		KEY_ONE = 0,
		KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE, KEY_TEN, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F
	};

	std::array<uint8_t, 4096> ram;  
	//should be set of bits
	//std::bitset<2048> graphics; // 64x32 bicolor
	std::array<uint8_t, 16> registers;
	std::array<uint16_t, 16> stack;

	uint16_t I;
	uint8_t DT; // decremented 60 times per second
	uint8_t ST; // decremented 60 times per second
	uint16_t PC;
	uint8_t  SP;
	bool	sound_played;

	std::thread timer;

	void fetch_opcode(uint16_t byte);
public:
	bool	can_run;
	bool should_draw;
	chip8();

	std::array<int8_t, 16> keys;
	std::array<uint8_t, 2048> graphics;

	void init();
	void load(std::string_view name);
	void cycle();
	void draw();
};

