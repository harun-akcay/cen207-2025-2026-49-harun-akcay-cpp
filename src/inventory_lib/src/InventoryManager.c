/**
 * @file InventoryManager.c
 * @brief Implementation of inventory management functions.
 */

#include "../header/InventoryManager.h"
#include "../header/MaterialInventory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Disable MSVC warnings for deprecated functions (strncpy, etc.)
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Test hook for memory allocation (only used in test builds)
#ifdef ENABLE_INVENTORYMANAGER_TEST
// Function pointer for test malloc override (shared between InventoryManager.c and MaterialInventory.c)
void* (*test_malloc_hook)(size_t) = NULL;

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

/** @brief Global login history stack */
static LoginHistoryStack* g_login_history = NULL;

/** @brief Timestamp counter for login history */
static uint32_t g_timestamp_counter = 1;

/** @brief Global material list for inventory */
static MaterialList* g_material_list = NULL;

int InventoryManager_Init(const char* filename) {
    /**
     * @brief Initialize inventory management system
     * 
     * Creates and initializes the global hash table for user authentication
     * and the global login history stack.
     * If a filename is provided, attempts to load users from binary file.
     * This function should be called before using any inventory management functions.
     * 
     * @param filename Optional filename to load users from (NULL to start fresh)
     * @return 0 on success, -1 on error (memory allocation failure)
     */
    if (g_user_hash_table == NULL) {
        // Try to load from file if filename is provided
        if (filename != NULL) {
            g_user_hash_table = HashTable_LoadFromFile(NULL, filename);
            if (g_user_hash_table != NULL) {
                // Initialize login history stack
                if (g_login_history == NULL) {
                    g_login_history = LoginHistoryStack_Create(MAX_STACK_SIZE);
                    if (g_login_history == NULL) {
                        return -1;
                    }
                }
                return 0; // Successfully loaded
            }
            // If load failed, continue to create new table
        }
        
        // Create new hash table
        g_user_hash_table = HashTable_Create();
        if (g_user_hash_table == NULL) {
            return -1;
        }
    }
    
    // Initialize login history stack if not already initialized
    if (g_login_history == NULL) {
        g_login_history = LoginHistoryStack_Create(MAX_STACK_SIZE);
        if (g_login_history == NULL) {
            return -1;
        }
    }
    
    // Initialize material inventory if not already initialized
    if (g_material_list == NULL) {
        if (InventoryManager_InitMaterialInventory("materials.bin") != 0) {
            return -1;
        }
    }
    
    return 0;
}

int InventoryManager_Cleanup(const char* filename) {
    /**
     * @brief Cleanup inventory management system
     * 
     * Saves the global hash table to binary file if filename is provided,
     * then destroys the global hash table and login history stack, freeing
     * all associated memory.
     * This function should be called when the inventory management system
     * is no longer needed (e.g., at program shutdown).
     * 
     * @param filename Optional filename to save users to (NULL to skip save)
     * @return 0 on success, -1 on error (file save error)
     */
    if (g_user_hash_table != NULL) {
        // Save to file if filename is provided
        if (filename != NULL) {
            HashTable_SaveToFile(g_user_hash_table, filename);
        }
        
        HashTable_Destroy(g_user_hash_table);
        g_user_hash_table = NULL;
    }
    
    // Cleanup login history stack
    if (g_login_history != NULL) {
        LoginHistoryStack_Destroy(g_login_history);
        g_login_history = NULL;
    }
    
    // Cleanup material inventory
    if (g_material_list != NULL) {
        InventoryManager_CleanupMaterialInventory("materials.bin");
        g_material_list = NULL;
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

int HashTable_SaveToFile(HashTable* ht, const char* filename) {
    /**
     * @brief Save hash table to binary file
     * 
     * Saves the hash table to a binary file using fwrite().
     * The file format is:
     * 1. size_t size (number of users)
     * 2. For each user: User structure (username, password_hash, is_active)
     * 
     * @param ht Pointer to the hash table (must not be NULL)
     * @param filename The filename to save to (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, file I/O error)
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     * @note All users are saved sequentially, regardless of their bucket position.
     */
    if (ht == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write the number of users first
    size_t user_count = ht->size;
    if (fwrite(&user_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all users sequentially
    for (size_t i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* current = ht->buckets[i];
        while (current != NULL) {
            // Write User structure
            if (fwrite(&(current->user), sizeof(User), 1, file) != 1) {
                fclose(file);
                return -1;
            }
            current = current->next;
        }
    }
    
    fclose(file);
    return 0;
}

HashTable* HashTable_LoadFromFile(HashTable* ht, const char* filename) {
    /**
     * @brief Load hash table from binary file
     * 
     * Loads the hash table from a binary file using fread().
     * The file format is:
     * 1. size_t size (number of users)
     * 2. For each user: User structure (username, password_hash, is_active)
     * 
     * @param ht Pointer to the hash table (if NULL, a new hash table will be created)
     * @param filename The filename to load from (must not be NULL)
     * @return Pointer to hash table on success, NULL on error
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     * @note If ht is NULL, a new hash table will be created and returned.
     * @note If ht is not NULL, existing users will be preserved and new users will be added.
     */
    if (filename == NULL) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL; // File doesn't exist or can't be opened
    }
    
    // Create hash table if not provided
    if (ht == NULL) {
        ht = HashTable_Create();
        if (ht == NULL) {
            fclose(file);
            return NULL;
        }
    }
    
    // Read the number of users
    size_t user_count;
    if (fread(&user_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        if (ht != NULL && HashTable_GetSize(ht) == 0) {
            HashTable_Destroy(ht);
        }
        return NULL;
    }
    
    // Read all users and add them to the hash table
    for (size_t i = 0; i < user_count; i++) {
        User user;
        if (fread(&user, sizeof(User), 1, file) != 1) {
            fclose(file);
            return ht; // Return partial load
        }
        
        // Add user to hash table (password will be stored as-is since it's already hashed)
        // Note: We need to reconstruct the password or use a special flag
        // For now, we'll add the user directly by creating a node
        size_t index = HashTable_Hash(user.username);
        
        HashNode* new_node = (HashNode*)safe_malloc(sizeof(HashNode));
        if (new_node == NULL) {
            fclose(file);
            return ht; // Return partial load
        }
        
        // Copy user data
        new_node->user = user;
        
        // Insert at the beginning of the chain
        new_node->next = ht->buckets[index];
        ht->buckets[index] = new_node;
        ht->size++;
    }
    
    fclose(file);
    return ht;
}

// Stack Implementation for Login History

LoginHistoryStack* LoginHistoryStack_Create(size_t capacity) {
    /**
     * @brief Create a new login history stack
     * 
     * Allocates memory for a new login history stack and initializes it.
     * The stack is ready to use after creation.
     * 
     * @param capacity Maximum number of entries (0 for default MAX_STACK_SIZE)
     * @return Pointer to the newly created stack, or NULL on memory allocation failure
     * 
     * @note The caller is responsible for destroying the stack using LoginHistoryStack_Destroy()
     * to avoid memory leaks.
     */
    LoginHistoryStack* stack = (LoginHistoryStack*)safe_malloc(sizeof(LoginHistoryStack));
    if (stack == NULL) {
        return NULL;
    }
    
    stack->top = NULL;
    stack->size = 0;
    stack->capacity = (capacity == 0) ? MAX_STACK_SIZE : capacity;
    
    return stack;
}

void LoginHistoryStack_Destroy(LoginHistoryStack* stack) {
    /**
     * @brief Destroy a login history stack and free all memory
     * 
     * Frees all nodes in the stack and then frees the stack structure itself.
     * This function is safe to call with NULL pointer (no-op).
     * 
     * @param stack Pointer to the stack to destroy (can be NULL)
     * 
     * @note After calling this function, the stack pointer becomes invalid
     * and should not be used.
     */
    if (stack == NULL) {
        return;
    }
    
    // Pop all entries
    while (stack->top != NULL) {
        StackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    
    free(stack);
}

int LoginHistoryStack_Push(LoginHistoryStack* stack, const char* username) {
    /**
     * @brief Push a login entry onto the stack
     * 
     * Adds a new login history entry to the top of the stack.
     * If the stack is full, the oldest entry (bottom) is removed.
     * 
     * @param stack Pointer to the stack (must not be NULL)
     * @param username The username that logged in (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters or stack full)
     */
    if (stack == NULL || username == NULL) {
        return -1;
    }
    
    // If stack is full, remove oldest entry (bottom)
    if (stack->size >= stack->capacity) {
        // Find the second-to-last node
        if (stack->top != NULL && stack->top->next != NULL) {
            StackNode* current = stack->top;
            while (current->next->next != NULL) {
                current = current->next;
            }
            // Remove the last node
            free(current->next);
            current->next = NULL;
            stack->size--;
        } else if (stack->top != NULL) {
            // Only one node, remove it
            free(stack->top);
            stack->top = NULL;
            stack->size = 0;
        }
    }
    
    // Create new node
    StackNode* new_node = (StackNode*)safe_malloc(sizeof(StackNode));
    if (new_node == NULL) {
        return -1;
    }
    
    // Initialize entry
    strncpy(new_node->entry.username, username, sizeof(new_node->entry.username) - 1);
    new_node->entry.username[sizeof(new_node->entry.username) - 1] = '\0';
    new_node->entry.timestamp = g_timestamp_counter++;
    
    // Push to top
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
    
    return 0;
}

int LoginHistoryStack_Pop(LoginHistoryStack* stack, LoginHistoryEntry* entry) {
    /**
     * @brief Pop a login entry from the stack
     * 
     * Removes and returns the top entry from the stack (LIFO).
     * 
     * @param stack Pointer to the stack (must not be NULL)
     * @param entry Pointer to store the popped entry (can be NULL)
     * @return 0 on success, -1 on error (stack empty or NULL stack)
     */
    if (stack == NULL || stack->top == NULL) {
        return -1;
    }
    
    StackNode* top_node = stack->top;
    
    // Copy entry if requested
    if (entry != NULL) {
        *entry = top_node->entry;
    }
    
    // Remove top node
    stack->top = top_node->next;
    free(top_node);
    stack->size--;
    
    return 0;
}

int LoginHistoryStack_Peek(LoginHistoryStack* stack, LoginHistoryEntry* entry) {
    /**
     * @brief Peek at the top entry without removing it
     * 
     * Returns the top entry without removing it from the stack.
     * 
     * @param stack Pointer to the stack (must not be NULL)
     * @param entry Pointer to store the top entry (must not be NULL)
     * @return 0 on success, -1 on error (stack empty or NULL parameters)
     */
    if (stack == NULL || stack->top == NULL || entry == NULL) {
        return -1;
    }
    
    *entry = stack->top->entry;
    return 0;
}

int LoginHistoryStack_IsEmpty(LoginHistoryStack* stack) {
    /**
     * @brief Check if stack is empty
     * 
     * @param stack Pointer to the stack
     * @return 1 if empty, 0 if not empty, -1 if NULL
     */
    if (stack == NULL) {
        return -1;
    }
    return (stack->top == NULL) ? 1 : 0;
}

size_t LoginHistoryStack_GetSize(LoginHistoryStack* stack) {
    /**
     * @brief Get the number of entries in the stack
     * 
     * @param stack Pointer to the stack
     * @return Number of entries, or 0 if NULL
     */
    if (stack == NULL) {
        return 0;
    }
    return stack->size;
}

LoginHistoryStack* InventoryManager_GetLoginHistory(void) {
    /**
     * @brief Get the global login history stack
     * 
     * Returns a pointer to the global login history stack.
     * The stack is initialized by InventoryManager_Init().
     * 
     * @return Pointer to the global stack, or NULL if not initialized
     */
    return g_login_history;
}

int InventoryManager_AddLoginHistory(const char* username) {
    /**
     * @brief Add a login entry to the global history
     * 
     * Adds a login entry to the global login history stack.
     * 
     * @param username The username that logged in (must not be NULL)
     * @return 0 on success, -1 on error (not initialized or NULL username)
     */
    if (g_login_history == NULL || username == NULL) {
        return -1;
    }
    
    return LoginHistoryStack_Push(g_login_history, username);
}

int InventoryManager_ViewLoginHistory(size_t count) {
    /**
     * @brief View recent login history
     * 
     * Displays the recent login history entries. If count is 0, displays all entries.
     * 
     * @param count Number of recent entries to display (0 for all)
     * @return 0 on success, -1 on error (not initialized)
     */
    if (g_login_history == NULL) {
        return -1;
    }
    
    if (LoginHistoryStack_IsEmpty(g_login_history)) {
        printf("No login history available.\n");
        return 0;
    }
    
    // Display entries (copy stack to preserve it)
    LoginHistoryEntry* entries = (LoginHistoryEntry*)safe_malloc(sizeof(LoginHistoryEntry) * g_login_history->size);
    if (entries == NULL) {
        return -1;
    }
    
    // Pop all entries and store them
    size_t entry_count = 0;
    LoginHistoryEntry entry;
    while (LoginHistoryStack_Pop(g_login_history, &entry) == 0) {
        entries[entry_count++] = entry;
    }
    
    // Display entries (most recent first)
    size_t display_count = (count == 0 || count > entry_count) ? entry_count : count;
    printf("\n--- Recent Login History (Last %zu entries) ---\n", display_count);
    printf("%-5s %-20s %-15s\n", "No.", "Username", "Timestamp");
    printf("------------------------------------------------\n");
    
    for (size_t i = 0; i < display_count; i++) {
        printf("%-5zu %-20s %-15u\n", i + 1, entries[i].username, entries[i].timestamp);
    }
    
    // Restore stack (push back in reverse order to maintain original order)
    for (int i = (int)entry_count - 1; i >= 0; i--) {
        LoginHistoryStack_Push(g_login_history, entries[i].username);
    }
    
    free(entries);
    return 0;
}

// Wrapper Functions for User Authentication

HashTable* InventoryManager_GetHashTable(void) {
    /**
     * @brief Get the global hash table for user operations
     * 
     * Returns a pointer to the global hash table used for user authentication.
     * The hash table is initialized by InventoryManager_Init().
     * 
     * @return Pointer to the global hash table, or NULL if not initialized
     */
    return g_user_hash_table;
}

int InventoryManager_RegisterUser(const char* username, const char* password) {
    /**
     * @brief Register a new user (wrapper function)
     * 
     * Registers a new user in the global hash table.
     * 
     * @param username The username (max 63 characters, must not be NULL)
     * @param password The password (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, duplicate username, or not initialized)
     */
    if (g_user_hash_table == NULL || username == NULL || password == NULL) {
        return -1;
    }
    
    return HashTable_AddUser(g_user_hash_table, username, password);
}

int InventoryManager_LoginUser(const char* username, const char* password) {
    /**
     * @brief Login a user (wrapper function with history tracking)
     * 
     * Authenticates a user and adds the login to the history stack if successful.
     * 
     * @param username The username (must not be NULL)
     * @param password The password (must not be NULL)
     * @return 1 if authentication successful, 0 otherwise
     */
    if (g_user_hash_table == NULL || username == NULL || password == NULL) {
        return 0;
    }
    
    int result = HashTable_Authenticate(g_user_hash_table, username, password);
    
    // Add to login history if successful
    if (result == 1) {
        InventoryManager_AddLoginHistory(username);
    }
    
    return result;
}

// Material Inventory Management Functions

MaterialList* InventoryManager_GetMaterialList(void) {
    /**
     * @brief Get the global material list
     * 
     * Returns a pointer to the global material list used for inventory management.
     * The material list is initialized by InventoryManager_Init().
     * 
     * @return Pointer to the global material list, or NULL if not initialized
     */
    return g_material_list;
}

int InventoryManager_InitMaterialInventory(const char* filename) {
    /**
     * @brief Initialize material inventory (load from file if exists)
     * 
     * Creates and initializes the global material list for inventory management.
     * If a filename is provided, attempts to load materials from binary file.
     * 
     * @param filename Optional filename to load materials from (NULL to start fresh)
     * @return 0 on success, -1 on error (memory allocation failure)
     */
    if (g_material_list == NULL) {
        // Try to load from file if filename is provided
        if (filename != NULL) {
            g_material_list = MaterialList_LoadFromFile(NULL, filename);
            if (g_material_list != NULL) {
                return 0; // Successfully loaded
            }
            // If load failed, continue to create new list
        }
        
        // Create new material list
        g_material_list = MaterialList_Create();
        if (g_material_list == NULL) {
            return -1;
        }
    }
    return 0;
}

int InventoryManager_CleanupMaterialInventory(const char* filename) {
    /**
     * @brief Cleanup material inventory (save to file)
     * 
     * Saves the global material list to binary file if filename is provided,
     * then destroys the global material list and frees all associated memory.
     * 
     * @param filename Optional filename to save materials to (NULL to skip save)
     * @return 0 on success, -1 on error (file save error)
     */
    if (g_material_list != NULL) {
        // Save to file if filename is provided
        if (filename != NULL) {
            MaterialList_SaveToFile(g_material_list, filename);
        }
        
        MaterialList_Destroy(g_material_list);
        g_material_list = NULL;
    }
    return 0;
}