/**
 * @file InventoryApp.c
 * @brief A simple inventory management application for crafters.
 *
 * This program demonstrates the usage of the inventory management library.
 */

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include "../header/InventoryApp.h"
#include "../../inventory_lib/header/InventoryManager.h"

int main(void) {
    printf("Inventory Management System for Crafters\n");
    printf("========================================\n\n");

    // Initialize the inventory management system
    if (InventoryManager_Init() != 0) {
        fprintf(stderr, "Error: Failed to initialize inventory management system\n");
        return EXIT_FAILURE;
    }

    printf("Inventory management system initialized successfully.\n");
    printf("TODO: Implement inventory management features.\n");

    // Cleanup the inventory management system
    if (InventoryManager_Cleanup() != 0) {
        fprintf(stderr, "Warning: Failed to cleanup inventory management system\n");
    }

    printf("Inventory management system cleaned up.\n");
    return EXIT_SUCCESS;
}
