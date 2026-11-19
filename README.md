# MicroTest

Лёгкая библиотека юнит-тестов для embedded-проектов. Тесты собираются и запускаются на хосте (gcc/clang), результаты выводятся через `printf`. Два файла (`MicroTest.h`, `MicroTest.c`), без Ruby, без yml, только стандартная библиотека C.

## Подключение

```c
#include "MicroTest.h"

static void testSum(void){
	MICROTEST_ASSERT_EQUAL_INT(4, 2 + 2);
}

int main(void){
	MICROTEST_BEGIN();
	MICROTEST_RUN_TEST(testSum);
	return (int)MICROTEST_END();  // количество упавших тестов
}
```

Сборка: `gcc MicroTest.c MyTest.c -lm -o MyTest && ./MyTest` (`-lm` нужен для `fabsf`).

`MicroTest_SetUp` / `MicroTest_TearDown` — weak-функции; переопредели их в тестовом файле, если нужны действия до и после каждого теста.

Формат вывода совместим с Unity: `file:line:test:PASS|FAIL: ...|IGNORE`.

## Настройка

`MICROTEST_FLOAT_PRECISION` (флаг `-D`) задаёт относительную точность `MICROTEST_ASSERT_EQUAL_FLOAT`, по умолчанию `0.00001f`.

Упавшая проверка прерывает тест через `longjmp`, поэтому проверки можно вызывать и во вспомогательных функциях.

## Проверки

У каждой проверки есть вариант `_MESSAGE` с дополнительным последним аргументом — текстом сообщения.

- Управление: `MICROTEST_PASS`, `MICROTEST_FAIL`, `MICROTEST_IGNORE`
- Логические и указатели: `MICROTEST_ASSERT`, `_TRUE`, `_FALSE`, `_NULL`, `_NOT_NULL`, `_EQUAL_PTR`
- Числа (`T` = `INT`, `INT8..64`, `UINT`, `UINT8..64`, `HEX8..64`):
  `EQUAL_T`, `NOT_EQUAL_T`, `GREATER_THAN_T`, `GREATER_OR_EQUAL_T`, `LESS_THAN_T`, `LESS_OR_EQUAL_T`, `T_WITHIN`, `EQUAL_T_ARRAY`
- Биты: `BITS`, `BITS_HIGH`, `BITS_LOW`, `BIT_HIGH`, `BIT_LOW`
- Строки и память: `EQUAL_STRING`, `EQUAL_STRING_LEN`, `EQUAL_MEMORY`
- Float: `FLOAT_WITHIN`, `EQUAL_FLOAT`, `EQUAL_FLOAT_ARRAY`, `FLOAT_IS_INF`, `FLOAT_IS_NEG_INF`, `FLOAT_IS_NAN`, `FLOAT_IS_NOT_NAN`

Порядок аргументов как в Unity: `(expected, actual)`, а для сравнений `(threshold, actual)`. Например, `MICROTEST_ASSERT_GREATER_THAN_INT(5, x)` проверяет `x > 5`.

Полный пример: [Examples/ExampleTest.c](Examples/ExampleTest.c).
