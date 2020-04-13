#include "chip8.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdio>

constexpr int memory_start  = 0x0;
constexpr int program_start = 0x200;
constexpr int last_byte		= 0xFFF;
constexpr int stack_size	= 0x100;
constexpr int sprite_max_size = 0xF;
constexpr int digit_sprite_size = 0x5;

void chip8::fetch_opcode(uint16_t byte) {
	int8_t first_4bits  = (byte & 0xF000) >> 12;
	int8_t second_4bits = (byte & 0x0F00) >> 8;
	int8_t third_4bits  = (byte & 0x00F0) >> 4;
	int8_t fourth_4bits = (byte & 0x000F) >> 0;


	switch (first_4bits) {
	case 0x0:
		switch (third_4bits) {
		case 0xE:
			if (fourth_4bits == 0xE) {// return opcode
				//std::cout << "return opcode[" << std::hex << byte << "]" << std::endl;
				PC = stack[SP--];
			} else if (fourth_4bits == 0x0) {// CLS opcode 
				//std::cout << "CLS opcode[" << std::hex << byte << "]" << std::endl;
				for (int i = 0; i < graphics.size(); i++)
					graphics[i] = 0;
			} else {
				//std::cout << "SYS opcode[" << std::hex << byte << "]" << std::endl;
				PC = byte & 0x0FFF;
			}
			break;
		default:
			//std::cout << "SYS opcode[" << std::hex << byte << "]" << std::endl;
			PC = byte & 0x0FFF;
			break;
		}
		break;
	case 0x1: // JUMP opcode
		//std::cout << "JP opcode[" << std::hex << byte << "]" << std::endl;
		PC = byte & 0x0FFF;	
		break;
	case 0x2:
		//std::cout << "CALL opcode[" << std::hex << byte << "]" << std::endl;
		stack[SP++] = PC;
		PC = byte & 0x0FFF;
		break;
	case 0x3:
		//std::cout << "SE opcode[" << std::hex << byte << "]" << std::endl;
		if (registers[second_4bits] == (byte & 0x00FF) )
			PC += 2;
		break;
	case 0x4:
		//std::cout << "SNE opcode[" << std::hex << byte << "]" << std::endl;;
		if (registers[second_4bits] == (byte & 0x00FF))
			PC += 2;
		break;
	case 0x5:
		if (registers[second_4bits] == registers[third_4bits] && fourth_4bits == 0) {
			//std::cout << "SE opcode[" << std::hex << byte << "]" << std::endl;
			PC += 2;
		} else if (fourth_4bits != 0) {
			//std::cout << "This opcode isnt right!" << std::endl;
		}
		break;
	case 0x6:
		//std::cout << "LD Vx opcode [" << std::hex << byte << "]" << std::endl;
		registers[second_4bits] = byte & 0x00FF;
		break;
	case 0x7:
		//std::cout << "ADD opcode [" << std::hex << byte << "]" << std::endl;
		registers[second_4bits] += byte * 0x00FF;
		break;
	case 0x8:
		switch (fourth_4bits) {
		case 0x0:
			//std::cout << "SET opcode [" << std::hex << byte << "]" << std::endl;
			registers[second_4bits] = registers[third_4bits];
			break;
		case 0x1: 
			//std::cout << "OR opcode [" << std::hex << byte << "]" << std::endl;
			registers[second_4bits] |= registers[third_4bits];
			break;
		case 0x2:
			//std::cout << "AND opcode [" << std::hex << byte << "]" << std::endl;
			registers[second_4bits] &= registers[third_4bits];
			break;
		case 0x3:
			//std::cout << "XOR opcode [" << std::hex << byte << "]" << std::endl;
			registers[second_4bits] ^= registers[third_4bits];
			break;
		case 0x4:
			//std::cout << "ADD opcode [" << std::hex << byte << "]" << std::endl;
			if (int(registers[second_4bits]) + int(registers[third_4bits]) > 255)
				registers[VF] = 1;
			else
				registers[VF] = 0;

			registers[second_4bits] += registers[third_4bits];
			break;
		case 0x5:
			//std::cout << "SUB opcode [" << std::hex << byte << "]" << std::endl;
			if (registers[second_4bits] > registers[third_4bits])
				registers[VF] = 1;
			else
				registers[VF] = 0;

			registers[second_4bits] -= registers[third_4bits];
			break;
		case 0x6:
			//std::cout << "SHR opcode [" << std::hex << byte << "]" << std::endl;
			if (( registers[second_4bits] & 0b0001) == 1)
				registers[VF] = 1;
			else
				registers[VF] = 0;

			registers[second_4bits] >>= 1;
			break;
		case 0x7:
			//std::cout << "SUBN opcode [[" << std::hex << byte << "]" << std::endl;
			if (registers[third_4bits] > registers[second_4bits])
				registers[VF] = 1;
			else
				registers[VF] = 0;

			registers[second_4bits] = registers[third_4bits] - registers[second_4bits];
			break;
		case 0xE:
			//std::cout << "SHR opcode [" << std::hex << byte << "]" << std::endl;
			if ((registers[second_4bits] & 0b1000) == 1)
				registers[VF] = 1;
			else
				registers[VF] = 0;

			registers[second_4bits] <<= 1;
			break;
		default:
			//std::cout << "This opcode isnt right!" << "[" << std::hex << byte << "]" << std::endl;
			break;
		}
		break;
	case 0x9:
		if (registers[second_4bits] != registers[third_4bits] && fourth_4bits == 0) {
			//std::cout << "SNE opcode[[" << std::hex << byte << "]" << std::endl;
			PC += 2;
		} else if (fourth_4bits != 0) {
			//std::cout << "This opcode isnt right!" << "[" << std::hex << byte << "]" << std::endl;
		}
		break;
	case 0xA:
		//std::cout << "LD I opcode[" << std::hex << byte << "]" << std::endl;
		I = byte & 0x0FFF;
		break;
	case 0xB:
		//std::cout << "JP V0+nnn opcode [" << std::hex << byte << "]" << std::endl;
		PC = (byte & 0xFFF) + registers[V0];
		break;
	case 0xC:
		//std::cout << "RND & kk opcode [" << std::hex << byte << "]" << std::endl;
		registers[second_4bits] = (rand() % 255) & (byte * 0x00FF);
		break;
	case 0xD:
		//std::cout << "DRW Vx, Vy, nibble opcode [" << std::hex << byte << "]" << std::endl;

		for (int i = 0; i < fourth_4bits; i++) {
			if (graphics[registers[second_4bits] + 64 * registers[third_4bits]] == 1)
				registers[VF] = 1;
			else
				registers[VF] = 0;

			graphics[registers[second_4bits] + 64*registers[third_4bits]] |= ram[I + i];
		}
		break;
	case 0xE:
		if ((byte & 0x00FF) == 0x9E) {
			//std::cout << "SKP opcode [" << std::hex << byte << "]" << std::endl;
			if (keys[registers[second_4bits]])
				PC += 2;
		} else if((byte & 0x00FF) == 0xA1) {
			//std::cout << "SKNP opcode [" << std::hex << byte << "]" << std::endl;
			if (!keys[registers[second_4bits]])
				PC += 2;
		} else {
			//std::cout << "This opcode isnt right!" << "[" << std::hex << byte << "]" << std::endl;
		}
		break;
	case 0xF:
		switch (byte & 0x00FF) {
		case 0x07:
			//std::cout << "LD Vx DT opcode[" << std::hex << byte << "]" << std::endl;
			registers[second_4bits] = DT;
			break;
		case 0x0A:
			//std::cout << " LD Vx, K opcode [" << std::hex << byte << "]" << std::endl;
//			int i = 0;
//			while (true)
//				for (i = 0; i < keys.size(); i++)
//					if (keys[i])
//						goto SET_KEY;
//SET_KEY:
//			registers[second_4bits] = i;
			for (int i = 0;; i = (++i) % keys.size()) {
				if (keys[i]) {
					registers[second_4bits] = i;
					break;
				}
			}
			break;
		case 0x15:
			//std::cout << "LD DT VX opcode [" << std::hex << byte << "]" << std::endl;
			DT = registers[second_4bits];
			break;
		case 0x18:
			//std::cout << "LD ST VX opcode [" << std::hex << byte << "]" << std::endl;
			ST = registers[second_4bits];
			break;
		case 0x1E:
			//std::cout << "ADD I VX opcode [" << std::hex << byte << "]" << std::endl;
			I += registers[second_4bits];
			break;
		case 0x29:
			//std::cout << "SET I Vx digit opcode [" << std::hex << byte << "]" << std::endl;
			I = registers[second_4bits] * 5;
			break;
		case 0x33:
			//std::cout << "LD B, Vx opcode [" << std::hex << byte << "]" << std::endl;
			ram[I] = registers[second_4bits]/100 % 10;
			ram[I + 1] = registers[second_4bits]/10 % 10;
			ram[I + 2] = registers[second_4bits] % 10;
			break;
		case 0x55:
			//std::cout << " LD [I], Vx opcode [" << std::hex << byte << "]" << std::endl;
			for (int i = 0; i < registers.size(); i++) {
				ram[I + i] = registers[i];
			}
			break;
		case 0x65:
			//std::cout << "LD Vx, [I] opcode [" << std::hex << byte << "]" << std::endl;
			for (int i = 0; i < registers.size(); i++) {
				registers[i] = ram[I + i];
			}
			break;
		default: 
			//std::cout << "This opcode isnt right!" << "[" << std::hex << byte << "]" << std::endl;
			break;
		}		
	}

	PC += 2;
}

#define SET_DIGIT(digit, byte_1, byte_2, byte_3, byte_4, byte_5) ram[digit] = byte_1; ram[digit*5 + 1] = byte_2; \
										 ram[digit*5 +2] = byte_3; ram[digit*5 +3] = byte_4; ram[digit*5 +4] = byte_5

void timer_setter(chip8* chip) {

}

chip8::chip8(){}

void chip8::init() {

	PC = 0x200;
	ST = 0;
	SP = 0;
	DT = 0;
	I  = 0;

	for (int i = 0; i < ram.size(); i++)
		ram[i] = 0;

	for (int i = 0; i < graphics.size(); i++)
		graphics[i] = 0;

	for (int i = 0; i < registers.size(); i++)
		registers[i] = 0;

	for (int i = 0; i < stack.size(); i++)
		stack[i] = 0;

	for (int i = 0; i < keys.size(); i++)
		keys[i] = 0;

	timer = std::thread([this]() {
		
		static bool sound_invoked = true;
		while (true) {
			if (this->DT != 0) {
				this->DT--;
			}

			if (this->ST != 0) {
				this->ST--;
				sound_invoked = false;
			} else {
				if (!sound_invoked) {
					std::cout << "Beep" << std::endl;
					sound_invoked = true;
				}
			}

			std::this_thread::sleep_for(std::chrono::microseconds(16667));
		}
	});

	SET_DIGIT(0x0, 0xF0, 
			  0x90, 
			  0x90, 
			  0x90, 
			  0xF0);

	SET_DIGIT(0x1, 0x20,
			  0x60,
			  0x20,
			  0x20,
			  0x70);

	SET_DIGIT(0x2, 0xF0,
			  0x10,
			  0xF0,
			  0x80,
			  0xF0);

	SET_DIGIT(0x3, 0xF0,
			  0x10,
			  0xF0,
			  0x10,
			  0xF0);

	SET_DIGIT(0x4, 0x90,
			  0x90,
			  0xF0,
			  0x10,
			  0x10);

	SET_DIGIT(0x5, 0xF0,
			  0x80,
			  0xF0,
			  0x10,
			  0xF0);

	SET_DIGIT(0x6, 0xF0,
			  0x80,
			  0xF0,
			  0x90,	
			  0xF0);

	SET_DIGIT(0x7, 0xF0,
			  0x10,
			  0x20,
			  0x40,
			  0x40);

	SET_DIGIT(0x8, 0xF0,
			  0x90,
			  0xF0,
			  0x90,
			  0xF0);

	SET_DIGIT(0x9, 0xF0,
			  0x90,
			  0xF0,
			  0x10,
			  0xF0);

	SET_DIGIT(0xA, 0xF0,
			  0x90,
			  0xF0,
			  0x90,
			  0x90);

	SET_DIGIT(0xB, 0xE0,
			  0x90,
			  0xE0,
			  0x90,
			  0xE0);

	SET_DIGIT(0xC, 0xF0,
			  0x80,
			  0x80,
			  0x80,
			  0xF0);

	SET_DIGIT(0xD, 0xE0,
			  0x90,
			  0x90,
			  0x90,
			  0xE0);

	SET_DIGIT(0xE, 0xF0,
			  0x80,
			  0xF0,
			  0x80,
			  0xF0);

	SET_DIGIT(0xF, 0xF0,
			  0x80,
			  0xF0,
			  0x80,
			  0x80);

	can_run = false;
	timer.detach();
}

void decompile(uint16_t byte, int i);

void chip8::load(std::string_view name) {
	std::fstream file;
	file.open(name.data(), std::ios::in | std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		int file_size = file.tellg();

		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(&ram[0x200]), file_size);
		can_run = true;

		for (int i = 0x200; i < 0x200 + file_size; i += 2) {
			uint16_t opcode = ram[i + 1] | ram[i] << 8;
			decompile(opcode, i);
		}
		file.close();
	}


}

void chip8::cycle() {
	uint16_t opcode =  ram[PC + 1] | ram[PC] << 8;
	fetch_opcode(opcode);	
}

void chip8::draw() {}

void decompile(uint16_t byte, int i) {
	int8_t first_4bits = (byte & 0xF000) >> 12;
	int8_t second_4bits = (byte & 0x0F00) >> 8;
	int8_t third_4bits = (byte & 0x00F0) >> 4;
	int8_t fourth_4bits = (byte & 0x000F) >> 0;

	int8_t zero_nnn = (byte & 0x0FFF);
	int8_t kk = (byte & 0x00FF);
	int8_t x = second_4bits;
	int8_t y = third_4bits;

	printf("Addr: %d ", i);

	switch (first_4bits) {
	case 0x0:
		switch (third_4bits) {
		case 0xE:
			if (fourth_4bits == 0xE) {// return opcode
				printf("RET[%X] // RETURN from a subroutine.\n", byte);
			} else if (fourth_4bits == 0x0) {// CLS opcode 
				printf("CLS[%X] // Clear the display.\n", byte);
			} else {
				printf("SYS %X [%X] // Jump to machine code routine at %X.\n", zero_nnn, byte, zero_nnn);
			}
			break;
		default:
			printf("SYS %X [%X] // Jump to machine code routine at %X.\n", zero_nnn, byte, zero_nnn);
			break;
		}
		break;
	case 0x1: // JUMP opcode
		printf("JP %X [%X] // Jump to a machine code routine at %X.\n", zero_nnn, byte, zero_nnn);
		break;
	case 0x2:
		printf("CALL %X [%X] // Call subroutine at %X.\n", zero_nnn, byte, zero_nnn);
		break;
	case 0x3:
		printf("SE V%X %X [%X] // Skip next instruction if V%X = %X\n", x, kk, byte, x, kk);
		break;
	case 0x4:
		printf("SNE V%X %X [%X] // Skip next instruction if V%X != %X\n", x, kk, byte, x, kk);
		break;
	case 0x5:
		if (fourth_4bits == 0) {
			printf("SE V%X Ì%X [%X] // Skip next instruction if V%X = V%X\n", x, y, byte, x, y);
		} else if (fourth_4bits != 0) {
			printf("Unsupported opcode [%X]\n", byte);
		}
		break;
	case 0x6:
		printf("LD V%X %X [%X] // V%X = %X\n", x, kk, byte, x, kk);
		break;
	case 0x7:
		printf("ADD V%X %X [%X] // V%X += %X\n", x, kk, byte, x, kk);
		break;
	case 0x8:
		switch (fourth_4bits) {
		case 0x0:
			printf("LD V%X V%X [%X] // V%X = V%X\n", x, y, byte, x, y);
			break;
		case 0x1:
			printf("OR V%X V%X [%X] // V%X |= V%X\n", x, y, byte, x, y);
			break;
		case 0x2:
			printf("AND V%X V%X [%X] // V%X &= V%X\n", x, y, byte, x, y);
			break;
		case 0x3:
			printf("XOR V%X V%X [%X] // V%X ^= V%X\n", x, y, byte, x, y);
			break;
		case 0x4:
			printf("ADD V%X V%X [%X] // V%X += V%X\n", x, y, byte, x, y);
			break;
		case 0x5:
			printf("SUB V%X V%X [%X] // V%X -= V%X\n", x, y, byte, x, y);
			break;
		case 0x6:
			printf("SHR V%X [%X] // V%X >>= 1\n", x, byte, x);
			break;
		case 0x7:
			printf("SUBN V%X V%X [%X] // V%X = V%X - V%X\n", x, y, byte, x, y, x);
			break;
		case 0xE:
			printf("SHL V%X [%X] // V%X <<= 1\n", x, byte, x);
			break;
		default:
			printf("Unsupported opcode [%X]\n", byte);
			break;
		}
		break;
	case 0x9:
		if (fourth_4bits == 0) {
			printf("SNE V%X V%X [%X] // Skip next instruction if V%X != V%X.\n", x, y, byte, x, y);
		} else if (fourth_4bits != 0) {
			printf("Unsupported opcode [%X]\n", byte);
		}
		break;
	case 0xA:
		printf("LD I %X [%X] // Set I = %X\n", zero_nnn, byte, zero_nnn);
		break;
	case 0xB:
		printf("JPRELV0 %X [%X] // GOTO V0 + %X\n", zero_nnn, byte, zero_nnn);
		break;
	case 0xC:
		printf("RNDAND %X %X [%X] // V%X = random(0, 255) & %X\n", x, kk, byte, x, kk);
		break;
	case 0xD:
		printf("DRW V%X V%X %X [%X] // Display %X-byte sprite starting at memory location I at (V%X, V%X)\n", x, y, fourth_4bits, byte, fourth_4bits, x, y);
		break;
	case 0xE:
		if ((byte & 0x00FF) == 0x9E) {
			printf("SKP V%X [%X] // Skip next instruction if key with the value of V%X is pressed.\n", x, byte, x);
		} else if ((byte & 0x00FF) == 0xA1) {
			printf("SKP V%X [%X] // Skip next instruction if key with the value of V%X isnt pressed.\n", x, byte, x);
		} else {
			printf("Unsupported opcode [%X]\n", byte);
		}
		break;
	case 0xF:
		switch (byte & 0x00FF) {
		case 0x07:
			printf("LD V%X DT [%X] // V%X = DT.\n", x, byte, x);
			break;
		case 0x0A:
			printf("LD V%X K [%X] // V%X = K, waits for key Key pressed is K.\n", x, byte, x);
			break;
		case 0x15:
			printf("LD DT V%X  [%X] // DT = V%X.\n", x, byte, x);
			break;
		case 0x18:
			printf("LD ST V%X  [%X] // ST = V%X.\n", x, byte, x);
			break;
		case 0x1E:
			printf("ADD I V%X  [%X] // I += V%X.\n", x, byte, x);
			break;
		case 0x29:
			printf("LD F V%X  [%X] // I is address of digit V%X.\n", x, byte, x);
			break;
		case 0x33:
			printf("LD I V%X  [%X] // Store BCD representation of V%X in memory locations I, I+1, and I+2\n", x, byte, x);
			break;
		case 0x55:
			printf("LD I V%X [%X] // Store registers V0 through V%X in memory starting at location I.\n", x, byte, x);
			break;
		case 0x65:
			printf("LD V%X I  [%X] // Read registers V0 through V%X in memory starting at location I.\n", x, byte, x);
			break;
		default:
			printf("Unsupported opcode [%X]\n", byte);
			break;
		}
	}
}