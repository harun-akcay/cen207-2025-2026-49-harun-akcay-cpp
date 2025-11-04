/**
 * @file InventoryApp.h
 * @brief Inventory Management Application header file
 *
 */

#ifndef INVENTORY_APP_H
#define INVENTORY_APP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup InventoryApp Inventory Management Application
 * @brief Main application entry point and menu system
 * @{
 */

/**
 * @brief Main menu options
 */
typedef enum {
    MENU_MAIN = 0,
    MENU_AUTHENTICATION = 1,
    MENU_MATERIAL_INVENTORY = 2,
    MENU_PROJECT_TRACKING = 3,
    MENU_EXPENSE_LOGGING = 4,
    MENU_SALES_TRACKER = 5,
    MENU_EXIT = 99
} MenuType;

/**
 * @brief Run the main application loop
 * 
 * This function initializes the system and starts the interactive menu system.
 * It handles user input and navigation between different modules.
 * 
 * @return EXIT_SUCCESS (0) on successful execution, EXIT_FAILURE (1) on error
 */
int InventoryApp_Run(void);

/**
 * @brief Display and handle main menu
 * @return Next menu to navigate to, or MENU_EXIT to quit
 */
MenuType InventoryApp_ShowMainMenu(void);

/**
 * @brief Display and handle user authentication menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowAuthenticationMenu(void);

/**
 * @brief Display and handle material inventory menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowMaterialInventoryMenu(void);

/**
 * @brief Display and handle project tracking menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowProjectTrackingMenu(void);

/**
 * @brief Display and handle expense logging menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowExpenseLoggingMenu(void);

/**
 * @brief Display and handle sales tracker menu
 * @return Next menu to navigate to
 */
MenuType InventoryApp_ShowSalesTrackerMenu(void);

/**
 * @brief Clear input buffer
 */
void InventoryApp_ClearInput(void);

/**
 * @brief Get integer input from user
 * @param prompt The prompt to display
 * @param value Pointer to store the input value
 * @return 0 on success, -1 on error
 */
int InventoryApp_GetIntInput(const char* prompt, int* value);

/**
 * @brief Get string input from user
 * @param prompt The prompt to display
 * @param buffer Buffer to store the input
 * @param buffer_size Size of the buffer
 * @return 0 on success, -1 on error
 */
int InventoryApp_GetStringInput(const char* prompt, char* buffer, size_t buffer_size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // INVENTORY_APP_H
