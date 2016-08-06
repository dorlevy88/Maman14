#include <stdlib.h>
#include "assembler.h"

void free_all(AssemblyStructure** assembly, FileContent** file) {
    if (*assembly != NULL)
        freeAssemblyStructure(assembly);
    if (*file != NULL) {
        freeFileContent(file);
    }
}

bool init(AssemblyStructure** assembly, FileContent** file) {
    free_all(assembly, file);
    return initAssemblyStructure(assembly) && initFileContent(file);
}

int main(int argc, char **argv) {
    /* TODO: malloc checks, syntax error strings, name refactoring */
    /*
        1. read .as files
        2. iterate on each one of the files
        3.0   create data array & code array (represent memory layout) : arrays type is bit12[]
        3.1   crease ic & dc
        3.2   create symbols tables
        4.    Run transitions
        5.    Translate arrays to special base8
        6.    build .ob file --> (ic + ' ' + dc) code array and then data array (special base 8)
        7.    build .ext .ent
        8.    write .ob [.ext] [.ent] files for each one of the original files

     */

    int fileCounter;
    char* filename;
    bool results;
    FileContent* fileContent = NULL;
    AssemblyStructure* assemblyStructure = NULL;

    for (fileCounter = 1; fileCounter < argc; ++fileCounter) {
        filename = argv[fileCounter];
        results = init(&assemblyStructure, &fileContent);
        if (results == false) {
            PrintProcessStep("Internal Memory issue", filename);
            exit(0);
        }

        PrintProcessStep("Start processing file", filename);
        if (getFileContent(filename, fileContent) == false) { /*Error in the file*/
            PrintProcessStep("Parsing file failed", filename);
            continue;
        }
        PrintProcessStep("Parsing file succeeded", filename);

        if(RunFirstTransition(fileContent, assemblyStructure) == false) {
            PrintProcessStep("Transition one failed", filename);
            continue;
        }
        PrintProcessStep("Transition one succeeded", filename);

        if (RunSecondTransition(fileContent, assemblyStructure) == false) {
            PrintProcessStep("Transition two failed", filename);
            continue;
        }
        PrintProcessStep("Transition two succeeded", filename);

        PrintProcessStep("Start writing files", filename);
        if (WriteAllOutputFiles(assemblyStructure, filename) == false) {
            PrintProcessStep("Writing files failed", filename);
        }
        PrintProcessStep("Writing files succeeded", filename);
        /*TODO: delete files in case of a failure*/
    }
    free_all(&assemblyStructure, &fileContent);
    return 0;
}