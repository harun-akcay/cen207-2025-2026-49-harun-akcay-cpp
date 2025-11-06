/**
 * @file SalesTracker.c
 * @brief Implementation of sales tracking management functions using Heap and Heap Sort data structures.
 */

#include "../header/SalesTracker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Disable MSVC warnings for deprecated functions
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Default heap capacity
#define DEFAULT_HEAP_CAPACITY 100

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

// Global timestamp counter for sales
static uint32_t g_sale_timestamp_counter = 1;

// Internal helper functions for heap operations

/**
 * @brief Get parent index in heap array
 */
static size_t heap_parent(size_t index) {
    return (index > 0) ? (index - 1) / 2 : 0;
}

/**
 * @brief Get left child index in heap array
 */
static size_t heap_left_child(size_t index) {
    return 2 * index + 1;
}

/**
 * @brief Get right child index in heap array
 */
static size_t heap_right_child(size_t index) {
    return 2 * index + 2;
}

/**
 * @brief Swap two heap nodes
 */
static void heap_swap_nodes(HeapNode* a, HeapNode* b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Heapify up (bubble up) to maintain max heap property
 * 
 * Moves a node up the heap until max heap property is satisfied.
 * Used after insertion.
 */
static void heapify_up(SalesHeap* heap, size_t index) {
    while (index > 0) {
        size_t parent_idx = heap_parent(index);
        
        // If current node's profit is greater than parent's profit, swap
        if (heap->nodes[index].sale.profit_cents > heap->nodes[parent_idx].sale.profit_cents) {
            heap_swap_nodes(&heap->nodes[index], &heap->nodes[parent_idx]);
            index = parent_idx;
        } else {
            break; // Max heap property satisfied
        }
    }
}

/**
 * @brief Heapify down (bubble down) to maintain max heap property
 * 
 * Moves a node down the heap until max heap property is satisfied.
 * Used after extraction or when building heap.
 */
static void heapify_down(SalesHeap* heap, size_t index) {
    while (index < heap->size) {
        size_t left = heap_left_child(index);
        size_t right = heap_right_child(index);
        size_t largest = index;
        
        // Compare with left child
        if (left < heap->size && 
            heap->nodes[left].sale.profit_cents > heap->nodes[largest].sale.profit_cents) {
            largest = left;
        }
        
        // Compare with right child
        if (right < heap->size && 
            heap->nodes[right].sale.profit_cents > heap->nodes[largest].sale.profit_cents) {
            largest = right;
        }
        
        // If largest is not current index, swap and continue
        if (largest != index) {
            heap_swap_nodes(&heap->nodes[index], &heap->nodes[largest]);
            index = largest;
        } else {
            break; // Max heap property satisfied
        }
    }
}

/**
 * @brief Resize heap capacity if needed
 */
static int heap_resize(SalesHeap* heap, size_t new_capacity) {
    HeapNode* new_nodes = (HeapNode*)realloc(heap->nodes, new_capacity * sizeof(HeapNode));
    if (new_nodes == NULL && new_capacity > 0) {
        return -1; // Memory allocation failed
    }
    
    heap->nodes = new_nodes;
    heap->capacity = new_capacity;
    return 0;
}

// Sales Heap Functions

SalesHeap* SalesHeap_Create(size_t capacity) {
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
    SalesHeap* heap = (SalesHeap*)safe_malloc(sizeof(SalesHeap));
    if (heap == NULL) {
        return NULL;
    }
    
    size_t initial_capacity = (capacity > 0) ? capacity : DEFAULT_HEAP_CAPACITY;
    
    heap->nodes = (HeapNode*)safe_malloc(initial_capacity * sizeof(HeapNode));
    if (heap->nodes == NULL) {
        free(heap);
        return NULL;
    }
    
    heap->size = 0;
    heap->capacity = initial_capacity;
    heap->next_id = 1;
    
    return heap;
}

void SalesHeap_Destroy(SalesHeap* heap) {
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
    if (heap == NULL) {
        return;
    }
    
    if (heap->nodes != NULL) {
        free(heap->nodes);
    }
    
    free(heap);
}

int SalesHeap_Insert(SalesHeap* heap, const Sale* sale) {
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
    if (heap == NULL || sale == NULL) {
        return -1;
    }
    
    // Resize if needed
    if (heap->size >= heap->capacity) {
        size_t new_capacity = heap->capacity * 2;
        if (heap_resize(heap, new_capacity) != 0) {
            return -1; // Memory allocation failed
        }
    }
    
    // Add new node at the end
    heap->nodes[heap->size].sale = *sale;
    heap->size++;
    
    // Heapify up to maintain max heap property
    heapify_up(heap, heap->size - 1);
    
    return 0;
}

int SalesHeap_ExtractMax(SalesHeap* heap, Sale* sale) {
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
    if (heap == NULL || sale == NULL || heap->size == 0) {
        return -1;
    }
    
    // Copy root (max) to output
    *sale = heap->nodes[0].sale;
    
    // Move last node to root
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    
    // Heapify down to maintain max heap property
    if (heap->size > 0) {
        heapify_down(heap, 0);
    }
    
    return 0;
}

int SalesHeap_PeekMax(SalesHeap* heap, Sale* sale) {
    /**
     * @brief Peek at the sale with maximum profit without removing it
     * 
     * @param heap Pointer to the sales heap (must not be NULL)
     * @param sale Pointer to store the sale data (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters or empty heap)
     * 
     * @note Time complexity: O(1)
     */
    if (heap == NULL || sale == NULL || heap->size == 0) {
        return -1;
    }
    
    *sale = heap->nodes[0].sale;
    return 0;
}

int SalesHeap_IsEmpty(SalesHeap* heap) {
    /**
     * @brief Check if the heap is empty
     * 
     * @param heap Pointer to the sales heap (must not be NULL)
     * @return 1 if empty, 0 if not empty, -1 if NULL
     * 
     * @note Time complexity: O(1)
     */
    if (heap == NULL) {
        return -1;
    }
    
    return (heap->size == 0) ? 1 : 0;
}

size_t SalesHeap_GetSize(SalesHeap* heap) {
    /**
     * @brief Get the number of sales in the heap
     * 
     * @param heap Pointer to the sales heap (must not be NULL)
     * @return Number of sales, or 0 if NULL or empty
     * 
     * @note Time complexity: O(1)
     */
    if (heap == NULL) {
        return 0;
    }
    
    return heap->size;
}

int SalesHeap_SortByProfit(Sale* sales, size_t count) {
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
    if (sales == NULL || count == 0) {
        return -1;
    }
    
    // Build max heap from array (heapify from last non-leaf node to root)
    for (size_t i = count / 2; i > 0; i--) {
        size_t idx = i - 1;
        size_t current = idx;
        
        // Heapify down from current position
        while (current < count) {
            size_t left = heap_left_child(current);
            size_t right = heap_right_child(current);
            size_t largest = current;
            
            // Compare with left child
            if (left < count && sales[left].profit_cents > sales[largest].profit_cents) {
                largest = left;
            }
            
            // Compare with right child
            if (right < count && sales[right].profit_cents > sales[largest].profit_cents) {
                largest = right;
            }
            
            // If largest is not current, swap and continue
            if (largest != current) {
                Sale temp = sales[current];
                sales[current] = sales[largest];
                sales[largest] = temp;
                current = largest;
            } else {
                break;
            }
        }
    }
    
    // Extract max repeatedly to sort (descending order)
    for (size_t i = count - 1; i > 0; i--) {
        // Swap root (max) with last element
        Sale temp = sales[0];
        sales[0] = sales[i];
        sales[i] = temp;
        
        // Heapify down on reduced heap (size = i)
        size_t current = 0;
        while (current < i) {
            size_t left = heap_left_child(current);
            size_t right = heap_right_child(current);
            size_t largest = current;
            
            // Compare with left child
            if (left < i && sales[left].profit_cents > sales[largest].profit_cents) {
                largest = left;
            }
            
            // Compare with right child
            if (right < i && sales[right].profit_cents > sales[largest].profit_cents) {
                largest = right;
            }
            
            // If largest is not current, swap and continue
            if (largest != current) {
                Sale temp = sales[current];
                sales[current] = sales[largest];
                sales[largest] = temp;
                current = largest;
            } else {
                break;
            }
        }
    }
    
    // Reverse array to get descending order (heap sort produces ascending)
    // Heap sort puts largest at end, we want largest at beginning
    for (size_t i = 0; i < count / 2; i++) {
        Sale temp = sales[i];
        sales[i] = sales[count - 1 - i];
        sales[count - 1 - i] = temp;
    }
    
    return 0;
}

size_t SalesHeap_GetAllSales(SalesHeap* heap, Sale* sales, size_t max_count) {
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
    if (heap == NULL || sales == NULL || max_count == 0) {
        return 0;
    }
    
    size_t count = (heap->size < max_count) ? heap->size : max_count;
    
    // Copy sales from heap array
    for (size_t i = 0; i < count; i++) {
        sales[i] = heap->nodes[i].sale;
    }
    
    return count;
}

// Sales Tracker Functions

SalesTracker* SalesTracker_Create(void) {
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
    SalesTracker* tracker = (SalesTracker*)safe_malloc(sizeof(SalesTracker));
    if (tracker == NULL) {
        return NULL;
    }
    
    tracker->heap = SalesHeap_Create(0); // Use default capacity
    if (tracker->heap == NULL) {
        free(tracker);
        return NULL;
    }
    
    return tracker;
}

void SalesTracker_Destroy(SalesTracker* tracker) {
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
    if (tracker == NULL) {
        return;
    }
    
    if (tracker->heap != NULL) {
        SalesHeap_Destroy(tracker->heap);
    }
    
    free(tracker);
}

uint32_t SalesTracker_RecordSale(SalesTracker* tracker, uint32_t material_id, uint32_t project_id,
                                 uint32_t quantity, uint32_t unit_price_cents,
                                 uint32_t unit_cost_cents, const char* customer_name) {
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
    if (tracker == NULL || tracker->heap == NULL || customer_name == NULL) {
        return 0;
    }
    
    Sale sale;
    sale.id = tracker->heap->next_id++;
    sale.material_id = material_id;
    sale.project_id = project_id;
    sale.quantity = quantity;
    sale.unit_price_cents = unit_price_cents;
    sale.total_amount_cents = (uint64_t)quantity * (uint64_t)unit_price_cents;
    sale.unit_cost_cents = unit_cost_cents;
    
    // Calculate profit: (unit_price - unit_cost) * quantity
    uint64_t profit_per_unit = (uint64_t)unit_price_cents - (uint64_t)unit_cost_cents;
    sale.profit_cents = profit_per_unit * (uint64_t)quantity;
    
    sale.sale_timestamp = g_sale_timestamp_counter++;
    
    // Copy customer name (with bounds checking)
    strncpy(sale.customer_name, customer_name, sizeof(sale.customer_name) - 1);
    sale.customer_name[sizeof(sale.customer_name) - 1] = '\0';
    
    if (SalesHeap_Insert(tracker->heap, &sale) != 0) {
        return 0; // Insertion failed
    }
    
    return sale.id;
}

void SalesTracker_ViewSales(SalesTracker* tracker, size_t max_count) {
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
    if (tracker == NULL || tracker->heap == NULL || tracker->heap->size == 0) {
        printf("No sales recorded.\n");
        return;
    }
    
    size_t count = tracker->heap->size;
    if (max_count > 0 && max_count < count) {
        count = max_count;
    }
    
    // Allocate array for sales
    Sale* sales = (Sale*)malloc(count * sizeof(Sale));
    if (sales == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    
    // Get all sales from heap
    size_t retrieved = SalesHeap_GetAllSales(tracker->heap, sales, count);
    
    // Sort by profit using Heap Sort
    if (retrieved > 0) {
        SalesHeap_SortByProfit(sales, retrieved);
        
        // Display sorted sales
        printf("\n=== Sales (Sorted by Profit - Highest First) ===\n");
        printf("%-5s %-10s %-10s %-8s %-12s %-12s %-15s %-20s\n",
               "ID", "Material", "Project", "Quantity", "Unit Price", "Total Amount", "Profit", "Customer");
        printf("--------------------------------------------------------------------------------------------------------\n");
        
        for (size_t i = 0; i < retrieved; i++) {
            printf("%-5u %-10u %-10u %-8u %-12llu %-12llu %-15llu %-20s\n",
                   sales[i].id,
                   sales[i].material_id,
                   sales[i].project_id,
                   sales[i].quantity,
                   (unsigned long long)(sales[i].unit_price_cents / 100.0),
                   (unsigned long long)(sales[i].total_amount_cents / 100.0),
                   (unsigned long long)(sales[i].profit_cents / 100.0),
                   sales[i].customer_name);
        }
        
        printf("--------------------------------------------------------------------------------------------------------\n");
        printf("Total Sales: %zu\n", retrieved);
    }
    
    free(sales);
}

uint64_t SalesTracker_CalculateTotalProfit(SalesTracker* tracker) {
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
    if (tracker == NULL || tracker->heap == NULL || tracker->heap->size == 0) {
        return 0;
    }
    
    uint64_t total_profit = 0;
    
    // Sum all profits from heap
    for (size_t i = 0; i < tracker->heap->size; i++) {
        total_profit += tracker->heap->nodes[i].sale.profit_cents;
    }
    
    return total_profit;
}

size_t SalesTracker_GetSalesCount(SalesTracker* tracker) {
    /**
     * @brief Get sales count
     * 
     * @param tracker Pointer to the sales tracker (must not be NULL)
     * @return Number of sales, or 0 if NULL
     * 
     * @note Time complexity: O(1)
     */
    if (tracker == NULL || tracker->heap == NULL) {
        return 0;
    }
    
    return SalesHeap_GetSize(tracker->heap);
}

int SalesTracker_SaveToFile(SalesTracker* tracker, const char* filename) {
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
    if (tracker == NULL || tracker->heap == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write heap metadata
    size_t count = tracker->heap->size;
    if (fwrite(&count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    if (fwrite(&tracker->heap->next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all sales
    for (size_t i = 0; i < count; i++) {
        if (fwrite(&tracker->heap->nodes[i].sale, sizeof(Sale), 1, file) != 1) {
            fclose(file);
            return -1;
        }
    }
    
    fclose(file);
    return 0;
}

SalesTracker* SalesTracker_LoadFromFile(SalesTracker* tracker, const char* filename) {
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
    if (filename == NULL) {
        return NULL;
    }
    
    // Destroy existing tracker if provided
    if (tracker != NULL) {
        SalesTracker_Destroy(tracker);
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        // File doesn't exist, create new tracker
        return SalesTracker_Create();
    }
    
    // Read heap metadata
    size_t count;
    uint32_t next_id;
    
    if (fread(&count, sizeof(size_t), 1, file) != 1 ||
        fread(&next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return NULL;
    }
    
    // Create new tracker
    tracker = SalesTracker_Create();
    if (tracker == NULL) {
        fclose(file);
        return NULL;
    }
    
    tracker->heap->next_id = next_id;
    
    // Read all sales
    for (size_t i = 0; i < count; i++) {
        Sale sale;
        if (fread(&sale, sizeof(Sale), 1, file) != 1) {
            SalesTracker_Destroy(tracker);
            fclose(file);
            return NULL;
        }
        
        if (SalesHeap_Insert(tracker->heap, &sale) != 0) {
            SalesTracker_Destroy(tracker);
            fclose(file);
            return NULL;
        }
    }
    
    fclose(file);
    return tracker;
}

