/**
 * @file ExpenseTracking.c
 * @brief Implementation of expense logging management functions using Sparse Matrix data structure.
 */

#include "../header/ExpenseTracking.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global timestamp counter for expense logging times
static uint32_t g_expense_timestamp_counter = 1;

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

ExpenseMatrix* ExpenseMatrix_Create(uint32_t max_rows, uint32_t max_cols) {
    /**
     * @brief Create a new expense matrix
     * 
     * Allocates memory for a new expense matrix and initializes it.
     * The matrix is ready to use after creation.
     * 
     * @param max_rows Maximum number of rows (0 for default MAX_EXPENSE_ROWS)
     * @param max_cols Maximum number of columns (0 for default MAX_EXPENSE_COLS)
     * @return Pointer to the newly created matrix, or NULL on memory allocation failure
     * 
     * @note The caller is responsible for destroying the matrix using ExpenseMatrix_Destroy()
     * to avoid memory leaks.
     */
    ExpenseMatrix* matrix = (ExpenseMatrix*)safe_malloc(sizeof(ExpenseMatrix));
    if (matrix == NULL) {
        return NULL;
    }
    
    matrix->max_rows = (max_rows == 0) ? MAX_EXPENSE_ROWS : max_rows;
    matrix->max_cols = (max_cols == 0) ? MAX_EXPENSE_COLS : max_cols;
    matrix->num_rows = 0;
    matrix->num_cols = 0;
    matrix->size = 0;
    matrix->next_id = 1;
    
    // Allocate row headers array
    matrix->row_headers = (ExpenseMatrixNode**)safe_malloc(sizeof(ExpenseMatrixNode*) * matrix->max_rows);
    if (matrix->row_headers == NULL) {
        free(matrix);
        return NULL;
    }
    
    // Allocate column headers array
    matrix->col_headers = (ExpenseMatrixNode**)safe_malloc(sizeof(ExpenseMatrixNode*) * matrix->max_cols);
    if (matrix->col_headers == NULL) {
        free(matrix->row_headers);
        free(matrix);
        return NULL;
    }
    
    // Initialize all headers to NULL
    for (uint32_t i = 0; i < matrix->max_rows; i++) {
        matrix->row_headers[i] = NULL;
    }
    for (uint32_t i = 0; i < matrix->max_cols; i++) {
        matrix->col_headers[i] = NULL;
    }
    
    return matrix;
}

void ExpenseMatrix_Destroy(ExpenseMatrix* matrix) {
    /**
     * @brief Destroy an expense matrix and free all memory
     * 
     * Frees all nodes in the matrix and then frees the matrix structure itself.
     * This function is safe to call with NULL pointer (no-op).
     * 
     * @param matrix Pointer to the matrix to destroy (can be NULL)
     * 
     * @note After calling this function, the matrix pointer becomes invalid
     * and should not be used.
     */
    if (matrix == NULL) {
        return;
    }
    
    // Free all nodes
    // We'll traverse all rows and free nodes
    for (uint32_t i = 0; i < matrix->max_rows; i++) {
        ExpenseMatrixNode* current = matrix->row_headers[i];
        while (current != NULL) {
            ExpenseMatrixNode* next = current->next_in_row;
            free(current);
            current = next;
        }
    }
    
    // Free header arrays
    if (matrix->row_headers != NULL) {
        free(matrix->row_headers);
    }
    if (matrix->col_headers != NULL) {
        free(matrix->col_headers);
    }
    
    free(matrix);
}

uint32_t ExpenseMatrix_CategoryToColumn(const char* category, uint32_t max_cols) {
    /**
     * @brief Calculate column index from category string (simple hash)
     * 
     * Uses djb2 hash algorithm to convert category string to column index.
     * 
     * @param category Category string (must not be NULL)
     * @param max_cols Maximum number of columns
     * @return Column index (0 to max_cols-1)
     */
    if (category == NULL || max_cols == 0) {
        return 0;
    }
    
    uint32_t hash = 5381;
    int c;
    const char* str = category;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % max_cols;
}

uint32_t ExpenseMatrix_AddExpense(ExpenseMatrix* matrix, uint32_t project_id, const char* category,
                                   uint32_t amount_cents, uint32_t material_id, const char* description) {
    /**
     * @brief Add a new expense to the matrix
     * 
     * Creates a new expense entry and adds it to the sparse matrix.
     * The expense is assigned a unique ID automatically.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param project_id Project ID (used as row index, 0 for no project)
     * @param category Expense category string (used to determine column index, must not be NULL)
     * @param amount_cents Expense amount in cents
     * @param material_id Material ID (0 for no material)
     * @param description Expense description string (must not be NULL)
     * @return Expense ID on success, 0 on error (NULL parameters or memory allocation failure)
     */
    if (matrix == NULL || category == NULL || description == NULL) {
        return 0;
    }
    
    // Validate row and column indices
    uint32_t row = project_id;
    if (row >= matrix->max_rows) {
        row = row % matrix->max_rows; // Wrap around if necessary
    }
    
    uint32_t col = ExpenseMatrix_CategoryToColumn(category, matrix->max_cols);
    
    // Create new node
    ExpenseMatrixNode* new_node = (ExpenseMatrixNode*)safe_malloc(sizeof(ExpenseMatrixNode));
    if (new_node == NULL) {
        return 0;
    }
    
    // Initialize expense
    new_node->expense.id = matrix->next_id++;
    new_node->expense.project_id = project_id;
    new_node->expense.material_id = material_id;
    new_node->expense.amount_cents = amount_cents;
    strncpy(new_node->expense.category, category, sizeof(new_node->expense.category) - 1);
    new_node->expense.category[sizeof(new_node->expense.category) - 1] = '\0';
    strncpy(new_node->expense.description, description, sizeof(new_node->expense.description) - 1);
    new_node->expense.description[sizeof(new_node->expense.description) - 1] = '\0';
    new_node->expense.timestamp = g_expense_timestamp_counter++;
    
    new_node->row = row;
    new_node->col = col;
    
    // Insert into row chain
    new_node->next_in_row = matrix->row_headers[row];
    matrix->row_headers[row] = new_node;
    
    // Insert into column chain
    new_node->next_in_col = matrix->col_headers[col];
    matrix->col_headers[col] = new_node;
    
    matrix->size++;
    
    // Update matrix dimensions
    if (project_id >= matrix->num_rows) {
        matrix->num_rows = project_id + 1;
    }
    
    return new_node->expense.id;
}

Expense* ExpenseMatrix_FindExpense(ExpenseMatrix* matrix, uint32_t id) {
    /**
     * @brief Find an expense by ID
     * 
     * Searches through the matrix to find an expense with the given ID.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param id Expense ID to search for
     * @return Pointer to the expense if found, NULL otherwise
     * 
     * @note The returned pointer is valid until the expense is removed or matrix is destroyed.
     */
    if (matrix == NULL) {
        return NULL;
    }
    
    // Search through all rows
    for (uint32_t i = 0; i < matrix->max_rows; i++) {
        ExpenseMatrixNode* current = matrix->row_headers[i];
        while (current != NULL) {
            if (current->expense.id == id) {
                return &(current->expense);
            }
            current = current->next_in_row;
        }
    }
    
    return NULL;
}

int ExpenseMatrix_RemoveExpense(ExpenseMatrix* matrix, uint32_t id) {
    /**
     * @brief Remove an expense from the matrix by ID
     * 
     * Removes the expense with the given ID from the matrix.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param id Expense ID to remove
     * @return 0 on success, -1 on error (expense not found)
     */
    if (matrix == NULL) {
        return -1;
    }
    
    // Search through all rows to find the expense
    for (uint32_t i = 0; i < matrix->max_rows; i++) {
        ExpenseMatrixNode* current = matrix->row_headers[i];
        ExpenseMatrixNode* prev = NULL;
        
        while (current != NULL) {
            if (current->expense.id == id) {
                // Found the expense, remove it
                
                // Remove from row chain
                if (prev == NULL) {
                    matrix->row_headers[i] = current->next_in_row;
                } else {
                    prev->next_in_row = current->next_in_row;
                }
                
                // Remove from column chain
                uint32_t col = current->col;
                ExpenseMatrixNode* col_current = matrix->col_headers[col];
                ExpenseMatrixNode* col_prev = NULL;
                
                while (col_current != NULL) {
                    if (col_current == current) {
                        if (col_prev == NULL) {
                            matrix->col_headers[col] = current->next_in_col;
                        } else {
                            col_prev->next_in_col = current->next_in_col;
                        }
                        break;
                    }
                    col_prev = col_current;
                    col_current = col_current->next_in_col;
                }
                
                free(current);
                matrix->size--;
                return 0;
            }
            prev = current;
            current = current->next_in_row;
        }
    }
    
    return -1; // Expense not found
}

size_t ExpenseMatrix_GetExpensesByProject(ExpenseMatrix* matrix, uint32_t project_id,
                                          Expense* expenses, size_t max_count) {
    /**
     * @brief Get all expenses for a specific project (row)
     * 
     * Retrieves all expenses associated with a specific project.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param project_id Project ID (row index)
     * @param expenses Array to store expenses (must be pre-allocated)
     * @param max_count Maximum number of expenses to retrieve
     * @return Number of expenses found
     */
    if (matrix == NULL || expenses == NULL || max_count == 0) {
        return 0;
    }
    
    uint32_t row = project_id;
    if (row >= matrix->max_rows) {
        row = row % matrix->max_rows;
    }
    
    size_t count = 0;
    ExpenseMatrixNode* current = matrix->row_headers[row];
    
    while (current != NULL && count < max_count) {
        // Only include expenses that match the exact project_id
        if (current->expense.project_id == project_id) {
            expenses[count] = current->expense;
            count++;
        }
        current = current->next_in_row;
    }
    
    return count;
}

size_t ExpenseMatrix_GetExpensesByCategory(ExpenseMatrix* matrix, const char* category,
                                           Expense* expenses, size_t max_count) {
    /**
     * @brief Get all expenses for a specific category (column)
     * 
     * Retrieves all expenses with a specific category.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param category Category string (must not be NULL)
     * @param expenses Array to store expenses (must be pre-allocated)
     * @param max_count Maximum number of expenses to retrieve
     * @return Number of expenses found
     */
    if (matrix == NULL || category == NULL || expenses == NULL || max_count == 0) {
        return 0;
    }
    
    uint32_t col = ExpenseMatrix_CategoryToColumn(category, matrix->max_cols);
    size_t count = 0;
    ExpenseMatrixNode* current = matrix->col_headers[col];
    
    while (current != NULL && count < max_count) {
        // Only include expenses that match the exact category
        if (strcmp(current->expense.category, category) == 0) {
            expenses[count] = current->expense;
            count++;
        }
        current = current->next_in_col;
    }
    
    return count;
}

uint64_t ExpenseMatrix_GetTotalExpensesByProject(ExpenseMatrix* matrix, uint32_t project_id) {
    /**
     * @brief Get total expenses for a project
     * 
     * Calculates the total amount of all expenses for a specific project.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param project_id Project ID
     * @return Total amount in cents, or 0 if no expenses found
     */
    if (matrix == NULL) {
        return 0;
    }
    
    uint64_t total = 0;
    Expense temp_expenses[1000]; // Temporary buffer
    size_t count = ExpenseMatrix_GetExpensesByProject(matrix, project_id, temp_expenses, 1000);
    
    for (size_t i = 0; i < count; i++) {
        total += temp_expenses[i].amount_cents;
    }
    
    return total;
}

uint64_t ExpenseMatrix_GetTotalExpensesByCategory(ExpenseMatrix* matrix, const char* category) {
    /**
     * @brief Get total expenses for a category
     * 
     * Calculates the total amount of all expenses for a specific category.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param category Category string (must not be NULL)
     * @return Total amount in cents, or 0 if no expenses found
     */
    if (matrix == NULL || category == NULL) {
        return 0;
    }
    
    uint64_t total = 0;
    Expense temp_expenses[1000]; // Temporary buffer
    size_t count = ExpenseMatrix_GetExpensesByCategory(matrix, category, temp_expenses, 1000);
    
    for (size_t i = 0; i < count; i++) {
        total += temp_expenses[i].amount_cents;
    }
    
    return total;
}

int ExpenseMatrix_IsEmpty(ExpenseMatrix* matrix) {
    /**
     * @brief Check if the matrix is empty
     * 
     * @param matrix Pointer to the expense matrix
     * @return 1 if empty, 0 if not empty, -1 if NULL
     */
    if (matrix == NULL) {
        return -1;
    }
    return (matrix->size == 0) ? 1 : 0;
}

size_t ExpenseMatrix_GetSize(ExpenseMatrix* matrix) {
    /**
     * @brief Get the number of expenses in the matrix
     * 
     * @param matrix Pointer to the expense matrix
     * @return Number of expenses, or 0 if NULL
     */
    if (matrix == NULL) {
        return 0;
    }
    return matrix->size;
}

void ExpenseMatrix_ViewExpenses(ExpenseMatrix* matrix, size_t max_count) {
    /**
     * @brief Display all expenses in the matrix
     * 
     * Displays expense information in a formatted way.
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param max_count Maximum number of expenses to display (0 for all)
     */
    if (matrix == NULL) {
        printf("Error: Matrix is NULL.\n");
        return;
    }
    
    if (matrix->size == 0) {
        printf("No expenses found.\n");
        return;
    }
    
    printf("\n");
    printf("========================================\n");
    printf("  Expense List\n");
    printf("========================================\n");
    printf("%-5s %-10s %-10s %-15s %-12s %-30s\n", 
           "ID", "Project", "Material", "Category", "Amount", "Description");
    printf("------------------------------------------------");
    printf("------------------------------------------------\n");
    
    size_t count = 0;
    
    // Traverse all rows
    for (uint32_t i = 0; i < matrix->max_rows && (max_count == 0 || count < max_count); i++) {
        ExpenseMatrixNode* current = matrix->row_headers[i];
        while (current != NULL && (max_count == 0 || count < max_count)) {
            double amount_dollars = (double)current->expense.amount_cents / 100.0;
            
            // Truncate description if too long for display
            char desc_display[31];
            strncpy(desc_display, current->expense.description, 30);
            desc_display[30] = '\0';
            if (strlen(current->expense.description) > 30) {
                desc_display[27] = '.';
                desc_display[28] = '.';
                desc_display[29] = '.';
                desc_display[30] = '\0';
            }
            
            printf("%-5u %-10u %-10u %-15s $%-11.2f %-30s\n",
                   current->expense.id,
                   current->expense.project_id,
                   current->expense.material_id,
                   current->expense.category,
                   amount_dollars,
                   desc_display);
            
            current = current->next_in_row;
            count++;
        }
    }
    
    printf("------------------------------------------------");
    printf("------------------------------------------------\n");
    
    if (max_count > 0 && count < matrix->size) {
        printf("\n... and %zu more expense(s).\n", matrix->size - count);
    }
    
    printf("\nTotal expenses: %zu\n", matrix->size);
    printf("========================================\n");
}

int ExpenseMatrix_SaveToFile(ExpenseMatrix* matrix, const char* filename) {
    /**
     * @brief Save expense matrix to binary file
     * 
     * Saves the expense matrix to a binary file using fwrite().
     * The file format is:
     * 1. size_t size (number of expenses)
     * 2. uint32_t next_id
     * 3. For each expense: Expense structure
     * 
     * @param matrix Pointer to the expense matrix (must not be NULL)
     * @param filename The filename to save to (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, file I/O error)
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     */
    if (matrix == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write the number of expenses first
    size_t expense_count = matrix->size;
    if (fwrite(&expense_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write next_id
    if (fwrite(&(matrix->next_id), sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all expenses sequentially
    for (uint32_t i = 0; i < matrix->max_rows; i++) {
        ExpenseMatrixNode* current = matrix->row_headers[i];
        while (current != NULL) {
            // Write Expense structure
            if (fwrite(&(current->expense), sizeof(Expense), 1, file) != 1) {
                fclose(file);
                return -1;
            }
            current = current->next_in_row;
        }
    }
    
    fclose(file);
    return 0;
}

ExpenseMatrix* ExpenseMatrix_LoadFromFile(ExpenseMatrix* matrix, const char* filename) {
    /**
     * @brief Load expense matrix from binary file
     * 
     * Loads the expense matrix from a binary file using fread().
     * The file format is:
     * 1. size_t size (number of expenses)
     * 2. uint32_t next_id
     * 3. For each expense: Expense structure
     * 
     * @param matrix Pointer to existing matrix (will be destroyed if not NULL), or NULL to create new
     * @param filename The filename to load from (must not be NULL)
     * @return Pointer to the loaded expense matrix, or NULL on error
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     */
    if (filename == NULL) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL; // File doesn't exist or can't be opened
    }
    
    // Destroy existing matrix if provided
    if (matrix != NULL) {
        ExpenseMatrix_Destroy(matrix);
    }
    
    // Create new matrix
    matrix = ExpenseMatrix_Create(0, 0); // Use defaults
    if (matrix == NULL) {
        fclose(file);
        return NULL;
    }
    
    // Read the number of expenses
    size_t expense_count;
    if (fread(&expense_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        ExpenseMatrix_Destroy(matrix);
        return NULL;
    }
    
    // Read next_id
    uint32_t next_id;
    if (fread(&next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        ExpenseMatrix_Destroy(matrix);
        return NULL;
    }
    
    matrix->next_id = next_id;
    
    // Read all expenses and add them to the matrix
    for (size_t i = 0; i < expense_count; i++) {
        Expense expense;
        if (fread(&expense, sizeof(Expense), 1, file) != 1) {
            fclose(file);
            return matrix; // Return partial load
        }
        
        // Restore timestamp counter if needed
        if (expense.timestamp >= g_expense_timestamp_counter) {
            g_expense_timestamp_counter = expense.timestamp + 1;
        }
        
        // Add expense to matrix
        ExpenseMatrix_AddExpense(matrix, expense.project_id, expense.category,
                                 expense.amount_cents, expense.material_id, expense.description);
        
        // Restore the original ID
        Expense* added = ExpenseMatrix_FindExpense(matrix, matrix->next_id - 1);
        if (added != NULL) {
            added->id = expense.id;
            added->timestamp = expense.timestamp;
        }
    }
    
    // Restore next_id
    matrix->next_id = next_id;
    
    fclose(file);
    return matrix;
}

