#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Index_Pair {
	uint16_t first;
	uint16_t second;
};

struct Constant_Info {
	uint8_t tag;
	union {
		Index_Pair index;
	    uint32_t u32;
	    float f32;
		uint64_t u64;
		double f64;
	} data;
	std::string utf8;
	void get(uint16_t& first) {
		first = data.index.first;
	}
	void get(uint16_t& first, uint16_t& second) {
		first = data.index.first;
		second = data.index.second;
	}
	void get(uint32_t& r) {
		r = data.u32;
	}
	void get(float& r) {
		r = data.f32;
	}
	void get(uint64_t& r) {
		r = data.u64;
	}
	void get(double& r) {
		r = data.f64;
	}
	void set(uint16_t first) {
		data.index.first = first;
	}
	void set(uint16_t first, uint16_t second) {
		data.index.first = first;
		data.index.second = second;
	}
	void set(uint32_t r) {
		data.u32 = r;
	}
	void set(float r) {
		data.f32 = r;
	}
	void set(uint64_t r) {
		data.u64 = r;
	}
	void set(double r) {
		data.f64 = r;
	}
};


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
		constants_.resize(length);
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
		constants_[index].tag = tag;
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
		constants_[index].set(name_index);
		std::cout << '@' << index << " constant_class.name_index = " << name_index << std::endl;
	}
	void constant_xref(uint16_t index, uint8_t tag) {
		uint16_t class_index = next_u2();
		uint16_t name_and_type_index = next_u2();
		constants_[index].set(class_index, name_and_type_index);
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
		constants_[index].set(string_index);
		std::cout << '@' << index << " constant_string.string_index = " << string_index << std::endl;
	}
	void constant_integer(uint16_t index, uint8_t tag) {
		uint32_t integer = next_u4();
		constants_[index].set(integer);
		std::cout << '@' << index << " constant_integer = " << integer << std::endl;
	}
	void constant_float(uint16_t index, uint8_t tag) {
		float f32 = next_float();
		constants_[index].set(f32);
		std::cout << '@' << index << " constant_float = " << f32 << std::endl;
	}
	void constant_long(uint16_t index, uint8_t tag) {
		uint64_t longint = next_u8();
		constants_[index].set(longint);
		std::cout << '@' << index << " constant_long = " << longint << std::endl;
	}
	void constant_double(uint16_t index, uint8_t tag) {
		uint64_t f64 = next_double();
		constants_[index].set(f64);
		std::cout << '@' << index << " constant_double = " << f64 << std::endl;
	}
	void constant_name_and_type(uint16_t index, uint8_t tag) {
		uint16_t name_index = next_u2();
		uint16_t descriptor_index = next_u2();
		constants_[index].set(name_index, descriptor_index);
		std::cout << '@' << index << " constant_name_and_type.name_index = " << name_index << " this.descriptor_index = " << descriptor_index << std::endl;
	}
	void constant_utf8(uint16_t index, uint8_t tag) {
		uint16_t length = next_u2();
		for (uint16_t l = 0; l < length; ++l) {
			char c = next_u1();
			constants_[index].utf8.push_back(c);
		}
		std::cout << '@' << index << " constant_utf8.length = " << length << " \"";
		std::cout << constants_[index].utf8;
		std::cout << '"' << std::endl;
	}
	enum ACC {
		PUBLIC = 0x0001,
		PRIVATE = 0x0002,
		PROTECTED = 0x0004,
		STATIC = 0x0008,
		FINAL = 0x0010,
		SUPER_OR_SYNCHRONIZED = 0x0020,
		VOLATILE = 0x0040,
		TRANSIENT = 0x0080,
		NATIVE = 0x0100,
		INTERFACE = 0x0200,
		ABSTRACT = 0x0400
	};
	void flags_and_classes() {
		uint16_t access_flags = next_u2();
		uint16_t this_class = next_u2();
		uint16_t super_class = next_u2();
		std::cout << "access_flags " << std::hex << access_flags << std::dec << " – ";
		if (ACC::PUBLIC & access_flags) std::cout << "public ";
		if (ACC::FINAL & access_flags) std::cout << "final ";
		if (ACC::SUPER_OR_SYNCHRONIZED & access_flags) std::cout << "super ";
		if (ACC::INTERFACE & access_flags) std::cout << "interface ";
		if (ACC::ABSTRACT & access_flags) std::cout << "abstract ";
		std::cout << std::endl;
		std::cout << "this_class " << this_class << std::endl;
		std::cout << "super_class " << super_class << std::endl;
	}
	void interfaces() {
		uint16_t length = next_u2();
		std::cout << "Interfaces length: " << length << std::endl;
		for (uint16_t index = 0; index < length; ++index) {
			interface(index);
		}
	}
	void interface(uint16_t index) {
		uint16_t that_interface = next_u2();
		std::cout << '@' << index << " interface: " << that_interface << std::endl;
	}
	void fields() {
		uint16_t length = next_u2();
		std::cout << "Fields length: " << length << std::endl;
		for (uint16_t index = 0; index < length; ++index) {
			field_info(index);
		}
	}
	void field_info(uint16_t index) {
		uint16_t access_flags = next_u2();
		uint16_t name_index = next_u2();
		uint16_t descriptor_index = next_u2();
		std::cout << '@' << index << " field_info - access_flags " << access_flags << " name_index " << name_index << " descriptor_index " << descriptor_index << std::endl;
		attributes("field_info");
	}
	void attributes(const char* belongs) {
		uint16_t length = next_u2();
		std::cout << belongs << " – attributes length: " << length << std::endl;
		for (uint16_t index = 0; index < length; ++index) {
			attribute_info(index);
		}
	}
	void attribute_info(uint16_t index) {
		uint16_t attribute_name_index = next_u2();
		uint32_t length = next_u4();
		std::cout << '@' << index << " attribute_info - attribute_name_index " << attribute_name_index << " length " << length << std::endl;
		std::cout << std::hex;
		for (uint32_t index = 0; index < length; ++index) {
			std::cout << next_u1() << ' '; // TODO handle attributes: constants, code, exception
		}
		std::cout << std::dec << std::endl;
	}
	void methods() {
		uint16_t length = next_u2();
		std::cout << "methods length: " << length << std::endl;
		for (uint16_t index = 0; index < length; ++index) {
			method_info(index);
		}
	}
	void method_info(uint16_t index) {
		uint16_t access_flags = next_u2();
		uint16_t name_index = next_u2();
		uint16_t descriptor_index = next_u2();
		std::cout << '@' << index << " method_info - access_flags " << access_flags << " name_index " << name_index << " descriptor_index " << descriptor_index << std::endl;
		attributes("method_info");
	}
private:
	std::string filename_;
	std::fstream classfile_;
	std::vector<Constant_Info> constants_;
};

int main() {
	From from;

	from.magic();
	from.version();
	from.constant_pool();
	from.flags_and_classes();
	from.interfaces();
	from.fields();
	from.methods();
	from.attributes("class");
	return 0;
}

