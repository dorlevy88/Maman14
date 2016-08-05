#include <stdlib.h>
#include "assembler.h"


int main(int argc, char **argv) {
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
    for (fileCounter = 1; fileCounter < argc; ++fileCounter) {

        char* filename = argv[fileCounter];
        PrintProcessStep("Start processing file", filename);
        FileContent* fileContent = (FileContent*)malloc(sizeof(FileContent));
        if (getFileContent(filename, fileContent) == false) { /*Error in the file*/
            PrintProcessStep("Parsing file failed", filename);
            continue;
        }

        AssemblyStructure* assemblyStructure = InitAssemblyStructure();
        if(RunFirstTransition(fileContent, assemblyStructure) == false) {
            PrintProcessStep("Transition one failed", filename);
            continue;
        }
        if (RunSecondTransition(fileContent, assemblyStructure) == false) {
            PrintProcessStep("Transition two failed", filename);
            continue;
        }

        if (WriteAllOutputFiles(assemblyStructure, filename) == false) {
            PrintProcessStep("Writing file failed", filename);
            /*TODO: delete files in case of a failure*/
        }
        PrintProcessStep("Processing file succeeded", filename);
    }
    return 0;
}