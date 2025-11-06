/**
 * @file NavigationHistory.h
 * 
 * @brief Provides functions for navigation history management using XOR Linked List data structure
 * 
 * This module implements a memory-efficient doubly linked list (XOR Linked List) for tracking
 * menu navigation history, allowing forward and backward navigation through the application.
 */

#ifndef NAVIGATION_HISTORY_H
#define NAVIGATION_HISTORY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup NavigationHistory Navigation History Implementation
 * @brief XOR Linked List data structure for navigation history
 * @{
 */

/**
 * @brief Navigation entry type enumeration
 */
typedef enum {
    NAV_ENTRY_MAIN_MENU = 0,
    NAV_ENTRY_AUTHENTICATION = 1,
    NAV_ENTRY_MATERIAL_INVENTORY = 2,
    NAV_ENTRY_PROJECT_TRACKING = 3,
    NAV_ENTRY_EXPENSE_LOGGING = 4,
    NAV_ENTRY_SALES_TRACKER = 5
} NavigationEntryType;

/**
 * @brief Navigation entry structure
 * 
 * Represents a single navigation entry in the history.
 * Stores the menu type and timestamp.
 */
typedef struct {
    /** @brief Navigation entry type (menu type) */
    NavigationEntryType type;
    /** @brief Timestamp when this entry was visited */
    uint32_t timestamp;
    /** @brief Optional description string (maximum 127 characters + null terminator) */
    char description[128];
} NavigationEntry;

/**
 * @brief XOR Linked List Node structure
 * 
 * Represents a single entry in the XOR linked list.
 * Each node contains a NavigationEntry and an XOR pointer.
 * 
 * @note XOR pointer stores XOR of previous and next node addresses,
 * allowing bidirectional traversal with only one pointer per node.
 */
typedef struct XORNavNode {
    /** @brief Navigation entry data stored in this node */
    NavigationEntry entry;
    /** @brief XOR pointer: XOR of previous and next node addresses
     * 
     * To get next node: next = XOR(prev, xor_ptr)
     * To get prev node: prev = XOR(next, xor_ptr)
     * 
     * @note For first node, prev is NULL, so xor_ptr = next
     * @note For last node, next is NULL, so xor_ptr = prev
     */
    struct XORNavNode* xor_ptr;
} XORNavNode;

/**
 * @brief XOR Linked List structure for navigation history
 * 
 * Memory-efficient doubly linked list that supports bidirectional traversal
 * using only one pointer per node (50% less memory than standard doubly linked list).
 * 
 * @note Time complexity: O(1) for insertion/deletion at head/tail, O(n) for traversal
 * @note Space complexity: O(n) but with 50% less memory than standard doubly linked list
 */
typedef struct {
    /** @brief Pointer to the first node in the list (NULL if empty) */
    XORNavNode* head;
    /** @brief Pointer to the last node in the list (NULL if empty) */
    XORNavNode* tail;
    /** @brief Current number of entries in the history */
    size_t size;
    /** @brief Maximum capacity of the history (0 for unlimited) */
    size_t capacity;
    /** @brief Current position in forward/backward traversal */
    XORNavNode* current;
} NavigationHistory;

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
XORNavNode* NavigationHistory_XOR(XORNavNode* a, XORNavNode* b);

/**
 * @}
 */

// Navigation History Functions

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
NavigationHistory* NavigationHistory_Create(size_t capacity);

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
void NavigationHistory_Destroy(NavigationHistory* history);

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
int NavigationHistory_AddEntry(NavigationHistory* history, NavigationEntryType type, const char* description);

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
int NavigationHistory_GoBack(NavigationHistory* history, NavigationEntry* entry);

/**
 * @brief Go forward in navigation history
 * 
 * Moves the current position forward by one entry.
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * @param entry Pointer to store the next entry (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters, empty history, or already at end)
 * 
 * @note Time complexity: O(1)
 */
int NavigationHistory_GoForward(NavigationHistory* history, NavigationEntry* entry);

/**
 * @brief Get current navigation entry without moving
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * @param entry Pointer to store the current entry (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters or empty history)
 * 
 * @note Time complexity: O(1)
 */
int NavigationHistory_GetCurrent(NavigationHistory* history, NavigationEntry* entry);

/**
 * @brief Check if backward navigation is possible
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * @return 1 if can go back, 0 if cannot, -1 if NULL
 * 
 * @note Time complexity: O(1)
 */
int NavigationHistory_CanGoBack(NavigationHistory* history);

/**
 * @brief Check if forward navigation is possible
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * @return 1 if can go forward, 0 if cannot, -1 if NULL
 * 
 * @note Time complexity: O(1)
 */
int NavigationHistory_CanGoForward(NavigationHistory* history);

/**
 * @brief Clear all navigation history
 * 
 * Removes all entries from the navigation history.
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * 
 * @note Time complexity: O(n) where n is the number of entries
 */
void NavigationHistory_Clear(NavigationHistory* history);

/**
 * @brief Check if the history is empty
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * @return 1 if empty, 0 if not empty, -1 if NULL
 * 
 * @note Time complexity: O(1)
 */
int NavigationHistory_IsEmpty(NavigationHistory* history);

/**
 * @brief Get the number of entries in the history
 * 
 * @param history Pointer to the navigation history (must not be NULL)
 * @return Number of entries, or 0 if NULL or empty
 * 
 * @note Time complexity: O(1)
 */
size_t NavigationHistory_GetSize(NavigationHistory* history);

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
int NavigationHistory_TraverseForward(NavigationHistory* history,
                                      void (*callback)(const NavigationEntry* entry, size_t index, void* user_data),
                                      void* user_data);

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
int NavigationHistory_TraverseBackward(NavigationHistory* history,
                                       void (*callback)(const NavigationEntry* entry, size_t index, void* user_data),
                                       void* user_data);

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
void NavigationHistory_DisplayForward(NavigationHistory* history, size_t max_count);

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
void NavigationHistory_DisplayBackward(NavigationHistory* history, size_t max_count);

/**
 * @brief Get entry type string representation
 * 
 * @param type Navigation entry type enumeration value
 * @return String representation of the entry type
 */
const char* NavigationHistory_GetTypeString(NavigationEntryType type);

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
int NavigationHistory_SaveToFile(NavigationHistory* history, const char* filename);

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
NavigationHistory* NavigationHistory_LoadFromFile(NavigationHistory* history, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // NAVIGATION_HISTORY_H

