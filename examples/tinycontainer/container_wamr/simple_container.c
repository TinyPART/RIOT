/* Copyright (C) World
 * a native printf is provided by the builtin libc of wamr */

#ifdef __cplusplus
extern "C" int printf( const char *, ...);
#define WASM_EXPORT __attribute__((visibility("default"))) extern "C"
#else
extern int printf( const char *, ...);
#define WASM_EXPORT __attribute__((visibility("default")))
#endif

WASM_EXPORT void start()
{
    printf("WASM: start()\n");
}

int n = 0;
WASM_EXPORT int loop()
{
    printf("WASM: loop n=%d of 100\n", ++n);
    return (n==100);
}

WASM_EXPORT void stop()
{
    printf("WASM: stop()\n");
}
