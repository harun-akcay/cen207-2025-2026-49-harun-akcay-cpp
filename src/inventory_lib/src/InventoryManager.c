/**
 * @file InventoryManager.c
 * @brief Implementation of inventory management functions.
 */

#include "../header/InventoryManager.h"
#include <stdlib.h>
#include <string.h>

// Global hash table for user authentication
static HashTable* g_user_hash_table = NULL;

int InventoryManager_Init(void) {
    // TODO: Initialize inventory management system
    if (g_user_hash_table == NULL) {
        g_user_hash_table = HashTable_Create();
        if (g_user_hash_table == NULL) {
            return -1;
        }
    }
    return 0;
}

int InventoryManager_Cleanup(void) {
    // TODO: Cleanup inventory management system
    if (g_user_hash_table != NULL) {
        HashTable_Destroy(g_user_hash_table);
        g_user_hash_table = NULL;
    }
    return 0;
}

// Hash Table Implementation

size_t HashTable_Hash(const char* username) {
    // djb2 hash algorithm
    size_t hash = 5381;
    int c;
    
    while ((c = *username++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % HASH_TABLE_SIZE;
}

uint32_t HashTable_HashPassword(const char* password) {
    // djb2 hash algorithm for password
    uint32_t hash = 5381;
    int c;
    
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash;
}

HashTable* HashTable_Create(void) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
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
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
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
    if (ht == NULL) {
        return 0;
    }
    return ht->size;
}