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

// Hash Table for User Authentication
#define HASH_TABLE_SIZE 101  // Prime number for better distribution

/**
 * @brief User structure for authentication
 */
typedef struct {
    char username[64];
    uint32_t password_hash;  // Simple hash, in production use proper password hashing
    int is_active;
} User;

/**
 * @brief Hash Table Node structure
 */
typedef struct HashNode {
    User user;
    struct HashNode* next;  // For chaining (collision handling)
} HashNode;

/**
 * @brief Hash Table structure
 */
typedef struct {
    HashNode* buckets[HASH_TABLE_SIZE];
    size_t size;  // Number of users in the table
} HashTable;

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