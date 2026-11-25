# MicroTest

MicroTest — небольшая библиотека для юнит-тестирования кода embedded-проектов на C. Аппаратно независимые части прошивки (библиотеки, модули, бизнес-логика) собираются вместе с тестами обычным компилятором на компьютере (gcc, clang) и проверяются без микроконтроллера. Результаты выводятся в консоль через `printf`.

Библиотека состоит из двух файлов в каталоге `src/` — `MicroTest.h` и `MicroTest.c` — и использует только стандартную библиотеку C.

Есть два способа подключения:

- **установка в систему** (Linux и Windows) — библиотека ставится один раз, тесты любого проекта подключают её через `-lMicroTest`, `pkg-config` или `find_package` в CMake. Копировать её в проект не нужно;
- **исходники в проекте** — `MicroTest.c` компилируется вместе с тестами (например, если библиотека добавлена в проект как git submodule).

## Содержание

- [Возможности](#возможности)
- [Быстрый старт](#быстрый-старт)
- [Установка в систему](#установка-в-систему)
- [Сборка и запуск](#сборка-и-запуск)
- [Структура тестового файла](#структура-тестового-файла)
- [Подготовка и очистка: SetUp и TearDown](#подготовка-и-очистка-setup-и-teardown)
- [Формат вывода](#формат-вывода)
- [Поведение при ошибке](#поведение-при-ошибке)
- [Справочник проверок](#справочник-проверок)
- [Настройка](#настройка)
- [Ограничения](#ограничения)

## Возможности

- Проверки логических условий, указателей, целых чисел всех размеров (8–64 бит, знаковых и беззнаковых), hex-значений, битовых масок, строк, областей памяти, массивов и чисел с плавающей точкой.
- Сравнения на равенство, неравенство, больше/меньше и попадание в диапазон.
- У каждой проверки есть вариант `_MESSAGE` с пояснением, которое выводится при ошибке.
- Упавшая проверка сразу завершает тест, в том числе из вспомогательной функции.
- Пропуск тестов (`MICROTEST_IGNORE`) и принудительный провал (`MICROTEST_FAIL`).
- Общие функции подготовки и очистки для каждого теста.
- Вывод в формате `файл:строка:тест:результат`: IDE и редакторы превращают такие строки в ссылки на место ошибки.
- `MICROTEST_END()` возвращает количество упавших тестов, которое можно отдать как код возврата программы для CI.
- Заголовок подключается и из C, и из C++.

## Быстрый старт

Тестовый файл `TestSum.c`:

```c
#include "MicroTest.h"

static int sum(int _a, int _b){
	return _a + _b;
}

static void testSumOfPositive(void){
	MICROTEST_ASSERT_EQUAL_INT(4, sum(2, 2));
}

static void testSumOfNegative(void){
	MICROTEST_ASSERT_EQUAL_INT_MESSAGE(-4, sum(-2, -2), "sum of negative numbers");
}

int main(void){
	MICROTEST_BEGIN();
	MICROTEST_RUN_TEST(testSumOfPositive);
	MICROTEST_RUN_TEST(testSumOfNegative);
	return (int)MICROTEST_END();
}
```

Результат:

```
TestSum.c:17:testSumOfPositive:PASS
TestSum.c:18:testSumOfNegative:PASS

-----------------------
2 Tests 0 Failures 0 Ignored
OK
```

Полный пример с разными видами проверок: [examples/ExampleTest.c](examples/ExampleTest.c).

## Установка в систему

Нужны CMake 3.14+ и компилятор C (gcc или clang; на Windows — MinGW-w64, например из MSYS2). Устанавливаются:

| Файл | Куда |
|---|---|
| `MicroTest.h` | `<prefix>/include` |
| `libMicroTest.a` | `<prefix>/lib` |
| CMake-пакет `MicroTestConfig.cmake` | `<prefix>/lib/cmake/MicroTest` |
| `MicroTest.pc` для pkg-config | `<prefix>/lib/pkgconfig` |

### Linux

```bash
git clone https://github.com/krllplotnikov-libraries/MicroTest.git
cd MicroTest
./install.sh               # в /usr/local, спросит пароль sudo
```

Другой каталог: `./install.sh ~/.local`. Удаление: `./install.sh --uninstall`.

`/usr/local/include` и `/usr/local/lib` gcc и clang просматривают по умолчанию, поэтому после установки в `/usr/local` флаги `-I` и `-L` не нужны.

### Windows

В PowerShell:

```powershell
git clone https://github.com/krllplotnikov-libraries/MicroTest.git
cd MicroTest
.\install.ps1
```

Или двойной клик по `install.bat`.

Если в `PATH` есть `gcc.exe` (MSYS2, MinGW-w64), библиотека устанавливается в каталог этого компилятора: например, для `C:\msys64\ucrt64\bin\gcc.exe` — в `C:\msys64\ucrt64`. Этот gcc сам находит там заголовок и библиотеку, флаги `-I` и `-L` не нужны. Если gcc не найден, библиотека собирается компилятором по умолчанию для CMake (MSVC) и устанавливается в `%LOCALAPPDATA%\MicroTest`.

Другой каталог: `.\install.ps1 -Prefix C:\Libs\MicroTest`. Удаление: `.\install.ps1 -Uninstall`.

### Вручную через CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<prefix>
cmake --build build --config Release
cmake --install build --config Release
cmake --build build --target uninstall     # удаление
```

Опция `-DMICROTEST_BUILD_EXAMPLES=ON` дополнительно собирает `examples/ExampleTest.c` и регистрирует его для `ctest`.

### Подключение установленной библиотеки

Напрямую компилятором:

```bash
gcc Tests/TestRelay.c Libraries/Relay/Relay.c -ILibraries/Relay -lMicroTest -lm -o TestRelay
```

Если библиотека установлена в нестандартный каталог, добавьте `-I<prefix>/include -L<prefix>/lib`.

Через pkg-config:

```bash
gcc Tests/TestRelay.c Libraries/Relay/Relay.c -ILibraries/Relay $(pkg-config --cflags --libs MicroTest) -o TestRelay
```

Через CMake:

```cmake
find_package(MicroTest 1.0 REQUIRED)

add_executable(TestRelay Tests/TestRelay.c Libraries/Relay/Relay.c)
target_include_directories(TestRelay PRIVATE Libraries/Relay)
target_link_libraries(TestRelay PRIVATE MicroTest::MicroTest)
```

Если CMake не находит пакет, укажите каталог установки: `-DCMAKE_PREFIX_PATH=<prefix>`.

Особенности установленной библиотеки:

- Она статическая (`.a`/`.lib`). Динамическая не подходит: свои `MicroTest_SetUp` и `MicroTest_TearDown` тестового файла должны заменять стандартные, а DLL на Windows так не умеет.
- Weak-символы в ней не используются: стандартные пустые `MicroTest_SetUp` и `MicroTest_TearDown` лежат в отдельных объектных файлах (`src/Defaults/`), и компоновщик берёт их, только если в тесте нет своей функции с тем же именем. Поэтому это работает на любом компоновщике, включая MinGW и MSVC.
- `MICROTEST_FLOAT_PRECISION` подставляется в тестовом файле, поэтому `-DMICROTEST_FLOAT_PRECISION=...` работает и с готовой библиотекой.
- Библиотеку надо собирать тем же компилятором, что и тесты: `libMicroTest.a` из MinGW не подойдёт для MSVC, и наоборот.

## Сборка и запуск

Этот раздел — для подключения исходниками, без установки. `src/MicroTest.c` компилируется вместе с тестовым файлом и тестируемым кодом, а каталог `src/` добавляется в пути поиска заголовков (`-I`). Флаг `-lm` нужен для функций из `<math.h>`.

```bash
gcc -IMicroTest/src MicroTest/src/MicroTest.c TestSum.c -lm -o TestSum
```

```bash
./TestSum
```

Пример `Makefile`, где каждый тестовый файл собирается в отдельную программу и запускается:

```make
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -IMicroTest/src -ILibraries/Relay -ILibraries/Button
TESTS   = TestRelay TestButton

all: $(TESTS)
	@for test in $(TESTS); do ./$$test || exit 1; done

TestRelay: MicroTest/src/MicroTest.c Tests/TestRelay.c Libraries/Relay/Relay.c
	$(CC) $(CFLAGS) $^ -lm -o $@

TestButton: MicroTest/src/MicroTest.c Tests/TestButton.c Libraries/Button/Button.c
	$(CC) $(CFLAGS) $^ -lm -o $@
```

Так как `MICROTEST_END()` возвращает число упавших тестов, при ошибке программа завершается с ненулевым кодом, и `make` или CI останавливается.

## Структура тестового файла

1. Подключение `MicroTest.h` и тестируемого модуля.
2. Тестовые функции вида `static void testName(void)`.
3. При необходимости — переопределение `MicroTest_SetUp` и `MicroTest_TearDown`.
4. `main`, в котором вызываются:

| Макрос | Назначение |
|---|---|
| `MICROTEST_BEGIN()` | Начинает набор тестов: запоминает имя файла и обнуляет счётчики |
| `MICROTEST_RUN_TEST(test)` | Запускает тестовую функцию `test`; в выводе используются её имя и строка вызова |
| `MICROTEST_END()` | Печатает итог и возвращает количество упавших тестов (`uint32_t`) |

Каждый тестовый файл обычно собирается в отдельную программу.

### Тестирование модуля с аппаратными функциями

Модули, которые работают с железом через указатели на функции из дескриптора, тестируются с фейковыми функциями. Фейковая функция не обращается к железу, а записывает полученные значения в переменные, которые затем проверяет тест.

Пусть есть модуль реле `Relay`, который управляет выводом через функцию `setPin` из дескриптора:

```c
typedef void (*Relay_SetPin)(Relay_st* _relay, bool _state);

struct Relay_Descriptor{
	Relay_SetPin setPin;
};

void Relay_Init(Relay_st* _relay, Relay_Descriptor_st* _descriptor);
void Relay_TurnOn(Relay_st* _relay);
void Relay_TurnOff(Relay_st* _relay);
```

Тестовый файл для него:

```c
#include "MicroTest.h"
#include "Relay.h"

static Relay_st relay          = {0};
static bool     lastPinState   = false;
static uint32_t setPinCalls    = 0;

static void fakeSetPin(Relay_st* _relay, bool _state){
	(void)_relay;
	lastPinState = _state;
	setPinCalls++;
}

void MicroTest_SetUp(void){
	Relay_Descriptor_st descriptor = {
		.setPin = fakeSetPin,
	};

	lastPinState = false;
	setPinCalls  = 0;
	Relay_Init(&relay, &descriptor);
}

static void testTurnOnSetsPinHigh(void){
	Relay_TurnOn(&relay);

	MICROTEST_ASSERT_TRUE(lastPinState);
	MICROTEST_ASSERT_EQUAL_UINT32(1, setPinCalls);
}

static void testTurnOffSetsPinLow(void){
	Relay_TurnOn(&relay);
	Relay_TurnOff(&relay);

	MICROTEST_ASSERT_FALSE(lastPinState);
}

int main(void){
	MICROTEST_BEGIN();
	MICROTEST_RUN_TEST(testTurnOnSetsPinHigh);
	MICROTEST_RUN_TEST(testTurnOffSetsPinLow);
	return (int)MICROTEST_END();
}
```

Так как `MicroTest_SetUp` вызывается перед каждым тестом, каждый тест начинается с нового состояния реле и обнулённых переменных.

## Подготовка и очистка: SetUp и TearDown

`MicroTest_SetUp` вызывается перед каждым тестом, `MicroTest_TearDown` — после него. В библиотеке они объявлены как weak-функции с пустым телом, поэтому определять их необязательно. Если в тестовом файле определена функция с тем же именем, вместо пустой вызывается она.

```c
void MicroTest_SetUp(void){
	// приведение тестируемого модуля в исходное состояние
}

void MicroTest_TearDown(void){
	// освобождение ресурсов после теста
}
```

Порядок выполнения одного теста:

1. Вызывается `MicroTest_SetUp`.
2. Если SetUp прошёл без ошибок, вызывается сама тестовая функция.
3. `MicroTest_TearDown` вызывается всегда, даже если тест упал или был пропущен.

Проверки можно использовать и внутри SetUp и TearDown: ошибка в них засчитывается текущему тесту.

## Формат вывода

Для каждого теста печатается одна строка:

| Результат | Формат | Строка в выводе |
|---|---|---|
| Успех | `файл:строка:тест:PASS` | строка вызова `MICROTEST_RUN_TEST` |
| Ошибка | `файл:строка:тест:FAIL: описание[. сообщение]` | строка упавшей проверки |
| Пропуск | `файл:строка:тест:IGNORE[: сообщение]` | строка `MICROTEST_IGNORE` |

Примеры сообщений об ошибках:

```
TestSum.c:12:testSum:FAIL: Expected 4 Was 5. sum of positive numbers
TestSum.c:20:testLimit:FAIL: Expected greater than 100 Was 42
TestSum.c:27:testRange:FAIL: Expected 250 +/- 1 Was 5
TestSum.c:33:testBuffer:FAIL: Element 2 Expected 3 Was 4
TestSum.c:40:testName:FAIL: Expected "abc" Was "ab\x0A"
TestSum.c:46:testPacket:FAIL: Memory mismatch at byte 2. Expected 0x03 Was 0x09
TestSum.c:51:testFlags:FAIL: Expected 0x8 Was 0x0 (mask 0x8)
```

Непечатаемые символы в строках выводятся как `\xHH`.

В конце печатается итог: количество тестов, ошибок и пропущенных тестов, а затем `OK`, если ошибок нет, или `FAIL`, если они есть.

## Поведение при ошибке

- Первая упавшая проверка сразу завершает тест (через `longjmp`), следующие строки теста не выполняются. Поэтому проверки можно вызывать и во вспомогательных функциях, которые вызываются из теста: ошибка всё равно завершит весь тест.
- Для каждого теста выводится только первая ошибка.
- После ошибки или пропуска выполняется `MicroTest_TearDown`, затем запускается следующий тест.

## Справочник проверок

### Общие правила

- У каждой проверки есть вариант с суффиксом `_MESSAGE`. Он принимает последним аргументом строку, которая выводится после описания ошибки:
  ```c
  MICROTEST_ASSERT_EQUAL_UINT8(0x10, status);
  MICROTEST_ASSERT_EQUAL_UINT8_MESSAGE(0x10, status, "status after init");
  ```
- Порядок аргументов: сначала ожидаемое значение, затем фактическое — `(expected, actual)`.
- В сравнениях первым идёт порог — `(threshold, actual)`: `MICROTEST_ASSERT_GREATER_THAN_INT(5, x)` проверяет `x > 5`.

### Управление тестом

| Макрос | Описание |
|---|---|
| `MICROTEST_PASS()` | Завершает тест как успешный |
| `MICROTEST_FAIL()` | Завершает тест с ошибкой |
| `MICROTEST_FAIL_MESSAGE(message)` | То же с сообщением |
| `MICROTEST_IGNORE()` | Пропускает тест: он засчитывается как пропущенный, а не упавший |
| `MICROTEST_IGNORE_MESSAGE(message)` | То же с сообщением |

### Логические условия и указатели

| Макрос | Проходит, если |
|---|---|
| `MICROTEST_ASSERT(condition)` | `condition` истинно |
| `MICROTEST_ASSERT_TRUE(condition)` | `condition` истинно |
| `MICROTEST_ASSERT_FALSE(condition)` | `condition` ложно |
| `MICROTEST_ASSERT_NULL(pointer)` | `pointer == NULL` |
| `MICROTEST_ASSERT_NOT_NULL(pointer)` | `pointer != NULL` |
| `MICROTEST_ASSERT_EQUAL_PTR(expected, actual)` | указатели равны |

### Целые числа

Вместо `T` подставляется тип:

| `T` | Тип значения | Вывод |
|---|---|---|
| `INT` | `int` | десятичный со знаком |
| `INT8`, `INT16`, `INT32`, `INT64` | `int8_t` … `int64_t` | десятичный со знаком |
| `UINT` | `unsigned int` | десятичный без знака |
| `UINT8`, `UINT16`, `UINT32`, `UINT64` | `uint8_t` … `uint64_t` | десятичный без знака |
| `HEX8`, `HEX16`, `HEX32`, `HEX64` | `uint8_t` … `uint64_t` | hex с ведущими нулями, например `0x00FF` |

| Макрос | Проходит, если |
|---|---|
| `MICROTEST_ASSERT_EQUAL_T(expected, actual)` | `actual == expected` |
| `MICROTEST_ASSERT_NOT_EQUAL_T(expected, actual)` | `actual != expected` |
| `MICROTEST_ASSERT_GREATER_THAN_T(threshold, actual)` | `actual > threshold` |
| `MICROTEST_ASSERT_GREATER_OR_EQUAL_T(threshold, actual)` | `actual >= threshold` |
| `MICROTEST_ASSERT_LESS_THAN_T(threshold, actual)` | `actual < threshold` |
| `MICROTEST_ASSERT_LESS_OR_EQUAL_T(threshold, actual)` | `actual <= threshold` |
| `MICROTEST_ASSERT_T_WITHIN(delta, expected, actual)` | `actual` отличается от `expected` не больше чем на `delta` |
| `MICROTEST_ASSERT_EQUAL_T_ARRAY(expected, actual, count)` | первые `count` элементов массивов совпадают |

Сокращения: `MICROTEST_ASSERT_EQUAL(expected, actual)` и `MICROTEST_ASSERT_NOT_EQUAL(expected, actual)` работают как варианты с `INT`.

Примеры:

```c
MICROTEST_ASSERT_EQUAL_UINT16(1000, adcValue);
MICROTEST_ASSERT_EQUAL_HEX8(0xA5, registerValue);
MICROTEST_ASSERT_LESS_THAN_UINT32(500, elapsedMs);
MICROTEST_ASSERT_INT16_WITHIN(5, 250, temperature);

const uint8_t expectedFrame[] = {0x01, 0x03, 0x00, 0x10};
MICROTEST_ASSERT_EQUAL_UINT8_ARRAY(expectedFrame, frame, sizeof(expectedFrame));
```

Особенности:

- Оба значения приводятся к типу `T` до сравнения. Поэтому `MICROTEST_ASSERT_EQUAL_INT8(-1, 255)` проходит: `255`, приведённое к `int8_t`, равно `-1`. Тип в проверке выбирается по типу тестируемой переменной.
- Для массивов `T` определяет размер элемента, поэтому он должен совпадать с типом элементов массива. `count` — количество элементов, а не байт.
- Проверка массива с `count == 0` или с `NULL` вместо массива завершается ошибкой.

### Биты

| Макрос | Проходит, если |
|---|---|
| `MICROTEST_ASSERT_BITS(mask, expected, actual)` | `(actual & mask) == (expected & mask)` |
| `MICROTEST_ASSERT_BITS_HIGH(mask, actual)` | все биты из `mask` установлены в `actual` |
| `MICROTEST_ASSERT_BITS_LOW(mask, actual)` | все биты из `mask` сброшены в `actual` |
| `MICROTEST_ASSERT_BIT_HIGH(bit, actual)` | бит с номером `bit` установлен |
| `MICROTEST_ASSERT_BIT_LOW(bit, actual)` | бит с номером `bit` сброшен |

```c
MICROTEST_ASSERT_BITS(0x0F, 0x05, controlRegister);  // младшая тетрада равна 0x5
MICROTEST_ASSERT_BIT_HIGH(7, statusRegister);        // бит 7 установлен
```

### Строки и память

| Макрос | Проходит, если |
|---|---|
| `MICROTEST_ASSERT_EQUAL_STRING(expected, actual)` | строки совпадают целиком |
| `MICROTEST_ASSERT_EQUAL_STRING_LEN(expected, actual, length)` | совпадают первые `length` символов |
| `MICROTEST_ASSERT_EQUAL_MEMORY(expected, actual, length)` | совпадают первые `length` байт |

- Две строки `NULL` считаются равными, `NULL` и обычная строка — разными.
- При ошибке `EQUAL_MEMORY` выводит номер первого несовпадающего байта и оба значения.
- `EQUAL_MEMORY` с `length == 0` или с `NULL` завершается ошибкой.

### Числа с плавающей точкой

| Макрос | Проходит, если |
|---|---|
| `MICROTEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)` | `actual` отличается от `expected` не больше чем на `delta` |
| `MICROTEST_ASSERT_EQUAL_FLOAT(expected, actual)` | `actual` отличается от `expected` не больше чем на `expected * MICROTEST_FLOAT_PRECISION` |
| `MICROTEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, count)` | каждый элемент проходит `EQUAL_FLOAT` |
| `MICROTEST_ASSERT_FLOAT_IS_INF(actual)` | `actual` равно `+inf` |
| `MICROTEST_ASSERT_FLOAT_IS_NEG_INF(actual)` | `actual` равно `-inf` |
| `MICROTEST_ASSERT_FLOAT_IS_NAN(actual)` | `actual` равно `nan` |
| `MICROTEST_ASSERT_FLOAT_IS_NOT_NAN(actual)` | `actual` не равно `nan` |

- Сравнивать float через `==` ненадёжно из-за ошибок округления: `0.1f + 0.2f` не равно в точности `0.3f`. Для этого и нужны `EQUAL_FLOAT` и `FLOAT_WITHIN`:
  ```c
  MICROTEST_ASSERT_EQUAL_FLOAT(0.3f, 0.1f + 0.2f);
  MICROTEST_ASSERT_FLOAT_WITHIN(0.01f, 3.14f, calculatedPi);
  ```
- Бесконечности равны только бесконечностям того же знака. `nan` не равно ничему, даже `nan`, поэтому для него есть отдельная проверка `FLOAT_IS_NAN`.
- Если ожидаемое значение `0.0f`, `EQUAL_FLOAT` требует точного нуля; для значений около нуля используется `FLOAT_WITHIN`.

## Настройка

| Параметр | По умолчанию | Описание |
|---|---|---|
| `MICROTEST_FLOAT_PRECISION` | `0.00001f` | Относительная точность `MICROTEST_ASSERT_EQUAL_FLOAT` и `MICROTEST_ASSERT_EQUAL_FLOAT_ARRAY` |

Параметр задаётся флагом компилятора:

```bash
gcc -DMICROTEST_FLOAT_PRECISION=0.001f -IMicroTest/src MicroTest/src/MicroTest.c TestSum.c -lm -o TestSum
```

## Ограничения

- Библиотека рассчитана на запуск тестов на компьютере, а не на микроконтроллере.
- `MicroTest_SetUp` и `MicroTest_TearDown` общие для всей программы. Чтобы у разных модулей были свои функции подготовки, каждый тестовый файл собирается в отдельную программу.
- Проверок для `double` нет: значения `double` в проверках float приводятся к `float`.
- При подключении исходниками weak-функции объявлены через `__attribute__((weak))`, который поддерживают gcc и clang. В установленной библиотеке weak-символы не используются (см. [Установка в систему](#установка-в-систему)).
