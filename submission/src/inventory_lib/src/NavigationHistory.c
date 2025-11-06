/**
 * @file NavigationHistory.c
 * @brief Implementation of navigation history management functions using XOR Linked List data structure.
 */

#include "../header/NavigationHistory.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

// Disable MSVC warnings for deprecated functions
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Global timestamp counter for navigation entries
static uint32_t g_nav_timestamp_counter = 1;

// Test hook for memory allocation (only used in test builds)
#ifdef ENABLE_INVENTORYMANAGER_TEST
// Forward declaration - test_malloc_hook is defined in InventoryManager.c
extern void* (*test_malloc_hook)(size_t);

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

// Helper function to calculate XOR of two pointers
XORNavNode* NavigationHistory_XOR(XORNavNode* a, XORNavNode* b) {
    /**
     * @brief Helper function to calculate XOR of two pointers
     * 
     * XOR operation on two pointers (addresses) to support bidirectional traversal.
     * 
     * @param a First pointer (can be NULL)
     * @param b Second pointer (can be NULL)
     * @return XOR of a and b (can be NULL if both are NULL)
     * 
     * @note This is a helper function for internal XOR operations
     */
    return (XORNavNode*)((uintptr_t)a ^ (uintptr_t)b);
}

NavigationHistory* NavigationHistory_Create(size_t capacity) {
    /**
     * @brief Create a new navigation history
     * 
     * Allocates memory for a new navigation history and initializes it.
     * The history is ready to use after creation.
     * 
     * @param capacity Maximum number of entries (0 for unlimited)
     * @return Pointer to the newly created history, or NULL on memory allocation failure
     * 
     * @note Time complexity: O(1)
     * @note The caller is responsible for destroying the history using NavigationHistory_Destroy()
     * to avoid memory leaks.
     */
    NavigationHistory* history = (NavigationHistory*)safe_malloc(sizeof(NavigationHistory));
    if (history == NULL) {
        return NULL;
    }
    
    history->head = NULL;
    history->tail = NULL;
    history->current = NULL;
    history->size = 0;
    history->capacity = capacity; // 0 means unlimited
    
    return history;
}

void NavigationHistory_Destroy(NavigationHistory* history) {
    /**
     * @brief Destroy a navigation history and free all memory
     * 
     * Frees all nodes in the history and then frees the history structure itself.
     * This function is safe to call with NULL pointer (no-op).
     * 
     * @param history Pointer to the navigation history to destroy (can be NULL)
     * 
     * @note Time complexity: O(n) where n is the number of entries
     * @note After calling this function, the history pointer becomes invalid and should not be used.
     */
    if (history == NULL) {
        return;
    }
    
    // Traverse forward and free all nodes
    XORNavNode* current = history->head;
    XORNavNode* prev = NULL;
    
    while (current != NULL) {
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        
        // Free current node
        free(current);
        
        // Move to next node
        prev = current;
        current = next;
    }
    
    free(history);
}

int NavigationHistory_AddEntry(NavigationHistory* history, NavigationEntryType type, const char* description) {
    /**
     * @brief Add a new navigation entry at the end (go forward)
     * 
     * Adds a new navigation entry to the end of the history.
     * This is used when navigating forward to a new menu.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param type Navigation entry type (menu type)
     * @param description Optional description string (can be NULL)
     * @return 0 on success, -1 on error (NULL parameter or memory allocation failure)
     * 
     * @note Time complexity: O(1)
     * @note If capacity is reached and unlimited, oldest entries are removed (FIFO)
     */
    if (history == NULL) {
        return -1;
    }
    
    // Check capacity and remove oldest if needed
    if (history->capacity > 0 && history->size >= history->capacity) {
        // Remove oldest entry (head)
        if (history->head != NULL) {
            XORNavNode* head = history->head;
            XORNavNode* next = NavigationHistory_XOR(NULL, head->xor_ptr);
            
            if (next != NULL) {
                next->xor_ptr = NavigationHistory_XOR(head, NavigationHistory_XOR(next, next->xor_ptr));
            } else {
                // Only one node
                history->tail = NULL;
            }
            
            history->head = next;
            free(head);
            history->size--;
        }
    }
    
    // Create new node
    XORNavNode* new_node = (XORNavNode*)safe_malloc(sizeof(XORNavNode));
    if (new_node == NULL) {
        return -1;
    }
    
    // Initialize entry
    new_node->entry.type = type;
    new_node->entry.timestamp = g_nav_timestamp_counter++;
    
    if (description != NULL) {
        strncpy(new_node->entry.description, description, sizeof(new_node->entry.description) - 1);
        new_node->entry.description[sizeof(new_node->entry.description) - 1] = '\0';
    } else {
        new_node->entry.description[0] = '\0';
    }
    
    // Set XOR pointer: XOR of previous (tail) and next (NULL)
    new_node->xor_ptr = history->tail; // For tail node, next is NULL, so XOR(prev, NULL) = prev
    
    // Update tail's XOR pointer
    if (history->tail != NULL) {
        XORNavNode* tail_prev = NavigationHistory_XOR(history->tail->xor_ptr, NULL);
        history->tail->xor_ptr = NavigationHistory_XOR(tail_prev, new_node);
    }
    
    // Update history
    if (history->head == NULL) {
        // First node
        history->head = new_node;
    }
    history->tail = new_node;
    history->current = new_node; // Always set current to the newly added node (tail)
    history->size++;
    
    return 0;
}

int NavigationHistory_GoBack(NavigationHistory* history, NavigationEntry* entry) {
    /**
     * @brief Go backward in navigation history
     * 
     * Moves the current position backward by one entry.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param entry Pointer to store the previous entry (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, empty history, or already at beginning)
     * 
     * @note Time complexity: O(1)
     */
    if (history == NULL || entry == NULL || history->current == NULL) {
        return -1;
    }
    
    // Get previous node: XOR(current->xor_ptr, next)
    // First, we need to find the next node
    XORNavNode* prev = NULL;
    XORNavNode* current = history->head;
    
    // Find current node and its previous node
    while (current != NULL && current != history->current) {
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        prev = current;
        current = next;
    }
    
    if (current == NULL) {
        return -1; // Current not found
    }
    
    // Get previous node using prev (which we found during traversal)
    // For current node: xor_ptr = XOR(prev, next)
    // So: prev = XOR(xor_ptr, next) = XOR(xor_ptr, NULL) = xor_ptr
    // But we already have prev from traversal, so use it directly
    XORNavNode* previous_node = prev;
    
    if (previous_node == NULL) {
        return -1; // Already at beginning
    }
    
    // Update current
    history->current = previous_node;
    *entry = previous_node->entry;
    
    return 0;
}

int NavigationHistory_GoForward(NavigationHistory* history, NavigationEntry* entry) {
    /**
     * @brief Go forward in navigation history
     * 
     * Moves the current position forward by one entry.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param entry Pointer to store the next entry (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, empty history, or already at end)
     * 
     * @note Time complexity: O(1) average case, O(n) worst case (if current is not maintained)
     */
    if (history == NULL || entry == NULL || history->current == NULL) {
        return -1;
    }
    
    // Get next node: XOR(prev, current->xor_ptr)
    // First, we need to find the previous node
    XORNavNode* prev = NULL;
    XORNavNode* current = history->head;
    
    // Find current node and its previous node
    while (current != NULL && current != history->current) {
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        prev = current;
        current = next;
    }
    
    if (current == NULL) {
        return -1; // Current not found
    }
    
    // Get next node: for current node, xor_ptr = XOR(prev, next)
    // So: next = XOR(xor_ptr, prev)
    XORNavNode* next_node = NavigationHistory_XOR(prev, current->xor_ptr);
    
    if (next_node == NULL) {
        return -1; // Already at end
    }
    
    // Update current
    history->current = next_node;
    *entry = next_node->entry;
    
    return 0;
}

int NavigationHistory_GetCurrent(NavigationHistory* history, NavigationEntry* entry) {
    /**
     * @brief Get current navigation entry without moving
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param entry Pointer to store the current entry (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters or empty history)
     * 
     * @note Time complexity: O(1)
     */
    if (history == NULL || entry == NULL || history->current == NULL) {
        return -1;
    }
    
    *entry = history->current->entry;
    return 0;
}

int NavigationHistory_CanGoBack(NavigationHistory* history) {
    /**
     * @brief Check if backward navigation is possible
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @return 1 if can go back, 0 if cannot, -1 if NULL
     * 
     * @note Time complexity: O(n) where n is the number of entries
     */
    if (history == NULL || history->current == NULL) {
        return -1;
    }
    
    // Check if current is not the head
    if (history->current == history->head) {
        return 0; // Already at beginning
    }
    
    return 1; // Can go back
}

int NavigationHistory_CanGoForward(NavigationHistory* history) {
    /**
     * @brief Check if forward navigation is possible
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @return 1 if can go forward, 0 if cannot, -1 if NULL
     * 
     * @note Time complexity: O(n) where n is the number of entries
     */
    if (history == NULL || history->current == NULL) {
        return -1;
    }
    
    // Simple check: if current is tail, can't go forward
    if (history->current == history->tail) {
        return 0; // Already at end
    }
    
    return 1; // Can go forward
}

void NavigationHistory_Clear(NavigationHistory* history) {
    /**
     * @brief Clear all navigation history
     * 
     * Removes all entries from the navigation history.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * 
     * @note Time complexity: O(n) where n is the number of entries
     */
    if (history == NULL) {
        return;
    }
    
    // Traverse forward and free all nodes
    XORNavNode* current = history->head;
    XORNavNode* prev = NULL;
    
    while (current != NULL) {
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        free(current);
        prev = current;
        current = next;
    }
    
    history->head = NULL;
    history->tail = NULL;
    history->current = NULL;
    history->size = 0;
}

int NavigationHistory_IsEmpty(NavigationHistory* history) {
    /**
     * @brief Check if the history is empty
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @return 1 if empty, 0 if not empty, -1 if NULL
     * 
     * @note Time complexity: O(1)
     */
    if (history == NULL) {
        return -1;
    }
    
    return (history->head == NULL) ? 1 : 0;
}

size_t NavigationHistory_GetSize(NavigationHistory* history) {
    /**
     * @brief Get the number of entries in the history
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @return Number of entries, or 0 if NULL or empty
     * 
     * @note Time complexity: O(1)
     */
    if (history == NULL) {
        return 0;
    }
    
    return history->size;
}

int NavigationHistory_TraverseForward(NavigationHistory* history,
                                      void (*callback)(const NavigationEntry* entry, size_t index, void* user_data),
                                      void* user_data) {
    /**
     * @brief Traverse the history from head to tail (forward)
     * 
     * Visits each entry in the history from head to tail and calls the callback function.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param callback Function to call for each entry (receives entry and index)
     * @param user_data Optional user data to pass to callback (can be NULL)
     * @return 0 on success, -1 on error (NULL parameters)
     * 
     * @note Time complexity: O(n) where n is the number of entries
     * @note The callback should not modify the history structure during traversal
     */
    if (history == NULL || callback == NULL) {
        return -1;
    }
    
    XORNavNode* current = history->head;
    XORNavNode* prev = NULL;
    size_t index = 0;
    
    while (current != NULL) {
        callback(&current->entry, index, user_data);
        
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        prev = current;
        current = next;
        index++;
    }
    
    return 0;
}

int NavigationHistory_TraverseBackward(NavigationHistory* history,
                                        void (*callback)(const NavigationEntry* entry, size_t index, void* user_data),
                                        void* user_data) {
    /**
     * @brief Traverse the history from tail to head (backward)
     * 
     * Visits each entry in the history from tail to head and calls the callback function.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param callback Function to call for each entry (receives entry and index)
     * @param user_data Optional user data to pass to callback (can be NULL)
     * @return 0 on success, -1 on error (NULL parameters)
     * 
     * @note Time complexity: O(n) where n is the number of entries
     * @note The callback should not modify the history structure during traversal
     */
    if (history == NULL || callback == NULL) {
        return -1;
    }
    
    XORNavNode* current = history->tail;
    XORNavNode* next = NULL;
    size_t index = history->size - 1;
    
    while (current != NULL) {
        callback(&current->entry, index, user_data);
        
        XORNavNode* prev = NavigationHistory_XOR(next, current->xor_ptr);
        next = current;
        current = prev;
        index--;
    }
    
    return 0;
}

void NavigationHistory_DisplayForward(NavigationHistory* history, size_t max_count) {
    /**
     * @brief Display all navigation history entries (forward)
     * 
     * Prints all entries in the history from head to tail.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param max_count Maximum number of entries to display (0 for all)
     * 
     * @note Time complexity: O(n) where n is the number of entries
     * @note For debugging purposes only
     */
    if (history == NULL || history->head == NULL) {
        printf("Navigation history is empty.\n");
        return;
    }
    
    printf("\n=== Navigation History (Forward) ===\n");
    printf("%-5s %-20s %-30s %-10s\n", "Index", "Type", "Description", "Timestamp");
    printf("--------------------------------------------------------------------------------------------------------\n");
    
    XORNavNode* current = history->head;
    XORNavNode* prev = NULL;
    size_t index = 0;
    size_t count = 0;
    
    while (current != NULL && (max_count == 0 || count < max_count)) {
        printf("%-5zu %-20s %-30s %-10u\n",
               index,
               NavigationHistory_GetTypeString(current->entry.type),
               current->entry.description,
               current->entry.timestamp);
        
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        prev = current;
        current = next;
        index++;
        count++;
    }
    
    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("Total Entries: %zu (Displayed: %zu)\n", history->size, count);
    printf("================================\n\n");
}

void NavigationHistory_DisplayBackward(NavigationHistory* history, size_t max_count) {
    /**
     * @brief Display all navigation history entries (backward)
     * 
     * Prints all entries in the history from tail to head.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param max_count Maximum number of entries to display (0 for all)
     * 
     * @note Time complexity: O(n) where n is the number of entries
     * @note For debugging purposes only
     */
    if (history == NULL || history->tail == NULL) {
        printf("Navigation history is empty.\n");
        return;
    }
    
    printf("\n=== Navigation History (Backward) ===\n");
    printf("%-5s %-20s %-30s %-10s\n", "Index", "Type", "Description", "Timestamp");
    printf("--------------------------------------------------------------------------------------------------------\n");
    
    XORNavNode* current = history->tail;
    XORNavNode* next = NULL;
    size_t index = history->size - 1;
    size_t count = 0;
    
    while (current != NULL && (max_count == 0 || count < max_count)) {
        printf("%-5zu %-20s %-30s %-10u\n",
               index,
               NavigationHistory_GetTypeString(current->entry.type),
               current->entry.description,
               current->entry.timestamp);
        
        XORNavNode* prev = NavigationHistory_XOR(next, current->xor_ptr);
        next = current;
        current = prev;
        index--;
        count++;
    }
    
    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("Total Entries: %zu (Displayed: %zu)\n", history->size, count);
    printf("================================\n\n");
}

const char* NavigationHistory_GetTypeString(NavigationEntryType type) {
    /**
     * @brief Get entry type string representation
     * 
     * @param type Navigation entry type enumeration value
     * @return String representation of the entry type
     */
    switch (type) {
        case NAV_ENTRY_MAIN_MENU:
            return "Main Menu";
        case NAV_ENTRY_AUTHENTICATION:
            return "Authentication";
        case NAV_ENTRY_MATERIAL_INVENTORY:
            return "Material Inventory";
        case NAV_ENTRY_PROJECT_TRACKING:
            return "Project Tracking";
        case NAV_ENTRY_EXPENSE_LOGGING:
            return "Expense Logging";
        case NAV_ENTRY_SALES_TRACKER:
            return "Sales Tracker";
        default:
            return "Unknown";
    }
}

int NavigationHistory_SaveToFile(NavigationHistory* history, const char* filename) {
    /**
     * @brief Save the navigation history to a binary file
     * 
     * Saves all navigation entries to a binary file for persistence.
     * 
     * @param history Pointer to the navigation history (must not be NULL)
     * @param filename Filename to save to (must not be NULL)
     * @return 0 on success, -1 on error
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     * @note Time complexity: O(n) where n is the number of entries
     */
    if (history == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write history metadata
    size_t count = history->size;
    if (fwrite(&count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all entries
    XORNavNode* current = history->head;
    XORNavNode* prev = NULL;
    
    while (current != NULL) {
        if (fwrite(&current->entry, sizeof(NavigationEntry), 1, file) != 1) {
            fclose(file);
            return -1;
        }
        
        XORNavNode* next = NavigationHistory_XOR(prev, current->xor_ptr);
        prev = current;
        current = next;
    }
    
    fclose(file);
    return 0;
}

NavigationHistory* NavigationHistory_LoadFromFile(NavigationHistory* history, const char* filename) {
    /**
     * @brief Load the navigation history from a binary file
     * 
     * Loads navigation entries from a binary file.
     * 
     * @param history Pointer to existing history (will be destroyed if not NULL), or NULL to create new.
     * @param filename Filename to load from (must not be NULL)
     * @return Pointer to the loaded history, or NULL on error
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     * @note Time complexity: O(n) where n is the number of entries
     */
    if (filename == NULL) {
        return NULL;
    }
    
    // Destroy existing history if provided
    if (history != NULL) {
        NavigationHistory_Destroy(history);
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        // File doesn't exist, create new history
        return NavigationHistory_Create(0);
    }
    
    // Read history metadata
    size_t count;
    if (fread(&count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return NULL;
    }
    
    // Create new history
    history = NavigationHistory_Create(0);
    if (history == NULL) {
        fclose(file);
        return NULL;
    }
    
    // Read all entries
    for (size_t i = 0; i < count; i++) {
        NavigationEntry entry;
        if (fread(&entry, sizeof(NavigationEntry), 1, file) != 1) {
            NavigationHistory_Destroy(history);
            fclose(file);
            return NULL;
        }
        
        if (NavigationHistory_AddEntry(history, entry.type, entry.description) != 0) {
            NavigationHistory_Destroy(history);
            fclose(file);
            return NULL;
        }
    }
    
    fclose(file);
    return history;
}

