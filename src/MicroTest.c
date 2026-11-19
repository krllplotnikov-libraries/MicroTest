#include "MicroTest.h"

static const char* fileName           = "";
static const char* currentTestName    = "";
static bool        currentTestFailed  = false;
static bool        currentTestIgnored = false;

static uint32_t    testsCount         = 0;
static uint32_t    failuresCount      = 0;
static uint32_t    ignoredCount       = 0;

static jmp_buf     abortFrame;

static bool    isAborted(void);
static void    beginFailure(uint32_t _line);
static void    endFailure(const char* _message);
static void    printLocation(uint32_t _line);
static void    printQuotedString(const char* _string, uint32_t _maxLength);
static void    printPointer(const void* _pointer);
static void    printNumber(int64_t _value, MicroTest_Display_et _display);
static uint8_t getDisplaySize(MicroTest_Display_et _display);
static bool    isDisplaySigned(MicroTest_Display_et _display);
static bool    isDisplayHex(MicroTest_Display_et _display);
static bool    compareNumbers(int64_t _threshold, int64_t _actual, MicroTest_Comparison_et _comparison, bool _signed);
static int64_t readArrayElement(const void* _array, uint32_t _index, MicroTest_Display_et _display);
static bool    checkArrayPointers(const void* _expected, const void* _actual, uint32_t _count, uint32_t _line, const char* _message);
static bool    floatsWithin(float _delta, float _expected, float _actual);

void MicroTest_Begin(const char* _fileName){
	fileName           = (_fileName != NULL) ? _fileName : "";
	currentTestName    = "";
	currentTestFailed  = false;
	currentTestIgnored = false;

	testsCount         = 0;
	failuresCount      = 0;
	ignoredCount       = 0;
}

uint32_t MicroTest_End(void){
	printf("\n-----------------------\n");
	printf("%" PRIu32 " Tests %" PRIu32 " Failures %" PRIu32 " Ignored\n", testsCount, failuresCount, ignoredCount);
	printf("%s\n", (failuresCount == 0) ? "OK" : "FAIL");
	fflush(stdout);

	return failuresCount;
}

void MicroTest_RunTest(MicroTest_TestFunction _test, const char* _testName, uint32_t _line){
	currentTestName    = _testName;
	currentTestFailed  = false;
	currentTestIgnored = false;

	if(setjmp(abortFrame) == 0){
		MicroTest_SetUp();
		_test();
	}
	if(setjmp(abortFrame) == 0){
		MicroTest_TearDown();
	}

	testsCount++;
	if(currentTestFailed){
		failuresCount++;
	}
	else if(currentTestIgnored){
		ignoredCount++;
	}
	else{
		printLocation(_line);
		printf("PASS\n");
		fflush(stdout);
	}
}

void MicroTest_Abort(void){
	longjmp(abortFrame, 1);
}

void MicroTest_Fail(uint32_t _line, const char* _message){
	if(isAborted()){
		return;
	}

	beginFailure(_line);
	printf("Failed");
	endFailure(_message);
}

void MicroTest_Ignore(uint32_t _line, const char* _message){
	if(isAborted()){
		return;
	}

	printLocation(_line);
	printf("IGNORE");
	if(_message != NULL){
		printf(": %s", _message);
	}
	printf("\n");
	fflush(stdout);

	currentTestIgnored = true;
}

bool MicroTest_AssertCondition(bool _condition, const char* _failureText, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(_condition){
		return true;
	}

	beginFailure(_line);
	printf("%s", _failureText);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertEqualPointer(const void* _expected, const void* _actual, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(_expected == _actual){
		return true;
	}

	beginFailure(_line);
	printf("Expected ");
	printPointer(_expected);
	printf(" Was ");
	printPointer(_actual);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertCompareNumber(int64_t _threshold, int64_t _actual, MicroTest_Comparison_et _comparison,
                                   MicroTest_Display_et _display, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(compareNumbers(_threshold, _actual, _comparison, isDisplaySigned(_display))){
		return true;
	}

	beginFailure(_line);
	switch(_comparison){
	case MICROTEST_COMPARISON_EQUAL:{
		printf("Expected ");
		break;
	}

	case MICROTEST_COMPARISON_NOT_EQUAL:{
		printf("Expected not equal to ");
		break;
	}

	case MICROTEST_COMPARISON_GREATER_THAN:{
		printf("Expected greater than ");
		break;
	}

	case MICROTEST_COMPARISON_GREATER_OR_EQUAL:{
		printf("Expected greater or equal to ");
		break;
	}

	case MICROTEST_COMPARISON_LESS_THAN:{
		printf("Expected less than ");
		break;
	}

	case MICROTEST_COMPARISON_LESS_OR_EQUAL:{
		printf("Expected less or equal to ");
		break;
	}

	default: break;
	}
	printNumber(_threshold, _display);
	printf(" Was ");
	printNumber(_actual, _display);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertNumberWithin(uint64_t _delta, int64_t _expected, int64_t _actual,
                                  MicroTest_Display_et _display, uint32_t _line, const char* _message){
	bool     actualIsGreater = false;
	uint64_t difference      = 0;

	if(isAborted()){
		return false;
	}

	actualIsGreater = isDisplaySigned(_display) ? (_actual > _expected) : ((uint64_t)_actual > (uint64_t)_expected);
	difference      = actualIsGreater ? ((uint64_t)_actual - (uint64_t)_expected) : ((uint64_t)_expected - (uint64_t)_actual);
	if(difference <= _delta){
		return true;
	}

	beginFailure(_line);
	printf("Expected ");
	printNumber(_expected, _display);
	printf(" +/- %" PRIu64 " Was ", _delta);
	printNumber(_actual, _display);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertEqualArray(const void* _expected, const void* _actual, uint32_t _count,
                                MicroTest_Display_et _display, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(checkArrayPointers(_expected, _actual, _count, _line, _message) == false){
		return false;
	}

	for(uint32_t i = 0; i < _count; i++){
		int64_t expectedElement = readArrayElement(_expected, i, _display);
		int64_t actualElement   = readArrayElement(_actual, i, _display);

		if(expectedElement != actualElement){
			beginFailure(_line);
			printf("Element %" PRIu32 " Expected ", i);
			printNumber(expectedElement, _display);
			printf(" Was ");
			printNumber(actualElement, _display);
			endFailure(_message);
			return false;
		}
	}
	return true;
}

bool MicroTest_AssertBits(uint64_t _mask, uint64_t _expected, uint64_t _actual, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if((_expected & _mask) == (_actual & _mask)){
		return true;
	}

	beginFailure(_line);
	printf("Expected 0x%" PRIX64 " Was 0x%" PRIX64 " (mask 0x%" PRIX64 ")", _expected & _mask, _actual & _mask, _mask);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertEqualString(const char* _expected, const char* _actual, uint32_t _maxLength, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(_expected == _actual){
		return true;
	}
	if((_expected != NULL) && (_actual != NULL) && (strncmp(_expected, _actual, _maxLength) == 0)){
		return true;
	}

	beginFailure(_line);
	printf("Expected ");
	printQuotedString(_expected, _maxLength);
	printf(" Was ");
	printQuotedString(_actual, _maxLength);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertEqualMemory(const void* _expected, const void* _actual, uint32_t _length, uint32_t _line, const char* _message){
	const uint8_t* expectedBytes = (const uint8_t*)_expected;
	const uint8_t* actualBytes   = (const uint8_t*)_actual;

	if(isAborted()){
		return false;
	}
	if(checkArrayPointers(_expected, _actual, _length, _line, _message) == false){
		return false;
	}

	for(uint32_t i = 0; i < _length; i++){
		if(expectedBytes[i] != actualBytes[i]){
			beginFailure(_line);
			printf("Memory mismatch at byte %" PRIu32 ". Expected 0x%02X Was 0x%02X", i, expectedBytes[i], actualBytes[i]);
			endFailure(_message);
			return false;
		}
	}
	return true;
}

bool MicroTest_AssertFloatWithin(float _delta, float _expected, float _actual, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(floatsWithin(_delta, _expected, _actual)){
		return true;
	}

	beginFailure(_line);
	printf("Expected %g +/- %g Was %g", (double)_expected, (double)fabsf(_delta), (double)_actual);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertEqualFloat(float _expected, float _actual, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(floatsWithin(_expected * MICROTEST_FLOAT_PRECISION, _expected, _actual)){
		return true;
	}

	beginFailure(_line);
	printf("Expected %g Was %g", (double)_expected, (double)_actual);
	endFailure(_message);
	return false;
}

bool MicroTest_AssertEqualFloatArray(const float* _expected, const float* _actual, uint32_t _count, uint32_t _line, const char* _message){
	if(isAborted()){
		return false;
	}
	if(checkArrayPointers(_expected, _actual, _count, _line, _message) == false){
		return false;
	}

	for(uint32_t i = 0; i < _count; i++){
		if(floatsWithin(_expected[i] * MICROTEST_FLOAT_PRECISION, _expected[i], _actual[i]) == false){
			beginFailure(_line);
			printf("Element %" PRIu32 " Expected %g Was %g", i, (double)_expected[i], (double)_actual[i]);
			endFailure(_message);
			return false;
		}
	}
	return true;
}

bool MicroTest_AssertFloatTrait(float _actual, MicroTest_FloatTrait_et _trait, uint32_t _line, const char* _message){
	bool        result       = false;
	const char* expectedText = "";

	if(isAborted()){
		return false;
	}

	switch(_trait){
	case MICROTEST_FLOAT_TRAIT_INF:{
		result       = isinf(_actual) && (_actual > 0.0f);
		expectedText = "Expected inf";
		break;
	}

	case MICROTEST_FLOAT_TRAIT_NEG_INF:{
		result       = isinf(_actual) && (_actual < 0.0f);
		expectedText = "Expected -inf";
		break;
	}

	case MICROTEST_FLOAT_TRAIT_NAN:{
		result       = isnan(_actual);
		expectedText = "Expected nan";
		break;
	}

	case MICROTEST_FLOAT_TRAIT_NOT_NAN:{
		result       = !isnan(_actual);
		expectedText = "Expected not nan";
		break;
	}

	default: break;
	}
	if(result){
		return true;
	}

	beginFailure(_line);
	printf("%s Was %g", expectedText, (double)_actual);
	endFailure(_message);
	return false;
}

//-----Weak-----
__attribute__((weak)) void MicroTest_SetUp(void){}
__attribute__((weak)) void MicroTest_TearDown(void){}
//-----Weak-----

static bool isAborted(void){
	return currentTestFailed || currentTestIgnored;
}

static void beginFailure(uint32_t _line){
	printLocation(_line);
	printf("FAIL: ");

	currentTestFailed = true;
}

static void endFailure(const char* _message){
	if(_message != NULL){
		printf(". %s", _message);
	}
	printf("\n");
	fflush(stdout);
}

static void printLocation(uint32_t _line){
	printf("%s:%" PRIu32 ":%s:", fileName, _line, currentTestName);
}

static void printQuotedString(const char* _string, uint32_t _maxLength){
	if(_string == NULL){
		printf("NULL");
		return;
	}

	printf("\"");
	for(uint32_t i = 0; (i < _maxLength) && (_string[i] != '\0'); i++){
		if((_string[i] >= ' ') && (_string[i] <= '~')){
			printf("%c", _string[i]);
		}
		else{
			printf("\\x%02X", (uint8_t)_string[i]);
		}
	}
	printf("\"");
}

static void printPointer(const void* _pointer){
	if(_pointer == NULL){
		printf("NULL");
	}
	else{
		printf("%p", _pointer);
	}
}

static void printNumber(int64_t _value, MicroTest_Display_et _display){
	if(isDisplayHex(_display)){
		printf("0x%0*" PRIX64, getDisplaySize(_display) * 2, (uint64_t)_value);
	}
	else if(isDisplaySigned(_display)){
		printf("%" PRId64, _value);
	}
	else{
		printf("%" PRIu64, (uint64_t)_value);
	}
}

static uint8_t getDisplaySize(MicroTest_Display_et _display){
	switch(_display){
	case MICROTEST_DISPLAY_INT:
	case MICROTEST_DISPLAY_UINT:{
		return (uint8_t)sizeof(int);
	}

	case MICROTEST_DISPLAY_INT8:
	case MICROTEST_DISPLAY_UINT8:
	case MICROTEST_DISPLAY_HEX8:{
		return 1;
	}

	case MICROTEST_DISPLAY_INT16:
	case MICROTEST_DISPLAY_UINT16:
	case MICROTEST_DISPLAY_HEX16:{
		return 2;
	}

	case MICROTEST_DISPLAY_INT32:
	case MICROTEST_DISPLAY_UINT32:
	case MICROTEST_DISPLAY_HEX32:{
		return 4;
	}

	case MICROTEST_DISPLAY_INT64:
	case MICROTEST_DISPLAY_UINT64:
	case MICROTEST_DISPLAY_HEX64:{
		return 8;
	}

	default: break;
	}
	return 0;
}

static bool isDisplaySigned(MicroTest_Display_et _display){
	return (_display == MICROTEST_DISPLAY_INT)   ||
	       (_display == MICROTEST_DISPLAY_INT8)  ||
	       (_display == MICROTEST_DISPLAY_INT16) ||
	       (_display == MICROTEST_DISPLAY_INT32) ||
	       (_display == MICROTEST_DISPLAY_INT64);
}

static bool isDisplayHex(MicroTest_Display_et _display){
	return (_display == MICROTEST_DISPLAY_HEX8)  ||
	       (_display == MICROTEST_DISPLAY_HEX16) ||
	       (_display == MICROTEST_DISPLAY_HEX32) ||
	       (_display == MICROTEST_DISPLAY_HEX64);
}

static bool compareNumbers(int64_t _threshold, int64_t _actual, MicroTest_Comparison_et _comparison, bool _signed){
	bool greater = _signed ? (_actual > _threshold) : ((uint64_t)_actual > (uint64_t)_threshold);
	bool equal   = (_actual == _threshold);

	switch(_comparison){
	case MICROTEST_COMPARISON_EQUAL:            return equal;
	case MICROTEST_COMPARISON_NOT_EQUAL:        return !equal;
	case MICROTEST_COMPARISON_GREATER_THAN:     return greater;
	case MICROTEST_COMPARISON_GREATER_OR_EQUAL: return greater || equal;
	case MICROTEST_COMPARISON_LESS_THAN:        return !(greater || equal);
	case MICROTEST_COMPARISON_LESS_OR_EQUAL:    return !greater;
	default: break;
	}
	return false;
}

static int64_t readArrayElement(const void* _array, uint32_t _index, MicroTest_Display_et _display){
	uint8_t        size    = getDisplaySize(_display);
	const uint8_t* element = (const uint8_t*)_array + ((size_t)_index * size);
	bool           sign    = isDisplaySigned(_display);

	switch(size){
	case 1: return sign ? (int64_t)*(const int8_t*)element  : (int64_t)*(const uint8_t*)element;
	case 2: return sign ? (int64_t)*(const int16_t*)element : (int64_t)*(const uint16_t*)element;
	case 4: return sign ? (int64_t)*(const int32_t*)element : (int64_t)*(const uint32_t*)element;
	case 8: return sign ? (int64_t)*(const int64_t*)element : (int64_t)*(const uint64_t*)element;
	default: break;
	}
	return 0;
}

static bool checkArrayPointers(const void* _expected, const void* _actual, uint32_t _count, uint32_t _line, const char* _message){
	const char* failureText = NULL;

	if(_count == 0){
		failureText = "Length is zero";
	}
	else if(_expected == NULL){
		failureText = "Expected pointer is NULL";
	}
	else if(_actual == NULL){
		failureText = "Actual pointer is NULL";
	}
	if(failureText == NULL){
		return true;
	}

	beginFailure(_line);
	printf("%s", failureText);
	endFailure(_message);
	return false;
}

static bool floatsWithin(float _delta, float _expected, float _actual){
	if(isinf(_expected) || isinf(_actual)){
		return _expected == _actual;
	}
	return fabsf(_actual - _expected) <= fabsf(_delta);
}
