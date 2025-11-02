

# Create the test executable
add_executable(inventory_lib_test InventoryManager_test.cpp)

# Link the libraries needed for this test:
target_link_libraries(inventory_lib_test PUBLIC
    inventory_lib   # Our C library
    gtest_main      # GoogleTest
)

# (All previous LNK2019 fix attempts (like ucrt.lib)
# have been removed from this file.)

# Add the test to CTest
include(GoogleTest)
gtest_discover_tests(inventory_lib_test)