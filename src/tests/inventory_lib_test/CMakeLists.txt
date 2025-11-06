# tests/inventory_lib_test/CMakeLists.txt
set(ROOT src/tests)
set(TESTNAME inventory_lib)
set(EXENAME ${TESTNAME}_tests)

message(STATUS "[${ROOT}/${TESTNAME}] Module Tests...")

# Collect files without having to explicitly list each header and source file
file(GLOB LIB_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

file(GLOB LIB_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# Exclude error test file from main test executable
list(REMOVE_ITEM LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/InventoryApp_error_test.c")

# Create named folders for the sources within the project
source_group("header" FILES ${LIB_HEADERS})
source_group("src" FILES ${LIB_SOURCES})

enable_testing()

# Define the target for inventory_lib tests
add_executable(${EXENAME} ${LIB_HEADERS} ${LIB_SOURCES})

# Add included headers
target_include_directories(${EXENAME} PUBLIC
						   ${CMAKE_CURRENT_SOURCE_DIR}/../../inventory_lib/header
						   ${CMAKE_CURRENT_SOURCE_DIR})

# Add any dependencies or compile options specific to inventory_lib tests
target_link_libraries(${EXENAME} PRIVATE inventory_lib gtest gtest_main)

# Enable test hooks for memory allocation testing
target_compile_definitions(${EXENAME} PRIVATE ENABLE_INVENTORYMANAGER_TEST)
target_compile_definitions(inventory_lib PRIVATE ENABLE_INVENTORYMANAGER_TEST)

# For MSVC: Ensure C runtime is linked
if(MSVC)
    target_compile_options(${EXENAME} PRIVATE $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif()

# Register the test with CTest
# add_test(NAME ${EXENAME} COMMAND ${EXENAME})

install(TARGETS ${EXENAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin )

# Discover tests using CTest
include(GoogleTest)
gtest_discover_tests(${EXENAME})

# Add error path test executable for InventoryApp.c
add_executable(inventory_app_error_test InventoryApp_error_test.c mock_malloc.c)
target_include_directories(inventory_app_error_test PUBLIC
						   ${CMAKE_CURRENT_SOURCE_DIR}/../../inventory_lib/header
						   ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(inventory_app_error_test PRIVATE inventory_lib)
install(TARGETS inventory_app_error_test
        RUNTIME DESTINATION bin)
		
message(STATUS "[${ROOT}/${TESTNAME}] Added target: ${EXENAME}")
message(STATUS "[${ROOT}/${TESTNAME}] Added error path test: inventory_app_error_test")