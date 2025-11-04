/**
 * @file InventoryApp.c
 * @brief A simple inventory management application for crafters.
 *
 * This program implements a console-based inventory management system with
 * menu navigation, user authentication, material inventory, project tracking,
 * expense logging, and sales tracking.
 */

 // Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../header/InventoryApp.h"
#include "../../inventory_lib/header/InventoryManager.h"
#include "../../inventory_lib/header/MaterialInventory.h"

/**
 * @brief Clear input buffer
 */
void InventoryApp_ClearInput(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Get integer input from user
 * @param prompt The prompt to display
 * @param value Pointer to store the input value
 * @return 0 on success, -1 on error
 */
int InventoryApp_GetIntInput(const char* prompt, int* value) {
    char buffer[64];
    
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    
    // Use fgets instead of scanf for better input handling
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }
    
    // Remove newline character
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    // Parse integer using sscanf
    if (sscanf(buffer, "%d", value) != 1) {
        return -1;
    }
    
    return 0;
}

/**
 * @brief Get string input from user
 * @param prompt The prompt to display
 * @param buffer Buffer to store the input
 * @param buffer_size Size of the buffer
 * @return 0 on success, -1 on error
 */
int InventoryApp_GetStringInput(const char* prompt, char* buffer, size_t buffer_size) {
    if (prompt != NULL) {
        printf("%s", prompt);
    }
    
    if (fgets(buffer, (int)buffer_size, stdin) == NULL) {
        return -1;
    }
    
    // Remove newline character
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return 0;
}

/**
 * @brief Display and handle main menu
 * @return Next menu to navigate to, or MENU_EXIT to quit
 */
MenuType InventoryApp_ShowMainMenu(void) {
    int choice;
    
    printf("\n");
    printf("========================================\n");
    printf("  Inventory Management System\n");
    printf("  for Crafters - Main Menu\n");
    printf("========================================\n");
    printf("1. User Authentication\n");
    printf("2. Material Inventory\n");
    printf("3. Project Tracking\n");
    printf("4. Expense Logging\n");
    printf("5. Sales Tracker\n");
    printf("99. Exit\n");
    printf("========================================\n");
    
    if (InventoryApp_GetIntInput("Enter your choice: ", &choice) != 0) {
        printf("Invalid input. Please enter a number.\n");
        return MENU_MAIN;
    }
    
    switch (choice) {
        case 1: return MENU_AUTHENTICATION;
        case 2: return MENU_MATERIAL_INVENTORY;
        case 3: return MENU_PROJECT_TRACKING;
        case 4: return MENU_EXPENSE_LOGGING;
        case 5: return MENU_SALES_TRACKER;
        case 99: return MENU_EXIT;
        default:
            printf("Invalid choice. Please try again.\n");
            return MENU_MAIN;
    }
}

/**
 * @brief Display and handle user authentication menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowAuthenticationMenu(void) {
    int choice;
    char username[64];
    char password[64];
    
    printf("\n");
    printf("========================================\n");
    printf("  User Authentication\n");
    printf("========================================\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Guest Mode\n");
    printf("4. View Login History\n");
    printf("0. Back to Main Menu\n");
    printf("========================================\n");
    
    if (InventoryApp_GetIntInput("Enter your choice: ", &choice) != 0) {
        printf("Invalid input. Please enter a number.\n");
        return MENU_AUTHENTICATION;
    }
    
    switch (choice) {
        case 1:
            printf("\n--- Login ---\n");
            if (InventoryApp_GetStringInput("Username: ", username, sizeof(username)) != 0) {
                printf("Error reading username.\n");
                return MENU_AUTHENTICATION;
            }
            if (InventoryApp_GetStringInput("Password: ", password, sizeof(password)) != 0) {
                printf("Error reading password.\n");
                return MENU_AUTHENTICATION;
            }
            
            if (InventoryManager_LoginUser(username, password)) {
                printf("Login successful! Welcome, %s!\n", username);
            } else {
                printf("Login failed. Invalid username or password.\n");
            }
            return MENU_AUTHENTICATION;
            
        case 2:
            printf("\n--- Register ---\n");
            if (InventoryApp_GetStringInput("Username: ", username, sizeof(username)) != 0) {
                printf("Error reading username.\n");
                return MENU_AUTHENTICATION;
            }
            if (InventoryApp_GetStringInput("Password: ", password, sizeof(password)) != 0) {
                printf("Error reading password.\n");
                return MENU_AUTHENTICATION;
            }
            
            if (InventoryManager_RegisterUser(username, password) == 0) {
                printf("Registration successful! Welcome, %s!\n", username);
            } else {
                printf("Registration failed. Username may already exist.\n");
            }
            return MENU_AUTHENTICATION;
            
        case 3:
            printf("\n--- Guest Mode ---\n");
            printf("Entering guest mode. Limited functionality available.\n");
            return MENU_MAIN;
            
        case 4:
            printf("\n--- Login History ---\n");
            {
                int count = 0;
                if (InventoryApp_GetIntInput("Number of recent entries to display (0 for all): ", &count) == 0) {
                    InventoryManager_ViewLoginHistory((size_t)count);
                } else {
                    InventoryManager_ViewLoginHistory(0);
                }
            }
            return MENU_AUTHENTICATION;
            
        case 0:
            return MENU_MAIN;
            
        default:
            printf("Invalid choice. Please try again.\n");
            return MENU_AUTHENTICATION;
    }
}

/**
 * @brief Display and handle material inventory menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowMaterialInventoryMenu(void) {
    int choice;
    MaterialList* material_list;
    char name[128];
    char category[64];
    char supplier[64];
    uint32_t quantity;
    uint32_t unit_price_cents;
    uint32_t material_id;
    int price_dollars, price_cents_input;
    Material* material;
    
    material_list = InventoryManager_GetMaterialList();
    if (material_list == NULL) {
        printf("Error: Material inventory not initialized.\n");
        return MENU_MAIN;
    }
    
    printf("\n");
    printf("========================================\n");
    printf("  Material Inventory\n");
    printf("========================================\n");
    printf("1. View Inventory\n");
    printf("2. Add Material\n");
    printf("3. Edit Material\n");
    printf("4. Remove Material\n");
    printf("0. Back to Main Menu\n");
    printf("========================================\n");
    
    if (InventoryApp_GetIntInput("Enter your choice: ", &choice) != 0) {
        printf("Invalid input. Please enter a number.\n");
        return MENU_MATERIAL_INVENTORY;
    }
    
    switch (choice) {
        case 1:
            printf("\n--- View Inventory ---\n");
            {
                size_t size = MaterialList_GetSize(material_list);
                if (size == 0) {
                    printf("Inventory is empty.\n");
                } else {
                    printf("\nTotal Materials: %zu\n", size);
                    printf("------------------------------------------------\n");
                    printf("%-5s %-20s %-15s %-10s %-12s %-15s\n", 
                           "ID", "Name", "Category", "Quantity", "Price (USD)", "Supplier");
                    printf("------------------------------------------------\n");
                    
                    MaterialNode* node = MaterialList_GetFirst(material_list);
                    while (node != NULL) {
                        double price_dollars = (double)node->material.unit_price_cents / 100.0;
                        printf("%-5u %-20s %-15s %-10u $%-11.2f %-15s\n",
                               node->material.id,
                               node->material.name,
                               node->material.category,
                               node->material.quantity,
                               price_dollars,
                               node->material.supplier);
                        node = MaterialList_GetNext(node);
                    }
                    printf("------------------------------------------------\n");
                }
            }
            return MENU_MATERIAL_INVENTORY;
            
        case 2:
            printf("\n--- Add Material ---\n");
            if (InventoryApp_GetStringInput("Material Name: ", name, sizeof(name)) != 0) {
                printf("Error reading material name.\n");
                return MENU_MATERIAL_INVENTORY;
            }
            if (InventoryApp_GetStringInput("Category: ", category, sizeof(category)) != 0) {
                printf("Error reading category.\n");
                return MENU_MATERIAL_INVENTORY;
            }
            if (InventoryApp_GetStringInput("Supplier: ", supplier, sizeof(supplier)) != 0) {
                printf("Error reading supplier.\n");
                return MENU_MATERIAL_INVENTORY;
            }
            {
                int qty;
                if (InventoryApp_GetIntInput("Quantity: ", &qty) != 0 || qty < 0) {
                    printf("Error reading quantity. Please enter a valid number.\n");
                    return MENU_MATERIAL_INVENTORY;
                }
                quantity = (uint32_t)qty;
            }
            {
                printf("Unit Price (enter dollars and cents separately):\n");
                if (InventoryApp_GetIntInput("Dollars: ", &price_dollars) != 0 || price_dollars < 0) {
                    printf("Error reading price dollars. Please enter a valid number.\n");
                    return MENU_MATERIAL_INVENTORY;
                }
                if (InventoryApp_GetIntInput("Cents: ", &price_cents_input) != 0 || price_cents_input < 0 || price_cents_input > 99) {
                    printf("Error reading price cents. Please enter a valid number (0-99).\n");
                    return MENU_MATERIAL_INVENTORY;
                }
                unit_price_cents = (uint32_t)(price_dollars * 100 + price_cents_input);
            }
            
            material_id = MaterialList_AddMaterial(material_list, name, category, quantity, unit_price_cents, supplier);
            if (material_id != 0) {
                printf("Material added successfully! ID: %u\n", material_id);
            } else {
                printf("Error: Failed to add material.\n");
            }
            return MENU_MATERIAL_INVENTORY;
            
        case 3:
            printf("\n--- Edit Material ---\n");
            {
                int id_input;
                if (InventoryApp_GetIntInput("Material ID to edit: ", &id_input) != 0 || id_input <= 0) {
                    printf("Error reading material ID. Please enter a valid number.\n");
                    return MENU_MATERIAL_INVENTORY;
                }
                material_id = (uint32_t)id_input;
                
                material = MaterialList_FindMaterial(material_list, material_id);
                if (material == NULL) {
                    printf("Error: Material with ID %u not found.\n", material_id);
                    return MENU_MATERIAL_INVENTORY;
                }
                
                printf("Current Material Information:\n");
                printf("  Name: %s\n", material->name);
                printf("  Category: %s\n", material->category);
                printf("  Quantity: %u\n", material->quantity);
                printf("  Price: $%.2f\n", (double)material->unit_price_cents / 100.0);
                printf("  Supplier: %s\n", material->supplier);
                printf("\nEnter new values (press Enter to keep unchanged):\n");
                
                // Edit name
                if (InventoryApp_GetStringInput("New Name (or press Enter to skip): ", name, sizeof(name)) == 0 && strlen(name) > 0) {
                    // Name provided
                } else {
                    name[0] = '\0'; // Empty string means keep unchanged
                }
                
                // Edit category
                if (InventoryApp_GetStringInput("New Category (or press Enter to skip): ", category, sizeof(category)) == 0 && strlen(category) > 0) {
                    // Category provided
                } else {
                    category[0] = '\0';
                }
                
                // Edit quantity
                {
                    int qty;
                    char qty_str[32];
                    if (InventoryApp_GetStringInput("New Quantity (or press Enter to skip): ", qty_str, sizeof(qty_str)) == 0 && strlen(qty_str) > 0) {
                        if (sscanf(qty_str, "%d", &qty) == 1 && qty >= 0) {
                            quantity = (uint32_t)qty;
                        } else {
                            quantity = UINT32_MAX; // Keep unchanged
                        }
                    } else {
                        quantity = UINT32_MAX; // Keep unchanged
                    }
                }
                
                // Edit price
                {
                    char price_str[32];
                    if (InventoryApp_GetStringInput("New Price in dollars (or press Enter to skip): ", price_str, sizeof(price_str)) == 0 && strlen(price_str) > 0) {
                        double price_dollars_double;
                        if (sscanf(price_str, "%lf", &price_dollars_double) == 1 && price_dollars_double >= 0) {
                            unit_price_cents = (uint32_t)(price_dollars_double * 100.0);
                        } else {
                            unit_price_cents = UINT32_MAX; // Keep unchanged
                        }
                    } else {
                        unit_price_cents = UINT32_MAX; // Keep unchanged
                    }
                }
                
                // Edit supplier
                if (InventoryApp_GetStringInput("New Supplier (or press Enter to skip): ", supplier, sizeof(supplier)) == 0 && strlen(supplier) > 0) {
                    // Supplier provided
                } else {
                    supplier[0] = '\0';
                }
                
                // Update material
                const char* name_ptr = (strlen(name) > 0) ? name : NULL;
                const char* category_ptr = (strlen(category) > 0) ? category : NULL;
                const char* supplier_ptr = (strlen(supplier) > 0) ? supplier : NULL;
                
                if (MaterialList_UpdateMaterial(material_list, material_id, name_ptr, category_ptr, quantity, unit_price_cents, supplier_ptr) == 0) {
                    printf("Material updated successfully!\n");
                } else {
                    printf("Error: Failed to update material.\n");
                }
            }
            return MENU_MATERIAL_INVENTORY;
            
        case 4:
            printf("\n--- Remove Material ---\n");
            {
                int id_input;
                if (InventoryApp_GetIntInput("Material ID to remove: ", &id_input) != 0 || id_input <= 0) {
                    printf("Error reading material ID. Please enter a valid number.\n");
                    return MENU_MATERIAL_INVENTORY;
                }
                material_id = (uint32_t)id_input;
                
                material = MaterialList_FindMaterial(material_list, material_id);
                if (material == NULL) {
                    printf("Error: Material with ID %u not found.\n", material_id);
                    return MENU_MATERIAL_INVENTORY;
                }
                
                printf("Material to remove:\n");
                printf("  ID: %u\n", material->id);
                printf("  Name: %s\n", material->name);
                printf("  Category: %s\n", material->category);
                
                {
                    int confirm;
                    if (InventoryApp_GetIntInput("Are you sure? (1=Yes, 0=No): ", &confirm) == 0 && confirm == 1) {
                        if (MaterialList_RemoveMaterial(material_list, material_id) == 0) {
                            printf("Material removed successfully!\n");
                        } else {
                            printf("Error: Failed to remove material.\n");
                        }
                    } else {
                        printf("Removal cancelled.\n");
                    }
                }
            }
            return MENU_MATERIAL_INVENTORY;
            
        case 0:
            return MENU_MAIN;
            
        default:
            printf("Invalid choice. Please try again.\n");
            return MENU_MATERIAL_INVENTORY;
    }
}

/**
 * @brief Display and handle project tracking menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowProjectTrackingMenu(void) {
    int choice;
    
    printf("\n");
    printf("========================================\n");
    printf("  Project Tracking\n");
    printf("========================================\n");
    printf("1. View Projects\n");
    printf("2. Add Project\n");
    printf("3. Edit Project\n");
    printf("4. Remove Project\n");
    printf("0. Back to Main Menu\n");
    printf("========================================\n");
    
    if (InventoryApp_GetIntInput("Enter your choice: ", &choice) != 0) {
        printf("Invalid input. Please enter a number.\n");
        return MENU_PROJECT_TRACKING;
    }
    
    switch (choice) {
        case 1:
            printf("\n--- View Projects ---\n");
            printf("Project tracking functionality will be implemented soon.\n");
            return MENU_PROJECT_TRACKING;
            
        case 2:
            printf("\n--- Add Project ---\n");
            printf("Project tracking functionality will be implemented soon.\n");
            return MENU_PROJECT_TRACKING;
            
        case 3:
            printf("\n--- Edit Project ---\n");
            printf("Project tracking functionality will be implemented soon.\n");
            return MENU_PROJECT_TRACKING;
            
        case 4:
            printf("\n--- Remove Project ---\n");
            printf("Project tracking functionality will be implemented soon.\n");
            return MENU_PROJECT_TRACKING;
            
        case 0:
            return MENU_MAIN;
            
        default:
            printf("Invalid choice. Please try again.\n");
            return MENU_PROJECT_TRACKING;
    }
}

/**
 * @brief Display and handle expense logging menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowExpenseLoggingMenu(void) {
    int choice;
    
    printf("\n");
    printf("========================================\n");
    printf("  Expense Logging\n");
    printf("========================================\n");
    printf("1. Log Expense\n");
    printf("2. View Expenses\n");
    printf("0. Back to Main Menu\n");
    printf("========================================\n");
    
    if (InventoryApp_GetIntInput("Enter your choice: ", &choice) != 0) {
        printf("Invalid input. Please enter a number.\n");
        return MENU_EXPENSE_LOGGING;
    }
    
    switch (choice) {
        case 1:
            printf("\n--- Log Expense ---\n");
            printf("Expense logging functionality will be implemented soon.\n");
            return MENU_EXPENSE_LOGGING;
            
        case 2:
            printf("\n--- View Expenses ---\n");
            printf("Expense logging functionality will be implemented soon.\n");
            return MENU_EXPENSE_LOGGING;
            
        case 0:
            return MENU_MAIN;
            
        default:
            printf("Invalid choice. Please try again.\n");
            return MENU_EXPENSE_LOGGING;
    }
}

/**
 * @brief Display and handle sales tracker menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowSalesTrackerMenu(void) {
    int choice;
    
    printf("\n");
    printf("========================================\n");
    printf("  Sales Tracker\n");
    printf("========================================\n");
    printf("1. Record Sale\n");
    printf("2. View Sales\n");
    printf("3. Profit Calculation\n");
    printf("0. Back to Main Menu\n");
    printf("========================================\n");
    
    if (InventoryApp_GetIntInput("Enter your choice: ", &choice) != 0) {
        printf("Invalid input. Please enter a number.\n");
        return MENU_SALES_TRACKER;
    }
    
    switch (choice) {
        case 1:
            printf("\n--- Record Sale ---\n");
            printf("Sales tracking functionality will be implemented soon.\n");
            return MENU_SALES_TRACKER;
            
        case 2:
            printf("\n--- View Sales ---\n");
            printf("Sales tracking functionality will be implemented soon.\n");
            return MENU_SALES_TRACKER;
            
        case 3:
            printf("\n--- Profit Calculation ---\n");
            printf("Profit calculation functionality will be implemented soon.\n");
            return MENU_SALES_TRACKER;
            
        case 0:
            return MENU_MAIN;
            
        default:
            printf("Invalid choice. Please try again.\n");
            return MENU_SALES_TRACKER;
    }
}

/**
 * @brief Run the main application loop
 * 
 * This function initializes the system and starts the interactive menu system.
 * It handles user input and navigation between different modules.
 * 
 * @return EXIT_SUCCESS (0) on successful execution, EXIT_FAILURE (1) on error
 */
int InventoryApp_Run(void) {
    MenuType current_menu = MENU_MAIN;
    MenuType next_menu;

    // Initialize the inventory management system (load from users.bin if exists)
    if (InventoryManager_Init("users.bin") != 0) {
        fprintf(stderr, "Error: Failed to initialize inventory management system\n");
        return EXIT_FAILURE;
    }

    printf("\n");
    printf("========================================\n");
    printf("  Inventory Management System\n");
    printf("  for Crafters\n");
    printf("========================================\n");
    printf("System initialized successfully.\n");
    
    // Main menu loop
    while (current_menu != MENU_EXIT) {
        switch (current_menu) {
            case MENU_MAIN:
                next_menu = InventoryApp_ShowMainMenu();
                break;
            case MENU_AUTHENTICATION:
                next_menu = InventoryApp_ShowAuthenticationMenu();
                break;
            case MENU_MATERIAL_INVENTORY:
                next_menu = InventoryApp_ShowMaterialInventoryMenu();
                break;
            case MENU_PROJECT_TRACKING:
                next_menu = InventoryApp_ShowProjectTrackingMenu();
                break;
            case MENU_EXPENSE_LOGGING:
                next_menu = InventoryApp_ShowExpenseLoggingMenu();
                break;
            case MENU_SALES_TRACKER:
                next_menu = InventoryApp_ShowSalesTrackerMenu();
                break;
            default:
                next_menu = MENU_MAIN;
                break;
        }
        current_menu = next_menu;
    }

    // Cleanup the inventory management system (save to users.bin)
    if (InventoryManager_Cleanup("users.bin") != 0) {
        fprintf(stderr, "Warning: Failed to cleanup inventory management system\n");
    }

    printf("\nThank you for using Inventory Management System!\n");
    printf("Goodbye!\n");
    
    return EXIT_SUCCESS;
}

/**
 * @brief Main entry point for the Inventory Management Application
 * 
 * Initializes the inventory management system, performs basic operations,
 * and cleans up resources before exiting.
 * 
 * @return EXIT_SUCCESS (0) on successful execution, EXIT_FAILURE (1) on error
 */
int main(void) {
    return InventoryApp_Run();
}
