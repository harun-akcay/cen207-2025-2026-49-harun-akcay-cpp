/**
 * @file ExpenseTracking.h
 * 
 * @brief Provides functions for expense logging management using Sparse Matrix data structure
 */

#ifndef EXPENSE_TRACKING_H
#define EXPENSE_TRACKING_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ExpenseTracking Expense Logging Implementation
 * @brief Sparse Matrix data structure for expense logging management
 * @{
 */

/**
 * @brief Expense structure for logging
 * 
 * Represents an expense entry in the inventory management system.
 * Stores expense details including project ID, material ID, amount, category, and description.
 */
typedef struct {
    /** @brief Expense ID (unique identifier) */
    uint32_t id;
    /** @brief Project ID this expense is associated with (0 for no project) */
    uint32_t project_id;
    /** @brief Material ID this expense is associated with (0 for no material) */
    uint32_t material_id;
    /** @brief Expense amount in cents (to avoid floating point issues) */
    uint32_t amount_cents;
    /** @brief Expense category string (maximum 63 characters + null terminator) */
    char category[64];
    /** @brief Expense description string (maximum 255 characters + null terminator) */
    char description[256];
    /** @brief Timestamp when expense was logged */
    uint32_t timestamp;
} Expense;

/**
 * @brief Sparse Matrix Node structure
 * 
 * Represents a single non-zero entry in the sparse matrix.
 * Each node contains an Expense and pointers to the next node in row and column.
 */
typedef struct ExpenseMatrixNode {
    /** @brief Expense data stored in this node */
    Expense expense;
    /** @brief Row index in the sparse matrix */
    uint32_t row;
    /** @brief Column index in the sparse matrix */
    uint32_t col;
    /** @brief Pointer to the next node in the same row (NULL if last in row) */
    struct ExpenseMatrixNode* next_in_row;
    /** @brief Pointer to the next node in the same column (NULL if last in column) */
    struct ExpenseMatrixNode* next_in_col;
} ExpenseMatrixNode;

/**
 * @brief Sparse Matrix structure for expense tracking
 * 
 * Represents a sparse matrix where rows can represent projects and columns can represent
 * expense categories or time periods. Only non-zero (non-empty) entries are stored.
 * 
 * Row organization: By project ID (row = project_id)
 * Column organization: By category hash or time period (col = category_index)
 */
typedef struct {
    /** @brief Array of row headers (pointers to first node in each row) */
    ExpenseMatrixNode** row_headers;
    /** @brief Array of column headers (pointers to first node in each column) */
    ExpenseMatrixNode** col_headers;
    /** @brief Number of rows in the matrix */
    uint32_t num_rows;
    /** @brief Number of columns in the matrix */
    uint32_t num_cols;
    /** @brief Current number of expenses (non-zero entries) */
    size_t size;
    /** @brief Next available expense ID */
    uint32_t next_id;
    /** @brief Maximum number of rows (capacity) */
    uint32_t max_rows;
    /** @brief Maximum number of columns (capacity) */
    uint32_t max_cols;
} ExpenseMatrix;

/**
 * @def MAX_EXPENSE_ROWS
 * @brief Maximum number of rows in the expense matrix
 */
#define MAX_EXPENSE_ROWS 1000

/**
 * @def MAX_EXPENSE_COLS
 * @brief Maximum number of columns in the expense matrix
 */
#define MAX_EXPENSE_COLS 100

/**
 * @}
 */

// Expense Matrix Functions

/**
 * @brief Create a new expense matrix.
 * @param max_rows Maximum number of rows (0 for default MAX_EXPENSE_ROWS).
 * @param max_cols Maximum number of columns (0 for default MAX_EXPENSE_COLS).
 * @return Pointer to the expense matrix, or NULL on error.
 */
ExpenseMatrix* ExpenseMatrix_Create(uint32_t max_rows, uint32_t max_cols);

/**
 * @brief Destroy an expense matrix and free all memory.
 * @param matrix Pointer to the expense matrix.
 */
void ExpenseMatrix_Destroy(ExpenseMatrix* matrix);

/**
 * @brief Add a new expense to the matrix.
 * @param matrix Pointer to the expense matrix.
 * @param project_id Project ID (0 for no project, used as row index).
 * @param category Expense category (used to determine column index).
 * @param amount_cents Expense amount in cents.
 * @param material_id Material ID (0 for no material).
 * @param description Expense description.
 * @return Expense ID on success, 0 on error.
 */
uint32_t ExpenseMatrix_AddExpense(ExpenseMatrix* matrix, uint32_t project_id, const char* category,
                                   uint32_t amount_cents, uint32_t material_id, const char* description);

/**
 * @brief Remove an expense from the matrix by ID.
 * @param matrix Pointer to the expense matrix.
 * @param id Expense ID to remove.
 * @return 0 on success, -1 on error (expense not found).
 */
int ExpenseMatrix_RemoveExpense(ExpenseMatrix* matrix, uint32_t id);

/**
 * @brief Find an expense by ID.
 * @param matrix Pointer to the expense matrix.
 * @param id Expense ID to search for.
 * @return Pointer to the expense if found, NULL otherwise.
 */
Expense* ExpenseMatrix_FindExpense(ExpenseMatrix* matrix, uint32_t id);

/**
 * @brief Get all expenses for a specific project (row).
 * @param matrix Pointer to the expense matrix.
 * @param project_id Project ID (row index).
 * @param expenses Array to store expenses (must be pre-allocated).
 * @param max_count Maximum number of expenses to retrieve.
 * @return Number of expenses found.
 */
size_t ExpenseMatrix_GetExpensesByProject(ExpenseMatrix* matrix, uint32_t project_id,
                                          Expense* expenses, size_t max_count);

/**
 * @brief Get all expenses for a specific category (column).
 * @param matrix Pointer to the expense matrix.
 * @param category Category string.
 * @param expenses Array to store expenses (must be pre-allocated).
 * @param max_count Maximum number of expenses to retrieve.
 * @return Number of expenses found.
 */
size_t ExpenseMatrix_GetExpensesByCategory(ExpenseMatrix* matrix, const char* category,
                                           Expense* expenses, size_t max_count);

/**
 * @brief Get total expenses for a project.
 * @param matrix Pointer to the expense matrix.
 * @param project_id Project ID.
 * @return Total amount in cents, or 0 if no expenses found.
 */
uint64_t ExpenseMatrix_GetTotalExpensesByProject(ExpenseMatrix* matrix, uint32_t project_id);

/**
 * @brief Get total expenses for a category.
 * @param matrix Pointer to the expense matrix.
 * @param category Category string.
 * @return Total amount in cents, or 0 if no expenses found.
 */
uint64_t ExpenseMatrix_GetTotalExpensesByCategory(ExpenseMatrix* matrix, const char* category);

/**
 * @brief Check if the matrix is empty.
 * @param matrix Pointer to the expense matrix.
 * @return 1 if empty, 0 if not empty, -1 if NULL.
 */
int ExpenseMatrix_IsEmpty(ExpenseMatrix* matrix);

/**
 * @brief Get the number of expenses in the matrix.
 * @param matrix Pointer to the expense matrix.
 * @return Number of expenses, or 0 if NULL.
 */
size_t ExpenseMatrix_GetSize(ExpenseMatrix* matrix);

/**
 * @brief Display all expenses in the matrix.
 * @param matrix Pointer to the expense matrix.
 * @param max_count Maximum number of expenses to display (0 for all).
 */
void ExpenseMatrix_ViewExpenses(ExpenseMatrix* matrix, size_t max_count);

/**
 * @brief Save the expense matrix to a binary file.
 * @param matrix Pointer to the expense matrix.
 * @param filename Filename to save to.
 * @return 0 on success, -1 on error.
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 */
int ExpenseMatrix_SaveToFile(ExpenseMatrix* matrix, const char* filename);

/**
 * @brief Load the expense matrix from a binary file.
 * @param matrix Pointer to existing matrix (will be destroyed if not NULL), or NULL to create new.
 * @param filename Filename to load from.
 * @return Pointer to the loaded expense matrix, or NULL on error.
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 */
ExpenseMatrix* ExpenseMatrix_LoadFromFile(ExpenseMatrix* matrix, const char* filename);

/**
 * @brief Calculate column index from category string (simple hash).
 * @param category Category string.
 * @param max_cols Maximum number of columns.
 * @return Column index (0 to max_cols-1).
 */
uint32_t ExpenseMatrix_CategoryToColumn(const char* category, uint32_t max_cols);

#ifdef __cplusplus
}
#endif

#endif // EXPENSE_TRACKING_H

