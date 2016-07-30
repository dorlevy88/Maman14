cmake_minimum_required(VERSION 3.5)
project(Maman14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    DataStructures/Command.c
    DataStructures/Command.h
        SymbolTables/CommandAREType.c
    SymbolTables/CommandAREType.h
        Transitions/SecondTransition.c
    Transitions/SecondTransition.h
    Transitions/FirstTransition.c
    Transitions/FirstTransition.h Main.c Main.h DataStructures/AssemblyStructure.h DataStructures/AssemblyBytes.h DataStructures/SymbolsTable.h Utilities/FileHandlers.c Utilities/FileHandlers.h DataStructures/SymbolsTable.c DataStructures/Enums.c DataStructures/Enums.h DataStructures/Operand.h DataStructures/AssemblyBytes.c)

add_executable(Maman14 ${SOURCE_FILES})