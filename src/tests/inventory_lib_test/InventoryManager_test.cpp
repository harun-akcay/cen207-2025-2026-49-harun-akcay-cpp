/**
 * @file InventoryManager_test.cpp
 * @brief Unit tests for InventoryManager module
 * 
 * This file contains comprehensive unit tests for the InventoryManager module
 * using the Google Test framework. The tests cover initialization, cleanup,
 * hash table operations, user management, authentication, and error handling.
 * 
 * @details The test suite includes:
 * - Basic initialization and cleanup tests
 * - Hash table creation and destruction tests
 * - User addition, removal, and lookup tests
 * - Authentication tests
 * - Collision handling tests
 * - Error path tests (NULL pointer checks, malloc failures)
 * - Edge case tests
 * 
 * @note All tests use the InventoryManagerTest fixture class which provides
 * setup and teardown functionality to ensure clean test state.
 */

#include "gtest/gtest.h"
#include "../../inventory_lib/header/InventoryManager.h"
#include "mock_malloc.h"
#include <cstring>

/**
 * @class InventoryManagerTest
 * @brief Test fixture for InventoryManager unit tests
 * 
 * This class provides a test fixture that sets up and tears down the
 * test environment for each test case. It ensures that the inventory
 * manager is properly initialized and cleaned up between tests.
 * 
 * @details The fixture provides:
 * - SetUp() method for test initialization (currently empty but can be
 *   extended for common setup operations)
 * - TearDown() method for test cleanup (currently empty but can be
 *   extended for common cleanup operations)
 * 
 * @note All test cases using this fixture should ensure proper cleanup
 * of any resources they allocate during the test.
 */
class InventoryManagerTest : public ::testing::Test {
protected:
	/**
	 * @brief Set up test environment before each test
	 * 
	 * This method is called before each test case runs. Currently empty
	 * but can be extended to perform common setup operations such as
	 * initializing test data or resetting global state.
	 */
	void SetUp() override {
		// Setup test data
	}

	/**
	 * @brief Clean up test environment after each test
	 * 
	 * This method is called after each test case completes. Currently empty
	 * but can be extended to perform common cleanup operations such as
	 * freeing test data or resetting global state.
	 */
	void TearDown() override {
		// Clean up test data
	}
};

TEST_F(InventoryManagerTest, TestInit) {
	int result = InventoryManager_Init(NULL);
	EXPECT_EQ(result, 0);
	
	// Cleanup after test
	InventoryManager_Cleanup(NULL);
}

TEST_F(InventoryManagerTest, TestCleanup) {
	// Initialize first
	int init_result = InventoryManager_Init(NULL);
	EXPECT_EQ(init_result, 0);
	
	// Then cleanup
	int cleanup_result = InventoryManager_Cleanup(NULL);
	EXPECT_EQ(cleanup_result, 0);
}

TEST_F(InventoryManagerTest, TestInitCleanupSequence) {
	// Test that we can initialize and cleanup multiple times
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(InventoryManager_Init(NULL), 0);
		EXPECT_EQ(InventoryManager_Cleanup(NULL), 0);
	}
}

// Hash Table Tests

TEST_F(InventoryManagerTest, HashTable_CreateDestroy) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	EXPECT_EQ(HashTable_GetSize(ht), 0);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_AddUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add a user
	int result = HashTable_AddUser(ht, "testuser", "password123");
	EXPECT_EQ(result, 0);
	EXPECT_EQ(HashTable_GetSize(ht), 1);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_AddMultipleUsers) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add multiple users
	EXPECT_EQ(HashTable_AddUser(ht, "user1", "pass1"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user2", "pass2"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user3", "pass3"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 3);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_DuplicateUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add a user
	EXPECT_EQ(HashTable_AddUser(ht, "testuser", "password123"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 1);
	
	// Try to add the same user again
	int result = HashTable_AddUser(ht, "testuser", "differentpass");
	EXPECT_EQ(result, -1); // Should fail
	EXPECT_EQ(HashTable_GetSize(ht), 1); // Size should not change
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_FindUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add a user
	EXPECT_EQ(HashTable_AddUser(ht, "testuser", "password123"), 0);
	
	// Find the user
	User* user = HashTable_FindUser(ht, "testuser");
	EXPECT_NE(user, nullptr);
	EXPECT_STREQ(user->username, "testuser");
	EXPECT_EQ(user->is_active, 1);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_FindNonExistentUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Try to find a user that doesn't exist
	User* user = HashTable_FindUser(ht, "nonexistent");
	EXPECT_EQ(user, nullptr);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_Authenticate) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add a user
	EXPECT_EQ(HashTable_AddUser(ht, "testuser", "password123"), 0);
	
	// Authenticate with correct password
	int result = HashTable_Authenticate(ht, "testuser", "password123");
	EXPECT_EQ(result, 1);
	
	// Authenticate with wrong password
	result = HashTable_Authenticate(ht, "testuser", "wrongpassword");
	EXPECT_EQ(result, 0);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_AuthenticateNonExistentUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Try to authenticate a non-existent user
	int result = HashTable_Authenticate(ht, "nonexistent", "password");
	EXPECT_EQ(result, 0);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_RemoveUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add a user
	EXPECT_EQ(HashTable_AddUser(ht, "testuser", "password123"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 1);
	
	// Remove the user
	int result = HashTable_RemoveUser(ht, "testuser");
	EXPECT_EQ(result, 0);
	EXPECT_EQ(HashTable_GetSize(ht), 0);
	
	// Try to find the removed user
	User* user = HashTable_FindUser(ht, "testuser");
	EXPECT_EQ(user, nullptr);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_RemoveNonExistentUser) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Try to remove a non-existent user
	int result = HashTable_RemoveUser(ht, "nonexistent");
	EXPECT_EQ(result, -1);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_CollisionHandling) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add many users to guarantee collisions (HASH_TABLE_SIZE = 101)
	// Adding 150 users guarantees at least some collisions
	for (int i = 0; i < 150; ++i) {
		char username[32];
		char password[32];
		std::snprintf(username, sizeof(username), "user%d", i);
		std::snprintf(password, sizeof(password), "pass%d", i);
		EXPECT_EQ(HashTable_AddUser(ht, username, password), 0);
	}
	
	EXPECT_EQ(HashTable_GetSize(ht), 150);
	
	// Verify users can be found (covers collision chain traversal - InventoryManager.c 259-260)
	for (int i = 0; i < 150; ++i) {
		char username[32];
		std::snprintf(username, sizeof(username), "user%d", i);
		EXPECT_NE(HashTable_FindUser(ht, username), nullptr);
	}
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_ComplexScenario) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add users
	EXPECT_EQ(HashTable_AddUser(ht, "alice", "alice123"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "bob", "bob456"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "charlie", "charlie789"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 3);
	
	// Authenticate all users
	EXPECT_EQ(HashTable_Authenticate(ht, "alice", "alice123"), 1);
	EXPECT_EQ(HashTable_Authenticate(ht, "bob", "bob456"), 1);
	EXPECT_EQ(HashTable_Authenticate(ht, "charlie", "charlie789"), 1);
	
	// Remove one user
	EXPECT_EQ(HashTable_RemoveUser(ht, "bob"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 2);
	
	// Verify bob is removed
	EXPECT_EQ(HashTable_Authenticate(ht, "bob", "bob456"), 0);
	
	// Verify other users still work
	EXPECT_EQ(HashTable_Authenticate(ht, "alice", "alice123"), 1);
	EXPECT_EQ(HashTable_Authenticate(ht, "charlie", "charlie789"), 1);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_RemoveMiddleNodeFromChain) {
	// Test removal of middle node in collision chain (covers InventoryManager.c 326, 334-336)
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add many users to guarantee collisions
	for (int i = 0; i < 150; ++i) {
		char username[32];
		char password[32];
		std::snprintf(username, sizeof(username), "user%d", i);
		std::snprintf(password, sizeof(password), "pass%d", i);
		EXPECT_EQ(HashTable_AddUser(ht, username, password), 0);
	}
	
	EXPECT_EQ(HashTable_GetSize(ht), 150);
	
	// Remove a user from the middle (will traverse chain - covers 334-336)
	// Remove user50 which should be in a collision chain
	EXPECT_EQ(HashTable_RemoveUser(ht, "user50"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 149);
	
	// Verify removed user is gone
	EXPECT_EQ(HashTable_FindUser(ht, "user50"), nullptr);
	
	// Remove more users to test middle node removal (covers 326)
	EXPECT_EQ(HashTable_RemoveUser(ht, "user75"), 0);
	EXPECT_EQ(HashTable_RemoveUser(ht, "user100"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 147);
	
	// Verify they are gone
	EXPECT_EQ(HashTable_FindUser(ht, "user75"), nullptr);
	EXPECT_EQ(HashTable_FindUser(ht, "user100"), nullptr);
	
	// Verify other users still exist
	EXPECT_NE(HashTable_FindUser(ht, "user0"), nullptr);
	EXPECT_NE(HashTable_FindUser(ht, "user149"), nullptr);
	
	HashTable_Destroy(ht);
}

/**
 * @brief The main function of the test program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program.
 */

TEST_F(InventoryManagerTest, MockMalloc_HookNormalMallocPath) {
	// Test normal malloc path when hook is set but should not fail (covers mock_malloc.c 73)
	mock_malloc_reset();
	mock_malloc_set_fail(0);  // Don't fail
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	// Create hash table - should use mock_malloc_hook but call normal malloc (line 73)
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add users - should use normal malloc through hook
	EXPECT_EQ(HashTable_AddUser(ht, "user1", "pass1"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user2", "pass2"), 0);
	
	HashTable_Destroy(ht);
	
	InventoryManager_SetMallocHook(NULL);
	mock_malloc_reset();
}

TEST_F(InventoryManagerTest, MockMalloc_SetFailCount) {
	// Test mock_malloc_set_fail_count (covers mock_malloc.c 34-36)
	mock_malloc_reset();
	mock_malloc_set_fail(1);
	mock_malloc_set_fail_count(2);  // Fail on 2nd call
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	// First call should succeed
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Second call should fail (covers mock_malloc.c 69 - return NULL)
	int result = HashTable_AddUser(ht, "user1", "pass1");
	EXPECT_EQ(result, -1);  // Should fail due to malloc failure
	
	HashTable_Destroy(ht);
	
	InventoryManager_SetMallocHook(NULL);
	mock_malloc_reset();
}

TEST_F(InventoryManagerTest, MockMalloc_GetCallCount) {
	// Test mock_malloc_get_call_count (covers mock_malloc.c 51-53)
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	int initial_count = mock_malloc_get_call_count();
	
	// Create hash table - should increment call count
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	int after_create = mock_malloc_get_call_count();
	EXPECT_GT(after_create, initial_count);
	
	// Add user - should increment call count more
	HashTable_AddUser(ht, "user1", "pass1");
	int after_add = mock_malloc_get_call_count();
	EXPECT_GT(after_add, after_create);
	
	HashTable_Destroy(ht);
	
	InventoryManager_SetMallocHook(NULL);
	mock_malloc_reset();
}

// NULL pointer tests for error paths
TEST_F(InventoryManagerTest, HashTable_DestroyNULL) {
	// Test HashTable_Destroy with NULL (covers InventoryManager.c 163)
	HashTable_Destroy(NULL);
	// Should not crash
}

TEST_F(InventoryManagerTest, HashTable_AddUserNULLParams) {
	// Test HashTable_AddUser with NULL parameters (covers InventoryManager.c 195)
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// NULL username
	EXPECT_EQ(HashTable_AddUser(ht, NULL, "pass"), -1);
	
	// NULL password
	EXPECT_EQ(HashTable_AddUser(ht, "user", NULL), -1);
	
	// NULL hash table
	EXPECT_EQ(HashTable_AddUser(NULL, "user", "pass"), -1);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_FindUserNULLParams) {
	// Test HashTable_FindUser with NULL parameters (covers InventoryManager.c 247)
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// NULL username
	EXPECT_EQ(HashTable_FindUser(ht, NULL), nullptr);
	
	// NULL hash table
	EXPECT_EQ(HashTable_FindUser(NULL, "user"), nullptr);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_AuthenticateNULLParams) {
	// Test HashTable_Authenticate with NULL parameters (covers InventoryManager.c 278)
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// NULL username
	EXPECT_EQ(HashTable_Authenticate(ht, NULL, "pass"), 0);
	
	// NULL password
	EXPECT_EQ(HashTable_Authenticate(ht, "user", NULL), 0);
	
	// NULL hash table
	EXPECT_EQ(HashTable_Authenticate(NULL, "user", "pass"), 0);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_RemoveUserNULLParams) {
	// Test HashTable_RemoveUser with NULL parameters (covers InventoryManager.c 308)
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// NULL username
	EXPECT_EQ(HashTable_RemoveUser(ht, NULL), -1);
	
	// NULL hash table
	EXPECT_EQ(HashTable_RemoveUser(NULL, "user"), -1);
	
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_GetSizeNULL) {
	// Test HashTable_GetSize with NULL (covers InventoryManager.c 352)
	EXPECT_EQ(HashTable_GetSize(NULL), 0);
}

TEST_F(InventoryManagerTest, InventoryManager_InitHashTableCreateFailure) {
	// Test InventoryManager_Init when HashTable_Create fails (covers InventoryManager.c 50)
	mock_malloc_reset();
	mock_malloc_set_fail(1);
	mock_malloc_set_fail_count(1);
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	// This should fail because HashTable_Create will fail
	int result = InventoryManager_Init(NULL);
	EXPECT_EQ(result, -1);
	
	InventoryManager_SetMallocHook(NULL);
	mock_malloc_reset();
	
	// Cleanup in case Init partially succeeded
	InventoryManager_Cleanup(NULL);
}

TEST_F(InventoryManagerTest, HashTable_CreateMallocFailure) {
	// Test HashTable_Create when malloc fails (covers InventoryManager.c 138)
	mock_malloc_reset();
	mock_malloc_set_fail(1);
	mock_malloc_set_fail_count(1);
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	HashTable* ht = HashTable_Create();
	EXPECT_EQ(ht, nullptr);
	
	InventoryManager_SetMallocHook(NULL);
	mock_malloc_reset();
}

TEST_F(InventoryManagerTest, HashTable_AddUserNodeMallocFailure) {
	// Test HashTable_AddUser when node malloc fails (covers InventoryManager.c 209)
	mock_malloc_reset();
	
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Set malloc to fail on node creation (after table creation)
	// We need to count how many mallocs happen during Create
	InventoryManager_SetMallocHook(mock_malloc_hook);
	int create_calls = mock_malloc_get_call_count();
	
	mock_malloc_set_fail(1);
	mock_malloc_set_fail_count(create_calls + 1);  // Fail on next malloc (node creation)
	
	int result = HashTable_AddUser(ht, "user1", "pass1");
	EXPECT_EQ(result, -1);  // Should fail due to malloc failure
	
	InventoryManager_SetMallocHook(NULL);
	mock_malloc_reset();
	
	HashTable_Destroy(ht);
}

// Binary File Operations Tests

TEST_F(InventoryManagerTest, HashTable_SaveToFile) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add some users
	EXPECT_EQ(HashTable_AddUser(ht, "user1", "pass1"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user2", "pass2"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user3", "pass3"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 3);
	
	// Save to file
	const char* filename = "test_users.bin";
	int result = HashTable_SaveToFile(ht, filename);
	EXPECT_EQ(result, 0);
	
	HashTable_Destroy(ht);
	
	// Clean up test file
	remove(filename);
}

TEST_F(InventoryManagerTest, HashTable_LoadFromFile) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add some users
	EXPECT_EQ(HashTable_AddUser(ht, "user1", "pass1"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user2", "pass2"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user3", "pass3"), 0);
	
	// Save to file
	const char* filename = "test_users_load.bin";
	EXPECT_EQ(HashTable_SaveToFile(ht, filename), 0);
	
	// Destroy original table
	HashTable_Destroy(ht);
	
	// Load from file
	ht = HashTable_LoadFromFile(NULL, filename);
	EXPECT_NE(ht, nullptr);
	EXPECT_EQ(HashTable_GetSize(ht), 3);
	
	// Verify users exist
	EXPECT_NE(HashTable_FindUser(ht, "user1"), nullptr);
	EXPECT_NE(HashTable_FindUser(ht, "user2"), nullptr);
	EXPECT_NE(HashTable_FindUser(ht, "user3"), nullptr);
	
	HashTable_Destroy(ht);
	
	// Clean up test file
	remove(filename);
}

TEST_F(InventoryManagerTest, HashTable_SaveLoadRoundTrip) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	
	// Add many users
	for (int i = 0; i < 50; ++i) {
		char username[32];
		char password[32];
		std::snprintf(username, sizeof(username), "user%d", i);
		std::snprintf(password, sizeof(password), "pass%d", i);
		EXPECT_EQ(HashTable_AddUser(ht, username, password), 0);
	}
	EXPECT_EQ(HashTable_GetSize(ht), 50);
	
	// Save to file
	const char* filename = "test_users_roundtrip.bin";
	EXPECT_EQ(HashTable_SaveToFile(ht, filename), 0);
	
	// Destroy original table
	HashTable_Destroy(ht);
	
	// Load from file
	ht = HashTable_LoadFromFile(NULL, filename);
	EXPECT_NE(ht, nullptr);
	EXPECT_EQ(HashTable_GetSize(ht), 50);
	
	// Verify all users exist and can authenticate
	for (int i = 0; i < 50; ++i) {
		char username[32];
		char password[32];
		std::snprintf(username, sizeof(username), "user%d", i);
		std::snprintf(password, sizeof(password), "pass%d", i);
		EXPECT_NE(HashTable_FindUser(ht, username), nullptr);
		EXPECT_EQ(HashTable_Authenticate(ht, username, password), 1);
	}
	
	HashTable_Destroy(ht);
	
	// Clean up test file
	remove(filename);
}

TEST_F(InventoryManagerTest, HashTable_SaveToFileNULLParams) {
	// Test with NULL hash table
	EXPECT_EQ(HashTable_SaveToFile(NULL, "test.bin"), -1);
	
	// Test with NULL filename
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	EXPECT_EQ(HashTable_SaveToFile(ht, NULL), -1);
	HashTable_Destroy(ht);
}

TEST_F(InventoryManagerTest, HashTable_LoadFromFileNonExistent) {
	// Try to load from non-existent file
	HashTable* ht = HashTable_LoadFromFile(NULL, "nonexistent.bin");
	EXPECT_EQ(ht, nullptr);
}

TEST_F(InventoryManagerTest, HashTable_SaveLoadEmpty) {
	HashTable* ht = HashTable_Create();
	EXPECT_NE(ht, nullptr);
	EXPECT_EQ(HashTable_GetSize(ht), 0);
	
	// Save empty table
	const char* filename = "test_empty.bin";
	EXPECT_EQ(HashTable_SaveToFile(ht, filename), 0);
	
	// Destroy original table
	HashTable_Destroy(ht);
	
	// Load empty table
	ht = HashTable_LoadFromFile(NULL, filename);
	EXPECT_NE(ht, nullptr);
	EXPECT_EQ(HashTable_GetSize(ht), 0);
	
	HashTable_Destroy(ht);
	
	// Clean up test file
	remove(filename);
}


int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}