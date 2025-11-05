/**
 * @file InventoryManager.h
 * 
 * @brief Provides functions for inventory management utilities
 */

#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include "MaterialInventory.h"
#include "ProjectTracking.h"
#include "ExpenseTracking.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup HashTable Hash Table Implementation
 * @brief Hash Table data structure for user authentication
 * @{
 */

/**
 * @def HASH_TABLE_SIZE
 * @brief Size of the hash table (prime number for better distribution)
 * 
 * Using a prime number helps reduce collisions when using modulo operation.
 * 101 is a good choice for small to medium-sized user bases.
 */
#define HASH_TABLE_SIZE 101  // Prime number for better distribution

/**
 * @brief User structure for authentication
 * 
 * Represents a user account in the inventory management system.
 * Stores username, hashed password, and active status.
 */
typedef struct {
    /** @brief Username string (maximum 63 characters + null terminator) */
    char username[64];
    /** @brief Hashed password value using djb2 algorithm
     * 
     * @warning In production, use proper password hashing (e.g., bcrypt, Argon2)
     * instead of simple hash functions for security.
     */
    uint32_t password_hash;  // Simple hash, in production use proper password hashing
    /** @brief Active status flag (1 = active, 0 = inactive) */
    int is_active;
} User;

/**
 * @brief Hash Table Node structure
 * 
 * Represents a single entry in the hash table. Uses chaining for collision handling.
 * Each node contains a User and a pointer to the next node in the chain.
 */
typedef struct HashNode {
    /** @brief User data stored in this node */
    User user;
    /** @brief Pointer to the next node in the collision chain (NULL if last node) */
    struct HashNode* next;  // For chaining (collision handling)
} HashNode;

/**
 * @brief Hash Table structure
 * 
 * Main hash table data structure that stores users using hash buckets.
 * Uses separate chaining to handle collisions.
 */
typedef struct {
    /** @brief Array of hash buckets (each bucket is a linked list of HashNodes) */
    HashNode* buckets[HASH_TABLE_SIZE];
    /** @brief Current number of users stored in the hash table */
    size_t size;  // Number of users in the table
} HashTable;

/**
 * @}
 */

/**
 * @defgroup Stack Stack Implementation
 * @brief Stack data structure for login history tracking
 * @{
 */

/**
 * @def MAX_STACK_SIZE
 * @brief Maximum number of login history entries
 */
#define MAX_STACK_SIZE 100

/**
 * @brief Login history entry structure
 * 
 * Represents a single login event in the history.
 */
typedef struct {
    /** @brief Username that logged in */
    char username[64];
    /** @brief Timestamp of login (simple counter, can be replaced with time_t) */
    uint32_t timestamp;
} LoginHistoryEntry;

/**
 * @brief Stack node structure
 * 
 * Represents a single entry in the login history stack.
 */
typedef struct StackNode {
    /** @brief Login history entry data */
    LoginHistoryEntry entry;
    /** @brief Pointer to the next node in the stack (NULL if bottom) */
    struct StackNode* next;
} StackNode;

/**
 * @brief Stack structure for login history
 * 
 * LIFO (Last In, First Out) data structure for tracking login history.
 */
typedef struct {
    /** @brief Pointer to the top of the stack (NULL if empty) */
    StackNode* top;
    /** @brief Current number of entries in the stack */
    size_t size;
    /** @brief Maximum capacity of the stack */
    size_t capacity;
} LoginHistoryStack;

/**
 * @}
 */

/**
 * @brief Initialize the inventory management system.
 * @param filename Optional filename to load users from (NULL to start fresh).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_Init(const char* filename);

/**
 * @brief Cleanup the inventory management system.
 * @param filename Optional filename to save users to (NULL to skip save).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_Cleanup(const char* filename);

#ifdef ENABLE_INVENTORYMANAGER_TEST
/**
 * @brief Set test malloc hook (for testing only)
 * @param hook Function pointer to malloc replacement, or NULL to use standard malloc
 * 
 * This function is only available in test builds and allows tests to inject
 * a custom malloc implementation for testing memory allocation failures.
 */
void InventoryManager_SetMallocHook(void* (*hook)(size_t));
#endif

// Hash Table Functions for User Authentication

/**
 * @brief Create a new hash table.
 * @return Pointer to the hash table, or NULL on error.
 */
HashTable* HashTable_Create(void);

/**
 * @brief Destroy a hash table and free all memory.
 * @param ht Pointer to the hash table.
 */
void HashTable_Destroy(HashTable* ht);

/**
 * @brief Hash function for username (djb2 algorithm).
 * @param username The username to hash.
 * @return Hash value (0 to HASH_TABLE_SIZE-1).
 */
size_t HashTable_Hash(const char* username);

/**
 * @brief Add a user to the hash table.
 * @param ht Pointer to the hash table.
 * @param username The username (max 63 characters).
 * @param password The password (will be hashed).
 * @return 0 on success, -1 on error (e.g., duplicate username).
 */
int HashTable_AddUser(HashTable* ht, const char* username, const char* password);

/**
 * @brief Find a user in the hash table by username.
 * @param ht Pointer to the hash table.
 * @param username The username to search for.
 * @return Pointer to the User structure, or NULL if not found.
 */
User* HashTable_FindUser(HashTable* ht, const char* username);

/**
 * @brief Authenticate a user by username and password.
 * @param ht Pointer to the hash table.
 * @param username The username.
 * @param password The password.
 * @return 1 if authentication successful, 0 otherwise.
 */
int HashTable_Authenticate(HashTable* ht, const char* username, const char* password);

/**
 * @brief Remove a user from the hash table.
 * @param ht Pointer to the hash table.
 * @param username The username to remove.
 * @return 0 on success, -1 if user not found.
 */
int HashTable_RemoveUser(HashTable* ht, const char* username);

/**
 * @brief Get the number of users in the hash table.
 * @param ht Pointer to the hash table.
 * @return Number of users.
 */
size_t HashTable_GetSize(HashTable* ht);

/**
 * @brief Simple password hashing function (djb2 algorithm).
 * @param password The password to hash.
 * @return Hash value.
 */
uint32_t HashTable_HashPassword(const char* password);

/**
 * @brief Save hash table to binary file.
 * @param ht Pointer to the hash table.
 * @param filename The filename to save to.
 * @return 0 on success, -1 on error.
 * 
 * @note Uses binary format (fwrite/fread) as required.
 * @note File format: First writes size, then writes each user sequentially.
 */
int HashTable_SaveToFile(HashTable* ht, const char* filename);

/**
 * @brief Load hash table from binary file.
 * @param ht Pointer to the hash table (will be created if NULL).
 * @param filename The filename to load from.
 * @return Pointer to hash table on success, NULL on error.
 * 
 * @note Uses binary format (fwrite/fread) as required.
 * @note If ht is NULL, a new hash table will be created.
 */
HashTable* HashTable_LoadFromFile(HashTable* ht, const char* filename);

// Stack Functions for Login History

/**
 * @brief Create a new login history stack.
 * @param capacity Maximum number of entries (0 for default MAX_STACK_SIZE).
 * @return Pointer to the stack, or NULL on error.
 */
LoginHistoryStack* LoginHistoryStack_Create(size_t capacity);

/**
 * @brief Destroy a login history stack and free all memory.
 * @param stack Pointer to the stack.
 */
void LoginHistoryStack_Destroy(LoginHistoryStack* stack);

/**
 * @brief Push a login entry onto the stack.
 * @param stack Pointer to the stack.
 * @param username The username that logged in.
 * @return 0 on success, -1 on error (stack full or NULL parameters).
 */
int LoginHistoryStack_Push(LoginHistoryStack* stack, const char* username);

/**
 * @brief Pop a login entry from the stack.
 * @param stack Pointer to the stack.
 * @param entry Pointer to store the popped entry (can be NULL).
 * @return 0 on success, -1 on error (stack empty or NULL stack).
 */
int LoginHistoryStack_Pop(LoginHistoryStack* stack, LoginHistoryEntry* entry);

/**
 * @brief Peek at the top entry without removing it.
 * @param stack Pointer to the stack.
 * @param entry Pointer to store the top entry.
 * @return 0 on success, -1 on error (stack empty or NULL parameters).
 */
int LoginHistoryStack_Peek(LoginHistoryStack* stack, LoginHistoryEntry* entry);

/**
 * @brief Check if stack is empty.
 * @param stack Pointer to the stack.
 * @return 1 if empty, 0 if not empty, -1 if NULL.
 */
int LoginHistoryStack_IsEmpty(LoginHistoryStack* stack);

/**
 * @brief Get the number of entries in the stack.
 * @param stack Pointer to the stack.
 * @return Number of entries, or 0 if NULL.
 */
size_t LoginHistoryStack_GetSize(LoginHistoryStack* stack);

/**
 * @brief Get the global login history stack.
 * @return Pointer to the global stack, or NULL if not initialized.
 */
LoginHistoryStack* InventoryManager_GetLoginHistory(void);

/**
 * @brief Add a login entry to the global history.
 * @param username The username that logged in.
 * @return 0 on success, -1 on error.
 */
int InventoryManager_AddLoginHistory(const char* username);

/**
 * @brief View recent login history.
 * @param count Number of recent entries to display (0 for all).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_ViewLoginHistory(size_t count);

/**
 * @brief Get the global hash table for user operations.
 * @return Pointer to the global hash table, or NULL if not initialized.
 */
HashTable* InventoryManager_GetHashTable(void);

/**
 * @brief Register a new user (wrapper function).
 * @param username The username (max 63 characters).
 * @param password The password.
 * @return 0 on success, -1 on error.
 */
int InventoryManager_RegisterUser(const char* username, const char* password);

/**
 * @brief Login a user (wrapper function with history tracking).
 * @param username The username.
 * @param password The password.
 * @return 1 if authentication successful, 0 otherwise.
 */
int InventoryManager_LoginUser(const char* username, const char* password);

// Material Inventory Management Functions

/**
 * @brief Get the global material list.
 * @return Pointer to the global material list, or NULL if not initialized.
 */
MaterialList* InventoryManager_GetMaterialList(void);

/**
 * @brief Initialize material inventory (load from file if exists).
 * @param filename Optional filename to load materials from (NULL to start fresh).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_InitMaterialInventory(const char* filename);

/**
 * @brief Cleanup material inventory (save to file).
 * @param filename Optional filename to save materials to (NULL to skip save).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_CleanupMaterialInventory(const char* filename);

// Project Tracking Functions

/**
 * @brief Get the global project stack.
 * @return Pointer to the global project stack, or NULL if not initialized.
 */
ProjectStack* InventoryManager_GetProjectStack(void);

/**
 * @brief Initialize project tracking (load from file if exists).
 * @param filename Optional filename to load projects from (NULL to start fresh).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_InitProjectTracking(const char* filename);

/**
 * @brief Cleanup project tracking (save to file).
 * @param filename Optional filename to save projects to (NULL to skip save).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_CleanupProjectTracking(const char* filename);

// Expense Tracking Functions

/**
 * @brief Get the global expense matrix.
 * @return Pointer to the global expense matrix, or NULL if not initialized.
 */
ExpenseMatrix* InventoryManager_GetExpenseMatrix(void);

/**
 * @brief Initialize expense tracking (load from file if exists).
 * @param filename Optional filename to load expenses from (NULL to start fresh).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_InitExpenseTracking(const char* filename);

/**
 * @brief Cleanup expense tracking (save to file).
 * @param filename Optional filename to save expenses to (NULL to skip save).
 * @return 0 on success, -1 on error.
 */
int InventoryManager_CleanupExpenseTracking(const char* filename);

#ifdef __cplusplus
}
#endif

#endif // INVENTORY_MANAGER_H