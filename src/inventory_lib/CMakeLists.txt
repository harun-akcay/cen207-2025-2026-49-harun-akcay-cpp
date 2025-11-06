# inventory_lib/CMakeLists.txt
set(ROOT src)
set(LIBNAME inventory_lib)

message(STATUS "[${ROOT}/${LIBNAME}] Module Processing...")

# Collect files without having to explicitly list each header and source file
file(GLOB LIB_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/header/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/header/*.hpp")

file(GLOB LIB_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

# Create named folders for the sources within the project
source_group("header" FILES ${LIB_HEADERS})
source_group("src" FILES ${LIB_SOURCES})

# Set Properties->General->Configuration Type to Static Library
add_library(${LIBNAME} STATIC ${LIB_HEADERS} ${LIB_SOURCES}) # for dynamic library use SHARED

target_include_directories(${LIBNAME} PUBLIC
						   ${CMAKE_CURRENT_SOURCE_DIR}/header)

# creates preprocessor definition used for library exports
add_compile_definitions("CORUH_INVENTORY_LIB_EXPORTS")

# For MSVC: Ensure C runtime is linked (needed for realloc, malloc, etc.)
if(MSVC)
    target_compile_options(${LIBNAME} PRIVATE $<$<CONFIG:Debug>:/MTd> $<$<CONFIG:Release>:/MT>)
endif()

install(TARGETS ${LIBNAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin )
		
# Copy required headers to the installation include folder		
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/header/InventoryManager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/header/MaterialInventory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/header/ProjectTracking.h
        ${CMAKE_CURRENT_SOURCE_DIR}/header/ExpenseTracking.h
        ${CMAKE_CURRENT_SOURCE_DIR}/header/SalesTracker.h
        ${CMAKE_CURRENT_SOURCE_DIR}/header/TaskQueue.h
        ${CMAKE_CURRENT_SOURCE_DIR}/header/NavigationHistory.h
        DESTINATION include)

# Export the inventory_lib target so other modules can use it
# export(TARGETS ${LIBNAME} FILE ${LIBNAME}Targets.cmake)

message(STATUS "[${ROOT}/${LIBNAME}] Added library target: ${LIBNAME}")
