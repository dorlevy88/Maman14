cmake_minimum_required(VERSION 3.5)
project(Maman14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    DataStructures/Command.c
    DataStructures/Command.h
    SymbolTables/CommandAdressingTypes.c
    SymbolTables/CommandAdressingTypes.h
    SymbolTables/CommandAREType.c
    SymbolTables/CommandAREType.h
    SymbolTables/CommandGroupTypes.c
    SymbolTables/CommandGroupTypes.h
    SymbolTables/CommandOperationTypes.c
    SymbolTables/CommandOperationTypes.h
    Utilities/FileReader.c
    Utilities/FileReader.h
    Utilities/FileWriter.c
    Utilities/FileWriter.h
    Transitions/SecondTransition.c
    Transitions/SecondTransition.h
    Transitions/FirstTransition.c
    Transitions/FirstTransition.h Main.c Main.h DataStructures/AssemblyStructure.c DataStructures/AssemblyStructure.h DataStructures/AssemblyByte.c DataStructures/AssemblyByte.h DataStructures/SymbolsTable.c DataStructures/SymbolsTable.h)

add_executable(Maman14 ${SOURCE_FILES})