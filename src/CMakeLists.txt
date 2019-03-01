cmake_minimum_required( VERSION 3.5 )
project( java_class_decoder CXX )
add_executable( decode decode )
add_definitions( -std=c++17 -Wall )

set( JAVA_CLASS "Statics" )

add_custom_target(
	java_target ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${JAVA_CLASS}.class
)

add_custom_command(
	OUTPUT ${JAVA_CLASS}.class
	DEPENDS ${JAVA_CLASS}.java
	COMMAND javac -d ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${JAVA_CLASS}.java
	COMMENT "Compiling ${JAVA_CLASS}.java"
)
