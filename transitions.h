#ifndef MAMAN14_TRANSITIONS_H
#define MAMAN14_TRANSITIONS_H

#include <stdbool.h>
#include "input_reader.h"
#include "utils.h"

/*
 * Maman 14 for the following students -
 * 1. Or Zamir - 301222063
 * 2. Dor Levy - 301641098
 */

Status runFirstTransition(FileContent *fileContent, AssemblyStructure *assembly);

Status runSecondTransition(FileContent *fileContent, AssemblyStructure *assembly);

#endif /* MAMAN14_TRANSITIONS_H */
