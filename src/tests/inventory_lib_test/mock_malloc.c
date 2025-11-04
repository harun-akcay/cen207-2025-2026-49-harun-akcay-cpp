/**
 * @file mock_malloc.c
 * @brief Mock malloc implementation for testing memory allocation failures
 * 
 * This file provides a mock malloc function that can be used to simulate
 * memory allocation failures in tests. On Windows MSVC, this requires
 * linking this file before the standard library and using appropriate linker flags.
 */

#include <stdlib.h>
#include <stddef.h>

// Mock malloc control variables
static int g_malloc_should_fail = 0;
static int g_malloc_fail_count = 0;
static int g_malloc_call_count = 0;

// Function pointer to real malloc
// We'll get it from the standard library on first call
static void* (*real_malloc_ptr)(size_t) = NULL;

/**
 * @brief Set whether malloc should fail
 * @param should_fail Non-zero to make malloc fail, 0 otherwise
 */
void mock_malloc_set_fail(int should_fail) {
    g_malloc_should_fail = should_fail;
}

/**
 * @brief Set the call count at which malloc should fail
 * @param fail_count The call number at which malloc should return NULL (1-based)
 */
void mock_malloc_set_fail_count(int fail_count) {
    g_malloc_fail_count = fail_count;
}

/**
 * @brief Reset the malloc mock state
 */
void mock_malloc_reset(void) {
    g_malloc_should_fail = 0;
    g_malloc_fail_count = 0;
    g_malloc_call_count = 0;
}

/**
 * @brief Get the current malloc call count
 * @return The number of times malloc has been called
 */
int mock_malloc_get_call_count(void) {
    return g_malloc_call_count;
}

/**
 * @brief Hook function for testing malloc failures
 * 
 * This function can be used as a malloc hook in InventoryManager_SetMallocHook.
 * It uses the mock_malloc control variables to determine if allocation should fail.
 * 
 * @param size The size to allocate
 * @return Pointer to allocated memory, or NULL if allocation should fail
 */
void* mock_malloc_hook(size_t size) {
    g_malloc_call_count++;
    
    // Check if we should fail this call
    if (g_malloc_should_fail && g_malloc_call_count >= g_malloc_fail_count) {
        return NULL;
    }
    
    // Call the real malloc
    return malloc(size);
}

// On Windows MSVC, overriding malloc safely is extremely difficult
// We'll provide a minimal implementation that doesn't cause heap corruption
#ifdef _MSC_VER
// For MSVC, we need to be very careful. Instead of overriding malloc globally,
// we should modify the source code to use a test hook. But for now, we'll
// provide a safe stub that won't crash.

// Note: This implementation won't actually test malloc failures on Windows MSVC
// because overriding malloc causes heap corruption. The proper solution would be
// to add a memory allocation hook in InventoryManager.c itself.

// For now, we'll just compile this file but not override malloc
// The tests will need to be adjusted to work without malloc override on Windows

// We'll provide empty implementations that don't override malloc
// This prevents heap corruption while still allowing the code to compile
#else
// For GCC/Clang, we can use weak symbols or __wrap_malloc
__attribute__((weak))
void* malloc(size_t size) {
    g_malloc_call_count++;
    
    // Check if we should fail this call
    if (g_malloc_should_fail && g_malloc_call_count >= g_malloc_fail_count) {
        return NULL;
    }
    
    // Call the real malloc using __real_malloc (if available with -Wl,--wrap=malloc)
    // Otherwise, fall back to standard malloc
    extern void* __real_malloc(size_t);
    return __real_malloc ? __real_malloc(size) : __libc_malloc(size);
}
#endif
