//#define ENABLE_INVENTORYMANAGER_TEST  // Uncomment this line to enable the Inventory tests

#include "gtest/gtest.h"
#include "../../inventory_lib/header/InventoryManager.h"  // Adjust this include path based on your project structure

using namespace Coruh::Inventory;

class InventoryTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

TEST_F(InventoryTest, TestAdd) {
	double result = Inventory::add(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 8.0);
}

TEST_F(InventoryTest, TestSubtract) {
	double result = Inventory::subtract(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST_F(InventoryTest, TestMultiply) {
	double result = Inventory::multiply(5.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 15.0);
}

TEST_F(InventoryTest, TestDivide) {
	double result = Inventory::divide(6.0, 3.0);
	EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST_F(InventoryTest, TestDivideByZero) {
	EXPECT_THROW(Inventory::divide(5.0, 0.0), std::invalid_argument);
}

/**
 * @brief The main function of the test program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line argument strings.
 * @return int The exit status of the program.
 */
int main(int argc, char** argv) {
#ifdef ENABLE_INVENTORYMANAGER_TEST
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#else
	return 0;
#endif
}