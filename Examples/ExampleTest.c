#include <stdio.h>
#include "MicroTest.h"

//-----Variables-----
MicroTest_st microTest = {0};
static int   counter   = 0;
//-----Variables-----



//-----Function prototypes-----
static void putChar(char _character);
static void testNumbers(void);
static void testArraysAndStrings(void);
static void testBits(void);
static void testFloat(void);
static void testIgnored(void);
//-----Function prototypes-----



//-----Descriptors-----
MicroTest_Descriptor_st microTestDescriptor = {
	.putChar = putChar,
	.flush   = NULL,
};
//-----Descriptors-----



int main(void){
	MicroTest_Init(&microTest, &microTestDescriptor);

	MICROTEST_BEGIN();
	MICROTEST_RUN_TEST(testNumbers);
	MICROTEST_RUN_TEST(testArraysAndStrings);
	MICROTEST_RUN_TEST(testBits);
	MICROTEST_RUN_TEST(testFloat);
	MICROTEST_RUN_TEST(testIgnored);
	return (int)MICROTEST_END();
}



//-----Function definitions-----
static void putChar(char _character){
	putchar(_character);
}

void MicroTest_SetUp(MicroTest_st* _microTest){
	(void)_microTest;
	counter = 10;
}

static void testNumbers(void){
	MICROTEST_ASSERT_EQUAL(10, counter);
	MICROTEST_ASSERT_EQUAL_UINT8(255, 0xFF);
	MICROTEST_ASSERT_EQUAL_HEX16(0xBEEF, 0xBEEF);
	MICROTEST_ASSERT_NOT_EQUAL_INT(1, 2);
	MICROTEST_ASSERT_GREATER_THAN_INT32(5, 6);
	MICROTEST_ASSERT_LESS_OR_EQUAL_UINT16(100, 100);
	MICROTEST_ASSERT_INT_WITHIN(2, 10, 11);
	MICROTEST_ASSERT_TRUE_MESSAGE(counter > 0, "counter must be positive");
}

static void testArraysAndStrings(void){
	const int16_t expected[] = {-1, 2, -3};
	const int16_t actual[]   = {-1, 2, -3};

	MICROTEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, 3);
	MICROTEST_ASSERT_EQUAL_STRING("MicroTest", "MicroTest");
	MICROTEST_ASSERT_EQUAL_STRING_LEN("Micro", "MicroTest", 5);
	MICROTEST_ASSERT_EQUAL_MEMORY(expected, actual, sizeof(expected));
	MICROTEST_ASSERT_NOT_NULL(expected);
}

static void testBits(void){
	MICROTEST_ASSERT_BITS(0x0F, 0x05, 0xA5);
	MICROTEST_ASSERT_BIT_HIGH(7, 0x80);
	MICROTEST_ASSERT_BIT_LOW(0, 0x80);
}

static void testFloat(void){
	MICROTEST_ASSERT_EQUAL_FLOAT(0.1f + 0.2f, 0.3f);
	MICROTEST_ASSERT_FLOAT_WITHIN(0.01f, 3.14f, 3.141f);
}

static void testIgnored(void){
	MICROTEST_IGNORE_MESSAGE("not implemented yet");
}
//-----Function definitions-----
