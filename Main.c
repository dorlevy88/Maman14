//
// Created by Or Zamir on 7/16/16.
//

#include "DataStructures/AssemblyStructure.h"
#include "Transitions/FirstTransition.h"
#include "Transitions/SecondTransition.h"
#include "Main.h"


int main(int argc, char **argv) {
    //1. read .as files
    //2. iterate on each one of the files
    //3.0   create data array & code array (represent memory layout) : arrays type is bit12[]
    //3.1   crease ic & dc
    //3.2   create symbols tables
    //4.    Run transitions
    //5.    Translate arrays to special base8
    //6.    build .ob file --> (ic + ' ' + dc) code array and then data array (special base 8)
    //7.    build .ext .ent
    //8.    write .ob [.ext] [.ent] files for each one of the original files

    for (int fileCounter = 1; fileCounter < argc; ++fileCounter) {

        AssemblyStructure assemblyStructure;
        FileContent fileContent = getFileContent(argv[fileCounter]);

        for (int i=0; i < fileContent.numRows; i++){
            for (int j=0; j < 4; j++){
                printf("Row Number - %d, CellNumber - %d, Value = %s\n", i,j,fileContent.line[i].word[j]);
            }
            printf("\n");
        }

        RunFirstTransition(fileContent, assemblyStructure);
        //RunSecondTransition(f, assemblyStructure);
    }
}