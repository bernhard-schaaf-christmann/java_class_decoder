#include <iostream>
#include <fstream>
#include <string>

struct From {
	From() : filename_("Statics.class"), classfile_(filename_, classfile_.binary | classfile_.in ) {	};
public:
	uint32_t next_u1() {
		uint8_t value = classfile_.get();
		return value;
	}
	uint32_t next_u2() {
		uint8_t value = classfile_.get(); uint16_t result = value;
		value = classfile_.get(); result = (result << 8) | value;
		return result;
	}
	uint32_t next_u4() {
		uint8_t value = classfile_.get(); uint32_t result = value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		return result;
	}
	uint64_t next_u8() {
		uint8_t value = classfile_.get(); uint64_t result = value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		value = classfile_.get(); result = (result << 8) | value;
		return result;
	}
	float next_float() {
		union conv32 {
		    uint32_t u32;
		    float f32;
		} x;
		x.u32 = next_u4();
		return x.f32; // TODO check byte order
	}
	double next_double() {
		union conv64 {
		    uint64_t u64;
		    double f64;
		} x;
		x.u64 = next_u8();
		return x.f64; // TODO check byte order
	}
public:
	void magic() {
		std::cout << "magic: " << std::hex << next_u4() << std::dec << std::endl;
	}
	void version() {
		uint16_t minor = next_u2();
		uint16_t major = next_u2();
		std::cout << "Java class file version: " << major << '.' << minor << std::endl;
	}
	void constant_pool() {
		uint16_t length = next_u2();
		std::cout << "Constant pool length: " << length << std::endl;
		for (uint16_t index = 1; index < length; ++index) {
			index += constant(index);
		}
	}
	enum CONSTANT {
		Class = 7,
		Fieldref = 9,
		Methodref = 10,
		InterfaceMethodref = 11,
		String = 8,
		Integer = 3,
		Float = 4,
		Long = 5,
		Double = 6,
		NameAndType = 12,
		Utf8 = 1
	};
	uint16_t constant(uint16_t index) {
		uint8_t tag = next_u1();
		switch (tag) {
			case (Class):
				constant_class(index, tag);
				break;
			case (Fieldref):
			case (Methodref):
			case (InterfaceMethodref):
				constant_xref(index, tag);
				break;
			case (String):
				constant_string(index, tag);
				break;
			case (Integer):
				constant_integer(index, tag);
				break;
			case (Float):
				constant_float(index, tag);
				break;
			case (Long):
				constant_long(index, tag);
				return 1; // counts as two entries
			case (Double):
				constant_double(index, tag);
				return 1; // counts as two entries
			case (NameAndType):
				constant_name_and_type(index, tag);
				break;
			case (Utf8):
				constant_utf8(index, tag);
				break;
			default:
				std::cout << '@' << index << " ERROR unknown constant - tag = " << 0+tag << std::endl;
				throw 42;
		}
		return 0;
	}
	void constant_class(uint16_t index, uint8_t tag) {
		uint16_t name_index = next_u2();
		std::cout << '@' << index << " constant_class.name_index = " << name_index << std::endl;
	}
	void constant_xref(uint16_t index, uint8_t tag) {
		uint16_t class_index = next_u2();
		uint16_t name_and_type_index = next_u2();
		std::cout << '@' << index << " constant_";
		if (9 == tag) {
			std::cout << "Fieldref";
		} else if (10 == tag) {
			std::cout << "Methodref";
		} else if (11== tag) {
			std::cout << "InterfaceMethodref";
		} else {
			std::cout << "UNKNOWNref";
		}
		std::cout << ".class_index = " << class_index << " this.name_and_type_index " << name_and_type_index << std::endl;
	}
	void constant_string(uint16_t index, uint8_t tag) {
		uint16_t string_index = next_u2();
		std::cout << '@' << index << " constant_string.string_index = " << string_index << std::endl;
	}
	void constant_integer(uint16_t index, uint8_t tag) {
		uint32_t integer = next_u4();
		std::cout << '@' << index << "constant_integer = " << integer << std::endl;
	}
	void constant_float(uint16_t index, uint8_t tag) {
		float f32 = next_float();
		std::cout << '@' << index << "constant_float = " << f32 << std::endl;
	}
	void constant_long(uint16_t index, uint8_t tag) {
		uint64_t longint = next_u8();
		std::cout << '@' << index << "constant_long = " << longint << std::endl;
	}
	void constant_double(uint16_t index, uint8_t tag) {
		uint64_t f64 = next_double();
		std::cout << '@' << index << "constant_double = " << f64 << std::endl;
	}
	void constant_name_and_type(uint16_t index, uint8_t tag) {
		uint16_t name_index = next_u2();
		uint16_t descriptor_index = next_u2();
		std::cout << '@' << index << " constant_name_and_type.name_index = " << name_index << " this.descriptor_index = " << descriptor_index << std::endl;
	}
	void constant_utf8(uint16_t index, uint8_t tag) {
		uint16_t length = next_u2();
		std::cout << '@' << index << " constant_utf8.length = " << length << " \"";
		for (uint16_t l = 0; l < length; ++l) {
			char c = next_u1();
			std::cout << c; // TODO make it output utf8 correctly, check
		}
		std::cout << '"' << std::endl;
	}
private:
	std::string filename_;
	std::fstream classfile_;
};

int main() {
	From from;

	from.magic();
	from.version();
	from.constant_pool();
	return 0;
}

