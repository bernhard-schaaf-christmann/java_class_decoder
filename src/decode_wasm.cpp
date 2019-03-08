#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

struct From_Wasm {
	From_Wasm() : filename_("minimal.wasm"), modulefile_(filename_, modulefile_.binary | modulefile_.in ) {};
	int module_file() {
		magic();
		version();
		return 0;
	}
public:
	uint32_t next_uint32() {
		uint32_t value = modulefile_.get(); uint32_t result = value;
		value = modulefile_.get(); result = result | (value << 8);
		value = modulefile_.get(); result = result | (value << 16);
		value = modulefile_.get(); result = result | (value << 24);
		return result;
	}
public:
	void magic() {
		std::cout << "magic (0x6d736100): " << std::hex << next_uint32() << std::dec << std::endl;
	}
	void version() {
		uint32_t version = next_uint32();
		std::cout << "wasm module file version: " << version << std::endl;
	}
private:
	std::string filename_;
	std::fstream modulefile_;
};

int main() {
	From_Wasm from_wasm;
	return from_wasm.module_file();
}

