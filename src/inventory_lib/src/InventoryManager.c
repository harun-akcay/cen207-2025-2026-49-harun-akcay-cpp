
#include "InventoryManager.h"

// --- Hash Table Implementation ---

#define HASH_TABLE_SIZE 10 // Keep small for easy collision testing

/**
 * @brief The global hash table for storing users.
 */
static HashTable* user_hash_table = NULL;

/**
 * @brief A simple DJB2 hash function for strings.
 * @param str The string to hash (username).
 * @return The hash value.
 */
static unsigned long hash_function_djb2(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

/**
 * @brief Creates a new HashNode.
 * @param user The User data to store.
 * @return A pointer to the new HashNode, or NULL on allocation failure.
 */
static HashNode* ht_create_node(User user) {
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) {
        return NULL;
    }
    new_node->user = user;
    new_node->next = NULL;
    return new_node;
}

/**
 * @brief (Internal) Frees the entire hash table and all its nodes.
 */
static void ht_free_internal(HashTable* ht) {
    if (!ht) return;
    for (int i = 0; i < ht->size; ++i) {
        HashNode* node = ht->table[i];
        while (node) {
            HashNode* temp = node;
            node = node->next;
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}

/**
 * @brief (Internal) Initializes a new hash table.
 */
static HashTable* ht_init_internal() {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) return NULL;

    ht->size = HASH_TABLE_SIZE;
    ht->element_count = 0;
    // Calloc initializes all pointers to NULL
    ht->table = (HashNode**)calloc(ht->size, sizeof(HashNode*));
    if (!ht->table) {
        free(ht);
        return NULL;
    }
    return ht;
}

// --- Public Function Implementations ---

void ht_init_user_table() {
    // If table already exists, free it first (for testing)
    if (user_hash_table) {
        ht_free_internal(user_hash_table);
    }
    user_hash_table = ht_init_internal();
}

void ht_free_user_table() {
    ht_free_internal(user_hash_table);
    user_hash_table = NULL;
}

int createUser(const char* username, const char* password) {
    if (!user_hash_table) {
        ht_init_user_table(); // Ensure table exists
    }

    // Check if user already exists
    if (findUserByName(username) != NULL) {
        return 0; // User already exists
    }

    // Create new user
    User newUser;
    newUser.id = user_hash_table->element_count + 1;
    strncpy(newUser.username, username, 50);
    newUser.username[49] = '\0';
    strncpy(newUser.password, password, 50);
    newUser.password[49] = '\0';

    // Create new hash node
    HashNode* new_node = ht_create_node(newUser);
    if (!new_node) return 0; // Malloc failure

    // Calculate hash index
    unsigned long hash_index = hash_function_djb2(username) % HASH_TABLE_SIZE;

    // Insert at the beginning of the chain (collision handling)
    new_node->next = user_hash_table->table[hash_index];
    user_hash_table->table[hash_index] = new_node;
    user_hash_table->element_count++;

    return 1; // Success
}

User* findUserByName(const char* username) {
    if (!user_hash_table) return NULL; // Table not initialized

    unsigned long hash_index = hash_function_djb2(username) % HASH_TABLE_SIZE;

    HashNode* node = user_hash_table->table[hash_index];
    while (node) {
        if (strcmp(node->user.username, username) == 0) {
            return &(node->user); // Found
        }
        node = node->next;
    }
    return NULL; // Not found
}

int saveUsersToBinary(const char* filename) {
    if (!user_hash_table) return 0; // Nothing to save

    FILE* file = fopen(filename, "wb");
    if (!file) return 0;

    // --- CRITICAL BINARY I/O REQUIREMENT ---
    // We must traverse the data structure, not dump pointers.
    for (int i = 0; i < user_hash_table->size; ++i) {
        HashNode* node = user_hash_table->table[i];
        while (node) {
            // Write the User struct (structured data)
            fwrite(&(node->user), sizeof(User), 1, file);
            node = node->next;
        }
    }

    fclose(file);
    return 1;
}

int loadUsersToBinary(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        return 0; // File not found, do nothing
    }

    // Ensure table is clean and ready
    ht_init_user_table();

    User tempUser;
    // Read one User struct at a time
    while (fread(&tempUser, sizeof(User), 1, file) == 1) {
        // Re-insert into the hash table
        // Note: This uses the public 'createUser' which checks for duplicates.
        // A faster internal insert could be used if we trust the file.
        createUser(tempUser.username, tempUser.password);
    }

    fclose(file);
    return 1;
}

// Old test-only function, now replaced by ht_init_user_table
void resetUserDatabase() {
    ht_init_user_table();
}