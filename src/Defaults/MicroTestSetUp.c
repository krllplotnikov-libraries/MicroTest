#include "MicroTest.h"

// Default MicroTest_SetUp of the installed library. It is a separate archive member: the linker takes it
// only if the test file does not define its own MicroTest_SetUp, so weak symbols are not needed.
void MicroTest_SetUp(void){}
