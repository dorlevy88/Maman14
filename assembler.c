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

    int filesCounter;
    char* filename;
    bool results;
    FileContent* fileContent = NULL;
    AssemblyStructure* assemblyStructure = NULL;

    for (filesCounter = 1; filesCounter < argc; ++filesCounter) {
        filename = argv[filesCounter];

        results = init(&assemblyStructure, &fileContent);
        if (results == false) {
            printInternalError(ERR_PROG_MEMORY_FAILURE, "While initializing data structures");
            exit(0);
        }

        printProcessStep("Start processing file", filename);
        if (getFileContent(filename, fileContent) == Fail) { /*Error in the file*/
            printProcessStep("Parsing file failed", filename);
            continue;
        }
        printProcessStep("Parsing file succeeded", filename);

        if(runFirstTransition(fileContent, assemblyStructure) == Fail) {
            printProcessStep("Transition one failed", filename);
            continue;
        }
        printProcessStep("Transition one succeeded", filename);

        if (runSecondTransition(fileContent, assemblyStructure) == Fail) {
            printProcessStep("Transition two failed", filename);
            continue;
        }
        printProcessStep("Transition two succeeded", filename);

        printProcessStep("Start writing files", filename);
        writeAllOutputFiles(assemblyStructure, filename);
        printProcessStep("Writing files succeeded", filename);
    }
    free_all(&assemblyStructure, &fileContent);
    return 0;
}