# Set the minimum required CMake version
cmake_minimum_required(VERSION 3.12)
project(inventory_app)
enable_language(C CXX)
set(ROOT src)

# Print useful information
message(STATUS "[${ROOT}] CMake version ${CMAKE_VERSION}")
message(STATUS "[${ROOT}] System ${CMAKE_SYSTEM_NAME}")
message(STATUS "[${ROOT}] Processor ${CMAKE_SYSTEM_PROCESSOR}")

# GoogleTest requires at least C11
if(NOT "${CMAKE_CXX_STANDARD}")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
message(STATUS "[${ROOT}] C standard version: ${CMAKE_C_STANDARD}")
endif()

message(STATUS "[${ROOT}] C++ standard version: ${CMAKE_CXX_STANDARD}")

# Set the default installation directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/install" CACHE PATH "Default installation directory" FORCE)
endif()

message(STATUS "[${ROOT}] Folder is set to : ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Set build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

#crypto++ settings, disable testing, documentation generation, install and DLL generation
set(CRYPTOPP_BUILD_TESTING OFF)
set(CRYPTOPP_BUILD_DOCUMENTATION OFF)
set(CRYPTOPP_INSTALL ON)
set(CRYPTOPP_BUILD_SHARED OFF)
set(CRYPTOPP_USE_MASTER_BRANCH FALSE)

option(ENABLE_INVENTORYMANAGER "Enable Inventory Module" ON)
option(ENABLE_INVENTORYMANAGER_APP "Enable Inventory Application" ON)
option(ENABLE_TESTS "Enable All Tests" ON)

# Configure tests
add_compile_definitions(ENABLE_INVENTORYMANAGER_TEST)


# Configure logs
add_compile_definitions(ENABLE_INVENTORYMANAGER_LOGGER)

# Set the output directories for Debug and Release configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/build/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/build/Release)

# Custom definitions based on environment
if(MSVC)
    add_definitions(-DMSVC_ENV)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-DGCC_ENV)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DMAC_ENV)
endif()

# Set Debug and Release configuration flags
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "[${ROOT}] Setting Ubuntu/MacOS C/C++ Flags")

    set(COMP_WARNINGS "-Wall -Wextra -Wunused-function -Wno-unknown-pragmas")
    set(COMP_PROFILE "-fprofile-arcs -ftest-coverage")
    set(COMP_OPTIMIZATION "-O0") #for debugging
    set(COMP_SHARED "-shared -fPIC")

    #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g ${COMP_OPTIMIZATION} ${COMP_WARNINGS} ${COMP_PROFILE}")
    #set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g ${COMP_OPTIMIZATION} ${COMP_WARNINGS} ${COMP_PROFILE}")

    #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${COMP_WARNINGS} ${COMP_PROFILE}")
    #set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COMP_WARNINGS} ${COMP_PROFILE}")

# It ensures both C and C++ code use the same C Runtime Library (/MT or /MTd)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "[${ROOT}] Setting Windows C/C++ Flags")
    
    # DEBUG FLAGS (Must match)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd") 
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    
    # RELEASE FLAGS (Must match)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
    message(WARNING "[${ROOT}] Not supported with the current compiler.")
endif()



# Logger submodule
if(ENABLE_INVENTORYMANAGER)
    add_subdirectory(${ROOT}/inventory_lib)
endif()

# BigInteger for Crypto submodule
if(ENABLE_INVENTORYMANAGER_APP)
    add_subdirectory(${ROOT}/inventory_app)
endif()

# Tests
if(ENABLE_TESTS)
    add_subdirectory(${ROOT}/tests)
endif()

# Include the Google Test framework
# add_subdirectory(src/tests/googletest)