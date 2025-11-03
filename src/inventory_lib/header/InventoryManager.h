
#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This block ensures that C++ code (like our tests)
// can correctly call these C functions.
#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief Defines the structure for a user.
     * @file InventoryManager.h
     */
    typedef struct {
        int id;
        char username[50];
        char password[50];
    } User;

    /**
     * @brief Defines the structure for a material in the inventory.
     */
    typedef struct {
        int id;
        char name[100];
        int quantity;
        double price;
    } Material;


    // --- Hash Table Data Structures (Requirement 1) ---

    /**
     * @brief A node in the hash table chain.
     * Contains the User data and a pointer to the next node (for collision handling).
     */
    typedef struct HashNode {
        User user;
        struct HashNode* next;
    } HashNode;

    /**
     * @brief The Hash Table structure.
     * Contains an array of pointers to HashNodes (the "buckets").
     */
    typedef struct {
        int size;
        HashNode** table;
        int element_count; // To track total users
    } HashTable;


    // --- User Authentication Functions (using Hash Table) ---

    /**
     * @brief Creates a new user and adds them to the hash table.
     * @param username The username for the new user.
     * @param password The password for the new user.
     * @return 1 on success, 0 on failure (e.g., user exists).
     */
    int createUser(const char* username, const char* password);

    /**
     * @brief Finds a user in the hash table by their username.
     * @param username The username to search for.
     * @return A pointer to the *internal* User struct if found, or NULL if not found.
     * @warning Do not modify the returned pointer; it points to internal data.
     */
    User* findUserByName(const char* username);


    // --- Binary File Operations (Critical Requirement) ---

    /**
     * @brief Saves all user data from the hash table to a binary file.
     * @param filename The name of the binary file (e.g., "users.bin").
     * @return 1 on success, 0 on failure.
     */
    int saveUsersToBinary(const char* filename);

    /**
     * @brief Loads all user data from a binary file into the hash table.
     * @param filename The name of the binary file (e.g., "users.bin").
     * @return 1 on success (if data was read), 0 on failure.
     */
    int loadUsersToBinary(const char* filename);


    // --- Test Utility Functions (DO NOT USE IN APP) ---

    /**
     * @brief Initializes the global user hash table.
     * FOR TESTING: This clears any existing table and creates a new one.
     */
    void ht_init_user_table();

    /**
     * @brief Frees all memory associated with the global user hash table.
     * FOR TESTING: This is used to clean up after tests.
     */
    void ht_free_user_table();


    // Close the 'extern "C"' block
#ifdef __cplusplus
}
#endif

#endif // INVENTORY_MANAGER_H