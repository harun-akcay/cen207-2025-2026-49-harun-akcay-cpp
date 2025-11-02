
// This is the C application (console menu).
// We have replaced the old C++ template code with C code
// to match our 'C only' midterm requirement.

#include <stdio.h>
#include <stdlib.h>

// Include our C library header
#include "InventoryManager.h"

/**
 * @brief Main entry point for the console application.
 */
int main(void) {

    printf("Welcome to the Crafter's Inventory Management App!\n");

    // This is a test to check if we can link our C library.
    // We create a dummy user.
    if (createUser("app_test_user", "password123")) {
        printf("Successfully linked inventory_lib and created a user.\n");
    }
    else {
        printf("Failed to link or create a user.\n");
    }

    printf("Application will start here...\n");

    // We will build the menu from 'resim.png' here later.
    // For now, the application just exits.

    return 0;
}