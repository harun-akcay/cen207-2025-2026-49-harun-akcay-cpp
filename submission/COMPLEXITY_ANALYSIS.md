# Complexity Analysis (Big-O Notation)
## CEN207 - Data Structures Project

**Date:** 2025-11-06  
**Project:** Simple Inventory Management for Crafters

---

## Overview

This document provides detailed complexity analysis (Big-O notation) for all data structures and algorithms implemented in the C project. The analysis covers time complexity, space complexity, and worst-case, average-case, and best-case scenarios where applicable.

---

## 1. Double Linked List (Material Inventory)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(1) | Allocates single structure |
| **Destroy** | O(n) | O(1) | Must traverse all nodes to free |
| **Add (Insert)** | O(1) | O(1) | Insert at head/tail is constant |
| **Add (Append)** | O(1) | O(1) | Insert at tail is constant |
| **Remove** | O(n) | O(1) | Must search for node first |
| **Find** | O(n) | O(1) | Linear search through list |
| **Get Size** | O(1) | O(1) | Size is stored in structure |
| **Traverse** | O(n) | O(1) | Must visit each node once |
| **Save to File** | O(n) | O(1) | Write each node to file |
| **Load from File** | O(n) | O(n) | Read and create n nodes |

### Analysis
- **Best Case:** O(1) for insertions at known positions
- **Average Case:** O(n) for search operations
- **Worst Case:** O(n) for search/remove operations
- **Space:** O(n) where n is the number of materials

---

## 2. XOR Linked List (Navigation History)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(1) | Allocates single structure |
| **Destroy** | O(n) | O(1) | Must traverse all nodes to free |
| **Add Entry** | O(1) | O(1) | Insert at tail is constant |
| **Remove Entry** | O(n) | O(1) | Must traverse to find node |
| **Get Previous** | O(1) | O(1) | XOR operation is constant |
| **Get Next** | O(1) | O(1) | XOR operation is constant |
| **Traverse Forward** | O(n) | O(1) | Visit each node once |
| **Traverse Backward** | O(n) | O(1) | Visit each node once |
| **Save to File** | O(n) | O(1) | Write each node to file |
| **Load from File** | O(n) | O(n) | Read and create n nodes |

### Analysis
- **Best Case:** O(1) for insertions and navigation
- **Average Case:** O(n) for traversal operations
- **Worst Case:** O(n) for search/remove operations
- **Space:** O(n) where n is the number of entries (50% memory savings vs. double linked list)

### Memory Efficiency
- **Standard Double Linked List:** 2 pointers per node = 16 bytes (64-bit)
- **XOR Linked List:** 1 pointer per node = 8 bytes (64-bit)
- **Memory Savings:** 50% reduction in pointer storage

---

## 3. Stack (Project Tracking)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(1) | Allocates single structure |
| **Destroy** | O(n) | O(1) | Must pop all elements |
| **Push** | O(1) | O(1) | Insert at top is constant |
| **Pop** | O(1) | O(1) | Remove from top is constant |
| **Peek** | O(1) | O(1) | Access top without removal |
| **Is Empty** | O(1) | O(1) | Check size field |
| **Get Size** | O(1) | O(1) | Size is stored in structure |
| **Find** | O(n) | O(1) | Must search through stack |
| **Save to File** | O(n) | O(1) | Write each element to file |
| **Load from File** | O(n) | O(n) | Read and push n elements |

### Analysis
- **Best Case:** O(1) for all stack operations
- **Average Case:** O(1) for push/pop, O(n) for search
- **Worst Case:** O(n) for search operations
- **Space:** O(n) where n is the number of projects

---

## 4. Queue (Task Queue)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(1) | Allocates single structure |
| **Destroy** | O(n) | O(1) | Must dequeue all elements |
| **Enqueue** | O(1) | O(1) | Insert at rear is constant |
| **Dequeue** | O(1) | O(1) | Remove from front is constant |
| **Peek** | O(1) | O(1) | Access front without removal |
| **Is Empty** | O(1) | O(1) | Check size field |
| **Get Size** | O(1) | O(1) | Size is stored in structure |
| **Find** | O(n) | O(1) | Must search through queue |
| **Save to File** | O(n) | O(1) | Write each element to file |
| **Load from File** | O(n) | O(n) | Read and enqueue n elements |

### Analysis
- **Best Case:** O(1) for all queue operations
- **Average Case:** O(1) for enqueue/dequeue, O(n) for search
- **Worst Case:** O(n) for search operations
- **Space:** O(n) where n is the number of tasks

---

## 5. Sparse Matrix (Expense Tracking)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(1) | Allocates structure |
| **Destroy** | O(n) | O(1) | Must free all entries |
| **Add Expense** | O(1) | O(1) | Insert into linked list |
| **Remove Expense** | O(n) | O(1) | Must search for entry |
| **Find Expense** | O(n) | O(1) | Linear search through entries |
| **Get by Project** | O(n) | O(k) | k = number of matching expenses |
| **Get by Category** | O(n) | O(k) | k = number of matching expenses |
| **Get Total by Project** | O(n) | O(1) | Sum all matching expenses |
| **Get Total by Category** | O(n) | O(1) | Sum all matching expenses |
| **Save to File** | O(n) | O(1) | Write each entry to file |
| **Load from File** | O(n) | O(n) | Read and create n entries |

### Analysis
- **Best Case:** O(1) for insertions
- **Average Case:** O(n) for search operations where n is non-zero entries
- **Worst Case:** O(n) for search/remove operations
- **Space:** O(n) where n is the number of non-zero entries (much less than full matrix)

### Memory Efficiency
- **Full Matrix:** O(rows × columns) space
- **Sparse Matrix:** O(n) space where n = non-zero entries
- **Savings:** Significant when matrix is mostly empty

---

## 6. Heap / Heap Sort (Sales Tracker)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(1) | Allocates structure |
| **Destroy** | O(1) | O(1) | Free structure and array |
| **Insert** | O(log n) | O(1) | Heapify up operation |
| **Extract Max** | O(log n) | O(1) | Heapify down operation |
| **Get Max** | O(1) | O(1) | Access root element |
| **Heap Sort** | O(n log n) | O(1) | Extract max n times |
| **Build Heap** | O(n) | O(1) | Bottom-up heap construction |
| **Find** | O(n) | O(1) | Linear search (not optimal) |
| **Save to File** | O(n) | O(1) | Write each element to file |
| **Load from File** | O(n log n) | O(n) | Insert n elements |

### Analysis
- **Best Case:** O(1) for get max, O(log n) for insert/extract
- **Average Case:** O(log n) for heap operations
- **Worst Case:** O(log n) for insert/extract, O(n log n) for heap sort
- **Space:** O(n) where n is the number of sales

### Heap Sort Complexity
- **Time:** O(n log n) - guaranteed, not average case
- **Space:** O(1) - in-place sorting
- **Stability:** Not stable (equal elements may change order)

---

## 7. Hash Table (User Authentication)

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Create** | O(1) | O(m) | m = hash table size (101) |
| **Destroy** | O(n) | O(1) | Must free all chains |
| **Insert** | O(1) average, O(n) worst | O(1) | Hash + insert into chain |
| **Search** | O(1) average, O(n) worst | O(1) | Hash + search chain |
| **Delete** | O(1) average, O(n) worst | O(1) | Hash + remove from chain |
| **Hash Function** | O(k) | O(1) | k = string length (djb2) |
| **Save to File** | O(n) | O(1) | Write all entries to file |
| **Load from File** | O(n) | O(n) | Read and insert n entries |

### Analysis
- **Best Case:** O(1) for all operations (no collisions)
- **Average Case:** O(1) for all operations (good hash distribution)
- **Worst Case:** O(n) when all keys hash to same bucket
- **Space:** O(n + m) where n = entries, m = table size (101)

### Hash Function (djb2)
- **Time:** O(k) where k is the length of the key string
- **Distribution:** Good for typical usernames (short strings)
- **Collision Handling:** Separate chaining (linked list per bucket)

### Load Factor
- **Current:** n/m where n = users, m = 101
- **Optimal:** < 0.75 for good performance
- **Worst Case:** All users in one bucket = O(n) search

---

## 8. BFS (Breadth-First Search)

### Algorithm Complexity

| Metric | Complexity | Notes |
|--------|------------|-------|
| **Time** | O(V + E) | V = vertices, E = edges |
| **Space** | O(V) | Queue stores visited vertices |
| **Shortest Path** | O(V + E) | Finds shortest unweighted path |

### Analysis
- **Best Case:** O(V) when graph is a tree (E = V-1)
- **Average Case:** O(V + E) for typical graphs
- **Worst Case:** O(V + E) for complete graphs (E = V²)
- **Space:** O(V) for queue and visited array

### Implementation Details
- Uses queue (FIFO) for level-order traversal
- Visits all vertices at distance k before distance k+1
- Guarantees shortest path in unweighted graphs

---

## 9. DFS (Depth-First Search)

### Algorithm Complexity

| Metric | Complexity | Notes |
|--------|------------|-------|
| **Time** | O(V + E) | V = vertices, E = edges |
| **Space** | O(V) | Recursion stack or explicit stack |
| **Complete Traversal** | O(V + E) | Visits all reachable vertices |

### Analysis
- **Best Case:** O(V) when graph is a tree (E = V-1)
- **Average Case:** O(V + E) for typical graphs
- **Worst Case:** O(V + E) for complete graphs (E = V²)
- **Space:** O(V) for recursion stack (worst case: linear graph)

### Implementation Details
- Uses recursion or explicit stack (LIFO)
- Visits vertices as deep as possible before backtracking
- Useful for cycle detection, topological sort

---

## 10. Binary File Operations

### Operations

| Operation | Time Complexity | Space Complexity | Notes |
|-----------|----------------|------------------|-------|
| **Save (fwrite)** | O(n) | O(1) | n = number of elements |
| **Load (fread)** | O(n) | O(n) | n = number of elements |
| **File I/O** | O(1) per element | O(1) | System call overhead |

### Analysis
- **Time:** O(n) where n is the number of data structures to save/load
- **Space:** O(1) for save, O(n) for load (must store loaded data)
- **Efficiency:** Binary format is faster and more compact than text

### File Format
- **Binary:** Uses `fwrite()` and `fread()` for structured data
- **Size:** Compact representation (no text formatting overhead)
- **Speed:** Faster than text parsing
- **Portability:** May have endianness issues (not addressed in this project)

---

## Summary Table

| Data Structure | Insert | Search | Delete | Space | Notes |
|---------------|--------|--------|--------|-------|-------|
| **Double Linked List** | O(1) | O(n) | O(n) | O(n) | Bidirectional traversal |
| **XOR Linked List** | O(1) | O(n) | O(n) | O(n) | 50% memory savings |
| **Stack** | O(1) | O(n) | O(1) | O(n) | LIFO operations |
| **Queue** | O(1) | O(n) | O(1) | O(n) | FIFO operations |
| **Sparse Matrix** | O(1) | O(n) | O(n) | O(n) | n = non-zero entries |
| **Heap** | O(log n) | O(n) | O(log n) | O(n) | Max heap |
| **Hash Table** | O(1)* | O(1)* | O(1)* | O(n+m) | *Average case |
| **BFS** | - | O(V+E) | - | O(V) | Graph traversal |
| **DFS** | - | O(V+E) | - | O(V) | Graph traversal |

*Average case complexity. Worst case is O(n) for hash table operations.

---

## Performance Considerations

### Memory Usage
1. **XOR Linked List** saves 50% memory vs. double linked list
2. **Sparse Matrix** saves significant memory for sparse data
3. **Hash Table** has fixed overhead (table size = 101)

### Time Efficiency
1. **Hash Table** provides O(1) average case for user operations
2. **Heap** provides O(log n) for priority operations
3. **Stack/Queue** provide O(1) for basic operations

### Trade-offs
1. **Linked Lists** vs. **Arrays**: Dynamic size vs. cache locality
2. **Hash Table** vs. **Binary Search Tree**: O(1) average vs. O(log n) guaranteed
3. **BFS** vs. **DFS**: Shortest path vs. memory efficiency

---

## Conclusion

All data structures and algorithms have been implemented with appropriate complexity characteristics. The project demonstrates understanding of:
- Time complexity analysis
- Space complexity analysis
- Trade-offs between different data structures
- Real-world application of data structures

The implementations are efficient and suitable for the inventory management use case.

---

**Last Updated:** 2025-11-06  
**Author:** CEN207 Project Team

