/**
 * @file MaterialInventory_test.cpp
 * @brief Unit tests for MaterialInventory module (Double Linked List)
 * 
 * This file contains comprehensive unit tests for the MaterialInventory module
 * using the Google Test framework. The tests cover double linked list operations,
 * material management, and error handling.
 */

#include "gtest/gtest.h"
#include "../../inventory_lib/header/MaterialInventory.h"
#include "../../inventory_lib/header/InventoryManager.h"
#include "mock_malloc.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

/**
 * @class MaterialInventoryTest
 * @brief Test fixture for MaterialInventory unit tests
 */
class MaterialInventoryTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

// Material List Creation and Destruction Tests

TEST_F(MaterialInventoryTest, MaterialList_CreateDestroy) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	EXPECT_EQ(MaterialList_GetSize(list), 0);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_DestroyNull) {
	MaterialList_Destroy(nullptr); // Should not crash
}

// Material Addition Tests

TEST_F(MaterialInventoryTest, MaterialList_AddMaterialSingle) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_NE(id, 0);
	EXPECT_EQ(MaterialList_GetSize(list), 1);
	
	Material* material = MaterialList_FindMaterial(list, id);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Wood");
	EXPECT_STREQ(material->category, "Raw Material");
	EXPECT_EQ(material->quantity, 100);
	EXPECT_EQ(material->unit_price_cents, 5000);
	EXPECT_STREQ(material->supplier, "Supplier1");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_AddMaterialMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id1 = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	uint32_t id2 = MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	uint32_t id3 = MaterialList_AddMaterial(list, "Plastic", "Raw Material", 200, 3000, "Supplier3");
	
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_NE(id3, 0);
	EXPECT_NE(id1, id2);
	EXPECT_NE(id2, id3);
	EXPECT_EQ(MaterialList_GetSize(list), 3);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_AddMaterialNullParams) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(nullptr, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_EQ(id, 0);
	
	id = MaterialList_AddMaterial(list, nullptr, "Raw Material", 100, 5000, "Supplier1");
	EXPECT_EQ(id, 0);
	
	id = MaterialList_AddMaterial(list, "Wood", nullptr, 100, 5000, "Supplier1");
	EXPECT_EQ(id, 0);
	
	id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, nullptr);
	EXPECT_EQ(id, 0);
	
	MaterialList_Destroy(list);
}

// Material Find Tests

TEST_F(MaterialInventoryTest, MaterialList_FindMaterial) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_NE(id, 0);
	
	Material* material = MaterialList_FindMaterial(list, id);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Wood");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	// Add multiple materials to trigger while loop
	uint32_t ids[5];
	for (int i = 0; i < 5; i++) {
		char name[32], category[32], supplier[32];
		snprintf(name, sizeof(name), "Material%d", i + 1);
		snprintf(category, sizeof(category), "Category%d", i + 1);
		snprintf(supplier, sizeof(supplier), "Supplier%d", i + 1);
		ids[i] = MaterialList_AddMaterial(list, name, category, 100 + i, 5000 + i * 100, supplier);
		EXPECT_NE(ids[i], 0);
	}
	
	// Find materials in the middle and end to trigger while loop
	Material* material = MaterialList_FindMaterial(list, ids[2]);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Material3");
	
	material = MaterialList_FindMaterial(list, ids[4]);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Material5");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialNonExistent) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	Material* material = MaterialList_FindMaterial(list, 999);
	EXPECT_EQ(material, nullptr);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialNull) {
	Material* material = MaterialList_FindMaterial(nullptr, 1);
	EXPECT_EQ(material, nullptr);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialByName) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_NE(id, 0);
	
	Material* material = MaterialList_FindMaterialByName(list, "Wood");
	EXPECT_NE(material, nullptr);
	EXPECT_EQ(material->id, id);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialByNameMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	// Add multiple materials to trigger while loop
	for (int i = 0; i < 5; i++) {
		char name[32], category[32], supplier[32];
		snprintf(name, sizeof(name), "Material%d", i + 1);
		snprintf(category, sizeof(category), "Category%d", i + 1);
		snprintf(supplier, sizeof(supplier), "Supplier%d", i + 1);
		MaterialList_AddMaterial(list, name, category, 100 + i, 5000 + i * 100, supplier);
	}
	
	// Find materials in the middle and end to trigger while loop
	Material* material = MaterialList_FindMaterialByName(list, "Material3");
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Material3");
	
	material = MaterialList_FindMaterialByName(list, "Material5");
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Material5");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialByNameNonExistent) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	Material* material = MaterialList_FindMaterialByName(list, "NonExistent");
	EXPECT_EQ(material, nullptr);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_FindMaterialByNameNull) {
	Material* material = MaterialList_FindMaterialByName(nullptr, "Wood");
	EXPECT_EQ(material, nullptr);
	
	MaterialList* list = MaterialList_Create();
	material = MaterialList_FindMaterialByName(list, nullptr);
	EXPECT_EQ(material, nullptr);
	MaterialList_Destroy(list);
}

// Material Update Tests

TEST_F(MaterialInventoryTest, MaterialList_UpdateMaterial) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_NE(id, 0);
	
	int result = MaterialList_UpdateMaterial(list, id, "Updated Wood", "Updated Category", 200, 6000, "Updated Supplier");
	EXPECT_EQ(result, 0);
	
	Material* material = MaterialList_FindMaterial(list, id);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Updated Wood");
	EXPECT_STREQ(material->category, "Updated Category");
	EXPECT_EQ(material->quantity, 200);
	EXPECT_EQ(material->unit_price_cents, 6000);
	EXPECT_STREQ(material->supplier, "Updated Supplier");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_UpdateMaterialPartial) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_NE(id, 0);
	
	// Update only quantity
	int result = MaterialList_UpdateMaterial(list, id, nullptr, nullptr, 200, UINT32_MAX, nullptr);
	EXPECT_EQ(result, 0);
	
	Material* material = MaterialList_FindMaterial(list, id);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Wood"); // Unchanged
	EXPECT_STREQ(material->category, "Raw Material"); // Unchanged
	EXPECT_EQ(material->quantity, 200); // Updated
	EXPECT_EQ(material->unit_price_cents, 5000); // Unchanged
	EXPECT_STREQ(material->supplier, "Supplier1"); // Unchanged
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_UpdateMaterialNonExistent) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	int result = MaterialList_UpdateMaterial(list, 999, "Updated", "Category", 200, 6000, "Supplier");
	EXPECT_EQ(result, -1);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_UpdateMaterialMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	// Add multiple materials to trigger while loop
	uint32_t ids[5];
	for (int i = 0; i < 5; i++) {
		char name[32], category[32], supplier[32];
		snprintf(name, sizeof(name), "Material%d", i + 1);
		snprintf(category, sizeof(category), "Category%d", i + 1);
		snprintf(supplier, sizeof(supplier), "Supplier%d", i + 1);
		ids[i] = MaterialList_AddMaterial(list, name, category, 100 + i, 5000 + i * 100, supplier);
		EXPECT_NE(ids[i], 0);
	}
	
	// Update materials in the middle and end to trigger while loop
	int result = MaterialList_UpdateMaterial(list, ids[2], "Updated3", "Category3", 300, 6000, "Supplier3");
	EXPECT_EQ(result, 0);
	
	Material* material = MaterialList_FindMaterial(list, ids[2]);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Updated3");
	
	result = MaterialList_UpdateMaterial(list, ids[4], "Updated5", "Category5", 500, 7000, "Supplier5");
	EXPECT_EQ(result, 0);
	
	material = MaterialList_FindMaterial(list, ids[4]);
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->name, "Updated5");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_UpdateMaterialNull) {
	int result = MaterialList_UpdateMaterial(nullptr, 1, "Updated", "Category", 200, 6000, "Supplier");
	EXPECT_EQ(result, -1);
}

// Material Remove Tests

TEST_F(MaterialInventoryTest, MaterialList_RemoveMaterial) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_NE(id, 0);
	EXPECT_EQ(MaterialList_GetSize(list), 1);
	
	int result = MaterialList_RemoveMaterial(list, id);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(MaterialList_GetSize(list), 0);
	
	Material* material = MaterialList_FindMaterial(list, id);
	EXPECT_EQ(material, nullptr);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_RemoveMaterialMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id1 = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	uint32_t id2 = MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	uint32_t id3 = MaterialList_AddMaterial(list, "Plastic", "Raw Material", 200, 3000, "Supplier3");
	
	EXPECT_EQ(MaterialList_GetSize(list), 3);
	
	// Remove middle
	int result = MaterialList_RemoveMaterial(list, id2);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(MaterialList_GetSize(list), 2);
	
	// Remove first
	result = MaterialList_RemoveMaterial(list, id1);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(MaterialList_GetSize(list), 1);
	
	// Remove last
	result = MaterialList_RemoveMaterial(list, id3);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(MaterialList_GetSize(list), 0);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_RemoveMaterialNonExistent) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	int result = MaterialList_RemoveMaterial(list, 999);
	EXPECT_EQ(result, -1);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_RemoveMaterialNull) {
	int result = MaterialList_RemoveMaterial(nullptr, 1);
	EXPECT_EQ(result, -1);
}

// Material List Size Tests

TEST_F(MaterialInventoryTest, MaterialList_GetSize) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	EXPECT_EQ(MaterialList_GetSize(list), 0);
	
	MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_EQ(MaterialList_GetSize(list), 1);
	
	MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	EXPECT_EQ(MaterialList_GetSize(list), 2);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_GetSizeNull) {
	EXPECT_EQ(MaterialList_GetSize(nullptr), 0);
}

// Double Linked List Traversal Tests

TEST_F(MaterialInventoryTest, MaterialList_TraverseForward) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id1 = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	uint32_t id2 = MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	uint32_t id3 = MaterialList_AddMaterial(list, "Plastic", "Raw Material", 200, 3000, "Supplier3");
	
	// Traverse forward
	MaterialNode* current = list->head;
	EXPECT_NE(current, nullptr);
	EXPECT_EQ(current->material.id, id1);
	
	current = current->next;
	EXPECT_NE(current, nullptr);
	EXPECT_EQ(current->material.id, id2);
	
	current = current->next;
	EXPECT_NE(current, nullptr);
	EXPECT_EQ(current->material.id, id3);
	
	EXPECT_EQ(current->next, nullptr); // Last node
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_TraverseBackward) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	uint32_t id1 = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	uint32_t id2 = MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	uint32_t id3 = MaterialList_AddMaterial(list, "Plastic", "Raw Material", 200, 3000, "Supplier3");
	
	// Traverse backward
	MaterialNode* current = list->tail;
	EXPECT_NE(current, nullptr);
	EXPECT_EQ(current->material.id, id3);
	
	current = current->prev;
	EXPECT_NE(current, nullptr);
	EXPECT_EQ(current->material.id, id2);
	
	current = current->prev;
	EXPECT_NE(current, nullptr);
	EXPECT_EQ(current->material.id, id1);
	
	EXPECT_EQ(current->prev, nullptr); // First node
	
	MaterialList_Destroy(list);
}

// File Operations Tests

TEST_F(MaterialInventoryTest, MaterialList_SaveToFile) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	
	int result = MaterialList_SaveToFile(list, "test_materials.bin");
	EXPECT_EQ(result, 0);
	
	MaterialList_Destroy(list);
	
	// Clean up
	remove("test_materials.bin");
}

TEST_F(MaterialInventoryTest, MaterialList_SaveToFileNull) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	int result = MaterialList_SaveToFile(nullptr, "test.bin");
	EXPECT_EQ(result, -1);
	
	result = MaterialList_SaveToFile(list, nullptr);
	EXPECT_EQ(result, -1);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_LoadFromFile) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	MaterialList_AddMaterial(list, "Metal", "Raw Material", 50, 10000, "Supplier2");
	
	MaterialList_SaveToFile(list, "test_materials.bin");
	MaterialList_Destroy(list);
	
	// Load from file
	MaterialList* loaded = MaterialList_LoadFromFile(nullptr, "test_materials.bin");
	EXPECT_NE(loaded, nullptr);
	EXPECT_EQ(MaterialList_GetSize(loaded), 2);
	
	Material* material = MaterialList_FindMaterialByName(loaded, "Wood");
	EXPECT_NE(material, nullptr);
	EXPECT_STREQ(material->category, "Raw Material");
	
	MaterialList_Destroy(loaded);
	
	// Clean up
	remove("test_materials.bin");
}

TEST_F(MaterialInventoryTest, MaterialList_LoadFromFileNonExistent) {
	MaterialList* list = MaterialList_LoadFromFile(nullptr, "non_existent.bin");
	EXPECT_EQ(list, nullptr);
}

// Memory Allocation Failure Tests

TEST_F(MaterialInventoryTest, MaterialList_CreateMallocFailure) {
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	MaterialList* list = MaterialList_Create();
	EXPECT_EQ(list, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

// GetFirst/GetNext Tests

TEST_F(MaterialInventoryTest, MaterialList_GetFirst) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Material1", "Category1", 10, 1000, "Supplier1");
	
	MaterialNode* first = MaterialList_GetFirst(list);
	EXPECT_NE(first, nullptr);
	EXPECT_STREQ(first->material.name, "Material1");
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_GetFirstNull) {
	MaterialNode* first = MaterialList_GetFirst(nullptr);
	EXPECT_EQ(first, nullptr);
}

TEST_F(MaterialInventoryTest, MaterialList_GetFirstEmpty) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialNode* first = MaterialList_GetFirst(list);
	EXPECT_EQ(first, nullptr);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_GetNext) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Material1", "Category1", 10, 1000, "Supplier1");
	MaterialList_AddMaterial(list, "Material2", "Category2", 20, 2000, "Supplier2");
	
	MaterialNode* first = MaterialList_GetFirst(list);
	EXPECT_NE(first, nullptr);
	
	MaterialNode* next = MaterialList_GetNext(first);
	EXPECT_NE(next, nullptr);
	EXPECT_STREQ(next->material.name, "Material2");
	
	MaterialNode* next2 = MaterialList_GetNext(next);
	EXPECT_EQ(next2, nullptr);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_GetNextNull) {
	MaterialNode* next = MaterialList_GetNext(nullptr);
	EXPECT_EQ(next, nullptr);
}

// Traverse Tests (to cover edge cases)

TEST_F(MaterialInventoryTest, MaterialList_TraverseForwardMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Material1", "Category1", 10, 1000, "Supplier1");
	MaterialList_AddMaterial(list, "Material2", "Category2", 20, 2000, "Supplier2");
	MaterialList_AddMaterial(list, "Material3", "Category3", 30, 3000, "Supplier3");
	
	Material material;
	size_t count = 0;
	MaterialNode* current = MaterialList_GetFirst(list);
	while (current != NULL) {
		material = current->material;
		count++;
		current = MaterialList_GetNext(current);
	}
	
	EXPECT_EQ(count, 3);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_TraverseBackwardMultiple) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Material1", "Category1", 10, 1000, "Supplier1");
	MaterialList_AddMaterial(list, "Material2", "Category2", 20, 2000, "Supplier2");
	MaterialList_AddMaterial(list, "Material3", "Category3", 30, 3000, "Supplier3");
	
	// Get last node
	MaterialNode* current = MaterialList_GetFirst(list);
	while (current != NULL && MaterialList_GetNext(current) != NULL) {
		current = MaterialList_GetNext(current);
	}
	
	EXPECT_NE(current, nullptr);
	EXPECT_STREQ(current->material.name, "Material3");
	
	MaterialList_Destroy(list);
}

// SaveToFile Error Cases

TEST_F(MaterialInventoryTest, MaterialList_SaveToFileErrorCases) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	MaterialList_AddMaterial(list, "Material1", "Category1", 10, 1000, "Supplier1");
	
	// Test NULL parameters
	int result = MaterialList_SaveToFile(nullptr, "test.bin");
	EXPECT_EQ(result, -1);
	
	result = MaterialList_SaveToFile(list, nullptr);
	EXPECT_EQ(result, -1);
	
	MaterialList_Destroy(list);
}

TEST_F(MaterialInventoryTest, MaterialList_AddMaterialMallocFailure) {
	MaterialList* list = MaterialList_Create();
	EXPECT_NE(list, nullptr);
	
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	uint32_t id = MaterialList_AddMaterial(list, "Wood", "Raw Material", 100, 5000, "Supplier1");
	EXPECT_EQ(id, 0);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	MaterialList_Destroy(list);
}

