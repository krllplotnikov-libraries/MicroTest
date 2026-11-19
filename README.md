# MicroTest

Лёгкая библиотека юнит-тестов для embedded-проектов. Два файла (`MicroTest.h`, `MicroTest.c`), без Ruby, без yml, без зависимостей от libc (кроме `<stdint.h>`, `<stdbool.h>`, `<stddef.h>` и опционально `<setjmp.h>`).

## Подключение

Вывод идёт через указатель на функцию в дескрипторе, так что результаты можно отправлять в UART, SWO, RTT или `putchar` на хосте.

```c
#include "MicroTest.h"

MicroTest_st            microTest           = {0};
MicroTest_Descriptor_st microTestDescriptor = {
	.putChar = uartPutChar,  // обязательный: void (*)(char)
	.flush   = NULL,         // необязательный: void (*)(void)
};

static void testSum(void){
	MICROTEST_ASSERT_EQUAL_INT(4, 2 + 2);
}

int main(void){
	MicroTest_Init(&microTest, &microTestDescriptor);

	MICROTEST_BEGIN();
	MICROTEST_RUN_TEST(testSum);
	return (int)MICROTEST_END();  // количество упавших тестов
}
```

`MicroTest_SetUp` / `MicroTest_TearDown` — weak-функции; переопредели их в тестовом файле, если нужны действия до и после каждого теста.

Формат вывода совместим с Unity: `file:line:test:PASS|FAIL: ...|IGNORE`.

## Настройка (флаги компилятора)

| Флаг | Назначение |
|---|---|
| `MICROTEST_EXCLUDE_SETJMP` | Упавшая проверка выходит из теста через `return` вместо `longjmp`. Проверки тогда можно использовать только в `void` функциях |
| `MICROTEST_EXCLUDE_64BIT` | Без 64-битных проверок; числа обрабатываются как 32-битные |
| `MICROTEST_EXCLUDE_FLOAT` | Без проверок для float |
| `MICROTEST_FLOAT_PRECISION` | Относительная точность `MICROTEST_ASSERT_EQUAL_FLOAT` (по умолчанию `0.00001f`) |
| `MICROTEST_WEAK` | Weak-атрибут компилятора (по умолчанию `__attribute__((weak))`, для IAR — `__weak`) |

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
