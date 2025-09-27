#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint64_t fib_iter(uint64_t n) {
    uint64_t a = 0, b = 1;
    for (uint64_t i = 0; i < n; ++i) {
        uint64_t t = a + b;
        a = b;
        b = t;
    }
    return a;
}

// Волатильный «приёмник», чтобы компилятор не выкинул работу.
static volatile uint64_t sink_global = 0;

int main(int argc, char** argv) {
    uint64_t n = 90;
    if (argc > 1) n = strtoull(argv[1], NULL, 10);

    const char* env_ir = getenv("INNER_REPS");
    uint64_t inner = 0;
    if (env_ir && env_ir[0]) inner = strtoull(env_ir, NULL, 10);

    if (inner == 0) {
        // Совместимость со старым режимом: один вызов.
        sink_global ^= fib_iter(n);
    } else {
        // «Чистые» замеры: крутим много вызовов, без printf в горячем участке.
        uint64_t s = 0;
        for (uint64_t i = 0; i < inner; ++i) {
            s ^= fib_iter(n);
        }
        sink_global ^= s;
    }

    // Ничего не печатаем по умолчанию (чтобы критический участок был чистым).
    // При желании отладить можно включить печать через переменную окружения PRINT_SINK=1.
    const char* ps = getenv("PRINT_SINK");
    if (ps && ps[0] == '1') {
        printf("%llu\n", (unsigned long long)sink_global);
    }
    return 0;
}