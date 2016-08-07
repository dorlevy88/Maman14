#ifndef MAMAN14_TRANSITIONS_H
#define MAMAN14_TRANSITIONS_H

#include <stdbool.h>
#include "input_reader.h"
#include "utils.h"

Status runFirstTransition(FileContent *fileContent, AssemblyStructure *assembly);

Status runSecondTransition(FileContent *fileContent, AssemblyStructure *assembly);

#endif /* MAMAN14_TRANSITIONS_H */
