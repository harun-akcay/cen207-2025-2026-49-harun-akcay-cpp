// src/tests/inventory_lib_test/InventoryManager_test.cpp

#include "gtest/gtest.h"

// Include the C library header we are testing
// The 'extern "C"' block in the header makes this work
#include "InventoryManager.h"

// A temporary binary file for testing I/O
const char* TEST_DB_FILE = "test_users.bin";

// Test Fixture: Sets up a clean environment for each test
class InventoryTest : public ::testing::Test {
protected:
    // This runs before each test
    void SetUp() override {
        // Delete the test file if it exists from a previous run
        remove(TEST_DB_FILE);

        // --- THIS IS THE FIX ---
        // Call our new C function to reset the in-memory database
        // instead of the old "non_existent_file" hack.
        resetUserDatabase();
    }

    // This runs after each test
    void TearDown() override {
        // Clean up the test file
        remove(TEST_DB_FILE);
    }
};

// --- Test Cases ---
// We are in C++ (gtest), but we are calling our C functions.

TEST_F(InventoryTest, UserAuth_CreateUser) {
    // Call the C function from inventory_lib
    int result = createUser("testuser", "pass123");
    ASSERT_EQ(result, 1); // Should return 1 (success)
}

TEST_F(InventoryTest, UserAuth_FindUser) {
    createUser("testuser", "pass123");

    // Call the C function
    User* foundUser = findUserByName("testuser");
    ASSERT_NE(foundUser, nullptr); // Should find the user (not NULL)
    EXPECT_STREQ(foundUser->username, "testuser");
    EXPECT_STREQ(foundUser->password, "pass123");
}

TEST_F(InventoryTest, UserAuth_PreventDuplicateUser) {
    createUser("testuser", "pass123");
    int result = createUser("testuser", "pass456"); // Add same user again
    ASSERT_EQ(result, 0); // Should return 0 (failure)
}

// Test for the critical "Binary File" requirement
TEST_F(InventoryTest, BinaryIO_SaveAndLoadUsers) {
    // 1. Create data in memory
    createUser("user1", "pass1");
    createUser("user2", "pass2");

    // 2. Save to binary file (C function)
    int save_result = saveUsersToBinary(TEST_DB_FILE);
    ASSERT_EQ(save_result, 1);

    // 3. Clear memory (simulate app restart)
    resetUserDatabase(); // Use our reliable reset function
    User* check_user = findUserByName("user1");
    ASSERT_EQ(check_user, nullptr); // Should not be in memory

    // 4. Load from binary file (C function)
    int load_result = loadUsersFromBinary(TEST_DB_FILE);
    ASSERT_EQ(load_result, 1); // Should return 1 (success, data loaded)

    // 5. Verify data is loaded correctly
    check_user = findUserByName("user1");
    ASSERT_NE(check_user, nullptr);
    EXPECT_STREQ(check_user->username, "user1");

    User* check_user2 = findUserByName("user2");
    ASSERT_NE(check_user2, nullptr);
    EXPECT_STREQ(check_user2->username, "user2");
}