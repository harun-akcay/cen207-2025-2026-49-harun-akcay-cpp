/**
 * @file SalesTracker_test.cpp
 * @brief Unit tests for SalesTracker module (Heap + Heap Sort)
 * 
 * This file contains comprehensive unit tests for the SalesTracker module
 * using the Google Test framework. The tests cover heap operations, heap sort,
 * sales tracking, and error handling.
 */

#include "gtest/gtest.h"
#include "../../inventory_lib/header/SalesTracker.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

/**
 * @class SalesTrackerTest
 * @brief Test fixture for SalesTracker unit tests
 */
class SalesTrackerTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

// Heap Creation and Destruction Tests

TEST_F(SalesTrackerTest, SalesHeap_CreateDestroy) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	EXPECT_EQ(SalesHeap_GetSize(heap), 0);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_CreateWithZeroCapacity) {
	SalesHeap* heap = SalesHeap_Create(0);
	EXPECT_NE(heap, nullptr);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_DestroyNull) {
	// Should not crash
	SalesHeap_Destroy(nullptr);
}

// Heap Insertion Tests

TEST_F(SalesTrackerTest, SalesHeap_InsertSingle) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale record;
	record.id = 0;
	record.material_id = 1;
	record.project_id = 0;
	record.quantity = 5;
	record.unit_price_cents = 1000;
	record.total_amount_cents = 5000;
	record.unit_cost_cents = 500;
	record.profit_cents = 2500;
	record.sale_timestamp = 1;
	strncpy(record.customer_name, "Customer1", sizeof(record.customer_name) - 1);
	record.customer_name[sizeof(record.customer_name) - 1] = '\0';
	
	int result = SalesHeap_Insert(heap, &record);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(SalesHeap_GetSize(heap), 1);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_InsertMultiple) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale record1;
	record1.id = 0;
	record1.material_id = 1;
	record1.project_id = 0;
	record1.quantity = 5;
	record1.unit_price_cents = 1000;
	record1.total_amount_cents = 5000;
	record1.unit_cost_cents = 500;
	record1.profit_cents = 2500;
	record1.sale_timestamp = 1;
	strncpy(record1.customer_name, "Customer1", sizeof(record1.customer_name) - 1);
	record1.customer_name[sizeof(record1.customer_name) - 1] = '\0';
	
	Sale record2;
	record2.id = 0;
	record2.material_id = 2;
	record2.project_id = 0;
	record2.quantity = 3;
	record2.unit_price_cents = 1500;
	record2.total_amount_cents = 4500;
	record2.unit_cost_cents = 500;
	record2.profit_cents = 3000;
	record2.sale_timestamp = 2;
	strncpy(record2.customer_name, "Customer2", sizeof(record2.customer_name) - 1);
	record2.customer_name[sizeof(record2.customer_name) - 1] = '\0';
	
	EXPECT_EQ(SalesHeap_Insert(heap, &record1), 0);
	EXPECT_EQ(SalesHeap_Insert(heap, &record2), 0);
	EXPECT_EQ(SalesHeap_GetSize(heap), 2);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_InsertNull) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	int result = SalesHeap_Insert(heap, nullptr);
	EXPECT_EQ(result, -1);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_InsertToNullHeap) {
	Sale record;
	record.id = 0;
	record.material_id = 1;
	record.project_id = 0;
	record.quantity = 5;
	record.unit_price_cents = 1000;
	record.total_amount_cents = 5000;
	record.unit_cost_cents = 500;
	record.profit_cents = 2500;
	record.sale_timestamp = 1;
	strncpy(record.customer_name, "Customer1", sizeof(record.customer_name) - 1);
	record.customer_name[sizeof(record.customer_name) - 1] = '\0';
	
	int result = SalesHeap_Insert(nullptr, &record);
	EXPECT_EQ(result, -1);
}

// Heap Extract Max Tests

TEST_F(SalesTrackerTest, SalesHeap_ExtractMax) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale record1;
	record1.id = 0;
	record1.material_id = 1;
	record1.project_id = 0;
	record1.quantity = 5;
	record1.unit_price_cents = 1000;
	record1.total_amount_cents = 5000;
	record1.unit_cost_cents = 500;
	record1.profit_cents = 2500;
	record1.sale_timestamp = 1;
	strncpy(record1.customer_name, "Customer1", sizeof(record1.customer_name) - 1);
	record1.customer_name[sizeof(record1.customer_name) - 1] = '\0';
	
	Sale record2;
	record2.id = 0;
	record2.material_id = 2;
	record2.project_id = 0;
	record2.quantity = 3;
	record2.unit_price_cents = 1500;
	record2.total_amount_cents = 4500;
	record2.unit_cost_cents = 500;
	record2.profit_cents = 3000;
	record2.sale_timestamp = 2;
	strncpy(record2.customer_name, "Customer2", sizeof(record2.customer_name) - 1);
	record2.customer_name[sizeof(record2.customer_name) - 1] = '\0';
	
	EXPECT_EQ(SalesHeap_Insert(heap, &record1), 0);
	EXPECT_EQ(SalesHeap_Insert(heap, &record2), 0);
	
	Sale extracted;
	int result = SalesHeap_ExtractMax(heap, &extracted);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(SalesHeap_GetSize(heap), 1);
	EXPECT_EQ(extracted.profit_cents, 3000ULL); // Max profit
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_ExtractMaxFromEmpty) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale extracted;
	int result = SalesHeap_ExtractMax(heap, &extracted);
	EXPECT_EQ(result, -1);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_ExtractMaxNull) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale record;
	record.id = 0;
	record.material_id = 1;
	record.project_id = 0;
	record.quantity = 5;
	record.unit_price_cents = 1000;
	record.total_amount_cents = 5000;
	record.unit_cost_cents = 500;
	record.profit_cents = 2500;
	record.sale_timestamp = 1;
	strncpy(record.customer_name, "Customer1", sizeof(record.customer_name) - 1);
	record.customer_name[sizeof(record.customer_name) - 1] = '\0';
	
	EXPECT_EQ(SalesHeap_Insert(heap, &record), 0);
	
	int result = SalesHeap_ExtractMax(heap, nullptr);
	EXPECT_EQ(result, -1);
	
	SalesHeap_Destroy(heap);
}

// Heap Peek Max Tests

TEST_F(SalesTrackerTest, SalesHeap_PeekMax) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale record1;
	record1.id = 0;
	record1.material_id = 1;
	record1.project_id = 0;
	record1.quantity = 5;
	record1.unit_price_cents = 1000;
	record1.total_amount_cents = 5000;
	record1.unit_cost_cents = 500;
	record1.profit_cents = 2500;
	record1.sale_timestamp = 1;
	strncpy(record1.customer_name, "Customer1", sizeof(record1.customer_name) - 1);
	record1.customer_name[sizeof(record1.customer_name) - 1] = '\0';
	
	Sale record2;
	record2.id = 0;
	record2.material_id = 2;
	record2.project_id = 0;
	record2.quantity = 3;
	record2.unit_price_cents = 1500;
	record2.total_amount_cents = 4500;
	record2.unit_cost_cents = 500;
	record2.profit_cents = 3000;
	record2.sale_timestamp = 2;
	strncpy(record2.customer_name, "Customer2", sizeof(record2.customer_name) - 1);
	record2.customer_name[sizeof(record2.customer_name) - 1] = '\0';
	
	EXPECT_EQ(SalesHeap_Insert(heap, &record1), 0);
	EXPECT_EQ(SalesHeap_Insert(heap, &record2), 0);
	
	Sale peeked;
	int result = SalesHeap_PeekMax(heap, &peeked);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(SalesHeap_GetSize(heap), 2); // Size should not change
	EXPECT_EQ(peeked.profit_cents, 3000ULL); // Max profit
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_PeekMaxFromEmpty) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale peeked;
	int result = SalesHeap_PeekMax(heap, &peeked);
	EXPECT_EQ(result, -1);
	
	SalesHeap_Destroy(heap);
}

// Heap Sort Tests

TEST_F(SalesTrackerTest, SalesHeap_SortByProfitEmpty) {
	Sale sales[1];
	size_t count = 0;
	int result = SalesHeap_SortByProfit(sales, count);
	EXPECT_EQ(result, -1); // Empty array should fail
}

TEST_F(SalesTrackerTest, SalesHeap_SortByProfitSingle) {
	Sale record;
	record.id = 0;
	record.material_id = 1;
	record.project_id = 0;
	record.quantity = 5;
	record.unit_price_cents = 1000;
	record.total_amount_cents = 5000;
	record.unit_cost_cents = 500;
	record.profit_cents = 2500;
	record.sale_timestamp = 1;
	strncpy(record.customer_name, "Customer1", sizeof(record.customer_name) - 1);
	record.customer_name[sizeof(record.customer_name) - 1] = '\0';
	
	Sale sales[1] = { record };
	size_t count = 1;
	int result = SalesHeap_SortByProfit(sales, count);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(sales[0].profit_cents, 2500ULL);
}

TEST_F(SalesTrackerTest, SalesHeap_SortByProfitMultiple) {
	Sale records[5];
	for (int i = 0; i < 5; i++) {
		records[i].id = 0;
		records[i].material_id = i + 1;
		records[i].project_id = 0;
		records[i].quantity = i + 1;
		records[i].unit_price_cents = (i + 1) * 1000;
		records[i].total_amount_cents = (i + 1) * (i + 1) * 1000;
		records[i].unit_cost_cents = 500;
		records[i].profit_cents = (i + 1) * 500ULL; // Profits: 500, 1000, 1500, 2000, 2500
		records[i].sale_timestamp = i + 1;
		snprintf(records[i].customer_name, sizeof(records[i].customer_name), "Customer%d", i + 1);
	}
	
	size_t count = 5;
	int result = SalesHeap_SortByProfit(records, count);
	EXPECT_EQ(result, 0);
	
	// Verify sorted order (descending by profit)
	for (size_t i = 0; i < count - 1; i++) {
		EXPECT_GE(records[i].profit_cents, records[i + 1].profit_cents);
	}
}

TEST_F(SalesTrackerTest, SalesHeap_SortByProfitNull) {
	int result = SalesHeap_SortByProfit(nullptr, 5);
	EXPECT_EQ(result, -1);
}

// Heap IsEmpty Tests

TEST_F(SalesTrackerTest, SalesHeap_IsEmpty) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	EXPECT_EQ(SalesHeap_IsEmpty(heap), 1);
	
	Sale record;
	record.id = 0;
	record.material_id = 1;
	record.project_id = 0;
	record.quantity = 5;
	record.unit_price_cents = 1000;
	record.total_amount_cents = 5000;
	record.unit_cost_cents = 500;
	record.profit_cents = 2500;
	record.sale_timestamp = 1;
	strncpy(record.customer_name, "Customer1", sizeof(record.customer_name) - 1);
	record.customer_name[sizeof(record.customer_name) - 1] = '\0';
	
	EXPECT_EQ(SalesHeap_Insert(heap, &record), 0);
	EXPECT_EQ(SalesHeap_IsEmpty(heap), 0);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_IsEmptyNull) {
	EXPECT_EQ(SalesHeap_IsEmpty(nullptr), -1);
}

// Heap GetSize Tests

TEST_F(SalesTrackerTest, SalesHeap_GetSize) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	EXPECT_EQ(SalesHeap_GetSize(heap), 0);
	
	Sale record;
	record.id = 0;
	record.material_id = 1;
	record.project_id = 0;
	record.quantity = 5;
	record.unit_price_cents = 1000;
	record.total_amount_cents = 5000;
	record.unit_cost_cents = 500;
	record.profit_cents = 2500;
	record.sale_timestamp = 1;
	strncpy(record.customer_name, "Customer1", sizeof(record.customer_name) - 1);
	record.customer_name[sizeof(record.customer_name) - 1] = '\0';
	
	EXPECT_EQ(SalesHeap_Insert(heap, &record), 0);
	EXPECT_EQ(SalesHeap_GetSize(heap), 1);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_GetSizeNull) {
	EXPECT_EQ(SalesHeap_GetSize(nullptr), 0);
}

// Heap Property Tests (Max Heap)

TEST_F(SalesTrackerTest, SalesHeap_MaxHeapProperty) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	// Insert records with different profits
	Sale records[5];
	for (int i = 0; i < 5; i++) {
		records[i].id = 0;
		records[i].material_id = i + 1;
		records[i].project_id = 0;
		records[i].quantity = i + 1;
		records[i].unit_price_cents = (i + 1) * 1000;
		records[i].total_amount_cents = (i + 1) * (i + 1) * 1000;
		records[i].unit_cost_cents = 500;
		records[i].profit_cents = (i + 1) * 500ULL;
		records[i].sale_timestamp = i + 1;
		snprintf(records[i].customer_name, sizeof(records[i].customer_name), "Customer%d", i + 1);
		EXPECT_EQ(SalesHeap_Insert(heap, &records[i]), 0);
	}
	
	// Extract max repeatedly - should get descending order
	uint64_t prev_profit = 10000ULL; // Start with high value
	for (int i = 0; i < 5; i++) {
		Sale extracted;
		int result = SalesHeap_ExtractMax(heap, &extracted);
		EXPECT_EQ(result, 0);
		EXPECT_LE(extracted.profit_cents, prev_profit); // Current should be <= previous
		prev_profit = extracted.profit_cents;
	}
	
	EXPECT_EQ(SalesHeap_GetSize(heap), 0);
	
	SalesHeap_Destroy(heap);
}


// SalesTracker Wrapper Functions Tests

TEST_F(SalesTrackerTest, SalesTracker_CreateDestroy) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_RecordSale) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	uint32_t id = SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, "Customer1");
	EXPECT_NE(id, 0);
	EXPECT_EQ(SalesTracker_GetSalesCount(tracker), 1);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_RecordSaleNull) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	uint32_t id = SalesTracker_RecordSale(nullptr, 1, 0, 5, 1000, 500, "Customer1");
	EXPECT_EQ(id, 0);
	
	id = SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, nullptr);
	EXPECT_EQ(id, 0);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_ViewSales) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, "Customer1");
	SalesTracker_RecordSale(tracker, 2, 0, 3, 2000, 1000, "Customer2");
	
	// ViewSales should not crash
	SalesTracker_ViewSales(tracker, 0);
	SalesTracker_ViewSales(tracker, 1);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_ViewSalesEmpty) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	// ViewSales empty tracker should not crash
	SalesTracker_ViewSales(tracker, 0);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_ViewSalesNull) {
	// ViewSales null tracker should not crash
	SalesTracker_ViewSales(nullptr, 0);
}

TEST_F(SalesTrackerTest, SalesTracker_CalculateTotalProfit) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, "Customer1"); // Profit: 2500
	SalesTracker_RecordSale(tracker, 2, 0, 3, 2000, 1000, "Customer2"); // Profit: 3000
	
	uint64_t total = SalesTracker_CalculateTotalProfit(tracker);
	EXPECT_EQ(total, 5500ULL);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_CalculateTotalProfitNull) {
	uint64_t total = SalesTracker_CalculateTotalProfit(nullptr);
	EXPECT_EQ(total, 0ULL);
}

TEST_F(SalesTrackerTest, SalesTracker_GetSalesCount) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	EXPECT_EQ(SalesTracker_GetSalesCount(tracker), 0);
	
	SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, "Customer1");
	EXPECT_EQ(SalesTracker_GetSalesCount(tracker), 1);
	
	SalesTracker_RecordSale(tracker, 2, 0, 3, 2000, 1000, "Customer2");
	EXPECT_EQ(SalesTracker_GetSalesCount(tracker), 2);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_GetSalesCountNull) {
	EXPECT_EQ(SalesTracker_GetSalesCount(nullptr), 0);
}

// SalesTracker File Operations Tests

TEST_F(SalesTrackerTest, SalesTracker_SaveToFile) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, "Customer1");
	SalesTracker_RecordSale(tracker, 2, 0, 3, 2000, 1000, "Customer2");
	
	int result = SalesTracker_SaveToFile(tracker, "test_sales.bin");
	EXPECT_EQ(result, 0);
	
	SalesTracker_Destroy(tracker);
	
	// Clean up
	remove("test_sales.bin");
}

TEST_F(SalesTrackerTest, SalesTracker_SaveToFileNull) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	int result = SalesTracker_SaveToFile(nullptr, "test.bin");
	EXPECT_EQ(result, -1);
	
	result = SalesTracker_SaveToFile(tracker, nullptr);
	EXPECT_EQ(result, -1);
	
	SalesTracker_Destroy(tracker);
}

TEST_F(SalesTrackerTest, SalesTracker_LoadFromFile) {
	SalesTracker* tracker = SalesTracker_Create();
	EXPECT_NE(tracker, nullptr);
	
	SalesTracker_RecordSale(tracker, 1, 0, 5, 1000, 500, "Customer1");
	SalesTracker_RecordSale(tracker, 2, 0, 3, 2000, 1000, "Customer2");
	
	SalesTracker_SaveToFile(tracker, "test_sales.bin");
	SalesTracker_Destroy(tracker);
	
	// Load from file
	SalesTracker* loaded = SalesTracker_LoadFromFile(nullptr, "test_sales.bin");
	EXPECT_NE(loaded, nullptr);
	EXPECT_EQ(SalesTracker_GetSalesCount(loaded), 2);
	
	SalesTracker_Destroy(loaded);
	
	// Clean up
	remove("test_sales.bin");
}

TEST_F(SalesTrackerTest, SalesTracker_LoadFromFileNonExistent) {
	SalesTracker* tracker = SalesTracker_LoadFromFile(nullptr, "non_existent.bin");
	EXPECT_EQ(tracker, nullptr);
}

TEST_F(SalesTrackerTest, SalesTracker_LoadFromFileNull) {
	SalesTracker* tracker = SalesTracker_LoadFromFile(nullptr, nullptr);
	EXPECT_EQ(tracker, nullptr);
}

// SalesHeap GetAllSales Tests

TEST_F(SalesTrackerTest, SalesHeap_GetAllSales) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale records[3];
	for (int i = 0; i < 3; i++) {
		records[i].id = 0;
		records[i].material_id = i + 1;
		records[i].project_id = 0;
		records[i].quantity = i + 1;
		records[i].unit_price_cents = (i + 1) * 1000;
		records[i].total_amount_cents = (i + 1) * (i + 1) * 1000;
		records[i].unit_cost_cents = 500;
		records[i].profit_cents = (i + 1) * 500ULL;
		records[i].sale_timestamp = i + 1;
		snprintf(records[i].customer_name, sizeof(records[i].customer_name), "Customer%d", i + 1);
		EXPECT_EQ(SalesHeap_Insert(heap, &records[i]), 0);
	}
	
	Sale all_sales[10];
	size_t count = SalesHeap_GetAllSales(heap, all_sales, 10);
	EXPECT_EQ(count, 3);
	
	SalesHeap_Destroy(heap);
}

TEST_F(SalesTrackerTest, SalesHeap_GetAllSalesNull) {
	SalesHeap* heap = SalesHeap_Create(10);
	EXPECT_NE(heap, nullptr);
	
	Sale all_sales[10];
	size_t count = SalesHeap_GetAllSales(nullptr, all_sales, 10);
	EXPECT_EQ(count, 0);
	
	count = SalesHeap_GetAllSales(heap, nullptr, 10);
	EXPECT_EQ(count, 0);
	
	SalesHeap_Destroy(heap);
}
