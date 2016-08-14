#include <stdlib.h>
#include "assembler.h"

/**
 * decommission assembly structure and file content
 * @param assembly - pointer to AssemblyStructure
 * @param file - pointer to FileContent
 */
void free_all(AssemblyStructure** assembly, FileContent** file) {
    if (*assembly != NULL)
        freeAssemblyStructure(assembly);
    if (*file != NULL) {
        freeFileContent(file);
    }
}

/**
 * init new assembly structure and file content
 * @param assembly - pointer to AssemblyStructure
 * @param file - pointer to FileContent
 * @return Fail/Pass
 */
Status init(AssemblyStructure** assembly, FileContent** file) {
    free_all(assembly, file);
    if (initAssemblyStructure(assembly) == Fail)
        return Fail;
    if (initFileContent(file) == Fail)
        return Fail;
    return Pass;
}

/**
 * Assembler Compiler Program:
 * Takes .as files parsing them and outputing the liker files (.ob/[.ext]/[.ent]) in special 8 base style
 * @param argc arguments count
 * @param argv .as file names as arguments
 * @return
 */
int main(int argc, char **argv) {
    /*
        1.  iterate on each one of the .as files
        2.1 initialize data structures (data and code arrays, ic & dc, symbols & extern tables)
        2.2 initialize parsed file content structure
        3.    Run first & second transitions
        4.    Translate arrays to special base8
        5.    write .ob file --> (ic + ' ' + dc) code array and then data array (special base 8)
        6.    write .ext .ent (symbols table & extern table)
     */

    int filesCounter;
    char* filename;
    FileContent* fileContent = NULL;
    AssemblyStructure* assemblyStructure = NULL;

    for (filesCounter = 1; filesCounter < argc; ++filesCounter) { /* iterate on each one of the .as files */
        filename = argv[filesCounter];
        printProcessStep("Start processing file", filename);
        /*
         * initialize data structures (data and code arrays, ic & dc, symbols & extern tables)
         * initialize parsed file content structure
         */
        if (init(&assemblyStructure, &fileContent) == Fail) {  /* error allocating data structures */
            printInternalError(ERR_PROG_MEMORY_FAILURE, "While initializing data structures");
            return 1;
        }

        printProcessStep("Start parsing file", filename);    /* parse file content into the FileContent struct */
        if (getFileContent(filename, fileContent) == Fail) {    /* syntax errors in the file continue to next file*/
            printProcessStep("Parsing file failed", filename);
            continue;
        }
        printProcessStep("Parsing file succeeded", filename);


        printProcessStep("Transition one started", filename);   /* Run first pass of the file */
        if(runFirstTransition(fileContent, assemblyStructure) == Fail) { /* compiler errors in the file continue to next file */
            printProcessStep("Transition one failed", filename);
            continue;
        }
        printProcessStep("Transition one succeeded", filename);


        printProcessStep("Transition two started", filename);   /* Run second pass of the file */
        if (runSecondTransition(fileContent, assemblyStructure) == Fail) { /* compiler errors in the file continue to next file */
            printProcessStep("Transition two failed", filename);
            continue;
        }
        printProcessStep("Transition two succeeded", filename);


        printProcessStep("Start writing files", filename); /* Write .ob [.ext] [.ent] files in special 8 base */
        writeAllOutputFiles(assemblyStructure, filename);
        printProcessStep("Writing files succeeded", filename);

        printProcessStep("File processing done", filename);
    }
    free_all(&assemblyStructure, &fileContent); /* Program done */
    return 0;
}