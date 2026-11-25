#ifndef MICROTEST_H_
#define MICROTEST_H_

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>

//-----Configuration-----
// MICROTEST_FLOAT_PRECISION - relative precision of MICROTEST_ASSERT_EQUAL_FLOAT, can be overridden with -D.
// It is substituted in the test file, so it also works with the installed (precompiled) library.
#ifndef MICROTEST_FLOAT_PRECISION
#define MICROTEST_FLOAT_PRECISION 0.00001f
#endif
//-----Configuration-----

// C++ forbids forward declaration of enum without underlying type
#ifdef __cplusplus
#define MICROTEST_ENUM_BASE : int
#else
#define MICROTEST_ENUM_BASE
#endif

//-----Internal-----
#define MICROTEST_INTERNAL_LINE ((uint32_t)__LINE__)

#define MICROTEST_INTERNAL_ABORT() MicroTest_Abort()

#define MICROTEST_INTERNAL_CHECK(assertion) do{ if(!(assertion)){ MICROTEST_INTERNAL_ABORT(); } }while(0)

#define MICROTEST_INTERNAL_CAST_INT(value)    ((int64_t)(int)(value))
#define MICROTEST_INTERNAL_CAST_INT8(value)   ((int64_t)(int8_t)(value))
#define MICROTEST_INTERNAL_CAST_INT16(value)  ((int64_t)(int16_t)(value))
#define MICROTEST_INTERNAL_CAST_INT32(value)  ((int64_t)(int32_t)(value))
#define MICROTEST_INTERNAL_CAST_INT64(value)  ((int64_t)(value))
#define MICROTEST_INTERNAL_CAST_UINT(value)   ((int64_t)(unsigned int)(value))
#define MICROTEST_INTERNAL_CAST_UINT8(value)  ((int64_t)(uint8_t)(value))
#define MICROTEST_INTERNAL_CAST_UINT16(value) ((int64_t)(uint16_t)(value))
#define MICROTEST_INTERNAL_CAST_UINT32(value) ((int64_t)(uint32_t)(value))
#define MICROTEST_INTERNAL_CAST_UINT64(value) ((int64_t)(uint64_t)(value))
#define MICROTEST_INTERNAL_CAST_HEX8(value)   ((int64_t)(uint8_t)(value))
#define MICROTEST_INTERNAL_CAST_HEX16(value)  ((int64_t)(uint16_t)(value))
#define MICROTEST_INTERNAL_CAST_HEX32(value)  ((int64_t)(uint32_t)(value))
#define MICROTEST_INTERNAL_CAST_HEX64(value)  ((int64_t)(uint64_t)(value))

#define MICROTEST_INTERNAL_COMPARE(type, comparison, threshold, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertCompareNumber(MICROTEST_INTERNAL_CAST_##type(threshold), MICROTEST_INTERNAL_CAST_##type(actual), \
	                                                       MICROTEST_COMPARISON_##comparison, MICROTEST_DISPLAY_##type, MICROTEST_INTERNAL_LINE, (message)))

#define MICROTEST_INTERNAL_WITHIN(type, delta, expected, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertNumberWithin((uint64_t)(delta), MICROTEST_INTERNAL_CAST_##type(expected), MICROTEST_INTERNAL_CAST_##type(actual), \
	                                                      MICROTEST_DISPLAY_##type, MICROTEST_INTERNAL_LINE, (message)))

#define MICROTEST_INTERNAL_ARRAY(type, expected, actual, count, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualArray((const void*)(expected), (const void*)(actual), (uint32_t)(count), \
	                                                    MICROTEST_DISPLAY_##type, MICROTEST_INTERNAL_LINE, (message)))
//-----Internal-----

//-----Test control-----
#define MICROTEST_BEGIN()          MicroTest_Begin(__FILE__)
#define MICROTEST_END()            MicroTest_End()
#define MICROTEST_RUN_TEST(test)   MicroTest_RunTest((test), #test, MICROTEST_INTERNAL_LINE)

#define MICROTEST_PASS()                MICROTEST_INTERNAL_ABORT()
#define MICROTEST_FAIL()                MICROTEST_FAIL_MESSAGE(NULL)
#define MICROTEST_FAIL_MESSAGE(message) do{ MicroTest_Fail(MICROTEST_INTERNAL_LINE, (message)); MICROTEST_INTERNAL_ABORT(); }while(0)
#define MICROTEST_IGNORE()                MICROTEST_IGNORE_MESSAGE(NULL)
#define MICROTEST_IGNORE_MESSAGE(message) do{ MicroTest_Ignore(MICROTEST_INTERNAL_LINE, (message)); MICROTEST_INTERNAL_ABORT(); }while(0)
//-----Test control-----

//-----Boolean and pointer assertions-----
#define MICROTEST_ASSERT(condition)          MICROTEST_ASSERT_MESSAGE(condition, NULL)
#define MICROTEST_ASSERT_TRUE(condition)     MICROTEST_ASSERT_TRUE_MESSAGE(condition, NULL)
#define MICROTEST_ASSERT_FALSE(condition)    MICROTEST_ASSERT_FALSE_MESSAGE(condition, NULL)
#define MICROTEST_ASSERT_NULL(pointer)       MICROTEST_ASSERT_NULL_MESSAGE(pointer, NULL)
#define MICROTEST_ASSERT_NOT_NULL(pointer)   MICROTEST_ASSERT_NOT_NULL_MESSAGE(pointer, NULL)
#define MICROTEST_ASSERT_EQUAL_PTR(expected, actual) MICROTEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, NULL)

#define MICROTEST_ASSERT_MESSAGE(condition, message)        MICROTEST_INTERNAL_CHECK(MicroTest_AssertCondition((condition) ? true : false, "Expected TRUE Was FALSE", MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_TRUE_MESSAGE(condition, message)   MICROTEST_INTERNAL_CHECK(MicroTest_AssertCondition((condition) ? true : false, "Expected TRUE Was FALSE", MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_FALSE_MESSAGE(condition, message)  MICROTEST_INTERNAL_CHECK(MicroTest_AssertCondition((condition) ? false : true, "Expected FALSE Was TRUE", MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_NULL_MESSAGE(pointer, message)     MICROTEST_INTERNAL_CHECK(MicroTest_AssertCondition(((pointer) == NULL), "Expected NULL", MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_NOT_NULL_MESSAGE(pointer, message) MICROTEST_INTERNAL_CHECK(MicroTest_AssertCondition(((pointer) != NULL), "Expected not NULL", MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualPointer((const void*)(expected), (const void*)(actual), MICROTEST_INTERNAL_LINE, (message)))
//-----Boolean and pointer assertions-----

//-----Number assertions-----
#define MICROTEST_ASSERT_EQUAL(expected, actual)                       MICROTEST_ASSERT_EQUAL_INT(expected, actual)
#define MICROTEST_ASSERT_NOT_EQUAL(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_INT(expected, actual)
#define MICROTEST_ASSERT_EQUAL_MESSAGE(expected, actual, message)      MICROTEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_MESSAGE(expected, actual, message)  MICROTEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, message)

#define MICROTEST_ASSERT_EQUAL_INT(expected, actual)                     MICROTEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_INT8(expected, actual)                    MICROTEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_INT16(expected, actual)                   MICROTEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_INT32(expected, actual)                   MICROTEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_INT64(expected, actual)                   MICROTEST_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT(expected, actual)                    MICROTEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT8(expected, actual)                   MICROTEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT16(expected, actual)                  MICROTEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT32(expected, actual)                  MICROTEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT64(expected, actual)                  MICROTEST_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX8(expected, actual)                    MICROTEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX16(expected, actual)                   MICROTEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX32(expected, actual)                   MICROTEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX64(expected, actual)                   MICROTEST_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)    MICROTEST_INTERNAL_COMPARE(INT, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, message)   MICROTEST_INTERNAL_COMPARE(INT8, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(INT16, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(INT32, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_INT64_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(INT64, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, message)   MICROTEST_INTERNAL_COMPARE(UINT, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(UINT8, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, message) MICROTEST_INTERNAL_COMPARE(UINT16, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, message) MICROTEST_INTERNAL_COMPARE(UINT32, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_UINT64_MESSAGE(expected, actual, message) MICROTEST_INTERNAL_COMPARE(UINT64, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, message)   MICROTEST_INTERNAL_COMPARE(HEX8, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX16, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX32, EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_EQUAL_HEX64_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX64, EQUAL, expected, actual, message)

#define MICROTEST_ASSERT_NOT_EQUAL_INT(expected, actual)                     MICROTEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_INT8(expected, actual)                    MICROTEST_ASSERT_NOT_EQUAL_INT8_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_INT16(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_INT16_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_INT32(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_INT32_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_INT64(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_INT64_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT(expected, actual)                    MICROTEST_ASSERT_NOT_EQUAL_UINT_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT8(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_UINT8_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT16(expected, actual)                  MICROTEST_ASSERT_NOT_EQUAL_UINT16_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT32(expected, actual)                  MICROTEST_ASSERT_NOT_EQUAL_UINT32_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT64(expected, actual)                  MICROTEST_ASSERT_NOT_EQUAL_UINT64_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX8(expected, actual)                    MICROTEST_ASSERT_NOT_EQUAL_HEX8_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX16(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_HEX16_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX32(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_HEX32_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX64(expected, actual)                   MICROTEST_ASSERT_NOT_EQUAL_HEX64_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, message)    MICROTEST_INTERNAL_COMPARE(INT, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_INT8_MESSAGE(expected, actual, message)   MICROTEST_INTERNAL_COMPARE(INT8, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_INT16_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(INT16, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_INT32_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(INT32, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_INT64_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(INT64, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT_MESSAGE(expected, actual, message)   MICROTEST_INTERNAL_COMPARE(UINT, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT8_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(UINT8, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT16_MESSAGE(expected, actual, message) MICROTEST_INTERNAL_COMPARE(UINT16, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT32_MESSAGE(expected, actual, message) MICROTEST_INTERNAL_COMPARE(UINT32, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_UINT64_MESSAGE(expected, actual, message) MICROTEST_INTERNAL_COMPARE(UINT64, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX8_MESSAGE(expected, actual, message)   MICROTEST_INTERNAL_COMPARE(HEX8, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX16_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX16, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX32_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX32, NOT_EQUAL, expected, actual, message)
#define MICROTEST_ASSERT_NOT_EQUAL_HEX64_MESSAGE(expected, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX64, NOT_EQUAL, expected, actual, message)

#define MICROTEST_ASSERT_GREATER_THAN_INT(threshold, actual)                     MICROTEST_ASSERT_GREATER_THAN_INT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_INT8(threshold, actual)                    MICROTEST_ASSERT_GREATER_THAN_INT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_INT16(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_INT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_INT32(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_INT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_INT64(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_INT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_UINT(threshold, actual)                    MICROTEST_ASSERT_GREATER_THAN_UINT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_UINT8(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_UINT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_UINT16(threshold, actual)                  MICROTEST_ASSERT_GREATER_THAN_UINT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_UINT32(threshold, actual)                  MICROTEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_UINT64(threshold, actual)                  MICROTEST_ASSERT_GREATER_THAN_UINT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_HEX8(threshold, actual)                    MICROTEST_ASSERT_GREATER_THAN_HEX8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_HEX16(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_HEX16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_HEX32(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_HEX32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_HEX64(threshold, actual)                   MICROTEST_ASSERT_GREATER_THAN_HEX64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_THAN_INT_MESSAGE(threshold, actual, message)    MICROTEST_INTERNAL_COMPARE(INT, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_INT8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(INT8, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_INT16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT16, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_INT32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT32, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_INT64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT64, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_UINT_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(UINT, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_UINT8_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(UINT8, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_UINT16_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT16, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT32, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_UINT64_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT64, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_HEX8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(HEX8, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_HEX16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX16, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_HEX32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX32, GREATER_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_THAN_HEX64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX64, GREATER_THAN, threshold, actual, message)

#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual)                     MICROTEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT8(threshold, actual)                    MICROTEST_ASSERT_GREATER_OR_EQUAL_INT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT16(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT32(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_INT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT64(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_INT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual)                    MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT8(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT16(threshold, actual)                  MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT32(threshold, actual)                  MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT64(threshold, actual)                  MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX8(threshold, actual)                    MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX16(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX32(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX64(threshold, actual)                   MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(threshold, actual, message)    MICROTEST_INTERNAL_COMPARE(INT, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(INT8, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT16, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT32, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_INT64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT64, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(UINT, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT8_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(UINT8, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT16_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT16, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT32_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT32, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT64, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(HEX8, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX16, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX32, GREATER_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_GREATER_OR_EQUAL_HEX64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX64, GREATER_OR_EQUAL, threshold, actual, message)

#define MICROTEST_ASSERT_LESS_THAN_INT(threshold, actual)                     MICROTEST_ASSERT_LESS_THAN_INT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_INT8(threshold, actual)                    MICROTEST_ASSERT_LESS_THAN_INT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_INT16(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_INT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_INT32(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_INT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_INT64(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_INT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_UINT(threshold, actual)                    MICROTEST_ASSERT_LESS_THAN_UINT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_UINT8(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_UINT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_UINT16(threshold, actual)                  MICROTEST_ASSERT_LESS_THAN_UINT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_UINT32(threshold, actual)                  MICROTEST_ASSERT_LESS_THAN_UINT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_UINT64(threshold, actual)                  MICROTEST_ASSERT_LESS_THAN_UINT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_HEX8(threshold, actual)                    MICROTEST_ASSERT_LESS_THAN_HEX8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_HEX16(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_HEX16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_HEX32(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_HEX32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_HEX64(threshold, actual)                   MICROTEST_ASSERT_LESS_THAN_HEX64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_THAN_INT_MESSAGE(threshold, actual, message)    MICROTEST_INTERNAL_COMPARE(INT, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_INT8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(INT8, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_INT16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT16, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_INT32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT32, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_INT64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT64, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_UINT_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(UINT, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_UINT8_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(UINT8, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_UINT16_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT16, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_UINT32_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT32, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_UINT64_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT64, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_HEX8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(HEX8, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_HEX16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX16, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_HEX32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX32, LESS_THAN, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_THAN_HEX64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX64, LESS_THAN, threshold, actual, message)

#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT(threshold, actual)                     MICROTEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT8(threshold, actual)                    MICROTEST_ASSERT_LESS_OR_EQUAL_INT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT16(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT32(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_INT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT64(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_INT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT(threshold, actual)                    MICROTEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT8(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_UINT8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT16(threshold, actual)                  MICROTEST_ASSERT_LESS_OR_EQUAL_UINT16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT32(threshold, actual)                  MICROTEST_ASSERT_LESS_OR_EQUAL_UINT32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT64(threshold, actual)                  MICROTEST_ASSERT_LESS_OR_EQUAL_UINT64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX8(threshold, actual)                    MICROTEST_ASSERT_LESS_OR_EQUAL_HEX8_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX16(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_HEX16_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX32(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_HEX32_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX64(threshold, actual)                   MICROTEST_ASSERT_LESS_OR_EQUAL_HEX64_MESSAGE(threshold, actual, NULL)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT_MESSAGE(threshold, actual, message)    MICROTEST_INTERNAL_COMPARE(INT, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(INT8, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT16, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT32, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_INT64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(INT64, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(UINT, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT8_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(UINT8, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT16_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT16, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT32_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT32, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_UINT64_MESSAGE(threshold, actual, message) MICROTEST_INTERNAL_COMPARE(UINT64, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX8_MESSAGE(threshold, actual, message)   MICROTEST_INTERNAL_COMPARE(HEX8, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX16_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX16, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX32_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX32, LESS_OR_EQUAL, threshold, actual, message)
#define MICROTEST_ASSERT_LESS_OR_EQUAL_HEX64_MESSAGE(threshold, actual, message)  MICROTEST_INTERNAL_COMPARE(HEX64, LESS_OR_EQUAL, threshold, actual, message)

#define MICROTEST_ASSERT_INT_WITHIN(delta, expected, actual)                     MICROTEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_INT8_WITHIN(delta, expected, actual)                    MICROTEST_ASSERT_INT8_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_INT16_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_INT16_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_INT32_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_INT32_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_INT64_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_INT64_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_UINT_WITHIN(delta, expected, actual)                    MICROTEST_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_UINT8_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_UINT8_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_UINT16_WITHIN(delta, expected, actual)                  MICROTEST_ASSERT_UINT16_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_UINT32_WITHIN(delta, expected, actual)                  MICROTEST_ASSERT_UINT32_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_UINT64_WITHIN(delta, expected, actual)                  MICROTEST_ASSERT_UINT64_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_HEX8_WITHIN(delta, expected, actual)                    MICROTEST_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_HEX16_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_HEX32_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_HEX64_WITHIN(delta, expected, actual)                   MICROTEST_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_INT_WITHIN_MESSAGE(delta, expected, actual, message)    MICROTEST_INTERNAL_WITHIN(INT, delta, expected, actual, message)
#define MICROTEST_ASSERT_INT8_WITHIN_MESSAGE(delta, expected, actual, message)   MICROTEST_INTERNAL_WITHIN(INT8, delta, expected, actual, message)
#define MICROTEST_ASSERT_INT16_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(INT16, delta, expected, actual, message)
#define MICROTEST_ASSERT_INT32_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(INT32, delta, expected, actual, message)
#define MICROTEST_ASSERT_INT64_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(INT64, delta, expected, actual, message)
#define MICROTEST_ASSERT_UINT_WITHIN_MESSAGE(delta, expected, actual, message)   MICROTEST_INTERNAL_WITHIN(UINT, delta, expected, actual, message)
#define MICROTEST_ASSERT_UINT8_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(UINT8, delta, expected, actual, message)
#define MICROTEST_ASSERT_UINT16_WITHIN_MESSAGE(delta, expected, actual, message) MICROTEST_INTERNAL_WITHIN(UINT16, delta, expected, actual, message)
#define MICROTEST_ASSERT_UINT32_WITHIN_MESSAGE(delta, expected, actual, message) MICROTEST_INTERNAL_WITHIN(UINT32, delta, expected, actual, message)
#define MICROTEST_ASSERT_UINT64_WITHIN_MESSAGE(delta, expected, actual, message) MICROTEST_INTERNAL_WITHIN(UINT64, delta, expected, actual, message)
#define MICROTEST_ASSERT_HEX8_WITHIN_MESSAGE(delta, expected, actual, message)   MICROTEST_INTERNAL_WITHIN(HEX8, delta, expected, actual, message)
#define MICROTEST_ASSERT_HEX16_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(HEX16, delta, expected, actual, message)
#define MICROTEST_ASSERT_HEX32_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(HEX32, delta, expected, actual, message)
#define MICROTEST_ASSERT_HEX64_WITHIN_MESSAGE(delta, expected, actual, message)  MICROTEST_INTERNAL_WITHIN(HEX64, delta, expected, actual, message)

#define MICROTEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, count)                     MICROTEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_INT8_ARRAY(expected, actual, count)                    MICROTEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_INT16_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_INT32_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_INT64_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, count)                    MICROTEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT16_ARRAY(expected, actual, count)                  MICROTEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT32_ARRAY(expected, actual, count)                  MICROTEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_UINT64_ARRAY(expected, actual, count)                  MICROTEST_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, count)                    MICROTEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX16_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX32_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_HEX64_ARRAY(expected, actual, count)                   MICROTEST_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected, actual, count, message)    MICROTEST_INTERNAL_ARRAY(INT, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_INT8_ARRAY_MESSAGE(expected, actual, count, message)   MICROTEST_INTERNAL_ARRAY(INT8, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_INT16_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(INT16, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(INT32, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_INT64_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(INT64, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_UINT_ARRAY_MESSAGE(expected, actual, count, message)   MICROTEST_INTERNAL_ARRAY(UINT, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(UINT8, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, actual, count, message) MICROTEST_INTERNAL_ARRAY(UINT16, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, actual, count, message) MICROTEST_INTERNAL_ARRAY(UINT32, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_UINT64_ARRAY_MESSAGE(expected, actual, count, message) MICROTEST_INTERNAL_ARRAY(UINT64, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_HEX8_ARRAY_MESSAGE(expected, actual, count, message)   MICROTEST_INTERNAL_ARRAY(HEX8, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_HEX16_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(HEX16, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_HEX32_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(HEX32, expected, actual, count, message)
#define MICROTEST_ASSERT_EQUAL_HEX64_ARRAY_MESSAGE(expected, actual, count, message)  MICROTEST_INTERNAL_ARRAY(HEX64, expected, actual, count, message)
//-----Number assertions-----

//-----Bit assertions-----
#define MICROTEST_ASSERT_BITS(mask, expected, actual) MICROTEST_ASSERT_BITS_MESSAGE(mask, expected, actual, NULL)
#define MICROTEST_ASSERT_BITS_HIGH(mask, actual)      MICROTEST_ASSERT_BITS_HIGH_MESSAGE(mask, actual, NULL)
#define MICROTEST_ASSERT_BITS_LOW(mask, actual)       MICROTEST_ASSERT_BITS_LOW_MESSAGE(mask, actual, NULL)
#define MICROTEST_ASSERT_BIT_HIGH(bit, actual)        MICROTEST_ASSERT_BIT_HIGH_MESSAGE(bit, actual, NULL)
#define MICROTEST_ASSERT_BIT_LOW(bit, actual)         MICROTEST_ASSERT_BIT_LOW_MESSAGE(bit, actual, NULL)

#define MICROTEST_ASSERT_BITS_MESSAGE(mask, expected, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertBits((uint64_t)(mask), (uint64_t)(expected), (uint64_t)(actual), MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_BITS_HIGH_MESSAGE(mask, actual, message) MICROTEST_ASSERT_BITS_MESSAGE((mask), ~(uint64_t)0, (actual), (message))
#define MICROTEST_ASSERT_BITS_LOW_MESSAGE(mask, actual, message)  MICROTEST_ASSERT_BITS_MESSAGE((mask), (uint64_t)0, (actual), (message))
#define MICROTEST_ASSERT_BIT_HIGH_MESSAGE(bit, actual, message)   MICROTEST_ASSERT_BITS_MESSAGE(((uint64_t)1 << (bit)), ~(uint64_t)0, (actual), (message))
#define MICROTEST_ASSERT_BIT_LOW_MESSAGE(bit, actual, message)    MICROTEST_ASSERT_BITS_MESSAGE(((uint64_t)1 << (bit)), (uint64_t)0, (actual), (message))
//-----Bit assertions-----

//-----String and memory assertions-----
#define MICROTEST_ASSERT_EQUAL_STRING(expected, actual)             MICROTEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_STRING_LEN(expected, actual, length) MICROTEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, length, NULL)
#define MICROTEST_ASSERT_EQUAL_MEMORY(expected, actual, length)     MICROTEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, length, NULL)

#define MICROTEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualString((const char*)(expected), (const char*)(actual), UINT32_MAX, MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(expected, actual, length, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualString((const char*)(expected), (const char*)(actual), (uint32_t)(length), MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_EQUAL_MEMORY_MESSAGE(expected, actual, length, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualMemory((const void*)(expected), (const void*)(actual), (uint32_t)(length), MICROTEST_INTERNAL_LINE, (message)))
//-----String and memory assertions-----

//-----Float assertions-----
#define MICROTEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)      MICROTEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_FLOAT(expected, actual)              MICROTEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, NULL)
#define MICROTEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, count) MICROTEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, count, NULL)
#define MICROTEST_ASSERT_FLOAT_IS_INF(actual)                       MICROTEST_ASSERT_FLOAT_IS_INF_MESSAGE(actual, NULL)
#define MICROTEST_ASSERT_FLOAT_IS_NEG_INF(actual)                   MICROTEST_ASSERT_FLOAT_IS_NEG_INF_MESSAGE(actual, NULL)
#define MICROTEST_ASSERT_FLOAT_IS_NAN(actual)                       MICROTEST_ASSERT_FLOAT_IS_NAN_MESSAGE(actual, NULL)
#define MICROTEST_ASSERT_FLOAT_IS_NOT_NAN(actual)                   MICROTEST_ASSERT_FLOAT_IS_NOT_NAN_MESSAGE(actual, NULL)

#define MICROTEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertFloatWithin((float)(delta), (float)(expected), (float)(actual), MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_EQUAL_FLOAT_MESSAGE(expected, actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualFloat((float)(expected), (float)(actual), (float)(MICROTEST_FLOAT_PRECISION), MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(expected, actual, count, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertEqualFloatArray((const float*)(expected), (const float*)(actual), (uint32_t)(count), (float)(MICROTEST_FLOAT_PRECISION), \
	                                                         MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_FLOAT_IS_INF_MESSAGE(actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertFloatTrait((float)(actual), MICROTEST_FLOAT_TRAIT_INF, MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_FLOAT_IS_NEG_INF_MESSAGE(actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertFloatTrait((float)(actual), MICROTEST_FLOAT_TRAIT_NEG_INF, MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_FLOAT_IS_NAN_MESSAGE(actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertFloatTrait((float)(actual), MICROTEST_FLOAT_TRAIT_NAN, MICROTEST_INTERNAL_LINE, (message)))
#define MICROTEST_ASSERT_FLOAT_IS_NOT_NAN_MESSAGE(actual, message) \
	MICROTEST_INTERNAL_CHECK(MicroTest_AssertFloatTrait((float)(actual), MICROTEST_FLOAT_TRAIT_NOT_NAN, MICROTEST_INTERNAL_LINE, (message)))
//-----Float assertions-----

#ifdef __cplusplus
enum MicroTest_Display    MICROTEST_ENUM_BASE;
enum MicroTest_Comparison MICROTEST_ENUM_BASE;
enum MicroTest_FloatTrait MICROTEST_ENUM_BASE;
#endif
typedef enum MicroTest_Display    MicroTest_Display_et;
typedef enum MicroTest_Comparison MicroTest_Comparison_et;
typedef enum MicroTest_FloatTrait MicroTest_FloatTrait_et;

typedef void (*MicroTest_TestFunction)(void);

enum MicroTest_Display MICROTEST_ENUM_BASE{
	MICROTEST_DISPLAY_INT,
	MICROTEST_DISPLAY_INT8,
	MICROTEST_DISPLAY_INT16,
	MICROTEST_DISPLAY_INT32,
	MICROTEST_DISPLAY_INT64,
	MICROTEST_DISPLAY_UINT,
	MICROTEST_DISPLAY_UINT8,
	MICROTEST_DISPLAY_UINT16,
	MICROTEST_DISPLAY_UINT32,
	MICROTEST_DISPLAY_UINT64,
	MICROTEST_DISPLAY_HEX8,
	MICROTEST_DISPLAY_HEX16,
	MICROTEST_DISPLAY_HEX32,
	MICROTEST_DISPLAY_HEX64
};

enum MicroTest_Comparison MICROTEST_ENUM_BASE{
	MICROTEST_COMPARISON_EQUAL,
	MICROTEST_COMPARISON_NOT_EQUAL,
	MICROTEST_COMPARISON_GREATER_THAN,
	MICROTEST_COMPARISON_GREATER_OR_EQUAL,
	MICROTEST_COMPARISON_LESS_THAN,
	MICROTEST_COMPARISON_LESS_OR_EQUAL
};

enum MicroTest_FloatTrait MICROTEST_ENUM_BASE{
	MICROTEST_FLOAT_TRAIT_INF,
	MICROTEST_FLOAT_TRAIT_NEG_INF,
	MICROTEST_FLOAT_TRAIT_NAN,
	MICROTEST_FLOAT_TRAIT_NOT_NAN
};

#ifdef __cplusplus
extern "C" {
#endif

void     MicroTest_Begin(const char* _fileName);
uint32_t MicroTest_End(void);
void     MicroTest_RunTest(MicroTest_TestFunction _test, const char* _testName, uint32_t _line);
void     MicroTest_Abort(void);
void     MicroTest_Fail(uint32_t _line, const char* _message);
void     MicroTest_Ignore(uint32_t _line, const char* _message);

bool     MicroTest_AssertCondition(bool _condition, const char* _failureText, uint32_t _line, const char* _message);
bool     MicroTest_AssertEqualPointer(const void* _expected, const void* _actual, uint32_t _line, const char* _message);
bool     MicroTest_AssertCompareNumber(int64_t _threshold, int64_t _actual, MicroTest_Comparison_et _comparison,
                                       MicroTest_Display_et _display, uint32_t _line, const char* _message);
bool     MicroTest_AssertNumberWithin(uint64_t _delta, int64_t _expected, int64_t _actual,
                                      MicroTest_Display_et _display, uint32_t _line, const char* _message);
bool     MicroTest_AssertEqualArray(const void* _expected, const void* _actual, uint32_t _count,
                                    MicroTest_Display_et _display, uint32_t _line, const char* _message);
bool     MicroTest_AssertBits(uint64_t _mask, uint64_t _expected, uint64_t _actual, uint32_t _line, const char* _message);
bool     MicroTest_AssertEqualString(const char* _expected, const char* _actual, uint32_t _maxLength, uint32_t _line, const char* _message);
bool     MicroTest_AssertEqualMemory(const void* _expected, const void* _actual, uint32_t _length, uint32_t _line, const char* _message);
bool     MicroTest_AssertFloatWithin(float _delta, float _expected, float _actual, uint32_t _line, const char* _message);
bool     MicroTest_AssertEqualFloat(float _expected, float _actual, float _precision, uint32_t _line, const char* _message);
bool     MicroTest_AssertEqualFloatArray(const float* _expected, const float* _actual, uint32_t _count, float _precision,
                                         uint32_t _line, const char* _message);
bool     MicroTest_AssertFloatTrait(float _actual, MicroTest_FloatTrait_et _trait, uint32_t _line, const char* _message);

//-----Weak-----
void     MicroTest_SetUp(void);
void     MicroTest_TearDown(void);
//-----Weak-----

#ifdef __cplusplus
}
#endif

#endif
