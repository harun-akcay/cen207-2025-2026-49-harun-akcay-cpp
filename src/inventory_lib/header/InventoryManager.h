
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
     * This structure will be used for authentication (later with a hash table).
     * @file InventoryManager.h
     */
    typedef struct {
        int id;
        char username[50];
        char password[50]; // Note: Should be hashed in a real-world app
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

    // --- User Authentication (To be implemented with Hash Table) ---

    /**
     * @brief Creates a new user and adds them to the database.
     * @param username The username for the new user.
     * @param password The password for the new user.
     * @return 1 on success, 0 on failure (e.g., user exists or db is full).
     */
    int createUser(const char* username, const char* password);

    /**
     * @brief Finds a user by their username.
     * @param username The username to search for.
     * @return A pointer to the User struct if found, or NULL if not found.
     */
    User* findUserByName(const char* username);

    // --- Binary File Operations (Critical Requirement) ---

    /**
     * @brief Saves all user data from memory to a binary file.
     * @param filename The name of the binary file (e.g., "users.bin").
     * @return 1 on success, 0 on failure (e.g., file cannot be opened).
     */
    int saveUsersToBinary(const char* filename);

    /**
     * @brief Loads all user data from a binary file into memory.
     * This will overwrite any existing user data in memory.
     * @param filename The name of the binary file (e.g., "users.bin").
     * @return 1 on success (if data was read), 0 on failure (e.g., file not found or empty).
     */
    int loadUsersFromBinary(const char* filename);


    // --- Test Utility Functions ---

    /**
     * @brief Resets the in-memory user database.
     * THIS FUNCTION SHOULD ONLY BE USED FOR TESTING PURPOSES.
     */
    void resetUserDatabase();


    // Close the 'extern "C"' block for C++ compatibility
#ifdef __cplusplus
}
#endif

#endif // INVENTORY_MANAGER_H