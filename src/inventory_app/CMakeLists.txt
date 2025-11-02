
# Create the executable
add_executable(inventory_app src/InventoryApp.cpp)

# Link our C library
target_link_libraries(inventory_app PUBLIC
    inventory_lib
)

# (All previous LNK2019 fix attempts (like ucrt.lib)
# have been removed from this file.)