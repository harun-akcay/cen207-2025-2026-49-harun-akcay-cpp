/**
 * @file InventoryManager.h
 * 
 * @brief Provides functions for inventory management utilities
 */

#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include <stddef.h>
#include <stdint.h>

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
 * @brief Initialize the inventory management system.
 * @return 0 on success, -1 on error.
 */
int InventoryManager_Init(void);

/**
 * @brief Cleanup the inventory management system.
 * @return 0 on success, -1 on error.
 */
int InventoryManager_Cleanup(void);

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

#ifdef __cplusplus
}
#endif

#endif // INVENTORY_MANAGER_H