cmake_minimum_required(VERSION 3.5)
project(Maman14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        DataStructures/Command.c
        DataStructures/Command.h
        DataStructures/AssemblyStructure.h
        DataStructures/AssemblyStructure.c
        DataStructures/AssemblyBytes.h
        DataStructures/AssemblyBytes.c
        DataStructures/SymbolsTable.h
        DataStructures/SymbolsTable.c
        DataStructures/Enums.c
        DataStructures/Enums.h
        DataStructures/Operand.h
        Transitions/SecondTransition.c
        Transitions/SecondTransition.h
        Transitions/FirstTransition.c
        Transitions/FirstTransition.h
        Main.c
        Main.h
        Utilities/FileHandlers.c
        Utilities/FileHandlers.h
        Utilities/Logger.c
        Utilities/Logger.h Output/OutputFiles.c Output/OutputFiles.h)

add_executable(Maman14 ${SOURCE_FILES})