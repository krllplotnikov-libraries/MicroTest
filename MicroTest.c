#include "MicroTest.h"

MicroTest_st* microTestInstance = NULL;

static bool    isAborted(MicroTest_st* _microTest);
static void    beginFailure(MicroTest_st* _microTest, uint32_t _line);
static void    endFailure(MicroTest_st* _microTest, const char* _message);
static void    printLocation(MicroTest_st* _microTest, uint32_t _line);
static void    printNewLine(MicroTest_st* _microTest);
static void    printCharacter(MicroTest_st* _microTest, char _character);
static void    printString(MicroTest_st* _microTest, const char* _string);
static void    printQuotedString(MicroTest_st* _microTest, const char* _string, uint32_t _maxLength);
static void    printUnsigned(MicroTest_st* _microTest, MicroTest_UInt _value);
static void    printSigned(MicroTest_st* _microTest, MicroTest_Int _value);
static void    printHex(MicroTest_st* _microTest, MicroTest_UInt _value, uint8_t _nibbles);
static void    printHexDigits(MicroTest_st* _microTest, MicroTest_UInt _value, uint8_t _nibbles);
static void    printPointer(MicroTest_st* _microTest, const void* _pointer);
static void    printNumber(MicroTest_st* _microTest, MicroTest_Int _value, MicroTest_Display_et _display);
static uint8_t getDisplaySize(MicroTest_Display_et _display);
static bool    isDisplaySigned(MicroTest_Display_et _display);
static bool    isDisplayHex(MicroTest_Display_et _display);
static bool    compareNumbers(MicroTest_Int _threshold, MicroTest_Int _actual, MicroTest_Comparison_et _comparison, bool _signed);
static MicroTest_Int readArrayElement(const void* _array, uint32_t _index, MicroTest_Display_et _display);
static bool    checkArrayPointers(MicroTest_st* _microTest, const void* _expected, const void* _actual, uint32_t _count, uint32_t _line, const char* _message);
#ifndef MICROTEST_EXCLUDE_FLOAT
static bool    floatIsNan(float _value);
static bool    floatIsInf(float _value);
static bool    floatsWithin(float _delta, float _expected, float _actual);
static void    printFloat(MicroTest_st* _microTest, float _value);
#endif

void MicroTest_Init(MicroTest_st* _microTest, MicroTest_Descriptor_st* _descriptor){
	microTestInstance = _microTest;

	_microTest->descriptor.putChar = _descriptor->putChar;
	_microTest->descriptor.flush   = _descriptor->flush;

	_microTest->fileName           = "";
	_microTest->currentTestName    = "";
	_microTest->currentTestLine    = 0;
	_microTest->currentTestFailed  = false;
	_microTest->currentTestIgnored = false;

	_microTest->testsCount         = 0;
	_microTest->failuresCount      = 0;
	_microTest->ignoredCount       = 0;
}

void MicroTest_Begin(MicroTest_st* _microTest, const char* _fileName){
	_microTest->fileName = (_fileName != NULL) ? _fileName : "";
}

uint32_t MicroTest_End(MicroTest_st* _microTest){
	printNewLine(_microTest);
	printString(_microTest, "-----------------------");
	printNewLine(_microTest);
	printUnsigned(_microTest, _microTest->testsCount);
	printString(_microTest, " Tests ");
	printUnsigned(_microTest, _microTest->failuresCount);
	printString(_microTest, " Failures ");
	printUnsigned(_microTest, _microTest->ignoredCount);
	printString(_microTest, " Ignored");
	printNewLine(_microTest);
	printString(_microTest, (_microTest->failuresCount == 0) ? "OK" : "FAIL");
	printNewLine(_microTest);

	return _microTest->failuresCount;
}

void MicroTest_RunTest(MicroTest_st* _microTest, MicroTest_TestFunction _test, const char* _testName, uint32_t _line){
	_microTest->currentTestName    = _testName;
	_microTest->currentTestLine    = _line;
	_microTest->currentTestFailed  = false;
	_microTest->currentTestIgnored = false;

#ifndef MICROTEST_EXCLUDE_SETJMP
	if(setjmp(_microTest->abortFrame) == 0){
		MicroTest_SetUp(_microTest);
		_test();
	}
	if(setjmp(_microTest->abortFrame) == 0){
		MicroTest_TearDown(_microTest);
	}
#else
	MicroTest_SetUp(_microTest);
	if(isAborted(_microTest) == false){
		_test();
	}
	MicroTest_TearDown(_microTest);
#endif

	_microTest->testsCount++;
	if(_microTest->currentTestFailed){
		_microTest->failuresCount++;
	}
	else if(_microTest->currentTestIgnored){
		_microTest->ignoredCount++;
	}
	else{
		printLocation(_microTest, _line);
		printString(_microTest, "PASS");
		printNewLine(_microTest);
	}
}

#ifndef MICROTEST_EXCLUDE_SETJMP
void MicroTest_Abort(MicroTest_st* _microTest){
	longjmp(_microTest->abortFrame, 1);
}
#endif

void MicroTest_Fail(MicroTest_st* _microTest, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Failed");
	endFailure(_microTest, _message);
}

void MicroTest_Ignore(MicroTest_st* _microTest, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return;
	}

	printLocation(_microTest, _line);
	printString(_microTest, "IGNORE");
	if(_message != NULL){
		printString(_microTest, ": ");
		printString(_microTest, _message);
	}
	printNewLine(_microTest);

	_microTest->currentTestIgnored = true;
}

bool MicroTest_AssertCondition(MicroTest_st* _microTest, bool _condition, const char* _failureText, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(_condition){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, _failureText);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertEqualPointer(MicroTest_st* _microTest, const void* _expected, const void* _actual, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(_expected == _actual){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Expected ");
	printPointer(_microTest, _expected);
	printString(_microTest, " Was ");
	printPointer(_microTest, _actual);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertCompareNumber(MicroTest_st* _microTest, MicroTest_Int _threshold, MicroTest_Int _actual, MicroTest_Comparison_et _comparison,
                                   MicroTest_Display_et _display, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(compareNumbers(_threshold, _actual, _comparison, isDisplaySigned(_display))){
		return true;
	}

	beginFailure(_microTest, _line);
	switch(_comparison){
	case MICROTEST_COMPARISON_EQUAL:{
		printString(_microTest, "Expected ");
		break;
	}

	case MICROTEST_COMPARISON_NOT_EQUAL:{
		printString(_microTest, "Expected not equal to ");
		break;
	}

	case MICROTEST_COMPARISON_GREATER_THAN:{
		printString(_microTest, "Expected greater than ");
		break;
	}

	case MICROTEST_COMPARISON_GREATER_OR_EQUAL:{
		printString(_microTest, "Expected greater or equal to ");
		break;
	}

	case MICROTEST_COMPARISON_LESS_THAN:{
		printString(_microTest, "Expected less than ");
		break;
	}

	case MICROTEST_COMPARISON_LESS_OR_EQUAL:{
		printString(_microTest, "Expected less or equal to ");
		break;
	}

	default: break;
	}
	printNumber(_microTest, _threshold, _display);
	printString(_microTest, " Was ");
	printNumber(_microTest, _actual, _display);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertNumberWithin(MicroTest_st* _microTest, MicroTest_UInt _delta, MicroTest_Int _expected, MicroTest_Int _actual,
                                  MicroTest_Display_et _display, uint32_t _line, const char* _message){
	MicroTest_UInt difference = 0;

	if(isAborted(_microTest)){
		return false;
	}

	if(isDisplaySigned(_display)){
		difference = (_actual > _expected) ? ((MicroTest_UInt)_actual - (MicroTest_UInt)_expected) : ((MicroTest_UInt)_expected - (MicroTest_UInt)_actual);
	}
	else{
		difference = ((MicroTest_UInt)_actual > (MicroTest_UInt)_expected) ? ((MicroTest_UInt)_actual - (MicroTest_UInt)_expected)
		                                                                   : ((MicroTest_UInt)_expected - (MicroTest_UInt)_actual);
	}
	if(difference <= _delta){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Expected ");
	printNumber(_microTest, _expected, _display);
	printString(_microTest, " +/- ");
	printUnsigned(_microTest, _delta);
	printString(_microTest, " Was ");
	printNumber(_microTest, _actual, _display);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertEqualArray(MicroTest_st* _microTest, const void* _expected, const void* _actual, uint32_t _count,
                                MicroTest_Display_et _display, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(checkArrayPointers(_microTest, _expected, _actual, _count, _line, _message) == false){
		return false;
	}

	for(uint32_t i = 0; i < _count; i++){
		MicroTest_Int expectedElement = readArrayElement(_expected, i, _display);
		MicroTest_Int actualElement   = readArrayElement(_actual, i, _display);

		if(expectedElement != actualElement){
			beginFailure(_microTest, _line);
			printString(_microTest, "Element ");
			printUnsigned(_microTest, i);
			printString(_microTest, " Expected ");
			printNumber(_microTest, expectedElement, _display);
			printString(_microTest, " Was ");
			printNumber(_microTest, actualElement, _display);
			endFailure(_microTest, _message);
			return false;
		}
	}
	return true;
}

bool MicroTest_AssertBits(MicroTest_st* _microTest, MicroTest_UInt _mask, MicroTest_UInt _expected, MicroTest_UInt _actual, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if((_expected & _mask) == (_actual & _mask)){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Expected ");
	printHex(_microTest, _expected & _mask, (uint8_t)(sizeof(MicroTest_UInt) * 2));
	printString(_microTest, " Was ");
	printHex(_microTest, _actual & _mask, (uint8_t)(sizeof(MicroTest_UInt) * 2));
	printString(_microTest, " (mask ");
	printHex(_microTest, _mask, (uint8_t)(sizeof(MicroTest_UInt) * 2));
	printString(_microTest, ")");
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertEqualString(MicroTest_st* _microTest, const char* _expected, const char* _actual, uint32_t _maxLength, uint32_t _line, const char* _message){
	bool equal = true;

	if(isAborted(_microTest)){
		return false;
	}

	if(_expected == _actual){
		return true;
	}
	if((_expected == NULL) || (_actual == NULL)){
		equal = false;
	}
	else{
		for(uint32_t i = 0; i < _maxLength; i++){
			if(_expected[i] != _actual[i]){
				equal = false;
				break;
			}
			if(_expected[i] == '\0'){
				break;
			}
		}
	}
	if(equal){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Expected ");
	printQuotedString(_microTest, _expected, _maxLength);
	printString(_microTest, " Was ");
	printQuotedString(_microTest, _actual, _maxLength);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertEqualMemory(MicroTest_st* _microTest, const void* _expected, const void* _actual, uint32_t _length, uint32_t _line, const char* _message){
	const uint8_t* expectedBytes = (const uint8_t*)_expected;
	const uint8_t* actualBytes   = (const uint8_t*)_actual;

	if(isAborted(_microTest)){
		return false;
	}
	if(checkArrayPointers(_microTest, _expected, _actual, _length, _line, _message) == false){
		return false;
	}

	for(uint32_t i = 0; i < _length; i++){
		if(expectedBytes[i] != actualBytes[i]){
			beginFailure(_microTest, _line);
			printString(_microTest, "Memory mismatch at byte ");
			printUnsigned(_microTest, i);
			printString(_microTest, ". Expected ");
			printHex(_microTest, expectedBytes[i], 2);
			printString(_microTest, " Was ");
			printHex(_microTest, actualBytes[i], 2);
			endFailure(_microTest, _message);
			return false;
		}
	}
	return true;
}

#ifndef MICROTEST_EXCLUDE_FLOAT
bool MicroTest_AssertFloatWithin(MicroTest_st* _microTest, float _delta, float _expected, float _actual, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(floatsWithin(_delta, _expected, _actual)){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Expected ");
	printFloat(_microTest, _expected);
	printString(_microTest, " +/- ");
	printFloat(_microTest, (_delta < 0.0f) ? -_delta : _delta);
	printString(_microTest, " Was ");
	printFloat(_microTest, _actual);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertEqualFloat(MicroTest_st* _microTest, float _expected, float _actual, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(floatsWithin(_expected * MICROTEST_FLOAT_PRECISION, _expected, _actual)){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, "Expected ");
	printFloat(_microTest, _expected);
	printString(_microTest, " Was ");
	printFloat(_microTest, _actual);
	endFailure(_microTest, _message);
	return false;
}

bool MicroTest_AssertEqualFloatArray(MicroTest_st* _microTest, const float* _expected, const float* _actual, uint32_t _count, uint32_t _line, const char* _message){
	if(isAborted(_microTest)){
		return false;
	}
	if(checkArrayPointers(_microTest, _expected, _actual, _count, _line, _message) == false){
		return false;
	}

	for(uint32_t i = 0; i < _count; i++){
		if(floatsWithin(_expected[i] * MICROTEST_FLOAT_PRECISION, _expected[i], _actual[i]) == false){
			beginFailure(_microTest, _line);
			printString(_microTest, "Element ");
			printUnsigned(_microTest, i);
			printString(_microTest, " Expected ");
			printFloat(_microTest, _expected[i]);
			printString(_microTest, " Was ");
			printFloat(_microTest, _actual[i]);
			endFailure(_microTest, _message);
			return false;
		}
	}
	return true;
}

bool MicroTest_AssertFloatTrait(MicroTest_st* _microTest, float _actual, MicroTest_FloatTrait_et _trait, uint32_t _line, const char* _message){
	bool        result       = false;
	const char* expectedText = "";

	if(isAborted(_microTest)){
		return false;
	}

	switch(_trait){
	case MICROTEST_FLOAT_TRAIT_INF:{
		result       = floatIsInf(_actual) && (_actual > 0.0f);
		expectedText = "Expected Inf";
		break;
	}

	case MICROTEST_FLOAT_TRAIT_NEG_INF:{
		result       = floatIsInf(_actual) && (_actual < 0.0f);
		expectedText = "Expected -Inf";
		break;
	}

	case MICROTEST_FLOAT_TRAIT_NAN:{
		result       = floatIsNan(_actual);
		expectedText = "Expected NaN";
		break;
	}

	case MICROTEST_FLOAT_TRAIT_NOT_NAN:{
		result       = (floatIsNan(_actual) == false);
		expectedText = "Expected not NaN";
		break;
	}

	default: break;
	}
	if(result){
		return true;
	}

	beginFailure(_microTest, _line);
	printString(_microTest, expectedText);
	printString(_microTest, " Was ");
	printFloat(_microTest, _actual);
	endFailure(_microTest, _message);
	return false;
}
#endif

//-----Weak-----
MICROTEST_WEAK void MicroTest_SetUp(MicroTest_st* _microTest){ (void)_microTest; }
MICROTEST_WEAK void MicroTest_TearDown(MicroTest_st* _microTest){ (void)_microTest; }
//-----Weak-----

static bool isAborted(MicroTest_st* _microTest){
	return _microTest->currentTestFailed || _microTest->currentTestIgnored;
}

static void beginFailure(MicroTest_st* _microTest, uint32_t _line){
	printLocation(_microTest, _line);
	printString(_microTest, "FAIL: ");

	_microTest->currentTestFailed = true;
}

static void endFailure(MicroTest_st* _microTest, const char* _message){
	if(_message != NULL){
		printString(_microTest, ". ");
		printString(_microTest, _message);
	}
	printNewLine(_microTest);
}

static void printLocation(MicroTest_st* _microTest, uint32_t _line){
	printString(_microTest, _microTest->fileName);
	printCharacter(_microTest, ':');
	printUnsigned(_microTest, _line);
	printCharacter(_microTest, ':');
	printString(_microTest, _microTest->currentTestName);
	printCharacter(_microTest, ':');
}

static void printNewLine(MicroTest_st* _microTest){
	printCharacter(_microTest, '\n');
	if(_microTest->descriptor.flush != NULL){
		_microTest->descriptor.flush();
	}
}

static void printCharacter(MicroTest_st* _microTest, char _character){
	if(_microTest->descriptor.putChar != NULL){
		_microTest->descriptor.putChar(_character);
	}
}

static void printString(MicroTest_st* _microTest, const char* _string){
	if(_string == NULL){
		return;
	}
	while(*_string != '\0'){
		printCharacter(_microTest, *_string);
		_string++;
	}
}

static void printQuotedString(MicroTest_st* _microTest, const char* _string, uint32_t _maxLength){
	if(_string == NULL){
		printString(_microTest, "NULL");
		return;
	}

	printCharacter(_microTest, '"');
	for(uint32_t i = 0; (i < _maxLength) && (_string[i] != '\0'); i++){
		if((_string[i] >= ' ') && (_string[i] <= '~')){
			printCharacter(_microTest, _string[i]);
		}
		else{
			printString(_microTest, "\\x");
			printHexDigits(_microTest, (uint8_t)_string[i], 2);
		}
	}
	printCharacter(_microTest, '"');
}

static void printUnsigned(MicroTest_st* _microTest, MicroTest_UInt _value){
	char    digits[20];
	uint8_t count = 0;

	do{
		digits[count++] = (char)('0' + (_value % 10));
		_value /= 10;
	}while(_value != 0);

	while(count > 0){
		printCharacter(_microTest, digits[--count]);
	}
}

static void printSigned(MicroTest_st* _microTest, MicroTest_Int _value){
	if(_value < 0){
		printCharacter(_microTest, '-');
		printUnsigned(_microTest, (MicroTest_UInt)0 - (MicroTest_UInt)_value);
	}
	else{
		printUnsigned(_microTest, (MicroTest_UInt)_value);
	}
}

static void printHex(MicroTest_st* _microTest, MicroTest_UInt _value, uint8_t _nibbles){
	printString(_microTest, "0x");
	printHexDigits(_microTest, _value, _nibbles);
}

static void printHexDigits(MicroTest_st* _microTest, MicroTest_UInt _value, uint8_t _nibbles){
	while(_nibbles > 0){
		_nibbles--;
		printCharacter(_microTest, "0123456789ABCDEF"[(_value >> (_nibbles * 4)) & 0x0F]);
	}
}

static void printPointer(MicroTest_st* _microTest, const void* _pointer){
	uintptr_t value   = (uintptr_t)_pointer;
	uint8_t   nibbles = (uint8_t)(sizeof(uintptr_t) * 2);

	if(_pointer == NULL){
		printString(_microTest, "NULL");
		return;
	}

	printString(_microTest, "0x");
	while(nibbles > 0){
		nibbles--;
		printCharacter(_microTest, "0123456789ABCDEF"[(value >> (nibbles * 4)) & 0x0F]);
	}
}

static void printNumber(MicroTest_st* _microTest, MicroTest_Int _value, MicroTest_Display_et _display){
	if(isDisplayHex(_display)){
		printHex(_microTest, (MicroTest_UInt)_value, (uint8_t)(getDisplaySize(_display) * 2));
	}
	else if(isDisplaySigned(_display)){
		printSigned(_microTest, _value);
	}
	else{
		printUnsigned(_microTest, (MicroTest_UInt)_value);
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

#ifndef MICROTEST_EXCLUDE_64BIT
	case MICROTEST_DISPLAY_INT64:
	case MICROTEST_DISPLAY_UINT64:
	case MICROTEST_DISPLAY_HEX64:{
		return 8;
	}
#endif

	default: break;
	}
	return 0;
}

static bool isDisplaySigned(MicroTest_Display_et _display){
	return (_display == MICROTEST_DISPLAY_INT)   ||
	       (_display == MICROTEST_DISPLAY_INT8)  ||
	       (_display == MICROTEST_DISPLAY_INT16) ||
#ifndef MICROTEST_EXCLUDE_64BIT
	       (_display == MICROTEST_DISPLAY_INT64) ||
#endif
	       (_display == MICROTEST_DISPLAY_INT32);
}

static bool isDisplayHex(MicroTest_Display_et _display){
	return (_display == MICROTEST_DISPLAY_HEX8)  ||
	       (_display == MICROTEST_DISPLAY_HEX16) ||
#ifndef MICROTEST_EXCLUDE_64BIT
	       (_display == MICROTEST_DISPLAY_HEX64) ||
#endif
	       (_display == MICROTEST_DISPLAY_HEX32);
}

static bool compareNumbers(MicroTest_Int _threshold, MicroTest_Int _actual, MicroTest_Comparison_et _comparison, bool _signed){
	bool greater = _signed ? (_actual > _threshold) : ((MicroTest_UInt)_actual > (MicroTest_UInt)_threshold);
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

static MicroTest_Int readArrayElement(const void* _array, uint32_t _index, MicroTest_Display_et _display){
	uint8_t        size    = getDisplaySize(_display);
	const uint8_t* element = (const uint8_t*)_array + ((size_t)_index * size);
	bool           sign    = isDisplaySigned(_display);

	switch(size){
	case 1: return sign ? (MicroTest_Int)*(const int8_t*)element  : (MicroTest_Int)*(const uint8_t*)element;
	case 2: return sign ? (MicroTest_Int)*(const int16_t*)element : (MicroTest_Int)*(const uint16_t*)element;
	case 4: return sign ? (MicroTest_Int)*(const int32_t*)element : (MicroTest_Int)*(const uint32_t*)element;
#ifndef MICROTEST_EXCLUDE_64BIT
	case 8: return sign ? (MicroTest_Int)*(const int64_t*)element : (MicroTest_Int)*(const uint64_t*)element;
#endif
	default: break;
	}
	return 0;
}

static bool checkArrayPointers(MicroTest_st* _microTest, const void* _expected, const void* _actual, uint32_t _count, uint32_t _line, const char* _message){
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

	beginFailure(_microTest, _line);
	printString(_microTest, failureText);
	endFailure(_microTest, _message);
	return false;
}

#ifndef MICROTEST_EXCLUDE_FLOAT
static bool floatIsNan(float _value){
	return _value != _value;
}

static bool floatIsInf(float _value){
	return (floatIsNan(_value) == false) && floatIsNan(_value - _value);
}

static bool floatsWithin(float _delta, float _expected, float _actual){
	float difference = _actual - _expected;

	if(floatIsInf(_expected) || floatIsInf(_actual)){
		return _expected == _actual;
	}
	if(difference < 0.0f){
		difference = -difference;
	}
	if(_delta < 0.0f){
		_delta = -_delta;
	}
	return difference <= _delta;
}

static void printFloat(MicroTest_st* _microTest, float _value){
	char     digits[6];
	uint8_t  significant = 6;
	int16_t  exponent    = 0;
	uint32_t mantissa    = 0;

	if(floatIsNan(_value)){
		printString(_microTest, "NaN");
		return;
	}
	if(_value < 0.0f){
		printCharacter(_microTest, '-');
		_value = -_value;
	}
	if(floatIsInf(_value)){
		printString(_microTest, "Inf");
		return;
	}
	if(_value == 0.0f){
		printCharacter(_microTest, '0');
		return;
	}

	while(_value >= 10.0f){
		_value /= 10.0f;
		exponent++;
	}
	while(_value < 1.0f){
		_value *= 10.0f;
		exponent--;
	}
	mantissa = (uint32_t)(_value * 100000.0f + 0.5f);
	if(mantissa >= 1000000){
		mantissa /= 10;
		exponent++;
	}
	for(int8_t i = 5; i >= 0; i--){
		digits[i] = (char)('0' + (mantissa % 10));
		mantissa /= 10;
	}
	while((significant > 1) && (digits[significant - 1] == '0')){
		significant--;
	}

	if((exponent >= -4) && (exponent <= 5)){
		if(exponent < 0){
			printString(_microTest, "0.");
			for(int16_t i = -1; i > exponent; i--){
				printCharacter(_microTest, '0');
			}
			for(uint8_t i = 0; i < significant; i++){
				printCharacter(_microTest, digits[i]);
			}
		}
		else{
			for(int16_t i = 0; i <= exponent; i++){
				printCharacter(_microTest, digits[i]);
			}
			if(significant > exponent + 1){
				printCharacter(_microTest, '.');
				for(uint8_t i = (uint8_t)(exponent + 1); i < significant; i++){
					printCharacter(_microTest, digits[i]);
				}
			}
		}
	}
	else{
		printCharacter(_microTest, digits[0]);
		if(significant > 1){
			printCharacter(_microTest, '.');
			for(uint8_t i = 1; i < significant; i++){
				printCharacter(_microTest, digits[i]);
			}
		}
		printCharacter(_microTest, 'e');
		printCharacter(_microTest, (exponent < 0) ? '-' : '+');
		if(exponent < 0){
			exponent = (int16_t)-exponent;
		}
		if(exponent < 10){
			printCharacter(_microTest, '0');
		}
		printUnsigned(_microTest, (MicroTest_UInt)exponent);
	}
}
#endif
