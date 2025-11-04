/**
 * @file InventoryManager.c
 * @brief Implementation of inventory management functions.
 */

#include "../header/InventoryManager.h"
#include <stdlib.h>
#include <string.h>

// Test hook for memory allocation (only used in test builds)
#ifdef ENABLE_INVENTORYMANAGER_TEST
// Function pointer for test malloc override
static void* (*test_malloc_hook)(size_t) = NULL;

/**
 * @brief Set test malloc hook (for testing only)
 * @param hook Function pointer to malloc replacement, or NULL to use standard malloc
 */
void InventoryManager_SetMallocHook(void* (*hook)(size_t)) {
    test_malloc_hook = hook;
}

// Internal malloc wrapper that uses test hook if available
static void* safe_malloc(size_t size) {
    if (test_malloc_hook != NULL) {
        return test_malloc_hook(size);
    }
    return malloc(size);
}
#else
// In non-test builds, use standard malloc
#define safe_malloc malloc
#endif

/** @brief Global hash table for user authentication */
static HashTable* g_user_hash_table = NULL;

int InventoryManager_Init(void) {
    /**
     * @brief Initialize inventory management system
     * 
     * Creates and initializes the global hash table for user authentication.
     * This function should be called before using any inventory management functions.
     * 
     * @return 0 on success, -1 on error (memory allocation failure)
     */
    if (g_user_hash_table == NULL) {
        g_user_hash_table = HashTable_Create();
        if (g_user_hash_table == NULL) {
            return -1;
        }
    }
    return 0;
}

int InventoryManager_Cleanup(void) {
    /**
     * @brief Cleanup inventory management system
     * 
     * Destroys the global hash table and frees all associated memory.
     * This function should be called when the inventory management system
     * is no longer needed (e.g., at program shutdown).
     * 
     * @return 0 on success (always succeeds)
     */
    if (g_user_hash_table != NULL) {
        HashTable_Destroy(g_user_hash_table);
        g_user_hash_table = NULL;
    }
    return 0;
}

// Hash Table Implementation

size_t HashTable_Hash(const char* username) {
    /**
     * @brief Hash function for username using djb2 algorithm
     * 
     * Implements the djb2 hash algorithm (Daniel J. Bernstein's hash function).
     * This function converts a username string into a hash value that maps
     * to one of the hash table buckets.
     * 
     * @param username The username string to hash (must not be NULL)
     * @return Hash value in the range [0, HASH_TABLE_SIZE-1]
     * 
     * @note The djb2 algorithm uses: hash = hash * 33 + c
     * This provides good distribution for string keys.
     */
    size_t hash = 5381;
    int c;
    
    while ((c = *username++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % HASH_TABLE_SIZE;
}

uint32_t HashTable_HashPassword(const char* password) {
    /**
     * @brief Hash function for password using djb2 algorithm
     * 
     * Implements the djb2 hash algorithm for password hashing.
     * 
     * @warning This is a simple hash function for educational purposes only.
     * In production systems, use proper password hashing algorithms such as
     * bcrypt, Argon2, or PBKDF2 for security.
     * 
     * @param password The password string to hash (must not be NULL)
     * @return 32-bit hash value of the password
     * 
     * @note The djb2 algorithm uses: hash = hash * 33 + c
     */
    uint32_t hash = 5381;
    int c;
    
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

HashTable* HashTable_Create(void) {
    /**
     * @brief Create a new hash table
     * 
     * Allocates memory for a new hash table and initializes all buckets to NULL.
     * The hash table is ready to use after creation.
     * 
     * @return Pointer to the newly created hash table, or NULL on memory allocation failure
     * 
     * @note The caller is responsible for destroying the hash table using HashTable_Destroy()
     * to avoid memory leaks.
     */
    HashTable* ht = (HashTable*)safe_malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }
    
    // Initialize all buckets to NULL
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        ht->buckets[i] = NULL;
    }

    ht->size = 0;
    return ht;
}

void HashTable_Destroy(HashTable* ht) {
    /**
     * @brief Destroy a hash table and free all memory
     * 
     * Frees all nodes in all buckets and then frees the hash table structure itself.
     * This function is safe to call with NULL pointer (no-op).
     * 
     * @param ht Pointer to the hash table to destroy (can be NULL)
     * 
     * @note After calling this function, the hash table pointer becomes invalid
     * and should not be used.
     */
    if (ht == NULL) {
        return;
    }
    
    // Free all nodes in all buckets
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = ht->buckets[i];
        while (current != NULL) {
            HashNode* next = current->next;
            free(current);
            current = next;
        }
    }
    
    free(ht);
}

int HashTable_AddUser(HashTable* ht, const char* username, const char* password) {
    /**
     * @brief Add a user to the hash table
     * 
     * Creates a new user account and adds it to the hash table. The username
     * is used as the key, and the password is hashed before storage.
     * 
     * @param ht Pointer to the hash table (must not be NULL)
     * @param username The username string (max 63 characters, must not be NULL)
     * @param password The password string to hash and store (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, duplicate username, or memory allocation failure)
     * 
     * @note Usernames longer than 63 characters will be truncated.
     * @note Duplicate usernames are not allowed and will cause the function to return -1.
     */
    if (ht == NULL || username == NULL || password == NULL) {
        return -1;
    }
    
    // Check if user already exists
    if (HashTable_FindUser(ht, username) != NULL) {
        return -1; // Duplicate username
    }
    
    // Calculate hash
    size_t index = HashTable_Hash(username);
    
    // Create new node
    HashNode* new_node = (HashNode*)safe_malloc(sizeof(HashNode));
    if (new_node == NULL) {
        return -1;
    }
    
    // Copy username (ensure null termination)
    size_t username_len = strlen(username);
    size_t copy_len = (username_len < sizeof(new_node->user.username) - 1) 
                      ? username_len 
                      : sizeof(new_node->user.username) - 1;
    memcpy(new_node->user.username, username, copy_len);
    new_node->user.username[copy_len] = '\0';
    
    // Hash password
    new_node->user.password_hash = HashTable_HashPassword(password);
    new_node->user.is_active = 1;
    
    // Insert at the beginning of the chain
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->size++;
    
    return 0;
}

User* HashTable_FindUser(HashTable* ht, const char* username) {
    /**
     * @brief Find a user in the hash table by username
     * 
     * Searches for a user with the given username in the hash table.
     * Uses the hash function to locate the bucket and then searches
     * through the collision chain if necessary.
     * 
     * @param ht Pointer to the hash table (must not be NULL)
     * @param username The username to search for (must not be NULL)
     * @return Pointer to the User structure if found, NULL if not found or on error
     * 
     * @note The returned pointer is valid until the user is removed or the hash table is destroyed.
     */
    if (ht == NULL || username == NULL) {
        return NULL;
    }
    
    // Calculate hash
    size_t index = HashTable_Hash(username);
    
    // Search in the chain
    HashNode* current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            return &(current->user);
        }
        current = current->next;
    }
    
    return NULL; // Not found
}

int HashTable_Authenticate(HashTable* ht, const char* username, const char* password) {
    /**
     * @brief Authenticate a user by username and password
     * 
     * Verifies that a user exists, is active, and the provided password matches
     * the stored password hash.
     * 
     * @param ht Pointer to the hash table (must not be NULL)
     * @param username The username to authenticate (must not be NULL)
     * @param password The password to verify (must not be NULL)
     * @return 1 if authentication successful, 0 otherwise (user not found, inactive, or wrong password)
     */
    if (ht == NULL || username == NULL || password == NULL) {
        return 0;
    }
    
    User* user = HashTable_FindUser(ht, username);
    if (user == NULL || !user->is_active) {
        return 0;
    }
    
    // Check password hash
    uint32_t password_hash = HashTable_HashPassword(password);
    if (user->password_hash == password_hash) {
        return 1;
    }
    
    return 0;
}

int HashTable_RemoveUser(HashTable* ht, const char* username) {
    /**
     * @brief Remove a user from the hash table
     * 
     * Finds and removes a user from the hash table, freeing the associated memory.
     * Handles both the case where the node is the first in the chain and when it's
     * in the middle or end of the chain.
     * 
     * @param ht Pointer to the hash table (must not be NULL)
     * @param username The username to remove (must not be NULL)
     * @return 0 on success, -1 if user not found or on error
     */
    if (ht == NULL || username == NULL) {
        return -1;
    }
    
    // Calculate hash
    size_t index = HashTable_Hash(username);
    
    // Search for the node
    HashNode* current = ht->buckets[index];
    HashNode* prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->user.username, username) == 0) {
            // Found the node, remove it
            if (prev == NULL) {
                // First node in chain
                ht->buckets[index] = current->next;
            } else {
                // Middle or last node
                prev->next = current->next;
            }
            
            free(current);
            ht->size--;
            return 0;
        }
        
        prev = current;
        current = current->next;
    }
    
    return -1; // Not found
}

size_t HashTable_GetSize(HashTable* ht) {
    /**
     * @brief Get the number of users in the hash table
     * 
     * Returns the current number of users stored in the hash table.
     * This is a constant-time operation.
     * 
     * @param ht Pointer to the hash table (can be NULL)
     * @return Number of users in the hash table, or 0 if ht is NULL
     */
    if (ht == NULL) {
        return 0;
    }
    return ht->size;
}