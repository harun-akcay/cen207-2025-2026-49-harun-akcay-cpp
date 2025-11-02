
# Create the library from the C source file
add_library(inventory_lib src/InventoryManager.c)

# Set this library's language to C (NOT C++)
# This is CRITICAL for the midterm requirements
set_target_properties(inventory_lib PROPERTIES LANGUAGE C)

# --- Include Directories ---
target_include_directories(inventory_lib
    PRIVATE
        header
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/header>
        $<INSTALL_INTERFACE:include/inventory_lib>
)

# (All previous LNK2019 fix attempts have been removed.
# The fix is now correctly placed in the root CMakeLists.txt)

# --- Export the library (for other modules to find) ---
include(GNUInstallDirs)
install(TARGETS inventory_lib
    EXPORT inventory_lib-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the header files to 'include/inventory_lib'
install(DIRECTORY header/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/inventory_lib)

# Generate and install the CMake config files
install(EXPORT inventory_lib-targets
    FILE inventory_lib-config.cmake
    NAMESPACE inventory_lib
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/inventory_lib
)