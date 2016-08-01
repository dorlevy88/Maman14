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
        FileContent fileContent = getFileContent(argv[fileCounter]);
        if (fileContent.hasError) { //Something went wrong with the file loading
            continue;
        }
        for (int i=0; i < fileContent.size; i++) {
            for (int j=0; j < 4; j++){
                printf("Row Number - %d, CellNumber - %d, Value = %s\n", i,j,fileContent.line[i].originalLine);
            }
            printf("\n");
        }

        //TODO: Check file validity
        //On Error in file validity (or any error) we need to print into stderr and not stdout
        //If one of the files has error we need to move print error and move to the next file

        //NOTE:Valid label starts with a character and only has chars and numbers and max length is 30
        //NOTE: between the label and the order we need to have at least one space
        //NOTE: if a number is bigger than 15 bits --> Error
        //NOTE: Choose for each line if it's empty\comment\guide(data)\command(code) sentence

        AssemblyStructure* assemblyStructure = InitAssemblyStructure();
        RunFirstTransition(fileContent, *assemblyStructure);
        RunSecondTransition(fileContent, *assemblyStructure);

        //TODO: Translate Assembly Structure to special * base
        //TODO: Save Files: .ob .ent. ext
    }
}