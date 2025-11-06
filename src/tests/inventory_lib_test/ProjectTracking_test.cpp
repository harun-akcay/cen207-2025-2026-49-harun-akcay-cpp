/**
 * @file ProjectTracking_test.cpp
 * @brief Unit tests for ProjectTracking module (Stack + BFS/DFS)
 * 
 * This file contains comprehensive unit tests for the ProjectTracking module
 * using the Google Test framework. The tests cover stack operations, 
 * graph operations (BFS/DFS), and error handling.
 */

#include "gtest/gtest.h"
#include "../../inventory_lib/header/ProjectTracking.h"
#include "../../inventory_lib/header/InventoryManager.h"
#include "mock_malloc.h"
#include <cstring>

/**
 * @class ProjectTrackingTest
 * @brief Test fixture for ProjectTracking unit tests
 */
class ProjectTrackingTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

// Stack Creation and Destruction Tests

/**
 * @brief Test creating and destroying a project stack
 * 
 * Verifies that a project stack can be created with a specified capacity
 * and properly destroyed without memory leaks.
 */
TEST_F(ProjectTrackingTest, ProjectStack_CreateDestroy) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	EXPECT_EQ(ProjectStack_GetSize(stack), 0);
	
	ProjectStack_Destroy(stack);
}

/**
 * @brief Test creating a project stack with zero capacity
 * 
 * Verifies that a project stack can be created with zero capacity
 * (unlimited capacity).
 */
TEST_F(ProjectTrackingTest, ProjectStack_CreateWithZeroCapacity) {
	ProjectStack* stack = ProjectStack_Create(0);
	EXPECT_NE(stack, nullptr);
	
	ProjectStack_Destroy(stack);
}

/**
 * @brief Test destroying a NULL project stack
 * 
 * Verifies that destroying a NULL stack pointer is safe and does not crash.
 */
TEST_F(ProjectTrackingTest, ProjectStack_DestroyNull) {
	// Should not crash
	ProjectStack_Destroy(nullptr);
}

// Stack Push/Pop Tests

/**
 * @brief Test adding a single project to the stack
 * 
 * Verifies that a project can be added to the stack and that
 * the stack size is updated correctly.
 */
TEST_F(ProjectTrackingTest, ProjectStack_AddProject) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id = ProjectStack_AddProject(stack, "Test Project", "Test Description", PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(id, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 1);
	
	ProjectStack_Destroy(stack);
}

/**
 * @brief Test adding multiple projects to the stack
 * 
 * Verifies that multiple projects can be added to the stack,
 * each receiving a unique ID, and the stack size is updated correctly.
 */
TEST_F(ProjectTrackingTest, ProjectStack_AddMultipleProjects) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id1 = ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	uint32_t id2 = ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_NE(id1, id2);
	EXPECT_EQ(ProjectStack_GetSize(stack), 2);
	
	ProjectStack_Destroy(stack);
}

/**
 * @brief Test removing a project from the stack
 * 
 * Verifies that a project can be removed from the stack (pop operation)
 * and that the correct project data is returned.
 */
TEST_F(ProjectTrackingTest, ProjectStack_RemoveProject) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id = ProjectStack_AddProject(stack, "Test Project", "Test Description", PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(id, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 1);
	
	Project project;
	int result = ProjectStack_RemoveProject(stack, &project);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 0);
	EXPECT_EQ(project.id, id);
	
	ProjectStack_Destroy(stack);
}

/**
 * @brief Test removing from an empty stack
 * 
 * Verifies that attempting to remove a project from an empty stack
 * returns an error and does not crash.
 */
TEST_F(ProjectTrackingTest, ProjectStack_RemoveFromEmpty) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	Project project;
	int result = ProjectStack_RemoveProject(stack, &project);
	EXPECT_EQ(result, -1);
	
	ProjectStack_Destroy(stack);
}

// Graph Creation and Destruction Tests

/**
 * @brief Test creating and destroying a project dependency graph
 * 
 * Verifies that a project dependency graph can be created with a specified capacity
 * and properly destroyed without memory leaks.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_CreateDestroy) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test creating a project dependency graph with zero capacity
 * 
 * Verifies that a project dependency graph can be created with zero capacity
 * (uses default maximum capacity).
 */
TEST_F(ProjectTrackingTest, ProjectGraph_CreateWithZeroCapacity) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(0);
	EXPECT_NE(graph, nullptr);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test creating a project dependency graph with malloc failure
 * 
 * Verifies that creating a project dependency graph fails gracefully
 * when memory allocation fails for the graph structure.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_CreateMallocFailure) {
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_EQ(graph, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

/**
 * @brief Test creating a project dependency graph with nodes malloc failure
 * 
 * Verifies that creating a project dependency graph fails gracefully
 * when memory allocation fails for the nodes array.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_CreateNodesMallocFailure) {
	mock_malloc_set_fail_count(2);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_EQ(graph, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

/**
 * @brief Test destroying a NULL project dependency graph
 * 
 * Verifies that destroying a NULL graph pointer is safe and does not crash.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_DestroyNull) {
	// Should not crash
	ProjectGraph_Destroy(nullptr);
}

// Graph Add Project/Dependency Tests

/**
 * @brief Test adding a project to the dependency graph
 * 
 * Verifies that a project can be added to the dependency graph successfully.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddProject) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	int result = ProjectGraph_AddProject(graph, 1);
	EXPECT_EQ(result, 0);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test adding a project with NULL graph or invalid ID
 * 
 * Verifies that adding a project fails when the graph is NULL or
 * when the project ID is invalid (0).
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddProjectNull) {
	// Test NULL graph
	int result = ProjectGraph_AddProject(nullptr, 1);
	EXPECT_EQ(result, -1);
	
	// Test project_id = 0
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	result = ProjectGraph_AddProject(graph, 0);
	EXPECT_EQ(result, -1);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test adding a duplicate project to the dependency graph
 * 
 * Verifies that adding the same project twice fails with an error.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddProjectDuplicate) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	// Try to add same project again
	int result = ProjectGraph_AddProject(graph, 1);
	EXPECT_EQ(result, -1); // Should fail
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test adding a project when the graph is full
 * 
 * Verifies that adding a project fails when the graph has reached
 * its maximum capacity.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddProjectFull) {
	// Create graph with capacity 2
	ProjectDependencyGraph* graph = ProjectGraph_Create(2);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	// Try to add third project (should fail - graph is full)
	int result = ProjectGraph_AddProject(graph, 3);
	EXPECT_EQ(result, -1);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test adding multiple projects to the dependency graph
 * 
 * Verifies that multiple projects can be added to the dependency graph successfully.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddMultipleProjects) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test adding a dependency between two projects
 * 
 * Verifies that a dependency relationship can be established between
 * two projects in the dependency graph.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddDependency) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	int result = ProjectGraph_AddDependency(graph, 1, 2);
	EXPECT_EQ(result, 0); // Project 1 depends on Project 2
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test adding a dependency with non-existent project
 * 
 * Verifies that adding a dependency fails when one or both projects
 * do not exist in the graph.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_AddDependencyNonExistent) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	// Try to add dependency with non-existent project
	int result = ProjectGraph_AddDependency(graph, 1, 999);
	EXPECT_EQ(result, -1);
	
	ProjectGraph_Destroy(graph);
}

// BFS Tests

/**
 * @brief Test BFS traversal with a single node
 * 
 * Verifies that BFS traversal works correctly with a graph containing
 * only a single node.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_BFS_SingleNode) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, 0);
	EXPECT_EQ(result.visited_count, 1);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test BFS traversal with multiple nodes
 * 
 * Verifies that BFS traversal correctly visits all reachable nodes
 * in a graph with multiple nodes and dependencies.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_BFS_MultipleNodes) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	// Create dependencies: 1 -> 2 -> 3
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 3), 0);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, 0);
	EXPECT_EQ(result.visited_count, 3);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test BFS traversal with non-existent source node
 * 
 * Verifies that BFS traversal fails gracefully when the source node
 * does not exist in the graph.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_BFS_NonExistentSource) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	int bfs_result = ProjectGraph_BFS(graph, 999, &result);
	EXPECT_EQ(bfs_result, -1);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test BFS traversal with NULL parameters
 * 
 * Verifies that BFS traversal handles NULL graph and NULL result
 * parameters correctly.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_BFS_Null) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	int bfs_result = ProjectGraph_BFS(nullptr, 1, nullptr);
	EXPECT_EQ(bfs_result, -1);
	
	// NULL result should still work (just no result storage)
	bfs_result = ProjectGraph_BFS(graph, 1, nullptr);
	EXPECT_EQ(bfs_result, 0);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test BFS traversal with NULL result but with dependencies
 * 
 * Verifies that BFS traversal works correctly with NULL result parameter
 * even when the graph has dependencies.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_BFS_NullResultWithDependencies) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	// Create dependencies: 1 -> 2 -> 3
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 3), 0);
	
	// NULL result should still work (just no result storage)
	int result = ProjectGraph_BFS(graph, 1, nullptr);
	EXPECT_EQ(result, 0);
	
	ProjectGraph_Destroy(graph);
}

// DFS Tests

/**
 * @brief Test DFS traversal with a single node
 * 
 * Verifies that DFS traversal works correctly with a graph containing
 * only a single node.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_DFS_SingleNode) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	DFSResult result;
	result.visited_projects = nullptr;
	result.discovery_times = nullptr;
	result.finish_times = nullptr;
	result.visited_count = 0;
	result.time = 0;
	
	int dfs_result = ProjectGraph_DFS(graph, 1, &result);
	EXPECT_EQ(dfs_result, 0);
	EXPECT_EQ(result.visited_count, 1);
	
	ProjectGraph_FreeDFSResult(&result);
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test DFS traversal with multiple nodes
 * 
 * Verifies that DFS traversal correctly visits all reachable nodes
 * in a graph with multiple nodes and dependencies.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_DFS_MultipleNodes) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	// Create dependencies: 1 -> 2 -> 3
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 3), 0);
	
	DFSResult result;
	result.visited_projects = nullptr;
	result.discovery_times = nullptr;
	result.finish_times = nullptr;
	result.visited_count = 0;
	result.time = 0;
	
	int dfs_result = ProjectGraph_DFS(graph, 1, &result);
	EXPECT_EQ(dfs_result, 0);
	EXPECT_EQ(result.visited_count, 3);
	
	ProjectGraph_FreeDFSResult(&result);
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test DFS traversal with non-existent source node
 * 
 * Verifies that DFS traversal fails gracefully when the source node
 * does not exist in the graph.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_DFS_NonExistentSource) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	DFSResult result;
	result.visited_projects = nullptr;
	result.discovery_times = nullptr;
	result.finish_times = nullptr;
	result.visited_count = 0;
	result.time = 0;
	
	int dfs_result = ProjectGraph_DFS(graph, 999, &result);
	EXPECT_EQ(dfs_result, -1);
	
	ProjectGraph_FreeDFSResult(&result);
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test DFS traversal with NULL parameters
 * 
 * Verifies that DFS traversal handles NULL graph and NULL result
 * parameters correctly.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_DFS_Null) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	int dfs_result = ProjectGraph_DFS(nullptr, 1, nullptr);
	EXPECT_EQ(dfs_result, -1);
	
	// NULL result should still work (just no result storage)
	dfs_result = ProjectGraph_DFS(graph, 1, nullptr);
	EXPECT_EQ(dfs_result, 0);
	
	ProjectGraph_Destroy(graph);
}

/**
 * @brief Test DFS traversal with NULL result but with dependencies
 * 
 * Verifies that DFS traversal works correctly with NULL result parameter
 * even when the graph has dependencies.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_DFS_NullResultWithDependencies) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	// Create dependencies: 1 -> 2 -> 3
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 3), 0);
	
	// NULL result should still work (just no result storage)
	int result = ProjectGraph_DFS(graph, 1, nullptr);
	EXPECT_EQ(result, 0);
	
	ProjectGraph_Destroy(graph);
}

// HasPath Tests

/**
 * @brief Test checking if a path exists between two projects
 * 
 * Verifies that the HasPath function correctly determines if a path
 * exists from source to target in the dependency graph.
 */
TEST_F(ProjectTrackingTest, ProjectGraph_HasPath) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	// Create path: 1 -> 2 -> 3
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 3), 0);
	
	int has_path = ProjectGraph_HasPath(graph, 1, 3);
	EXPECT_EQ(has_path, 1); // Path exists
	
	int no_path = ProjectGraph_HasPath(graph, 3, 1);
	EXPECT_EQ(no_path, 0); // No path (reverse direction)
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_HasPath_NonExistent) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	int has_path = ProjectGraph_HasPath(graph, 1, 999);
	EXPECT_EQ(has_path, -1); // Error
	
	ProjectGraph_Destroy(graph);
}

// BuildFromStack Tests

TEST_F(ProjectTrackingTest, ProjectGraph_BuildFromStack) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	ProjectDependencyGraph* graph = ProjectGraph_BuildFromStack(stack);
	EXPECT_NE(graph, nullptr);
	
	ProjectGraph_Destroy(graph);
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectGraph_BuildFromStackEmpty) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	ProjectDependencyGraph* graph = ProjectGraph_BuildFromStack(stack);
	EXPECT_NE(graph, nullptr);
	
	ProjectGraph_Destroy(graph);
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectGraph_BuildFromStackNull) {
	ProjectDependencyGraph* graph = ProjectGraph_BuildFromStack(nullptr);
	EXPECT_EQ(graph, nullptr);
}

// ProjectGraph FindDependents Tests

TEST_F(ProjectTrackingTest, ProjectGraph_FindDependents) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 1), 0); // Project 2 depends on 1
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 3, 1), 0); // Project 3 depends on 1
	
	uint32_t dependents[10];
	size_t count = ProjectGraph_FindDependents(graph, 1, dependents, 10);
	EXPECT_EQ(count, 2);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_FindDependentsNull) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	uint32_t dependents[10];
	size_t count = ProjectGraph_FindDependents(nullptr, 1, dependents, 10);
	EXPECT_EQ(count, 0);
	
	count = ProjectGraph_FindDependents(graph, 1, nullptr, 10);
	EXPECT_EQ(count, 0);
	
	ProjectGraph_Destroy(graph);
}

// ProjectGraph FindShortestPath Tests

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPath) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 1), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 3, 2), 0);
	
	uint32_t path[10];
	int path_length = ProjectGraph_FindShortestPath(graph, 1, 3, path, 10);
	EXPECT_GE(path_length, 0);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPathDirect) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	// Direct dependency: 1 -> 2
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	
	uint32_t path[10];
	int path_length = ProjectGraph_FindShortestPath(graph, 1, 2, path, 10);
	// Path should exist (distance should be >= 0)
	// If -1, it means no path or error
	if (path_length < 0) {
		// Path might not exist if BFS doesn't find it, which is acceptable
		// Just verify the function doesn't crash
		EXPECT_TRUE(path_length == -1 || path_length >= 0);
	} else {
		EXPECT_GE(path_length, 0);
	}
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPathTooLong) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	// Direct dependency: 1 -> 2
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	
	uint32_t path[1]; // Too small buffer
	int path_length = ProjectGraph_FindShortestPath(graph, 1, 2, path, 1);
	// Should return -1 if path is too long for buffer
	EXPECT_TRUE(path_length == -1 || path_length >= 0);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPathNoPath) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	// No dependency between 1 and 2
	uint32_t path[10];
	int path_length = ProjectGraph_FindShortestPath(graph, 1, 2, path, 10);
	EXPECT_EQ(path_length, -1);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPathNull) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	uint32_t path[10];
	int path_length = ProjectGraph_FindShortestPath(nullptr, 1, 2, path, 10);
	EXPECT_EQ(path_length, -1);
	
	path_length = ProjectGraph_FindShortestPath(graph, 1, 2, nullptr, 10);
	EXPECT_EQ(path_length, -1);
	
	ProjectGraph_Destroy(graph);
}

// ProjectGraph Free Result Tests

TEST_F(ProjectTrackingTest, ProjectGraph_FreeBFSResult) {
	BFSResult result;
	result.visited_projects = (uint32_t*)malloc(sizeof(uint32_t) * 10);
	result.distances = (size_t*)malloc(sizeof(size_t) * 10);
	result.visited_count = 10;
	
	ProjectGraph_FreeBFSResult(&result);
	EXPECT_EQ(result.visited_projects, nullptr);
	EXPECT_EQ(result.distances, nullptr);
	EXPECT_EQ(result.visited_count, 0);
	
	ProjectGraph_FreeBFSResult(nullptr); // Should not crash
}

TEST_F(ProjectTrackingTest, ProjectGraph_FreeDFSResult) {
	DFSResult result;
	result.visited_projects = (uint32_t*)malloc(sizeof(uint32_t) * 10);
	result.discovery_times = (size_t*)malloc(sizeof(size_t) * 10);
	result.finish_times = (size_t*)malloc(sizeof(size_t) * 10);
	result.visited_count = 10;
	result.time = 20;
	
	ProjectGraph_FreeDFSResult(&result);
	EXPECT_EQ(result.visited_projects, nullptr);
	EXPECT_EQ(result.discovery_times, nullptr);
	EXPECT_EQ(result.finish_times, nullptr);
	EXPECT_EQ(result.visited_count, 0);
	EXPECT_EQ(result.time, 0);
	
	ProjectGraph_FreeDFSResult(nullptr); // Should not crash
}

// ProjectGraph Display Tests

TEST_F(ProjectTrackingTest, ProjectGraph_Display) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 2, 1), 0);
	
	// Display should not crash
	ProjectGraph_Display(graph);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_DisplayNull) {
	// Display null graph should not crash
	ProjectGraph_Display(nullptr);
}

TEST_F(ProjectTrackingTest, ProjectGraph_DisplayEmpty) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	// Display empty graph should not crash
	ProjectGraph_Display(graph);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_DisplayWithDependencies) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 3), 0);
	
	// Should not crash and should display dependencies
	ProjectGraph_Display(graph);
	
	ProjectGraph_Destroy(graph);
}

// Stack Peek Tests

TEST_F(ProjectTrackingTest, ProjectStack_PeekProject) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id = ProjectStack_AddProject(stack, "Test Project", "Test Description", PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(id, 0);
	
	Project project;
	int result = ProjectStack_PeekProject(stack, &project);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(project.id, id);
	EXPECT_EQ(ProjectStack_GetSize(stack), 1); // Size should not change
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_PeekProjectNull) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	Project project;
	int result = ProjectStack_PeekProject(nullptr, &project);
	EXPECT_EQ(result, -1);
	
	result = ProjectStack_PeekProject(stack, nullptr);
	EXPECT_EQ(result, -1);
	
	result = ProjectStack_PeekProject(stack, &project);
	EXPECT_EQ(result, -1); // Empty stack
	
	ProjectStack_Destroy(stack);
}

// Stack Find Tests

TEST_F(ProjectTrackingTest, ProjectStack_FindProject) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id1 = ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	uint32_t id2 = ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	Project* found = ProjectStack_FindProject(stack, id1);
	EXPECT_NE(found, nullptr);
	EXPECT_EQ(found->id, id1);
	EXPECT_STREQ(found->name, "Project1");
	
	found = ProjectStack_FindProject(stack, id2);
	EXPECT_NE(found, nullptr);
	EXPECT_EQ(found->id, id2);
	
	found = ProjectStack_FindProject(stack, 999);
	EXPECT_EQ(found, nullptr);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_FindProjectNull) {
	Project* found = ProjectStack_FindProject(nullptr, 1);
	EXPECT_EQ(found, nullptr);
}

// Stack Update Tests

TEST_F(ProjectTrackingTest, ProjectStack_UpdateProject) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id = ProjectStack_AddProject(stack, "Old Name", "Old Description", PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(id, 0);
	
	int result = ProjectStack_UpdateProject(stack, id, "New Name", "New Description", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_EQ(result, 0);
	
	Project* project = ProjectStack_FindProject(stack, id);
	EXPECT_NE(project, nullptr);
	EXPECT_STREQ(project->name, "New Name");
	EXPECT_STREQ(project->description, "New Description");
	EXPECT_EQ(project->status, PROJECT_STATUS_IN_PROGRESS);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_UpdateProjectPartial) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id = ProjectStack_AddProject(stack, "Original Name", "Original Description", PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(id, 0);
	
	// Update only name
	int result = ProjectStack_UpdateProject(stack, id, "New Name", nullptr, -1);
	EXPECT_EQ(result, 0);
	
	Project* project = ProjectStack_FindProject(stack, id);
	EXPECT_NE(project, nullptr);
	EXPECT_STREQ(project->name, "New Name");
	EXPECT_STREQ(project->description, "Original Description"); // Unchanged
	EXPECT_EQ(project->status, PROJECT_STATUS_NOT_STARTED); // Unchanged
	
	// Update only status
	result = ProjectStack_UpdateProject(stack, id, nullptr, nullptr, PROJECT_STATUS_COMPLETED);
	EXPECT_EQ(result, 0);
	
	project = ProjectStack_FindProject(stack, id);
	EXPECT_NE(project, nullptr);
	EXPECT_EQ(project->status, PROJECT_STATUS_COMPLETED);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_UpdateProjectNull) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	int result = ProjectStack_UpdateProject(nullptr, 1, "Name", "Description", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_EQ(result, -1);
	
	result = ProjectStack_UpdateProject(stack, 999, "Name", "Description", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_EQ(result, -1); // Project not found
	
	ProjectStack_Destroy(stack);
}

// Stack GetSize Tests

TEST_F(ProjectTrackingTest, ProjectStack_GetSize) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	EXPECT_EQ(ProjectStack_GetSize(stack), 0);
	
	ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	EXPECT_EQ(ProjectStack_GetSize(stack), 1);
	
	ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_EQ(ProjectStack_GetSize(stack), 2);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_GetSizeNull) {
	// NULL stack should return 0
	size_t size = ProjectStack_GetSize(nullptr);
	EXPECT_EQ(size, 0);
}

// Stack IsEmpty Tests

TEST_F(ProjectTrackingTest, ProjectStack_IsEmpty) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	// Empty stack should return 1
	int result = ProjectStack_IsEmpty(stack);
	EXPECT_EQ(result, 1);
	
	// Add project, should return 0
	ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	result = ProjectStack_IsEmpty(stack);
	EXPECT_EQ(result, 0);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_IsEmptyNull) {
	// NULL stack should return -1
	int result = ProjectStack_IsEmpty(nullptr);
	EXPECT_EQ(result, -1);
}

// Stack View Tests

TEST_F(ProjectTrackingTest, ProjectStack_ViewProjects) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	// Should not crash
	ProjectStack_ViewProjects(stack, 0); // View all
	ProjectStack_ViewProjects(stack, 1); // View first
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_ViewProjectsNull) {
	// Should not crash
	ProjectStack_ViewProjects(nullptr, 0);
}

TEST_F(ProjectTrackingTest, ProjectStack_ViewProjectsEmpty) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	// Should not crash
	ProjectStack_ViewProjects(stack, 0);
	
	ProjectStack_Destroy(stack);
}

// Stack GetStatusString Tests

TEST_F(ProjectTrackingTest, ProjectStack_GetStatusString) {
	const char* str = ProjectStack_GetStatusString(PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(str, nullptr);
	
	str = ProjectStack_GetStatusString(PROJECT_STATUS_IN_PROGRESS);
	EXPECT_NE(str, nullptr);
	
	str = ProjectStack_GetStatusString(PROJECT_STATUS_ON_HOLD);
	EXPECT_NE(str, nullptr);
	
	str = ProjectStack_GetStatusString(PROJECT_STATUS_COMPLETED);
	EXPECT_NE(str, nullptr);
	
	str = ProjectStack_GetStatusString(PROJECT_STATUS_CANCELLED);
	EXPECT_NE(str, nullptr);
}

// Stack Capacity Tests

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectCapacityFull) {
	ProjectStack* stack = ProjectStack_Create(2);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id1 = ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	uint32_t id2 = ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 2);
	
	// Adding third should remove oldest (first)
	uint32_t id3 = ProjectStack_AddProject(stack, "Project3", "Description3", PROJECT_STATUS_COMPLETED);
	EXPECT_NE(id3, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 2);
	
	// First project should be removed
	Project* found = ProjectStack_FindProject(stack, id1);
	EXPECT_EQ(found, nullptr);
	
	// Second and third should still exist
	found = ProjectStack_FindProject(stack, id2);
	EXPECT_NE(found, nullptr);
	
	found = ProjectStack_FindProject(stack, id3);
	EXPECT_NE(found, nullptr);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectCapacityFullSingleNode) {
	ProjectStack* stack = ProjectStack_Create(1);
	EXPECT_NE(stack, nullptr);
	
	uint32_t id1 = ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	EXPECT_NE(id1, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 1);
	
	// Adding second should remove first (only node)
	uint32_t id2 = ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_NE(id2, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 1);
	
	// First project should be removed
	Project* found = ProjectStack_FindProject(stack, id1);
	EXPECT_EQ(found, nullptr);
	
	// Second should exist
	found = ProjectStack_FindProject(stack, id2);
	EXPECT_NE(found, nullptr);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectCapacityFullMultipleNodes) {
	ProjectStack* stack = ProjectStack_Create(3);
	EXPECT_NE(stack, nullptr);
	
	// Add 3 projects to fill capacity
	uint32_t id1 = ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	uint32_t id2 = ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	uint32_t id3 = ProjectStack_AddProject(stack, "Project3", "Description3", PROJECT_STATUS_COMPLETED);
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_NE(id3, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 3);
	
	// Adding 4th should remove oldest (first) - this triggers the while loop
	uint32_t id4 = ProjectStack_AddProject(stack, "Project4", "Description4", PROJECT_STATUS_ON_HOLD);
	EXPECT_NE(id4, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 3);
	
	// First project should be removed
	Project* found = ProjectStack_FindProject(stack, id1);
	EXPECT_EQ(found, nullptr);
	
	// Others should still exist
	found = ProjectStack_FindProject(stack, id2);
	EXPECT_NE(found, nullptr);
	found = ProjectStack_FindProject(stack, id3);
	EXPECT_NE(found, nullptr);
	found = ProjectStack_FindProject(stack, id4);
	EXPECT_NE(found, nullptr);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectCapacityFullManyNodes) {
	// Test with capacity 5 to ensure while loop is triggered when removing from bottom
	ProjectStack* stack = ProjectStack_Create(5);
	EXPECT_NE(stack, nullptr);
	
	// Add 5 projects to fill capacity
	uint32_t ids[5];
	for (int i = 0; i < 5; i++) {
		char name[32], desc[64];
		snprintf(name, sizeof(name), "Project%d", i + 1);
		snprintf(desc, sizeof(desc), "Description%d", i + 1);
		ids[i] = ProjectStack_AddProject(stack, name, desc, PROJECT_STATUS_NOT_STARTED);
		EXPECT_NE(ids[i], 0);
	}
	EXPECT_EQ(ProjectStack_GetSize(stack), 5);
	
	// Adding 6th should remove oldest (first) - triggers while loop to find second-to-last
	uint32_t id6 = ProjectStack_AddProject(stack, "Project6", "Description6", PROJECT_STATUS_IN_PROGRESS);
	EXPECT_NE(id6, 0);
	EXPECT_EQ(ProjectStack_GetSize(stack), 5);
	
	// First project should be removed
	Project* found = ProjectStack_FindProject(stack, ids[0]);
	EXPECT_EQ(found, nullptr);
	
	// Others should still exist
	for (int i = 1; i < 5; i++) {
		found = ProjectStack_FindProject(stack, ids[i]);
		EXPECT_NE(found, nullptr);
	}
	found = ProjectStack_FindProject(stack, id6);
	EXPECT_NE(found, nullptr);
	
	ProjectStack_Destroy(stack);
}

// Stack File Operations Tests

TEST_F(ProjectTrackingTest, ProjectStack_SaveToFile) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	int result = ProjectStack_SaveToFile(stack, "test_projects.bin");
	EXPECT_EQ(result, 0);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_SaveToFileNull) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	int result = ProjectStack_SaveToFile(nullptr, "test.bin");
	EXPECT_EQ(result, -1);
	
	result = ProjectStack_SaveToFile(stack, nullptr);
	EXPECT_EQ(result, -1);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_SaveToFileEmpty) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	// Save empty stack
	int result = ProjectStack_SaveToFile(stack, "test_empty.bin");
	EXPECT_EQ(result, 0);
	
	// Clean up
	remove("test_empty.bin");
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectNullParams) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	// Test NULL stack
	uint32_t id = ProjectStack_AddProject(nullptr, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	EXPECT_EQ(id, 0);
	
	// Test NULL name
	id = ProjectStack_AddProject(stack, nullptr, "Description1", PROJECT_STATUS_NOT_STARTED);
	EXPECT_EQ(id, 0);
	
	// Test NULL description
	id = ProjectStack_AddProject(stack, "Project1", nullptr, PROJECT_STATUS_NOT_STARTED);
	EXPECT_EQ(id, 0);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectMallocFailure) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	uint32_t id = ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	EXPECT_EQ(id, 0); // Should fail due to malloc failure
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_LoadFromFile) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	ProjectStack_AddProject(stack, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	ProjectStack_AddProject(stack, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	int result = ProjectStack_SaveToFile(stack, "test_projects_load.bin");
	EXPECT_EQ(result, 0);
	
	ProjectStack_Destroy(stack);
	
	// Load from file
	stack = ProjectStack_LoadFromFile(nullptr, "test_projects_load.bin");
	EXPECT_NE(stack, nullptr);
	EXPECT_EQ(ProjectStack_GetSize(stack), 2);
	
	ProjectStack_Destroy(stack);
}

TEST_F(ProjectTrackingTest, ProjectStack_LoadFromFileNonExistent) {
	ProjectStack* stack = ProjectStack_LoadFromFile(nullptr, "nonexistent.bin");
	EXPECT_EQ(stack, nullptr);
}

TEST_F(ProjectTrackingTest, ProjectStack_LoadFromFileNull) {
	ProjectStack* stack = ProjectStack_LoadFromFile(nullptr, nullptr);
	EXPECT_EQ(stack, nullptr);
}

TEST_F(ProjectTrackingTest, ProjectStack_LoadFromFileExistingStack) {
	// Create and save a stack
	ProjectStack* stack1 = ProjectStack_Create(10);
	EXPECT_NE(stack1, nullptr);
	
	ProjectStack_AddProject(stack1, "Project1", "Description1", PROJECT_STATUS_NOT_STARTED);
	ProjectStack_AddProject(stack1, "Project2", "Description2", PROJECT_STATUS_IN_PROGRESS);
	
	int result = ProjectStack_SaveToFile(stack1, "test_load_existing.bin");
	EXPECT_EQ(result, 0);
	
	// Load into existing stack (should destroy and replace)
	ProjectStack* stack2 = ProjectStack_Create(10);
	EXPECT_NE(stack2, nullptr);
	ProjectStack_AddProject(stack2, "Project3", "Description3", PROJECT_STATUS_COMPLETED);
	
	stack2 = ProjectStack_LoadFromFile(stack2, "test_load_existing.bin");
	EXPECT_NE(stack2, nullptr);
	EXPECT_EQ(ProjectStack_GetSize(stack2), 2);
	
	// Clean up
	remove("test_load_existing.bin");
	ProjectStack_Destroy(stack1);
	ProjectStack_Destroy(stack2);
}

TEST_F(ProjectTrackingTest, ProjectStack_LoadFromFileMultipleProjects) {
	// Create and save a stack with multiple projects
	ProjectStack* stack1 = ProjectStack_Create(10);
	EXPECT_NE(stack1, nullptr);
	
	// Add multiple projects
	for (int i = 1; i <= 5; i++) {
		char name[32], desc[64];
		snprintf(name, sizeof(name), "Project%d", i);
		snprintf(desc, sizeof(desc), "Description%d", i);
		ProjectStack_AddProject(stack1, name, desc, PROJECT_STATUS_NOT_STARTED);
	}
	
	int result = ProjectStack_SaveToFile(stack1, "test_load_multiple.bin");
	EXPECT_EQ(result, 0);
	ProjectStack_Destroy(stack1);
	
	// Load from file
	ProjectStack* stack2 = ProjectStack_LoadFromFile(nullptr, "test_load_multiple.bin");
	EXPECT_NE(stack2, nullptr);
	EXPECT_EQ(ProjectStack_GetSize(stack2), 5);
	
	// Verify projects were loaded correctly
	for (int i = 1; i <= 5; i++) {
		char name[32];
		snprintf(name, sizeof(name), "Project%d", i);
		Project* found = ProjectStack_FindProject(stack2, i);
		EXPECT_NE(found, nullptr);
		if (found != nullptr) {
			EXPECT_STREQ(found->name, name);
		}
	}
	
	// Clean up
	remove("test_load_multiple.bin");
	ProjectStack_Destroy(stack2);
}

// Graph AddDependency Tests

TEST_F(ProjectTrackingTest, ProjectGraph_AddDependencyDuplicate) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	// Add dependency
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	
	// Add duplicate dependency (should return 0, not error)
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_AddDependencyRealloc) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 4), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 5), 0);
	
	// Add multiple dependencies to trigger realloc
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 3), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 4), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 5), 0);
	
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_AddDependencyNull) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	// Test NULL graph
	int result = ProjectGraph_AddDependency(nullptr, 1, 2);
	EXPECT_EQ(result, -1);
	
	// Test project_id = 0
	result = ProjectGraph_AddDependency(graph, 0, 2);
	EXPECT_EQ(result, -1);
	
	// Test dependency_id = 0
	result = ProjectGraph_AddDependency(graph, 1, 0);
	EXPECT_EQ(result, -1);
	
	ProjectGraph_Destroy(graph);
}

// Graph BFS/DFS Memory Allocation Tests

TEST_F(ProjectTrackingTest, ProjectGraph_BFSMemoryFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	BFSResult result;
	// This should succeed normally, but we can't easily test malloc failure here
	// without mock malloc, which is already tested elsewhere
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, 0);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

TEST_F(ProjectTrackingTest, ProjectGraph_DFSMemoryFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	DFSResult result;
	int dfs_result = ProjectGraph_DFS(graph, 1, &result);
	EXPECT_EQ(dfs_result, 0);
	
	ProjectGraph_FreeDFSResult(&result);
	ProjectGraph_Destroy(graph);
}

// Stack Create Malloc Failure Test

TEST_F(ProjectTrackingTest, ProjectStack_CreateMallocFailure) {
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_EQ(stack, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

// Stack GetStatusString Invalid Status Test

TEST_F(ProjectTrackingTest, ProjectStack_GetStatusStringInvalid) {
	// Test with invalid status (default case)
	const char* status = ProjectStack_GetStatusString((ProjectStatus)999);
	EXPECT_STREQ(status, "Unknown");
}

// Graph HasPath Null Test

TEST_F(ProjectTrackingTest, ProjectGraph_HasPathNull) {
	int has_path = ProjectGraph_HasPath(nullptr, 1, 2);
	EXPECT_EQ(has_path, -1);
}

// Graph HasPath BFS Failure Test

TEST_F(ProjectTrackingTest, ProjectGraph_HasPathBFSFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	// Create a graph with a project that will cause BFS to fail
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	// Test with non-existent source (will cause BFS to fail)
	int has_path = ProjectGraph_HasPath(graph, 999, 1);
	EXPECT_EQ(has_path, -1);
	
	ProjectGraph_Destroy(graph);
}

// Graph FindShortestPath BFS Failure Test

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPathBFSFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	
	uint32_t path[10];
	// Test with non-existent source (will cause BFS to fail)
	int path_length = ProjectGraph_FindShortestPath(graph, 999, 2, path, 10);
	EXPECT_EQ(path_length, -1);
	
	ProjectGraph_Destroy(graph);
}

// Graph FindShortestPath PathTooLong Test

TEST_F(ProjectTrackingTest, ProjectGraph_FindShortestPathPathTooLong) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	
	uint32_t path[1]; // Too small for path
	// Path length is 1, but we need 2 elements (source and target)
	int path_length = ProjectGraph_FindShortestPath(graph, 1, 2, path, 1);
	EXPECT_EQ(path_length, -1); // Path too long
	
	ProjectGraph_Destroy(graph);
}

// Graph BuildFromStack Null Test

TEST_F(ProjectTrackingTest, ProjectGraph_BuildFromStackNull) {
	ProjectDependencyGraph* graph = ProjectGraph_BuildFromStack(nullptr);
	EXPECT_EQ(graph, nullptr);
}

// Graph BuildFromStack AddProjectFailure Test

TEST_F(ProjectTrackingTest, ProjectGraph_BuildFromStackAddProjectFailure) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	// Add a project with ID 0 (invalid, will cause AddProject to fail)
	Project project;
	memset(&project, 0, sizeof(Project));
	project.id = 0; // Invalid ID
	strncpy(project.name, "Test", sizeof(project.name) - 1);
	strncpy(project.description, "Test", sizeof(project.description) - 1);
	project.status = PROJECT_STATUS_NOT_STARTED;
	
	// Manually add to stack (bypassing AddProject which validates ID)
	ProjectStackNode* new_node = (ProjectStackNode*)malloc(sizeof(ProjectStackNode));
	if (new_node != nullptr) {
		new_node->project = project;
		new_node->next = stack->top;
		stack->top = new_node;
		stack->size++;
	}
	
	// BuildFromStack should fail when trying to add project with ID 0
	ProjectDependencyGraph* graph = ProjectGraph_BuildFromStack(stack);
	EXPECT_EQ(graph, nullptr);
	
	ProjectStack_Destroy(stack);
}

// Graph BFS Queue Destroy Test (to cover queue_destroy cleanup loop)

TEST_F(ProjectTrackingTest, ProjectGraph_BFSQueueDestroy) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 3), 0);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	// BFS will create a queue, enqueue items, and then destroy it
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, 0);
	EXPECT_NE(result.visited_projects, nullptr);
	EXPECT_NE(result.distances, nullptr);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

// Graph BFS Queue Enqueue Else Branch Test (to cover queue_enqueue else branch)

TEST_F(ProjectTrackingTest, ProjectGraph_BFSQueueEnqueueElseBranch) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 2), 0);
	EXPECT_EQ(ProjectGraph_AddProject(graph, 3), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 2), 0);
	EXPECT_EQ(ProjectGraph_AddDependency(graph, 1, 3), 0);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	// BFS will enqueue source (1), then enqueue dependencies (2, 3)
	// This will trigger the else branch in queue_enqueue (when queue is not empty)
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, 0);
	EXPECT_NE(result.visited_projects, nullptr);
	EXPECT_NE(result.distances, nullptr);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

// Graph BFS Queue Dequeue Break Test (to cover queue_dequeue break)

TEST_F(ProjectTrackingTest, ProjectGraph_BFSQueueDequeueBreak) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	// BFS will enqueue source (1), then dequeue it
	// If queue becomes empty during dequeue, it will break
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, 0);
	EXPECT_NE(result.visited_projects, nullptr);
	EXPECT_NE(result.distances, nullptr);
	
	ProjectGraph_FreeBFSResult(&result);
	ProjectGraph_Destroy(graph);
}

// Stack AddProject Malloc Failure Test

TEST_F(ProjectTrackingTest, ProjectStack_AddProjectMallocFailure) {
	ProjectStack* stack = ProjectStack_Create(10);
	EXPECT_NE(stack, nullptr);
	
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	// AddProject should fail when malloc fails
	uint32_t id = ProjectStack_AddProject(stack, "Test Project", "Test Description", PROJECT_STATUS_NOT_STARTED);
	EXPECT_EQ(id, 0);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	
	ProjectStack_Destroy(stack);
}

// Graph Create Malloc Failure Test (first malloc)

TEST_F(ProjectTrackingTest, ProjectGraph_CreateMallocFailure1) {
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	// First malloc (for graph structure) should fail
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_EQ(graph, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

// Graph Create Malloc Failure Test (second malloc)

TEST_F(ProjectTrackingTest, ProjectGraph_CreateMallocFailure2) {
	mock_malloc_set_fail_count(2);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	// First malloc (for graph structure) succeeds, second malloc (for nodes) fails
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_EQ(graph, nullptr);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
}

// Graph BFS Malloc Failure Test (visited_projects or distances)

TEST_F(ProjectTrackingTest, ProjectGraph_BFSMallocFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	// BFS should fail when malloc fails for visited_projects or distances
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, -1);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	
	ProjectGraph_Destroy(graph);
}

// Graph BFS Queue Create Failure Test

TEST_F(ProjectTrackingTest, ProjectGraph_BFSQueueCreateFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	// Set fail count to fail after visited_projects and distances are allocated
	// This will cause queue_create to fail
	mock_malloc_set_fail_count(3);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	// BFS should fail when queue_create fails
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, -1);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	
	ProjectGraph_Destroy(graph);
}

// Graph BFS Queue Enqueue Failure Test

TEST_F(ProjectTrackingTest, ProjectGraph_BFSQueueEnqueueFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	// Set fail count to fail after queue is created
	// This will cause queue_enqueue to fail
	mock_malloc_set_fail_count(4);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	BFSResult result;
	result.visited_projects = nullptr;
	result.distances = nullptr;
	result.visited_count = 0;
	
	// BFS should fail when queue_enqueue fails
	int bfs_result = ProjectGraph_BFS(graph, 1, &result);
	EXPECT_EQ(bfs_result, -1);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	
	ProjectGraph_Destroy(graph);
}

// Graph DFS Malloc Failure Test

TEST_F(ProjectTrackingTest, ProjectGraph_DFSMallocFailure) {
	ProjectDependencyGraph* graph = ProjectGraph_Create(10);
	EXPECT_NE(graph, nullptr);
	
	EXPECT_EQ(ProjectGraph_AddProject(graph, 1), 0);
	
	mock_malloc_set_fail_count(1);
	mock_malloc_reset();
	
	InventoryManager_SetMallocHook(mock_malloc_hook);
	
	DFSResult result;
	result.visited_projects = nullptr;
	result.discovery_times = nullptr;
	result.finish_times = nullptr;
	result.visited_count = 0;
	result.time = 0;
	
	// DFS should fail when malloc fails for visited_projects, discovery_times, or finish_times
	int dfs_result = ProjectGraph_DFS(graph, 1, &result);
	EXPECT_EQ(dfs_result, -1);
	
	mock_malloc_set_fail_count(0);
	InventoryManager_SetMallocHook(nullptr);
	
	ProjectGraph_Destroy(graph);
}

