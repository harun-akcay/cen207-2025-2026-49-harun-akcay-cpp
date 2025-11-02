
#include "InventoryManager.h"

// Temporary simple user array for testing purposes.
// This will be replaced by a proper Hash Table implementation.
#define MAX_USERS 10
static User user_database[MAX_USERS];
static int user_count = 0;

int createUser(const char* username, const char* password) {
    if (user_count >= MAX_USERS) {
        return 0; // Database is full
    }
    if (findUserByName(username) != NULL) {
        return 0; // User already exists
    }

    User* newUser = &user_database[user_count];
    newUser->id = user_count + 1;
    strncpy(newUser->username, username, 50);
    newUser->username[49] = '\0'; // Ensure null-termination
    strncpy(newUser->password, password, 50);
    newUser->password[49] = '\0'; // Ensure null-termination

    user_count++;
    return 1; // Success
}

User* findUserByName(const char* username) {
    for (int i = 0; i < user_count; ++i) {
        if (strcmp(user_database[i].username, username) == 0) {
            return &user_database[i];
        }
    }
    return NULL; // Not found
}

int saveUsersToBinary(const char* filename) {
    FILE* file = fopen(filename, "wb"); // 'wb' = Write Binary
    if (!file) {
        return 0; // Failed to open file
    }

    // Write the structs directly to the file in binary format, as requested.
    size_t written_count = fwrite(user_database, sizeof(User), user_count, file);

    fclose(file);
    return (written_count == user_count) ? 1 : 0; // Return 1 if all users were written
}

int loadUsersFromBinary(const char* filename) {
    FILE* file = fopen(filename, "rb"); // 'rb' = Read Binary
    if (!file) {
        // If file doesn't exist, clear the database and return 0
        resetUserDatabase();
        return 0;
    }

    // Clear the existing in-memory database first
    resetUserDatabase();

    // Read from the file directly into the database array
    size_t read_count = fread(user_database, sizeof(User), MAX_USERS, file);
    user_count = (int)read_count;

    fclose(file);
    // Return 1 if data was read, 0 if file was empty
    return (read_count > 0) ? 1 : 0;
}

// --- Test Utility Function Implementation ---
void resetUserDatabase() {
    // This function resets the static in-memory database
    user_count = 0;
    memset(user_database, 0, sizeof(User) * MAX_USERS);
}