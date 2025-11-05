# Project Design and Architecture

## System Overview

The **Simple Inventory Management for Crafters** is a console-based application designed to help crafters manage their inventory, track projects, log expenses, and monitor sales. The system is implemented in C programming language using various data structures and algorithms.

## Architecture Design

### System Components

```
┌─────────────────────────────────────────────────────────┐
│                   Inventory Application                  │
│                  (Console Interface)                     │
└────────────────────┬────────────────────────────────────┘
                     │
        ┌────────────┴────────────┐
        │                         │
┌───────▼────────┐      ┌────────▼──────────┐
│ Inventory Lib  │      │   Application     │
│  (Core Logic)  │      │   (User Input)    │
└───────┬────────┘      └───────────────────┘
        │
   ┌────┴─────────────────────────────────┐
   │                                      │
┌──▼──────────────┐            ┌─────────▼─────────┐
│ Material        │            │ Inventory         │
│ Inventory       │            │ Manager           │
│ (Double Linked  │            │ (Hash Table +     │
│  List)          │            │  Stack)           │
└─────────────────┘            └───────────────────┘
```

## Data Structures Implemented

### 1. Hash Table (User Authentication)
- **Location:** `src/inventory_lib/header/InventoryManager.h`
- **Implementation:** Separate chaining for collision handling
- **Hash Function:** djb2 algorithm
- **Size:** 101 buckets (prime number for better distribution)
- **Use Case:** User registration, login, and authentication
- **Time Complexity:**
  - Insert: O(1) average, O(n) worst case
  - Search: O(1) average, O(n) worst case
  - Delete: O(1) average, O(n) worst case

### 2. Double Linked List (Material Inventory)
- **Location:** `src/inventory_lib/header/MaterialInventory.h`
- **Implementation:** Bidirectional linked list with head and tail pointers
- **Use Case:** Material inventory management (add, edit, remove, view)
- **Features:**
  - Forward and backward traversal
  - Efficient insertion/deletion at both ends
  - Material tracking with ID, name, category, quantity, price, supplier
- **Time Complexity:**
  - Insert at head/tail: O(1)
  - Insert at position: O(n)
  - Delete: O(1) with node pointer, O(n) with search
  - Search: O(n)

### 3. Stack (Login History)
- **Location:** `src/inventory_lib/header/InventoryManager.h`
- **Implementation:** Linked list-based stack
- **Use Case:** Track login history (LIFO - Last In, First Out)
- **Features:**
  - Push login events
  - Pop to view recent logins
  - Capacity management
- **Time Complexity:**
  - Push: O(1)
  - Pop: O(1)
  - Peek: O(1)

## Module Structure

### Inventory Manager Module
- **Purpose:** Core management system for users and system initialization
- **Key Functions:**
  - `InventoryManager_Init()`: Initialize the entire system
  - `InventoryManager_Cleanup()`: Cleanup and save data
  - User authentication functions
  - Login history management

### Material Inventory Module
- **Purpose:** Material inventory management using Double Linked List
- **Key Functions:**
  - `MaterialInventory_Create()`: Create new inventory list
  - `MaterialInventory_Add()`: Add material to inventory
  - `MaterialInventory_Remove()`: Remove material from inventory
  - `MaterialInventory_Search()`: Search for materials
  - `MaterialInventory_SaveToFile()`: Save to binary file
  - `MaterialInventory_LoadFromFile()`: Load from binary file

### Application Module
- **Purpose:** Console-based user interface
- **Location:** `src/inventory_app/`
- **Features:**
  - Main menu system
  - User input handling
  - Menu navigation
  - Integration with library modules

## File Operations

### Binary File Format
All data structures use binary file format (NOT text files) for persistence:
- **Hash Table:** `users.bin` - User authentication data
- **Material Inventory:** `materials.bin` - Material inventory data
- **Functions:** `fwrite()` and `fread()` for binary I/O

### File Structure
- Binary format ensures efficient storage and loading
- Structured data storage for all entities
- Error handling for file operations

## Memory Management

### Dynamic Memory Allocation
- All data structures use dynamic memory allocation
- Proper cleanup functions provided for all modules
- Memory leak prevention through comprehensive cleanup

### Error Handling
- Null pointer checks
- Memory allocation failure handling
- File operation error handling
- Mock malloc system for testing error paths

## Testing Architecture

### Test Framework
- **Framework:** GoogleTest
- **Coverage:** 99.4% code coverage (target: 100%)
- **Test Types:**
  - Unit tests for individual functions
  - Integration tests for module interactions
  - Error path testing with mock malloc

### Test Structure
- **Location:** `src/tests/inventory_lib_test/`
- **Test Files:**
  - `InventoryManager_test.cpp`: Hash table and user management tests
  - `InventoryApp_error_test.c`: Error handling tests
  - `mock_malloc.c/h`: Mock memory allocation for testing

## Build System

### CMake Configuration
- **Minimum Version:** CMake 3.12
- **C Standard:** C11
- **Platforms:** Windows, Linux (WSL), macOS
- **Build Types:** Debug and Release

### Build Scripts
- `7-build-app-windows.bat`: Full build on Windows
- `7-build-app-linux.sh`: Full build on Linux/WSL
- `7-build-doc-windows.bat`: Documentation generation
- `8-build-test-windows.bat`: Test execution

## Documentation System

### Doxygen Configuration
- **Tool:** Doxygen with PDF output
- **Coverage:** 100% documentation coverage required
- **Language:** English only
- **Output Formats:** HTML and PDF

### Documentation Files
- Library documentation: `docs/doxygenlibwin/`
- Test documentation: `docs/doxygentestwin/`
- Coverage reports: `docs/coveragereportlibwin/`

## Future Enhancements

### Planned Modules (Not Yet Implemented)
1. **Project Tracking** - Stack/Queue implementation
2. **Expense Logging** - Sparse Matrix implementation
3. **Sales Tracker** - Heap/Heap Sort implementation
4. **Graph Algorithms** - BFS/DFS for project dependencies

### Planned Features
- Binary file operations for all data structures
- Main menu system with full navigation
- Complexity analysis documentation
- Performance testing and benchmarking

---

**Last Updated:** 2025-01-XX  
**Version:** 1.0.0  
**Status:** In Development
