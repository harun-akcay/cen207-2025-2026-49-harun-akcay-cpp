/**
 * @file mock_malloc.h
 * @brief Header for mock malloc functions used in testing
 * 
 * This header provides functions to control mock malloc behavior
 * for testing memory allocation failures.
 */

#ifndef MOCK_MALLOC_H
#define MOCK_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set whether malloc should fail
 * @param should_fail Non-zero value to make malloc fail, 0 to allow normal allocation
 */
void mock_malloc_set_fail(int should_fail);

/**
 * @brief Set the call count at which malloc should fail
 * @param fail_count The call number at which malloc should return NULL (1-based)
 */
void mock_malloc_set_fail_count(int fail_count);

/**
 * @brief Reset the malloc mock state
 */
void mock_malloc_reset(void);

/**
 * @brief Get the current malloc call count
 * @return The number of times malloc has been called
 */
int mock_malloc_get_call_count(void);

/**
 * @brief Hook function for testing malloc failures
 * 
 * This function can be used as a malloc hook in InventoryManager_SetMallocHook.
 * It uses the mock_malloc control variables to determine if allocation should fail.
 * 
 * @param size The size to allocate in bytes
 * @return Pointer to allocated memory on success, or NULL if allocation should fail
 */
void* mock_malloc_hook(size_t size);

#ifdef __cplusplus
}
#endif

#endif // MOCK_MALLOC_H
