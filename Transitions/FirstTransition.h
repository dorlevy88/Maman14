//
// Created by Or Zamir on 7/16/16.
//
#ifndef MAMAN14_FIRSTTRASITIONS_H
#define MAMAN14_FIRSTTRASITIONS_H

#include <stdbool.h>
#include <memory.h>
#include <stdio.h>

#include "../Utilities/FileHandlers.h"
#include "../DataStructures/AssemblyStructure.h"
#include "../DataStructures/Command.h"

bool RunFirstTransition(FileContent* fileContent, AssemblyStructure* assembly);

#endif //MAMAN14_FIRSTTRASITIONS_H