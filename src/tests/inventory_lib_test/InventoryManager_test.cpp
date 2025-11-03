#include "gtest/gtest.h"
#include "../../inventory_lib/header/InventoryManager.h"
#include <cstring>

class InventoryManagerTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

TEST_F(InventoryManagerTest, TestInit) {
	int result = InventoryManager_Init();
	EXPECT_EQ(result, 0);
	
	// Cleanup after test
	InventoryManager_Cleanup();
}

TEST_F(InventoryManagerTest, TestCleanup) {
	// Initialize first
	int init_result = InventoryManager_Init();
	EXPECT_EQ(init_result, 0);
	
	// Then cleanup
	int cleanup_result = InventoryManager_Cleanup();
	EXPECT_EQ(cleanup_result, 0);
}

TEST_F(InventoryManagerTest, TestInitCleanupSequence) {
	// Test that we can initialize and cleanup multiple times
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(InventoryManager_Init(), 0);
		EXPECT_EQ(InventoryManager_Cleanup(), 0);
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
	
	// Add multiple users that might cause hash collisions
	// (We can't guarantee collisions, but we can test multiple users)
	EXPECT_EQ(HashTable_AddUser(ht, "user1", "pass1"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user2", "pass2"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user3", "pass3"), 0);
	EXPECT_EQ(HashTable_AddUser(ht, "user4", "pass4"), 0);
	EXPECT_EQ(HashTable_GetSize(ht), 4);
	
	// Verify all users can be found
	EXPECT_NE(HashTable_FindUser(ht, "user1"), nullptr);
	EXPECT_NE(HashTable_FindUser(ht, "user2"), nullptr);
	EXPECT_NE(HashTable_FindUser(ht, "user3"), nullptr);
	EXPECT_NE(HashTable_FindUser(ht, "user4"), nullptr);
	
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

/**
 * @brief The main function of the test program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}