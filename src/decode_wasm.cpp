#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

// see https://webassembly.org/docs/binary-encoding/

struct From_Wasm {
	From_Wasm() : filename_("minimal.wasm"), modulefile_(filename_, modulefile_.binary | modulefile_.in ) {};
	int module_file() {
		magic();
		version();
		while (section()) {}
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
	uint32_t next_varuint1() {
		return modulefile_.get() && 0x01;
	}
	uint32_t next_varuint7() {
		struct {unsigned int x:7;} s;
		s.x = modulefile_.get();
		return s.x;
	}
	uint8_t next_uint8() {
		return modulefile_.get();
	}
	uint32_t tell() {
		return modulefile_.tellg();
	}
	void next_utf8(uint32_t length) {
		std::cout << '"';
		for (uint32_t index = 0; index < length; ++index) {
			char c = next_uint8();
			std::cout << c;
		}
		std::cout << '"' << std::endl;
	}
	uint32_t next_varint7() {
		struct {signed int x:7;} s;
		s.x = modulefile_.get();
		return s.x;
	}
	uint32_t next_varuint32() {
		uint32_t result = 0;
		uint32_t shift = 0;
		while (shift < 32) {
			uint8_t byte = modulefile_.get();
			result |= (byte & 0x0000007F) << shift;
			if (not (byte & 0x80)) break;
			shift += 7;
		}
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
	bool section() {
		uint32_t id = next_varuint7();
		if (modulefile_.eof()) return false;
		std::cout << std::endl;
		uint32_t payload_len = next_varuint32();
		switch (id) {
			case 0:
				section_of_custom(id, payload_len);
				break;
			case 1:
				section_of_type(id, payload_len);
				break;
			case 3:
				section_of_function(id, payload_len);
				break;
			case 6:
				section_of_global(id, payload_len);
				break;
			case 7:
				section_of_export(id, payload_len);
				break;
			case 10:
				section_of_code(id, payload_len);
				break;
			default:
				std::cout << "UNKNOWN section type " << id << std::endl;
				throw 42;
		}
		return true;
	}
	void section_of_custom(uint32_t id, uint32_t payload_len) {
		std::cout << "new section: Custom – ";
		std::cout << "section.id = " << id << std::endl;
		std::cout << "section.payload_len = " << payload_len << std::endl;
		uint32_t name_len = next_varuint32();
		next_utf8(name_len); // TODO read UTF8
		// TODO read rest of section
	}
	void section_of_type(uint32_t id, uint32_t payload_len) {
		std::cout << "new section: Type – ";
		std::cout << "section.id = " << id << std::endl;
		std::cout << "section.payload_len = " << payload_len << std::endl;
		uint32_t count = next_varuint32();
		std::cout << "section.count = " << count << std::endl;
		for (uint32_t entry_index = 0; entry_index < count; ++entry_index) {
			func_type();
		}
	}
	void section_of_function(uint32_t id, uint32_t payload_len) {
		std::cout << "new section: Function – ";
		std::cout << "section.id = " << id << std::endl;
		std::cout << "section.payload_len = " << payload_len << std::endl;
		uint32_t count = next_varuint32();
		std::cout << "section.count = " << count << std::endl;
		for (uint32_t entry_index = 0; entry_index < count; ++entry_index) {
			uint32_t index_to_types = next_varuint32();
			std::cout << "this.index_to_type_section = " << index_to_types << std::endl;
		}
	}
	void section_of_global(uint32_t id, uint32_t payload_len) {
		std::cout << "new section: Global – ";
		std::cout << "section.id = " << id << std::endl;
		std::cout << "section.payload_len = " << payload_len << std::endl;
		uint32_t count = next_varuint32();
		std::cout << "section.count = " << count << std::endl;
		for (uint32_t entry_index = 0; entry_index < count; ++entry_index) {
			global();
		}
	}
	void section_of_export(uint32_t id, uint32_t payload_len) {
		std::cout << "new section: Export – ";
		std::cout << "section.id = " << id << std::endl;
		std::cout << "section.payload_len = " << payload_len << std::endl;
		uint32_t count = next_varuint32();
		std::cout << "section.count = " << count << std::endl;
		for (uint32_t entry_index = 0; entry_index < count; ++entry_index) {
			export_entry();
		}
	}
	void section_of_code(uint32_t id, uint32_t payload_len) {
		std::cout << "new section: Code – ";
		std::cout << "section.id = " << id << std::endl;
		std::cout << "section.payload_len = " << payload_len << std::endl;
		uint32_t count = next_varuint32();
		std::cout << "section.count = " << count << std::endl;
		for (uint32_t entry_index = 0; entry_index < count; ++entry_index) {
			function_body();
		}
	}
	void func_type() {
		int32_t form = next_varint7();
		uint32_t param_count = next_varuint32();
		std::cout << "func_type.form = " << parse_type(form) << std::endl; // TODO convert to describing name
		std::cout << "func_type.param_count = " << param_count << std::endl;
		for (uint32_t param_index = 0; param_index < param_count; ++param_index) {
			std::cout << "param $" << param_index << ' ';
			value_type();
		}
		uint32_t return_count = next_varuint1();
		if (return_count) {
			std::cout << "result ";
			value_type();
		}
	}
	void value_type() {
		int32_t value_type = next_varint7();
		std::cout << "value_type.value_type = " << parse_type(value_type) << std::endl;
	}
	void global() {
		global_type();
		init_expr();
	}
	void global_type() {
		value_type();
		uint32_t mutability = next_varuint1();
		std::cout << "global_type.mutability = " << (mutability?"yes":"no") << std::endl;
	}
	void init_expr() { // bytecode ???
		std::cout << "tell " << tell() << std::endl;
		throw 42;
	}
	void export_entry() {
		uint32_t field_len = next_varuint32();
		std::cout << "export_entry.field_len = " << field_len << " utf8 " << std::endl;
		next_utf8(field_len); // TODO read UTF8 field_str
		uint8_t kind = next_uint8();
		std::cout << "export_entry.kind = " << parse_external_kind(kind) << std::endl;
		uint32_t index = next_varuint32();
		std::cout << "export_entry.index = " << index << std::endl;
	}
	const char* parse_type(int8_t type) {
		switch (type) {
			case -0x01: return "i32";
			case -0x02: return "i64";
			case -0x03: return "f32";
			case -0x04: return "f64";
			case -0x10: return "anyfunc";
			case -0x20: return "func";
			case -0x40: return "block_type";
		}
		return "UNKNOWN TYPE";
	}
	const char* parse_external_kind(uint8_t kind) {
		switch (kind) {
			case 0: return "Function";
			case 1: return "Table";
			case 2: return "Memory";
			case 3: return "Global";
		}
		return "UNKNOWN KIND";
	}
	void function_body() {
		uint32_t body_size = next_varuint32();
		uint32_t body_start = tell();
		uint32_t body_end = body_start + body_size;
		uint32_t local_count = next_varuint32();
		std::cout << "function_body.body_size = " << body_size << " function_body.local_count = " << local_count << std::endl;
		for (uint32_t index = 0; index < local_count; ++index) {
			local_entry();
		}
		uint32_t start_of_code = tell();
		uint32_t end_of_code = body_end;
		uint32_t length_of_code = end_of_code - start_of_code; // we include the final end byte 0x0b
		std::cout << "function_body.code_length = " << length_of_code << std::endl;
		read_bytes(length_of_code);
	}
	void read_bytes(uint32_t length) {
		std::cout << std::hex;
		for (uint32_t index = 0; index < length; ++index) {
			uint32_t byte = next_uint8();
			std::cout << byte << ' ';
		}
		std::cout << std::dec << std::endl;
	}
	void local_entry() {
		uint32_t count = next_varuint32();
		std::cout << "local variabels count " << count << " of following type" << std::endl;
		value_type();
	}
private:
	std::string filename_;
	std::fstream modulefile_;
};

int main() {
	From_Wasm from_wasm;
	return from_wasm.module_file();
}

