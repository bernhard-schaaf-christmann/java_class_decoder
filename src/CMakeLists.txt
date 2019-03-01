cmake_minimum_required( VERSION 3.5 )
project( java_class_decoder CXX )
add_executable( decode decode )
add_definitions( -std=c++17 -Wall )

configure_file( Statics.class Statics.class COPYONLY )
