cmake_minimum_required(VERSION 3.5)
project(Maman14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        assembler.c
        input_reader.c
        input_reader.h
        utils.c
        utils.h
        output_writer.c
        output_writer.h
        data_structures.c
        data_structures.h transitions.c transitions.h definitions.h assembler.h)

add_executable(assembler ${SOURCE_FILES})