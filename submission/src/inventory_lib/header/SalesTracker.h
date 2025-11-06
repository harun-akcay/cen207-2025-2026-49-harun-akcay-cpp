/**
 * @file SalesTracker.h
 * 
 * @brief Provides functions for sales tracking management using Heap and Heap Sort data structures
 * 
 * This module implements a priority queue (Max Heap) for sales tracking,
 * allowing efficient sorting of sales by profit using Heap Sort algorithm.
 */

#ifndef SALES_TRACKER_H
#define SALES_TRACKER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SalesTracker Sales Tracker Implementation
 * @brief Heap and Heap Sort data structure for sales tracking
 * @{
 */

/**
 * @brief Sale structure for tracking
 * 
 * Represents a sale transaction in the inventory management system.
 * Stores sale details including item, quantity, price, and profit.
 */
typedef struct {
    /** @brief Sale ID (unique identifier) */
    uint32_t id;
    /** @brief Material ID that was sold (0 for no material) */
    uint32_t material_id;
    /** @brief Project ID associated with this sale (0 for no project) */
    uint32_t project_id;
    /** @brief Quantity sold */
    uint32_t quantity;
    /** @brief Unit price in cents */
    uint32_t unit_price_cents;
    /** @brief Total sale amount in cents (quantity * unit_price_cents) */
    uint64_t total_amount_cents;
    /** @brief Cost per unit in cents (for profit calculation) */
    uint32_t unit_cost_cents;
    /** @brief Total profit in cents (total_amount - total_cost) */
    uint64_t profit_cents;
    /** @brief Sale date timestamp */
    uint32_t sale_timestamp;
    /** @brief Customer name (maximum 127 characters + null terminator) */
    char customer_name[128];
} Sale;

/**
 * @brief Heap node structure for sales
 * 
 * Represents a single entry in the sales heap.
 * Each node contains a Sale and maintains heap properties.
 */
typedef struct {
    /** @brief Sale data stored in this node */
    Sale sale;
} HeapNode;

/**
 * @brief Max Heap structure for sales tracking
 * 
 * Max Heap data structure that maintains sales sorted by profit in descending order.
 * The root node always contains the sale with the highest profit.
 * 
 * @note Time complexity: O(log n) for insert/delete, O(1) for peek
 * @note Space complexity: O(n)
 */
typedef struct {
    /** @brief Array of heap nodes (dynamic array) */
    HeapNode* nodes;
    /** @brief Current number of sales in the heap */
    size_t size;
    /** @brief Maximum capacity of the heap */
    size_t capacity;
    /** @brief Next available sale ID */
    uint32_t next_id;
} SalesHeap;

/**
 * @brief Sales Tracker structure
 * 
 * Main structure that manages all sales tracking operations.
 * Uses Max Heap for efficient profit-based sorting.
 */
typedef struct {
    /** @brief Max Heap for sales storage */
    SalesHeap* heap;
} SalesTracker;

/**
 * @}
 */

// Sales Heap Functions

/**
 * @brief Create a new sales heap
 * 
 * Allocates memory for a new sales heap and initializes it.
 * The heap is ready to use after creation.
 * 
 * @param capacity Initial capacity of the heap (0 for default capacity)
 * @return Pointer to the newly created heap, or NULL on memory allocation failure
 * 
 * @note Time complexity: O(1)
 * @note The caller is responsible for destroying the heap using SalesHeap_Destroy()
 * to avoid memory leaks.
 */
SalesHeap* SalesHeap_Create(size_t capacity);

/**
 * @brief Destroy a sales heap and free all memory
 * 
 * Frees all nodes in the heap and then frees the heap structure itself.
 * This function is safe to call with NULL pointer (no-op).
 * 
 * @param heap Pointer to the sales heap to destroy (can be NULL)
 * 
 * @note Time complexity: O(n) where n is the number of nodes
 * @note After calling this function, the heap pointer becomes invalid and should not be used.
 */
void SalesHeap_Destroy(SalesHeap* heap);

/**
 * @brief Insert a new sale into the heap
 * 
 * Adds a new sale to the heap and maintains max heap property.
 * The sale with highest profit will bubble up to the root.
 * 
 * @param heap Pointer to the sales heap (must not be NULL)
 * @param sale Pointer to the sale data to insert (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters or memory allocation failure)
 * 
 * @note Time complexity: O(log n) where n is the number of nodes
 * @note The sale structure is copied into the heap
 */
int SalesHeap_Insert(SalesHeap* heap, const Sale* sale);

/**
 * @brief Remove and return the sale with maximum profit (root)
 * 
 * Extracts the root node (highest profit sale) and maintains max heap property.
 * 
 * @param heap Pointer to the sales heap (must not be NULL)
 * @param sale Pointer to store the extracted sale (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters or empty heap)
 * 
 * @note Time complexity: O(log n) where n is the number of nodes
 */
int SalesHeap_ExtractMax(SalesHeap* heap, Sale* sale);

/**
 * @brief Peek at the sale with maximum profit without removing it
 * 
 * @param heap Pointer to the sales heap (must not be NULL)
 * @param sale Pointer to store the sale data (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters or empty heap)
 * 
 * @note Time complexity: O(1)
 */
int SalesHeap_PeekMax(SalesHeap* heap, Sale* sale);

/**
 * @brief Check if the heap is empty
 * 
 * @param heap Pointer to the sales heap (must not be NULL)
 * @return 1 if empty, 0 if not empty, -1 if NULL
 * 
 * @note Time complexity: O(1)
 */
int SalesHeap_IsEmpty(SalesHeap* heap);

/**
 * @brief Get the number of sales in the heap
 * 
 * @param heap Pointer to the sales heap (must not be NULL)
 * @return Number of sales, or 0 if NULL or empty
 * 
 * @note Time complexity: O(1)
 */
size_t SalesHeap_GetSize(SalesHeap* heap);

/**
 * @brief Sort sales by profit using Heap Sort algorithm
 * 
 * Sorts the sales array by profit in descending order using Heap Sort.
 * This function creates a temporary heap, sorts the array, and returns sorted array.
 * 
 * @param sales Array of sales to sort (must not be NULL)
 * @param count Number of sales in the array (must be > 0)
 * @return 0 on success, -1 on error (NULL parameters or invalid count)
 * 
 * @note Time complexity: O(n log n) where n is the number of sales
 * @note Space complexity: O(1) for in-place sorting
 * @note The original array is modified (sorted in place)
 */
int SalesHeap_SortByProfit(Sale* sales, size_t count);

/**
 * @brief Get all sales from heap as an array (for display or sorting)
 * 
 * Extracts all sales from the heap and returns them as an array.
 * The heap remains unchanged after this operation.
 * 
 * @param heap Pointer to the sales heap (must not be NULL)
 * @param sales Array to store sales (must be pre-allocated with sufficient size)
 * @param max_count Maximum number of sales to retrieve
 * @return Number of sales retrieved
 * 
 * @note Time complexity: O(n) where n is the number of nodes
 * @note The heap structure is not modified
 */
size_t SalesHeap_GetAllSales(SalesHeap* heap, Sale* sales, size_t max_count);

// Sales Tracker Functions

/**
 * @brief Create a new sales tracker
 * 
 * Allocates memory for a new sales tracker and initializes it.
 * The tracker is ready to use after creation.
 * 
 * @return Pointer to the newly created tracker, or NULL on memory allocation failure
 * 
 * @note Time complexity: O(1)
 * @note The caller is responsible for destroying the tracker using SalesTracker_Destroy()
 * to avoid memory leaks.
 */
SalesTracker* SalesTracker_Create(void);

/**
 * @brief Destroy a sales tracker and free all memory
 * 
 * Frees all resources associated with the tracker.
 * This function is safe to call with NULL pointer (no-op).
 * 
 * @param tracker Pointer to the sales tracker to destroy (can be NULL)
 * 
 * @note Time complexity: O(n) where n is the number of sales
 */
void SalesTracker_Destroy(SalesTracker* tracker);

/**
 * @brief Record a new sale
 * 
 * Creates a new sale entry and adds it to the sales heap.
 * 
 * @param tracker Pointer to the sales tracker (must not be NULL)
 * @param material_id Material ID that was sold (0 for no material)
 * @param project_id Project ID associated with this sale (0 for no project)
 * @param quantity Quantity sold
 * @param unit_price_cents Unit price in cents
 * @param unit_cost_cents Unit cost in cents (for profit calculation)
 * @param customer_name Customer name string (max 127 characters)
 * @return Sale ID on success, 0 on error (NULL parameters or memory allocation failure)
 * 
 * @note Time complexity: O(log n) where n is the number of sales
 * @note Profit is automatically calculated as (unit_price - unit_cost) * quantity
 */
uint32_t SalesTracker_RecordSale(SalesTracker* tracker, uint32_t material_id, uint32_t project_id,
                                 uint32_t quantity, uint32_t unit_price_cents,
                                 uint32_t unit_cost_cents, const char* customer_name);

/**
 * @brief View all sales sorted by profit (highest first)
 * 
 * Displays all sales sorted by profit in descending order using Heap Sort.
 * 
 * @param tracker Pointer to the sales tracker (must not be NULL)
 * @param max_count Maximum number of sales to display (0 for all)
 * 
 * @note Time complexity: O(n log n) for sorting + O(n) for display
 */
void SalesTracker_ViewSales(SalesTracker* tracker, size_t max_count);

/**
 * @brief Calculate total profit from all sales
 * 
 * Calculates and returns the total profit from all recorded sales.
 * 
 * @param tracker Pointer to the sales tracker (must not be NULL)
 * @return Total profit in cents, or 0 if NULL or empty
 * 
 * @note Time complexity: O(n) where n is the number of sales
 */
uint64_t SalesTracker_CalculateTotalProfit(SalesTracker* tracker);

/**
 * @brief Get sales count
 * 
 * @param tracker Pointer to the sales tracker (must not be NULL)
 * @return Number of sales, or 0 if NULL
 * 
 * @note Time complexity: O(1)
 */
size_t SalesTracker_GetSalesCount(SalesTracker* tracker);

/**
 * @brief Save sales tracker to a binary file
 * 
 * Saves all sales data to a binary file for persistence.
 * 
 * @param tracker Pointer to the sales tracker (must not be NULL)
 * @param filename Filename to save to (must not be NULL)
 * @return 0 on success, -1 on error
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 * @note Time complexity: O(n) where n is the number of sales
 */
int SalesTracker_SaveToFile(SalesTracker* tracker, const char* filename);

/**
 * @brief Load sales tracker from a binary file
 * 
 * Loads sales data from a binary file.
 * 
 * @param tracker Pointer to existing tracker (will be destroyed if not NULL), or NULL to create new.
 * @param filename Filename to load from (must not be NULL)
 * @return Pointer to the loaded tracker, or NULL on error
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 * @note Time complexity: O(n) where n is the number of sales
 */
SalesTracker* SalesTracker_LoadFromFile(SalesTracker* tracker, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // SALES_TRACKER_H

