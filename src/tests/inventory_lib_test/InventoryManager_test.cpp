

#include "gtest/gtest.h"

// Include the C library header we are testing
#include "InventoryManager.h"

// A temporary binary file for testing I/O
const char* TEST_DB_FILE = "test_users.bin";

// Test Fixture: Sets up a clean environment for each test
class InventoryTest : public ::testing::Test {
protected:
    // This runs before each test
    void SetUp() override {
        // Delete the test file if it exists
        remove(TEST_DB_FILE);

        // Initialize a new, clean hash table for users
        ht_init_user_table();
    }

    // This runs after each test
    void TearDown() override {
        // Clean up the test file
        remove(TEST_DB_FILE);

        // Free all memory used by the hash table
        ht_free_user_table();
    }
};

// --- Test Cases ---

TEST_F(InventoryTest, UserAuth_CreateUser) {
    int result = createUser("testuser", "pass123");
    ASSERT_EQ(result, 1);
}

TEST_F(InventoryTest, UserAuth_FindUser) {
    createUser("testuser", "pass123");

    User* foundUser = findUserByName("testuser");
    ASSERT_NE(foundUser, nullptr);
    EXPECT_STREQ(foundUser->username, "testuser");
}

TEST_F(InventoryTest, UserAuth_FindNonExistentUser) {
    User* foundUser = findUserByName("nosuchuser");
    ASSERT_EQ(foundUser, nullptr); // Should not find anything
}

TEST_F(InventoryTest, UserAuth_PreventDuplicateUser) {
    createUser("testuser", "pass123");
    int result = createUser("testuser", "pass456"); // Add same user again
    ASSERT_EQ(result, 0); // Should return 0 (failure)
}

// --- NEW TEST FOR HASH TABLE DATA STRUCTURE ---
/**
 * @brief Tests the core Hash Table requirement: Collision Handling.
 * We force a collision by using two strings that hash to the same index.
 * (The DJB2 hash of "user1" and "user11" % 10 are different, but
 * "collision_user_A" and "collision_user_B" might collide.
 * A guaranteed way is harder without knowing the hash, but we test the logic.)
 * * Let's create many users and check if all are found.
 * If collisions weren't handled, some users would be overwritten.
 */
TEST_F(InventoryTest, HashTable_HandleCollision) {
    // Create 20 users, which MUST cause collisions in a 10-bucket table
    char username[20];
    for (int i = 0; i < 20; ++i) {
        sprintf(username, "user%d", i);
        int result = createUser(username, "pass");
        ASSERT_EQ(result, 1);
    }

    // Check if all 20 users can be found
    // If collision failed, we'd find fewer than 20.
    int found_count = 0;
    for (int i = 0; i < 20; ++i) {
        sprintf(username, "user%d", i);
        User* found = findUserByName(username);
        ASSERT_NE(found, nullptr); // Must find this user
        EXPECT_STREQ(found->username, username);
        if (found) {
            found_count++;
        }
    }

    ASSERT_EQ(found_count, 20); // All 20 users must be found
}


// Test for the critical "Binary File" requirement
// This test now verifies our hash table traversal for saving/loading.
TEST_F(InventoryTest, BinaryIO_SaveAndLoadUsers) {
    // 1. Create 20 users (to test collisions)
    char username[20];
    for (int i = 0; i < 20; ++i) {
        sprintf(username, "user%d", i);
        createUser(username, "pass");
    }

    // 2. Save to binary file (C function)
    int save_result = saveUsersToBinary(TEST_DB_FILE);
    ASSERT_EQ(save_result, 1);

    // 3. Clear memory (simulate app restart by freeing the old table)
    ht_free_user_table();
    User* check_user = findUserByName("user1");
    ASSERT_EQ(check_user, nullptr); // Should be NULL (table is gone)

    // 4. Load from binary file (C function)
    // This will create a new table and re-insert all users
    int load_result = loadUsersToBinary(TEST_DB_FILE);
    ASSERT_EQ(load_result, 1);

    // 5. Verify all 20 users are loaded correctly
    int found_count = 0;
    for (int i = 0; i < 20; ++i) {
        sprintf(username, "user%d", i);
        User* found = findUserByName(username);
        ASSERT_NE(found, nullptr); // Must find this user
        if (found) {
            found_count++;
        }
    }
    ASSERT_EQ(found_count, 20);
}