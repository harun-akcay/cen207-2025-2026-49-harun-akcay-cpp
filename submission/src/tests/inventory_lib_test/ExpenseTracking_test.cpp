/**
 * @file ExpenseTracking_test.cpp
 * @brief Unit tests for ExpenseTracking module (Sparse Matrix)
 * 
 * This file contains comprehensive unit tests for the ExpenseTracking module
 * using the Google Test framework. The tests cover sparse matrix operations,
 * expense management, and error handling.
 */

#include "gtest/gtest.h"
#include "../../inventory_lib/header/ExpenseTracking.h"
#include "../../inventory_lib/header/InventoryManager.h"
#include "mock_malloc.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

/**
 * @class ExpenseTrackingTest
 * @brief Test fixture for ExpenseTracking unit tests
 */
class ExpenseTrackingTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

// Expense Matrix Creation and Destruction Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_CreateDestroy) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 0);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_CreateWithZeroCapacity) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(0, 0);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_DestroyNull) {
	ExpenseMatrix_Destroy(nullptr); // Should not crash
}

// Expense Addition Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_AddExpenseSingle) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	uint32_t id = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Test expense");
	EXPECT_NE(id, 0);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 1);
	
	Expense* expense = ExpenseMatrix_FindExpense(matrix, id);
	EXPECT_NE(expense, nullptr);
	EXPECT_EQ(expense->project_id, 1);
	EXPECT_STREQ(expense->category, "Materials");
	EXPECT_EQ(expense->amount_cents, 5000);
	EXPECT_EQ(expense->material_id, 10);
	EXPECT_STREQ(expense->description, "Test expense");
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_AddExpenseMultiple) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	uint32_t id1 = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	uint32_t id2 = ExpenseMatrix_AddExpense(matrix, 2, "Labor", 10000, 20, "Expense2");
	uint32_t id3 = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 3000, 30, "Expense3");
	
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_NE(id3, 0);
	EXPECT_NE(id1, id2);
	EXPECT_NE(id2, id3);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 3);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_AddExpenseNullParams) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	uint32_t id = ExpenseMatrix_AddExpense(nullptr, 1, "Materials", 5000, 10, "Test");
	EXPECT_EQ(id, 0);
	
	id = ExpenseMatrix_AddExpense(matrix, 1, nullptr, 5000, 10, "Test");
	EXPECT_EQ(id, 0);
	
	id = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, nullptr);
	EXPECT_EQ(id, 0);
	
	ExpenseMatrix_Destroy(matrix);
}

// Expense Find Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_FindExpense) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	uint32_t id = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Test expense");
	EXPECT_NE(id, 0);
	
	Expense* expense = ExpenseMatrix_FindExpense(matrix, id);
	EXPECT_NE(expense, nullptr);
	EXPECT_STREQ(expense->category, "Materials");
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_FindExpenseNonExistent) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	Expense* expense = ExpenseMatrix_FindExpense(matrix, 999);
	EXPECT_EQ(expense, nullptr);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_FindExpenseNull) {
	Expense* expense = ExpenseMatrix_FindExpense(nullptr, 1);
	EXPECT_EQ(expense, nullptr);
}

// Expense Remove Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_RemoveExpense) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	uint32_t id = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Test expense");
	EXPECT_NE(id, 0);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 1);
	
	int result = ExpenseMatrix_RemoveExpense(matrix, id);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 0);
	
	Expense* expense = ExpenseMatrix_FindExpense(matrix, id);
	EXPECT_EQ(expense, nullptr);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_RemoveExpenseMultiple) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	uint32_t id1 = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	uint32_t id2 = ExpenseMatrix_AddExpense(matrix, 2, "Labor", 10000, 20, "Expense2");
	uint32_t id3 = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 3000, 30, "Expense3");
	
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 3);
	
	// Remove first
	int result = ExpenseMatrix_RemoveExpense(matrix, id1);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 2);
	
	// Remove middle
	result = ExpenseMatrix_RemoveExpense(matrix, id2);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 1);
	
	// Remove last
	result = ExpenseMatrix_RemoveExpense(matrix, id3);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 0);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_RemoveExpenseNonExistent) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	int result = ExpenseMatrix_RemoveExpense(matrix, 999);
	EXPECT_EQ(result, -1);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_RemoveExpenseNull) {
	int result = ExpenseMatrix_RemoveExpense(nullptr, 1);
	EXPECT_EQ(result, -1);
}

// Expense Query Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetExpensesByProject) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 1, "Labor", 10000, 20, "Expense2");
	ExpenseMatrix_AddExpense(matrix, 2, "Materials", 3000, 30, "Expense3");
	
	Expense expenses[10];
	size_t count = ExpenseMatrix_GetExpensesByProject(matrix, 1, expenses, 10);
	EXPECT_EQ(count, 2);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetExpensesByProjectNull) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	Expense expenses[10];
	
	// Test NULL matrix
	size_t count = ExpenseMatrix_GetExpensesByProject(nullptr, 1, expenses, 10);
	EXPECT_EQ(count, 0);
	
	// Test NULL expenses array
	count = ExpenseMatrix_GetExpensesByProject(matrix, 1, nullptr, 10);
	EXPECT_EQ(count, 0);
	
	// Test max_count = 0
	count = ExpenseMatrix_GetExpensesByProject(matrix, 1, expenses, 0);
	EXPECT_EQ(count, 0);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetExpensesByCategory) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 2, "Materials", 3000, 30, "Expense2");
	ExpenseMatrix_AddExpense(matrix, 1, "Labor", 10000, 20, "Expense3");
	
	Expense expenses[10];
	size_t count = ExpenseMatrix_GetExpensesByCategory(matrix, "Materials", expenses, 10);
	EXPECT_EQ(count, 2);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetExpensesByCategoryNull) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	Expense expenses[10];
	
	// Test NULL matrix
	size_t count = ExpenseMatrix_GetExpensesByCategory(nullptr, "Materials", expenses, 10);
	EXPECT_EQ(count, 0);
	
	// Test NULL category
	count = ExpenseMatrix_GetExpensesByCategory(matrix, nullptr, expenses, 10);
	EXPECT_EQ(count, 0);
	
	// Test NULL expenses array
	count = ExpenseMatrix_GetExpensesByCategory(matrix, "Materials", nullptr, 10);
	EXPECT_EQ(count, 0);
	
	// Test max_count = 0
	count = ExpenseMatrix_GetExpensesByCategory(matrix, "Materials", expenses, 0);
	EXPECT_EQ(count, 0);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetTotalExpensesByProject) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 1, "Labor", 10000, 20, "Expense2");
	ExpenseMatrix_AddExpense(matrix, 2, "Materials", 3000, 30, "Expense3");
	
	uint64_t total = ExpenseMatrix_GetTotalExpensesByProject(matrix, 1);
	EXPECT_EQ(total, 15000ULL);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetTotalExpensesByProjectNull) {
	// Test NULL matrix
	uint64_t total = ExpenseMatrix_GetTotalExpensesByProject(nullptr, 1);
	EXPECT_EQ(total, 0ULL);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetTotalExpensesByCategory) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 2, "Materials", 3000, 30, "Expense2");
	ExpenseMatrix_AddExpense(matrix, 1, "Labor", 10000, 20, "Expense3");
	
	uint64_t total = ExpenseMatrix_GetTotalExpensesByCategory(matrix, "Materials");
	EXPECT_EQ(total, 8000ULL);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetTotalExpensesByCategoryNull) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	// Test NULL matrix
	uint64_t total = ExpenseMatrix_GetTotalExpensesByCategory(nullptr, "Materials");
	EXPECT_EQ(total, 0ULL);
	
	// Test NULL category
	total = ExpenseMatrix_GetTotalExpensesByCategory(matrix, nullptr);
	EXPECT_EQ(total, 0ULL);
	
	ExpenseMatrix_Destroy(matrix);
}

// Matrix Size Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetSize) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 0);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 1);
	
	ExpenseMatrix_AddExpense(matrix, 2, "Labor", 10000, 20, "Expense2");
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 2);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetSizeNull) {
	EXPECT_EQ(ExpenseMatrix_GetSize(nullptr), 0);
}

// Matrix IsEmpty Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_IsEmpty) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	EXPECT_EQ(ExpenseMatrix_IsEmpty(matrix), 1);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	EXPECT_EQ(ExpenseMatrix_IsEmpty(matrix), 0);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_IsEmptyNull) {
	EXPECT_EQ(ExpenseMatrix_IsEmpty(nullptr), -1);
}

// Category to Column Conversion Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_CategoryToColumn) {
	uint32_t col1 = ExpenseMatrix_CategoryToColumn("Materials", 10);
	uint32_t col2 = ExpenseMatrix_CategoryToColumn("Labor", 10);
	uint32_t col3 = ExpenseMatrix_CategoryToColumn("Materials", 10);
	
	EXPECT_EQ(col1, col3); // Same category should map to same column
	EXPECT_LT(col1, 10);
	EXPECT_LT(col2, 10);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_CategoryToColumnNull) {
	uint32_t col = ExpenseMatrix_CategoryToColumn(nullptr, 10);
	EXPECT_EQ(col, 0);
}

// File Operations Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_SaveToFile) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 2, "Labor", 10000, 20, "Expense2");
	
	int result = ExpenseMatrix_SaveToFile(matrix, "test_expenses.bin");
	EXPECT_EQ(result, 0);
	
	ExpenseMatrix_Destroy(matrix);
	
	// Clean up
	remove("test_expenses.bin");
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_SaveToFileNull) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	int result = ExpenseMatrix_SaveToFile(nullptr, "test.bin");
	EXPECT_EQ(result, -1);
	
	result = ExpenseMatrix_SaveToFile(matrix, nullptr);
	EXPECT_EQ(result, -1);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_LoadFromFile) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 2, "Labor", 10000, 20, "Expense2");
	
	ExpenseMatrix_SaveToFile(matrix, "test_expenses.bin");
	ExpenseMatrix_Destroy(matrix);
	
	// Load from file
	ExpenseMatrix* loaded = ExpenseMatrix_LoadFromFile(nullptr, "test_expenses.bin");
	EXPECT_NE(loaded, nullptr);
	EXPECT_EQ(ExpenseMatrix_GetSize(loaded), 2);
	
	Expense* expense = ExpenseMatrix_FindExpense(loaded, 1);
	EXPECT_NE(expense, nullptr);
	EXPECT_STREQ(expense->category, "Materials");
	
	ExpenseMatrix_Destroy(loaded);
	
	// Clean up
	remove("test_expenses.bin");
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_LoadFromFileNonExistent) {
	ExpenseMatrix* matrix = ExpenseMatrix_LoadFromFile(nullptr, "non_existent.bin");
	EXPECT_EQ(matrix, nullptr);
}

// Memory Allocation Failure Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_CreateMallocFailure) {
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_EQ(matrix, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_AddExpenseMallocFailure) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	uint32_t id = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Test");
	EXPECT_EQ(id, 0);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	ExpenseMatrix_Destroy(matrix);
}

// Sparse Matrix Structure Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_SparseStructure) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	// Add expenses to different rows and columns
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 5000, 10, "Expense1");
	ExpenseMatrix_AddExpense(matrix, 5, "Labor", 10000, 20, "Expense2");
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 3000, 30, "Expense3");
	
	// Verify sparse structure - only non-zero entries should exist
	EXPECT_EQ(ExpenseMatrix_GetSize(matrix), 3);
	
	// Verify row access
	Expense expenses[10];
	size_t count = ExpenseMatrix_GetExpensesByProject(matrix, 1, expenses, 10);
	EXPECT_EQ(count, 2);
	
	ExpenseMatrix_Destroy(matrix);
}

// View Expenses Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_ViewExpenses) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	ExpenseMatrix_AddExpense(matrix, 1, "Materials", 1000, 10, "Test expense");
	ExpenseMatrix_AddExpense(matrix, 2, "Labor", 2000, 20, "Another expense");
	
	// Should not crash
	ExpenseMatrix_ViewExpenses(matrix, 0); // View all
	ExpenseMatrix_ViewExpenses(matrix, 1); // View first
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_ViewExpensesNull) {
	// Should not crash
	ExpenseMatrix_ViewExpenses(nullptr, 0);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_ViewExpensesEmpty) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	// Should not crash
	ExpenseMatrix_ViewExpenses(matrix, 0);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_ViewExpensesLongDescription) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_NE(matrix, nullptr);
	
	// Add expense with long description (>30 chars)
	char long_desc[100];
	memset(long_desc, 'A', 99);
	long_desc[99] = '\0';
	
	uint32_t id = ExpenseMatrix_AddExpense(matrix, 1, "Materials", 1000, 10, long_desc);
	EXPECT_NE(id, 0);
	
	// Should not crash and should truncate description
	ExpenseMatrix_ViewExpenses(matrix, 0);
	
	ExpenseMatrix_Destroy(matrix);
}

// Wrap Around Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_AddExpenseWrapAround) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(5, 5);
	EXPECT_NE(matrix, nullptr);
	
	// Add expense with project_id > max_rows (should wrap around)
	uint32_t id = ExpenseMatrix_AddExpense(matrix, 10, "Materials", 1000, 10, "Wrap test");
	EXPECT_NE(id, 0);
	
	// Verify it was added (wrapped to row 0)
	Expense* expense = ExpenseMatrix_FindExpense(matrix, id);
	EXPECT_NE(expense, nullptr);
	
	ExpenseMatrix_Destroy(matrix);
}

TEST_F(ExpenseTrackingTest, ExpenseMatrix_GetExpensesByProjectWrapAround) {
	ExpenseMatrix* matrix = ExpenseMatrix_Create(5, 5);
	EXPECT_NE(matrix, nullptr);
	
	// Add expense with project_id > max_rows
	ExpenseMatrix_AddExpense(matrix, 10, "Materials", 1000, 10, "Wrap test");
	
	// Get expenses by project (should wrap around)
	Expense expenses[10];
	size_t count = ExpenseMatrix_GetExpensesByProject(matrix, 10, expenses, 10);
	EXPECT_GT(count, 0);
	
	ExpenseMatrix_Destroy(matrix);
}

// Additional Malloc Failure Tests

TEST_F(ExpenseTrackingTest, ExpenseMatrix_CreateRowHeadersMallocFailure) {
	mock_malloc_set_fail_count(2);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	ExpenseMatrix* matrix = ExpenseMatrix_Create(10, 10);
	EXPECT_EQ(matrix, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

