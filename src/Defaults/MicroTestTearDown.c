#include "MicroTest.h"

// Default MicroTest_TearDown of the installed library. It is a separate archive member: the linker takes it
// only if the test file does not define its own MicroTest_TearDown, so weak symbols are not needed.
void MicroTest_TearDown(void){}
