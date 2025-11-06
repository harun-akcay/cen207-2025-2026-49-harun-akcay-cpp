/**
 * @file ProjectTracking.h
 * 
 * @brief Provides functions for project tracking management using Stack data structure
 */

#ifndef PROJECT_TRACKING_H
#define PROJECT_TRACKING_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ProjectTracking Project Tracking Implementation
 * @brief Stack data structure for project tracking management
 * @{
 */

/**
 * @brief Project status enumeration
 */
typedef enum {
    PROJECT_STATUS_NOT_STARTED = 0,
    PROJECT_STATUS_IN_PROGRESS = 1,
    PROJECT_STATUS_ON_HOLD = 2,
    PROJECT_STATUS_COMPLETED = 3,
    PROJECT_STATUS_CANCELLED = 4
} ProjectStatus;

/**
 * @brief Project structure for tracking
 * 
 * Represents a craft project in the inventory management system.
 * Stores project name, description, status, and creation date.
 */
typedef struct {
    /** @brief Project ID (unique identifier) */
    uint32_t id;
    /** @brief Project name string (maximum 127 characters + null terminator) */
    char name[128];
    /** @brief Project description string (maximum 255 characters + null terminator) */
    char description[256];
    /** @brief Project status */
    ProjectStatus status;
    /** @brief Creation timestamp (simple counter, can be replaced with time_t) */
    uint32_t created_timestamp;
    /** @brief Last modified timestamp */
    uint32_t modified_timestamp;
} Project;

/**
 * @brief Stack node structure for projects
 * 
 * Represents a single entry in the project stack.
 * Each node contains a Project and a pointer to the next node.
 */
typedef struct ProjectStackNode {
    /** @brief Project data stored in this node */
    Project project;
    /** @brief Pointer to the next node in the stack (NULL if bottom) */
    struct ProjectStackNode* next;
} ProjectStackNode;

/**
 * @brief Stack structure for project tracking
 * 
 * LIFO (Last In, First Out) data structure for tracking projects.
 * The most recently added project appears at the top.
 */
typedef struct {
    /** @brief Pointer to the top of the stack (NULL if empty) */
    ProjectStackNode* top;
    /** @brief Current number of projects in the stack */
    size_t size;
    /** @brief Maximum capacity of the stack */
    size_t capacity;
    /** @brief Next available project ID */
    uint32_t next_id;
} ProjectStack;

/**
 * @}
 */

// Project Stack Functions

/**
 * @brief Create a new project stack.
 * @param capacity Maximum number of projects (0 for unlimited).
 * @return Pointer to the project stack, or NULL on error.
 */
ProjectStack* ProjectStack_Create(size_t capacity);

/**
 * @brief Destroy a project stack and free all memory.
 * @param stack Pointer to the project stack.
 */
void ProjectStack_Destroy(ProjectStack* stack);

/**
 * @brief Add a new project to the stack (push).
 * @param stack Pointer to the project stack.
 * @param name Project name (max 127 characters).
 * @param description Project description (max 255 characters).
 * @param status Initial project status.
 * @return Project ID on success, 0 on error.
 */
uint32_t ProjectStack_AddProject(ProjectStack* stack, const char* name, const char* description, ProjectStatus status);

/**
 * @brief Remove the top project from the stack (pop).
 * @param stack Pointer to the project stack.
 * @param project Pointer to store the removed project (can be NULL).
 * @return 0 on success, -1 on error (stack empty or NULL stack).
 */
int ProjectStack_RemoveProject(ProjectStack* stack, Project* project);

/**
 * @brief Peek at the top project without removing it.
 * @param stack Pointer to the project stack.
 * @param project Pointer to store the project data.
 * @return 0 on success, -1 on error (stack empty or NULL parameters).
 */
int ProjectStack_PeekProject(ProjectStack* stack, Project* project);

/**
 * @brief Find a project by ID.
 * @param stack Pointer to the project stack.
 * @param id Project ID to search for.
 * @return Pointer to the project if found, NULL otherwise.
 */
Project* ProjectStack_FindProject(ProjectStack* stack, uint32_t id);

/**
 * @brief Update an existing project.
 * @param stack Pointer to the project stack.
 * @param id Project ID to update.
 * @param name New project name (NULL to keep unchanged).
 * @param description New project description (NULL to keep unchanged).
 * @param status New project status (use -1 to keep unchanged).
 * @return 0 on success, -1 on error (project not found or NULL stack).
 */
int ProjectStack_UpdateProject(ProjectStack* stack, uint32_t id, const char* name, const char* description, int status);

/**
 * @brief Check if the stack is empty.
 * @param stack Pointer to the project stack.
 * @return 1 if empty, 0 if not empty, -1 if NULL.
 */
int ProjectStack_IsEmpty(ProjectStack* stack);

/**
 * @brief Get the number of projects in the stack.
 * @param stack Pointer to the project stack.
 * @return Number of projects, or 0 if NULL.
 */
size_t ProjectStack_GetSize(ProjectStack* stack);

/**
 * @brief Display all projects in the stack (from top to bottom).
 * @param stack Pointer to the project stack.
 * @param max_count Maximum number of projects to display (0 for all).
 */
void ProjectStack_ViewProjects(ProjectStack* stack, size_t max_count);

/**
 * @brief Save the project stack to a binary file.
 * @param stack Pointer to the project stack.
 * @param filename Filename to save to.
 * @return 0 on success, -1 on error.
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 */
int ProjectStack_SaveToFile(ProjectStack* stack, const char* filename);

/**
 * @brief Load the project stack from a binary file.
 * @param stack Pointer to existing stack (will be destroyed if not NULL), or NULL to create new.
 * @param filename Filename to load from.
 * @return Pointer to the loaded project stack, or NULL on error.
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 */
ProjectStack* ProjectStack_LoadFromFile(ProjectStack* stack, const char* filename);

/**
 * @brief Get status string representation.
 * @param status Project status enumeration value.
 * @return String representation of the status.
 */
const char* ProjectStack_GetStatusString(ProjectStatus status);

// Graph Algorithms for Project Dependencies (BFS/DFS)

/**
 * @defgroup GraphAlgorithms Graph Algorithms for Project Dependencies
 * @brief BFS and DFS algorithms for analyzing project dependencies
 * @{
 */

/**
 * @brief Maximum number of projects in the dependency graph
 */
#define MAX_PROJECTS_IN_GRAPH 100

/**
 * @brief Project dependency graph node
 * 
 * Represents a node in the project dependency graph.
 * Each node contains a project ID and a list of dependent project IDs.
 */
typedef struct ProjectGraphNode {
    /** @brief Project ID */
    uint32_t project_id;
    /** @brief Array of dependent project IDs (adjacency list) */
    uint32_t* dependencies;
    /** @brief Number of dependencies */
    size_t dependency_count;
    /** @brief Maximum capacity for dependencies */
    size_t dependency_capacity;
    /** @brief Visited flag for graph traversal */
    int visited;
} ProjectGraphNode;

/**
 * @brief Project dependency graph structure
 * 
 * Graph representation of project dependencies using adjacency list.
 * Supports BFS and DFS traversal algorithms.
 */
typedef struct {
    /** @brief Array of graph nodes (one per project) */
    ProjectGraphNode* nodes;
    /** @brief Number of projects in the graph */
    size_t project_count;
    /** @brief Maximum capacity of the graph */
    size_t capacity;
} ProjectDependencyGraph;

/**
 * @brief BFS traversal result structure
 * 
 * Stores the result of BFS traversal including visited projects and distances.
 */
typedef struct {
    /** @brief Array of visited project IDs in BFS order */
    uint32_t* visited_projects;
    /** @brief Number of visited projects */
    size_t visited_count;
    /** @brief Array of distances from source (shortest path lengths) */
    size_t* distances;
} BFSResult;

/**
 * @brief DFS traversal result structure
 * 
 * Stores the result of DFS traversal including visited projects and finish times.
 */
typedef struct {
    /** @brief Array of visited project IDs in DFS order */
    uint32_t* visited_projects;
    /** @brief Number of visited projects */
    size_t visited_count;
    /** @brief Array of discovery times (when each project was first visited) */
    size_t* discovery_times;
    /** @brief Array of finish times (when each project's DFS finished) */
    size_t* finish_times;
    /** @brief Current time counter for DFS */
    size_t time;
} DFSResult;

/**
 * @brief Create a new project dependency graph
 * 
 * Allocates memory for a new project dependency graph.
 * The graph is ready to use after creation.
 * 
 * @param capacity Maximum number of projects (0 for default MAX_PROJECTS_IN_GRAPH)
 * @return Pointer to the newly created graph, or NULL on memory allocation failure
 * 
 * @note Time complexity: O(1)
 * @note The caller is responsible for destroying the graph using ProjectGraph_Destroy()
 * to avoid memory leaks.
 */
ProjectDependencyGraph* ProjectGraph_Create(size_t capacity);

/**
 * @brief Destroy a project dependency graph and free all memory
 * 
 * Frees all nodes and dependencies in the graph and then frees the graph structure itself.
 * This function is safe to call with NULL pointer (no-op).
 * 
 * @param graph Pointer to the graph to destroy (can be NULL)
 * 
 * @note Time complexity: O(V + E) where V is vertices and E is edges
 */
void ProjectGraph_Destroy(ProjectDependencyGraph* graph);

/**
 * @brief Add a project to the dependency graph
 * 
 * Adds a new project node to the graph.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param project_id Project ID to add
 * @return 0 on success, -1 on error (NULL parameter or project already exists)
 * 
 * @note Time complexity: O(1) average case
 */
int ProjectGraph_AddProject(ProjectDependencyGraph* graph, uint32_t project_id);

/**
 * @brief Add a dependency between two projects
 * 
 * Adds an edge from project_id to dependency_id, meaning project_id depends on dependency_id.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param project_id Project ID that depends on another project
 * @param dependency_id Project ID that is a dependency
 * @return 0 on success, -1 on error (NULL parameters or projects not found)
 * 
 * @note Time complexity: O(1) average case
 */
int ProjectGraph_AddDependency(ProjectDependencyGraph* graph, uint32_t project_id, uint32_t dependency_id);

/**
 * @brief Perform Breadth-First Search (BFS) traversal
 * 
 * Performs BFS traversal starting from the given source project ID.
 * Finds the shortest path from source to all reachable projects.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param source_id Source project ID to start BFS from
 * @param result Pointer to store BFS result (must be pre-allocated, can be NULL to ignore)
 * @return 0 on success, -1 on error (NULL parameters or source not found)
 * 
 * @note Time complexity: O(V + E) where V is vertices and E is edges
 * @note Space complexity: O(V) for queue and visited array
 * @note The result structure must be freed using ProjectGraph_FreeBFSResult() after use
 */
int ProjectGraph_BFS(ProjectDependencyGraph* graph, uint32_t source_id, BFSResult* result);

/**
 * @brief Perform Depth-First Search (DFS) traversal
 * 
 * Performs DFS traversal starting from the given source project ID.
 * Explores as far as possible along each branch before backtracking.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param source_id Source project ID to start DFS from
 * @param result Pointer to store DFS result (must be pre-allocated, can be NULL to ignore)
 * @return 0 on success, -1 on error (NULL parameters or source not found)
 * 
 * @note Time complexity: O(V + E) where V is vertices and E is edges
 * @note Space complexity: O(V) for recursion stack and visited array
 * @note The result structure must be freed using ProjectGraph_FreeDFSResult() after use
 */
int ProjectGraph_DFS(ProjectDependencyGraph* graph, uint32_t source_id, DFSResult* result);

/**
 * @brief Find all projects that depend on a given project (reverse dependencies)
 * 
 * Finds all projects that have the given project as a dependency.
 * Uses BFS to find all reachable dependent projects.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param project_id Project ID to find dependents for
 * @param dependents Array to store dependent project IDs (must be pre-allocated)
 * @param max_count Maximum number of dependents to retrieve
 * @return Number of dependents found
 * 
 * @note Time complexity: O(V + E) where V is vertices and E is edges
 */
size_t ProjectGraph_FindDependents(ProjectDependencyGraph* graph, uint32_t project_id,
                                   uint32_t* dependents, size_t max_count);

/**
 * @brief Check if there is a path from source to target project
 * 
 * Uses BFS to check if target is reachable from source.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param source_id Source project ID
 * @param target_id Target project ID
 * @return 1 if path exists, 0 if no path, -1 on error
 * 
 * @note Time complexity: O(V + E) where V is vertices and E is edges
 */
int ProjectGraph_HasPath(ProjectDependencyGraph* graph, uint32_t source_id, uint32_t target_id);

/**
 * @brief Find shortest path from source to target using BFS
 * 
 * Finds the shortest path (in terms of number of edges) from source to target.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 * @param source_id Source project ID
 * @param target_id Target project ID
 * @param path Array to store path (project IDs in order, must be pre-allocated)
 * @param max_path_length Maximum path length
 * @return Path length (number of edges) on success, -1 on error or no path
 * 
 * @note Time complexity: O(V + E) where V is vertices and E is edges
 */
int ProjectGraph_FindShortestPath(ProjectDependencyGraph* graph, uint32_t source_id, uint32_t target_id,
                                   uint32_t* path, size_t max_path_length);

/**
 * @brief Free BFS result structure
 * 
 * Frees memory allocated for BFS result structure.
 * 
 * @param result Pointer to BFS result to free (can be NULL)
 */
void ProjectGraph_FreeBFSResult(BFSResult* result);

/**
 * @brief Free DFS result structure
 * 
 * Frees memory allocated for DFS result structure.
 * 
 * @param result Pointer to DFS result to free (can be NULL)
 */
void ProjectGraph_FreeDFSResult(DFSResult* result);

/**
 * @brief Build dependency graph from project stack
 * 
 * Builds a dependency graph from an existing project stack.
 * This is a helper function to convert project stack to graph format.
 * 
 * @param stack Pointer to the project stack (must not be NULL)
 * @return Pointer to the created graph, or NULL on error
 * 
 * @note Time complexity: O(V) where V is the number of projects
 */
ProjectDependencyGraph* ProjectGraph_BuildFromStack(ProjectStack* stack);

/**
 * @brief Display dependency graph structure
 * 
 * Prints the dependency graph structure for debugging.
 * 
 * @param graph Pointer to the dependency graph (must not be NULL)
 */
void ProjectGraph_Display(ProjectDependencyGraph* graph);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif // PROJECT_TRACKING_H

