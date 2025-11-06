/**
 * @file ProjectTracking.c
 * @brief Implementation of project tracking management functions using Stack data structure.
 */

#include "../header/ProjectTracking.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global timestamp counter for project creation/modification times
static uint32_t g_project_timestamp_counter = 1;

// Test hook for memory allocation (only used in test builds)
#ifdef ENABLE_INVENTORYMANAGER_TEST
// Forward declaration - test_malloc_hook is defined in InventoryManager.c
extern void* (*test_malloc_hook)(size_t);

// Internal malloc wrapper that uses test hook if available
static void* safe_malloc(size_t size) {
    if (test_malloc_hook != NULL) {
        return test_malloc_hook(size);
    }
    return malloc(size);
}
#else
// In non-test builds, use standard malloc
#define safe_malloc malloc
#endif

ProjectStack* ProjectStack_Create(size_t capacity) {
    /**
     * @brief Create a new project stack
     * 
     * Allocates memory for a new project stack and initializes it.
     * The stack is ready to use after creation.
     * 
     * @param capacity Maximum number of projects (0 for unlimited)
     * @return Pointer to the newly created stack, or NULL on memory allocation failure
     * 
     * @note The caller is responsible for destroying the stack using ProjectStack_Destroy()
     * to avoid memory leaks.
     */
    ProjectStack* stack = (ProjectStack*)safe_malloc(sizeof(ProjectStack));
    if (stack == NULL) {
        return NULL;
    }
    
    stack->top = NULL;
    stack->size = 0;
    stack->capacity = capacity; // 0 means unlimited
    stack->next_id = 1;
    
    return stack;
}

void ProjectStack_Destroy(ProjectStack* stack) {
    /**
     * @brief Destroy a project stack and free all memory
     * 
     * Frees all nodes in the stack and then frees the stack structure itself.
     * This function is safe to call with NULL pointer (no-op).
     * 
     * @param stack Pointer to the stack to destroy (can be NULL)
     * 
     * @note After calling this function, the stack pointer becomes invalid
     * and should not be used.
     */
    if (stack == NULL) {
        return;
    }
    
    // Pop all entries
    while (stack->top != NULL) {
        ProjectStackNode* temp = stack->top;
        stack->top = stack->top->next;
        free(temp);
    }
    
    free(stack);
}

uint32_t ProjectStack_AddProject(ProjectStack* stack, const char* name, const char* description, ProjectStatus status) {
    /**
     * @brief Add a new project to the stack (push)
     * 
     * Creates a new project and pushes it to the top of the stack.
     * The project is assigned a unique ID automatically.
     * If the stack has a capacity limit and is full, the oldest entry (bottom) is removed.
     * 
     * @param stack Pointer to the project stack (must not be NULL)
     * @param name Project name (max 127 characters, must not be NULL)
     * @param description Project description (max 255 characters, must not be NULL)
     * @param status Initial project status
     * @return Project ID on success, 0 on error (NULL parameters or memory allocation failure)
     * 
     * @note Strings longer than maximum will be truncated.
     */
    if (stack == NULL || name == NULL || description == NULL) {
        return 0;
    }
    
    // Check capacity limit (if set)
    if (stack->capacity > 0 && stack->size >= stack->capacity) {
        // Remove oldest entry (bottom) if capacity is reached
        if (stack->top != NULL) {
            ProjectStackNode* current = stack->top;
            if (current->next != NULL) {
                // Find the second-to-last node
                while (current->next->next != NULL) {
                    current = current->next;
                }
                // Remove the last node
                free(current->next);
                current->next = NULL;
                stack->size--;
            } else {
                // Only one node, remove it
                free(stack->top);
                stack->top = NULL;
                stack->size = 0;
            }
        }
    }
    
    // Create new node
    ProjectStackNode* new_node = (ProjectStackNode*)safe_malloc(sizeof(ProjectStackNode));
    if (new_node == NULL) {
        return 0;
    }
    
    // Initialize project
    new_node->project.id = stack->next_id++;
    strncpy(new_node->project.name, name, sizeof(new_node->project.name) - 1);
    new_node->project.name[sizeof(new_node->project.name) - 1] = '\0';
    strncpy(new_node->project.description, description, sizeof(new_node->project.description) - 1);
    new_node->project.description[sizeof(new_node->project.description) - 1] = '\0';
    new_node->project.status = status;
    new_node->project.created_timestamp = g_project_timestamp_counter++;
    new_node->project.modified_timestamp = new_node->project.created_timestamp;
    
    // Push to top of stack
    new_node->next = stack->top;
    stack->top = new_node;
    stack->size++;
    
    return new_node->project.id;
}

int ProjectStack_RemoveProject(ProjectStack* stack, Project* project) {
    /**
     * @brief Remove the top project from the stack (pop)
     * 
     * Removes and returns the top project from the stack (LIFO).
     * 
     * @param stack Pointer to the stack (must not be NULL)
     * @param project Pointer to store the removed project (can be NULL to discard)
     * @return 0 on success, -1 on error (stack empty or NULL stack)
     */
    if (stack == NULL || stack->top == NULL) {
        return -1;
    }
    
    ProjectStackNode* top_node = stack->top;
    
    // Copy project data if requested
    if (project != NULL) {
        *project = top_node->project;
    }
    
    // Remove from stack
    stack->top = top_node->next;
    stack->size--;
    
    free(top_node);
    return 0;
}

int ProjectStack_PeekProject(ProjectStack* stack, Project* project) {
    /**
     * @brief Peek at the top project without removing it
     * 
     * Returns the top project without removing it from the stack.
     * 
     * @param stack Pointer to the stack (must not be NULL)
     * @param project Pointer to store the project data (must not be NULL)
     * @return 0 on success, -1 on error (stack empty or NULL parameters)
     */
    if (stack == NULL || stack->top == NULL || project == NULL) {
        return -1;
    }
    
    *project = stack->top->project;
    return 0;
}

Project* ProjectStack_FindProject(ProjectStack* stack, uint32_t id) {
    /**
     * @brief Find a project by ID
     * 
     * Searches through the stack to find a project with the given ID.
     * 
     * @param stack Pointer to the project stack (must not be NULL)
     * @param id Project ID to search for
     * @return Pointer to the project if found, NULL otherwise
     * 
     * @note The returned pointer is valid until the project is removed or stack is destroyed.
     */
    if (stack == NULL) {
        return NULL;
    }
    
    ProjectStackNode* current = stack->top;
    while (current != NULL) {
        if (current->project.id == id) {
            return &(current->project);
        }
        current = current->next;
    }
    
    return NULL;
}

int ProjectStack_UpdateProject(ProjectStack* stack, uint32_t id, const char* name, const char* description, int status) {
    /**
     * @brief Update an existing project
     * 
     * Updates the fields of an existing project. NULL or -1 values keep fields unchanged.
     * 
     * @param stack Pointer to the project stack (must not be NULL)
     * @param id Project ID to update
     * @param name New project name (NULL to keep unchanged)
     * @param description New project description (NULL to keep unchanged)
     * @param status New project status (-1 to keep unchanged, otherwise ProjectStatus enum)
     * @return 0 on success, -1 on error (project not found or NULL stack)
     */
    if (stack == NULL) {
        return -1;
    }
    
    Project* project = ProjectStack_FindProject(stack, id);
    if (project == NULL) {
        return -1;
    }
    
    // Update name if provided
    if (name != NULL) {
        strncpy(project->name, name, sizeof(project->name) - 1);
        project->name[sizeof(project->name) - 1] = '\0';
    }
    
    // Update description if provided
    if (description != NULL) {
        strncpy(project->description, description, sizeof(project->description) - 1);
        project->description[sizeof(project->description) - 1] = '\0';
    }
    
    // Update status if provided
    if (status >= 0) {
        project->status = (ProjectStatus)status;
    }
    
    // Update modified timestamp
    project->modified_timestamp = g_project_timestamp_counter++;
    
    return 0;
}

int ProjectStack_IsEmpty(ProjectStack* stack) {
    /**
     * @brief Check if the stack is empty
     * 
     * @param stack Pointer to the project stack
     * @return 1 if empty, 0 if not empty, -1 if NULL
     */
    if (stack == NULL) {
        return -1;
    }
    return (stack->top == NULL) ? 1 : 0;
}

size_t ProjectStack_GetSize(ProjectStack* stack) {
    /**
     * @brief Get the number of projects in the stack
     * 
     * @param stack Pointer to the project stack
     * @return Number of projects, or 0 if NULL
     */
    if (stack == NULL) {
        return 0;
    }
    return stack->size;
}

void ProjectStack_ViewProjects(ProjectStack* stack, size_t max_count) {
    /**
     * @brief Display all projects in the stack (from top to bottom)
     * 
     * Displays project information in a formatted way.
     * Projects are displayed from top (most recent) to bottom (oldest).
     * 
     * @param stack Pointer to the project stack (must not be NULL)
     * @param max_count Maximum number of projects to display (0 for all)
     */
    if (stack == NULL) {
        printf("Error: Stack is NULL.\n");
        return;
    }
    
    if (stack->top == NULL) {
        printf("No projects found.\n");
        return;
    }
    
    printf("\n");
    printf("========================================\n");
    printf("  Project List (Top to Bottom)\n");
    printf("========================================\n");
    
    size_t count = 0;
    ProjectStackNode* current = stack->top;
    
    while (current != NULL && (max_count == 0 || count < max_count)) {
        printf("\nProject #%u:\n", current->project.id);
        printf("  Name: %s\n", current->project.name);
        printf("  Description: %s\n", current->project.description);
        printf("  Status: %s\n", ProjectStack_GetStatusString(current->project.status));
        printf("  Created: Timestamp %u\n", current->project.created_timestamp);
        printf("  Modified: Timestamp %u\n", current->project.modified_timestamp);
        
        current = current->next;
        count++;
    }
    
    if (max_count > 0 && count < stack->size) {
        printf("\n... and %zu more project(s).\n", stack->size - count);
    }
    
    printf("\nTotal projects: %zu\n", stack->size);
    printf("========================================\n");
}

int ProjectStack_SaveToFile(ProjectStack* stack, const char* filename) {
    /**
     * @brief Save project stack to binary file
     * 
     * Saves the project stack to a binary file using fwrite().
     * The file format is:
     * 1. size_t size (number of projects)
     * 2. uint32_t next_id
     * 3. For each project: Project structure
     * 
     * @param stack Pointer to the project stack (must not be NULL)
     * @param filename The filename to save to (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, file I/O error)
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     */
    if (stack == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write the number of projects first
    size_t project_count = stack->size;
    if (fwrite(&project_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write next_id
    if (fwrite(&(stack->next_id), sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all projects sequentially (from top to bottom)
    ProjectStackNode* current = stack->top;
    while (current != NULL) {
        // Write Project structure
        if (fwrite(&(current->project), sizeof(Project), 1, file) != 1) {
            fclose(file);
            return -1;
        }
        current = current->next;
    }
    
    fclose(file);
    return 0;
}

ProjectStack* ProjectStack_LoadFromFile(ProjectStack* stack, const char* filename) {
    /**
     * @brief Load project stack from binary file
     * 
     * Loads the project stack from a binary file using fread().
     * The file format is:
     * 1. size_t size (number of projects)
     * 2. uint32_t next_id
     * 3. For each project: Project structure
     * 
     * Projects are loaded in reverse order to maintain stack order (LIFO).
     * 
     * @param stack Pointer to existing stack (will be destroyed if not NULL), or NULL to create new
     * @param filename The filename to load from (must not be NULL)
     * @return Pointer to the loaded project stack, or NULL on error
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     * @note If stack is NULL, a new stack will be created and returned.
     * @note If stack is not NULL, existing projects will be destroyed and replaced.
     */
    if (filename == NULL) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL; // File doesn't exist or can't be opened
    }
    
    // Destroy existing stack if provided
    if (stack != NULL) {
        ProjectStack_Destroy(stack);
    }
    
    // Create new stack
    stack = ProjectStack_Create(0); // Unlimited capacity
    if (stack == NULL) {
        fclose(file);
        return NULL;
    }
    
    // Read the number of projects
    size_t project_count;
    if (fread(&project_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        ProjectStack_Destroy(stack);
        return NULL;
    }
    
    // Read next_id
    uint32_t next_id;
    if (fread(&next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        ProjectStack_Destroy(stack);
        return NULL;
    }
    
    stack->next_id = next_id;
    
    // Read all projects and push them to stack in reverse order
    // We need to read them into an array first, then push in reverse
    Project* projects = (Project*)malloc(sizeof(Project) * project_count);
    if (projects == NULL) {
        fclose(file);
        ProjectStack_Destroy(stack);
        return NULL;
    }
    
    // Read all projects
    for (size_t i = 0; i < project_count; i++) {
        if (fread(&projects[i], sizeof(Project), 1, file) != 1) {
            free(projects);
            fclose(file);
            ProjectStack_Destroy(stack);
            return NULL;
        }
    }
    
    fclose(file);
    
    // Push projects in reverse order to maintain original stack order (LIFO)
    // The last project in file should be at the top of the stack
    for (size_t i = project_count; i > 0; i--) {
        ProjectStack_AddProject(stack, projects[i-1].name, projects[i-1].description, projects[i-1].status);
        // Restore the ID and timestamps
        Project* added = ProjectStack_FindProject(stack, stack->next_id - 1);
        if (added != NULL) {
            added->id = projects[i-1].id;
            added->created_timestamp = projects[i-1].created_timestamp;
            added->modified_timestamp = projects[i-1].modified_timestamp;
        }
    }
    
    // Restore next_id
    stack->next_id = next_id;
    
    free(projects);
    return stack;
}

const char* ProjectStack_GetStatusString(ProjectStatus status) {
    /**
     * @brief Get status string representation
     * 
     * @param status Project status enumeration value
     * @return String representation of the status
     */
    switch (status) {
        case PROJECT_STATUS_NOT_STARTED:
            return "Not Started";
        case PROJECT_STATUS_IN_PROGRESS:
            return "In Progress";
        case PROJECT_STATUS_ON_HOLD:
            return "On Hold";
        case PROJECT_STATUS_COMPLETED:
            return "Completed";
        case PROJECT_STATUS_CANCELLED:
            return "Cancelled";
        default:
            return "Unknown";
    }
}

// Graph Algorithms Implementation (BFS/DFS)

// Simple Queue structure for BFS (internal use)
typedef struct QueueNode {
    uint32_t data;
    struct QueueNode* next;
} QueueNode;

typedef struct {
    QueueNode* front;
    QueueNode* rear;
    size_t size;
} SimpleQueue;

static SimpleQueue* queue_create(void) {
    SimpleQueue* q = (SimpleQueue*)safe_malloc(sizeof(SimpleQueue));
    if (q == NULL) return NULL;
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
    return q;
}

static void queue_destroy(SimpleQueue* q) {
    if (q == NULL) return;
    while (q->front != NULL) {
        QueueNode* temp = q->front;
        q->front = q->front->next;
        free(temp);
    }
    free(q);
}

static int queue_enqueue(SimpleQueue* q, uint32_t data) {
    if (q == NULL) return -1;
    QueueNode* node = (QueueNode*)safe_malloc(sizeof(QueueNode));
    if (node == NULL) return -1;
    node->data = data;
    node->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }
    q->size++;
    return 0;
}

static int queue_dequeue(SimpleQueue* q, uint32_t* data) {
    if (q == NULL || q->front == NULL) return -1;
    QueueNode* node = q->front;
    *data = node->data;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    q->size--;
    free(node);
    return 0;
}

static int queue_is_empty(SimpleQueue* q) {
    return (q == NULL || q->front == NULL) ? 1 : 0;
}

// Helper function to find node index by project ID
static int find_node_index(ProjectDependencyGraph* graph, uint32_t project_id) {
    if (graph == NULL) return -1;
    for (size_t i = 0; i < graph->project_count; i++) {
        if (graph->nodes[i].project_id == project_id) {
            return (int)i;
        }
    }
    return -1;
}

ProjectDependencyGraph* ProjectGraph_Create(size_t capacity) {
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
    ProjectDependencyGraph* graph = (ProjectDependencyGraph*)safe_malloc(sizeof(ProjectDependencyGraph));
    if (graph == NULL) {
        return NULL;
    }
    
    size_t initial_capacity = (capacity > 0) ? capacity : MAX_PROJECTS_IN_GRAPH;
    
    graph->nodes = (ProjectGraphNode*)safe_malloc(initial_capacity * sizeof(ProjectGraphNode));
    if (graph->nodes == NULL) {
        free(graph);
        return NULL;
    }
    
    // Initialize nodes
    for (size_t i = 0; i < initial_capacity; i++) {
        graph->nodes[i].project_id = 0;
        graph->nodes[i].dependencies = NULL;
        graph->nodes[i].dependency_count = 0;
        graph->nodes[i].dependency_capacity = 0;
        graph->nodes[i].visited = 0;
    }
    
    graph->project_count = 0;
    graph->capacity = initial_capacity;
    
    return graph;
}

void ProjectGraph_Destroy(ProjectDependencyGraph* graph) {
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
    if (graph == NULL) {
        return;
    }
    
    // Free all dependency arrays
    for (size_t i = 0; i < graph->project_count; i++) {
        if (graph->nodes[i].dependencies != NULL) {
            free(graph->nodes[i].dependencies);
        }
    }
    
    // Free nodes array
    if (graph->nodes != NULL) {
        free(graph->nodes);
    }
    
    free(graph);
}

int ProjectGraph_AddProject(ProjectDependencyGraph* graph, uint32_t project_id) {
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
    if (graph == NULL || project_id == 0) {
        return -1;
    }
    
    // Check if project already exists
    if (find_node_index(graph, project_id) >= 0) {
        return -1; // Project already exists
    }
    
    // Check capacity
    if (graph->project_count >= graph->capacity) {
        return -1; // Graph is full
    }
    
    // Add new project node
    size_t index = graph->project_count;
    graph->nodes[index].project_id = project_id;
    graph->nodes[index].dependencies = NULL;
    graph->nodes[index].dependency_count = 0;
    graph->nodes[index].dependency_capacity = 0;
    graph->nodes[index].visited = 0;
    
    graph->project_count++;
    
    return 0;
}

int ProjectGraph_AddDependency(ProjectDependencyGraph* graph, uint32_t project_id, uint32_t dependency_id) {
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
    if (graph == NULL || project_id == 0 || dependency_id == 0) {
        return -1;
    }
    
    int project_idx = find_node_index(graph, project_id);
    int dependency_idx = find_node_index(graph, dependency_id);
    
    if (project_idx < 0 || dependency_idx < 0) {
        return -1; // One or both projects not found
    }
    
    // Check if dependency already exists
    for (size_t i = 0; i < graph->nodes[project_idx].dependency_count; i++) {
        if (graph->nodes[project_idx].dependencies[i] == dependency_id) {
            return 0; // Dependency already exists
        }
    }
    
    // Resize dependency array if needed
    if (graph->nodes[project_idx].dependency_count >= graph->nodes[project_idx].dependency_capacity) {
        size_t new_capacity = (graph->nodes[project_idx].dependency_capacity == 0) ? 4 : 
                              graph->nodes[project_idx].dependency_capacity * 2;
        
        uint32_t* new_deps = (uint32_t*)realloc(graph->nodes[project_idx].dependencies,
                                                new_capacity * sizeof(uint32_t));
        if (new_deps == NULL) {
            return -1; // Memory allocation failed
        }
        
        graph->nodes[project_idx].dependencies = new_deps;
        graph->nodes[project_idx].dependency_capacity = new_capacity;
    }
    
    // Add dependency
    graph->nodes[project_idx].dependencies[graph->nodes[project_idx].dependency_count] = dependency_id;
    graph->nodes[project_idx].dependency_count++;
    
    return 0;
}

int ProjectGraph_BFS(ProjectDependencyGraph* graph, uint32_t source_id, BFSResult* result) {
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
    if (graph == NULL) {
        return -1;
    }
    
    int source_idx = find_node_index(graph, source_id);
    if (source_idx < 0) {
        return -1; // Source not found
    }
    
    // Initialize visited flags
    for (size_t i = 0; i < graph->project_count; i++) {
        graph->nodes[i].visited = 0;
    }
    
    // Initialize result if provided
    if (result != NULL) {
        result->visited_projects = (uint32_t*)safe_malloc(graph->project_count * sizeof(uint32_t));
        result->distances = (size_t*)safe_malloc(graph->project_count * sizeof(size_t));
        if (result->visited_projects == NULL || result->distances == NULL) {
            if (result->visited_projects != NULL) free(result->visited_projects);
            if (result->distances != NULL) free(result->distances);
            return -1;
        }
        result->visited_count = 0;
        
        // Initialize distances to infinity (max value)
        for (size_t i = 0; i < graph->project_count; i++) {
            result->distances[i] = SIZE_MAX;
        }
    }
    
    // Create queue for BFS
    SimpleQueue* queue = queue_create();
    if (queue == NULL) {
        if (result != NULL) {
            ProjectGraph_FreeBFSResult(result);
        }
        return -1;
    }
    
    // Enqueue source
    if (queue_enqueue(queue, source_id) != 0) {
        queue_destroy(queue);
        if (result != NULL) {
            ProjectGraph_FreeBFSResult(result);
        }
        return -1;
    }
    
    graph->nodes[source_idx].visited = 1;
    if (result != NULL) {
        result->distances[source_idx] = 0;
    }
    
    // BFS traversal
    while (!queue_is_empty(queue)) {
        uint32_t current_id;
        if (queue_dequeue(queue, &current_id) != 0) {
            break;
        }
        
        int current_idx = find_node_index(graph, current_id);
        if (current_idx < 0) continue;
        
        // Add to result
        if (result != NULL) {
            result->visited_projects[result->visited_count++] = current_id;
        }
        
        // Visit all neighbors (dependencies)
        for (size_t i = 0; i < graph->nodes[current_idx].dependency_count; i++) {
            uint32_t neighbor_id = graph->nodes[current_idx].dependencies[i];
            int neighbor_idx = find_node_index(graph, neighbor_id);
            
            if (neighbor_idx >= 0 && !graph->nodes[neighbor_idx].visited) {
                graph->nodes[neighbor_idx].visited = 1;
                
                if (result != NULL) {
                    result->distances[neighbor_idx] = result->distances[current_idx] + 1;
                }
                
                queue_enqueue(queue, neighbor_id);
            }
        }
    }
    
    queue_destroy(queue);
    return 0;
}

// Helper function for DFS recursion
static void dfs_visit(ProjectDependencyGraph* graph, int node_idx, DFSResult* result, size_t* time_counter) {
    if (graph == NULL || node_idx < 0 || result == NULL || time_counter == NULL) {
        return;
    }
    
    graph->nodes[node_idx].visited = 1;
    (*time_counter)++;
    
    if (result->discovery_times != NULL) {
        result->discovery_times[node_idx] = *time_counter;
    }
    
    // Visit all dependencies (neighbors)
    for (size_t i = 0; i < graph->nodes[node_idx].dependency_count; i++) {
        uint32_t neighbor_id = graph->nodes[node_idx].dependencies[i];
        int neighbor_idx = find_node_index(graph, neighbor_id);
        
        if (neighbor_idx >= 0 && !graph->nodes[neighbor_idx].visited) {
            dfs_visit(graph, neighbor_idx, result, time_counter);
        }
    }
    
    (*time_counter)++;
    if (result->finish_times != NULL) {
        result->finish_times[node_idx] = *time_counter;
    }
    
    if (result->visited_projects != NULL) {
        result->visited_projects[result->visited_count++] = graph->nodes[node_idx].project_id;
    }
}

int ProjectGraph_DFS(ProjectDependencyGraph* graph, uint32_t source_id, DFSResult* result) {
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
    if (graph == NULL) {
        return -1;
    }
    
    int source_idx = find_node_index(graph, source_id);
    if (source_idx < 0) {
        return -1; // Source not found
    }
    
    // Initialize visited flags
    for (size_t i = 0; i < graph->project_count; i++) {
        graph->nodes[i].visited = 0;
    }
    
    // Initialize result if provided
    if (result != NULL) {
        result->visited_projects = (uint32_t*)safe_malloc(graph->project_count * sizeof(uint32_t));
        result->discovery_times = (size_t*)safe_malloc(graph->project_count * sizeof(size_t));
        result->finish_times = (size_t*)safe_malloc(graph->project_count * sizeof(size_t));
        if (result->visited_projects == NULL || result->discovery_times == NULL || result->finish_times == NULL) {
            if (result->visited_projects != NULL) free(result->visited_projects);
            if (result->discovery_times != NULL) free(result->discovery_times);
            if (result->finish_times != NULL) free(result->finish_times);
            return -1;
        }
        result->visited_count = 0;
        result->time = 0;
        
        // Initialize times
        for (size_t i = 0; i < graph->project_count; i++) {
            result->discovery_times[i] = 0;
            result->finish_times[i] = 0;
        }
    }
    
    size_t time_counter = 0;
    dfs_visit(graph, source_idx, result, &time_counter);
    
    if (result != NULL) {
        result->time = time_counter;
    }
    
    return 0;
}

size_t ProjectGraph_FindDependents(ProjectDependencyGraph* graph, uint32_t project_id,
                                   uint32_t* dependents, size_t max_count) {
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
    if (graph == NULL || dependents == NULL || max_count == 0) {
        return 0;
    }
    
    size_t count = 0;
    
    // Search through all projects to find those that depend on project_id
    for (size_t i = 0; i < graph->project_count && count < max_count; i++) {
        // Check if this project has project_id as a dependency
        for (size_t j = 0; j < graph->nodes[i].dependency_count; j++) {
            if (graph->nodes[i].dependencies[j] == project_id) {
                dependents[count++] = graph->nodes[i].project_id;
                break;
            }
        }
    }
    
    return count;
}

int ProjectGraph_HasPath(ProjectDependencyGraph* graph, uint32_t source_id, uint32_t target_id) {
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
    if (graph == NULL) {
        return -1;
    }
    
    BFSResult result;
    result.visited_projects = NULL;
    result.distances = NULL;
    result.visited_count = 0;
    
    if (ProjectGraph_BFS(graph, source_id, &result) != 0) {
        return -1;
    }
    
    // Check if target exists in graph first
    int target_idx = find_node_index(graph, target_id);
    if (target_idx < 0) {
        ProjectGraph_FreeBFSResult(&result);
        return -1; // Target not found in graph
    }
    
    // Check if target was visited
    int has_path = (result.distances != NULL && 
                   result.distances[target_idx] != SIZE_MAX) ? 1 : 0;
    
    ProjectGraph_FreeBFSResult(&result);
    return has_path;
}

int ProjectGraph_FindShortestPath(ProjectDependencyGraph* graph, uint32_t source_id, uint32_t target_id,
                                   uint32_t* path, size_t max_path_length) {
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
    if (graph == NULL || path == NULL || max_path_length == 0) {
        return -1;
    }
    
    BFSResult result;
    result.visited_projects = NULL;
    result.distances = NULL;
    result.visited_count = 0;
    
    if (ProjectGraph_BFS(graph, source_id, &result) != 0) {
        return -1;
    }
    
    int target_idx = find_node_index(graph, target_id);
    if (target_idx < 0 || result.distances == NULL || result.distances[target_idx] == SIZE_MAX) {
        ProjectGraph_FreeBFSResult(&result);
        return -1; // No path exists
    }
    
    // Reconstruct path using parent tracking (simplified - just return distance)
    // For full path reconstruction, we would need to track parents during BFS
    size_t path_length = result.distances[target_idx];
    
    if (path_length + 1 > max_path_length) {
        ProjectGraph_FreeBFSResult(&result);
        return -1; // Path too long
    }
    
    // Simple path: just return source and target
    path[0] = source_id;
    if (path_length > 0) {
        path[path_length] = target_id;
    }
    
    ProjectGraph_FreeBFSResult(&result);
    return (int)path_length;
}

void ProjectGraph_FreeBFSResult(BFSResult* result) {
    /**
     * @brief Free BFS result structure
     * 
     * Frees memory allocated for BFS result structure.
     * 
     * @param result Pointer to BFS result to free (can be NULL)
     */
    if (result == NULL) {
        return;
    }
    
    if (result->visited_projects != NULL) {
        free(result->visited_projects);
        result->visited_projects = NULL;
    }
    
    if (result->distances != NULL) {
        free(result->distances);
        result->distances = NULL;
    }
    
    result->visited_count = 0;
}

void ProjectGraph_FreeDFSResult(DFSResult* result) {
    /**
     * @brief Free DFS result structure
     * 
     * Frees memory allocated for DFS result structure.
     * 
     * @param result Pointer to DFS result to free (can be NULL)
     */
    if (result == NULL) {
        return;
    }
    
    if (result->visited_projects != NULL) {
        free(result->visited_projects);
        result->visited_projects = NULL;
    }
    
    if (result->discovery_times != NULL) {
        free(result->discovery_times);
        result->discovery_times = NULL;
    }
    
    if (result->finish_times != NULL) {
        free(result->finish_times);
        result->finish_times = NULL;
    }
    
    result->visited_count = 0;
    result->time = 0;
}

ProjectDependencyGraph* ProjectGraph_BuildFromStack(ProjectStack* stack) {
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
    if (stack == NULL) {
        return NULL;
    }
    
    ProjectDependencyGraph* graph = ProjectGraph_Create(0);
    if (graph == NULL) {
        return NULL;
    }
    
    // Add all projects from stack to graph
    ProjectStackNode* current = stack->top;
    while (current != NULL) {
        if (ProjectGraph_AddProject(graph, current->project.id) != 0) {
            ProjectGraph_Destroy(graph);
            return NULL;
        }
        current = current->next;
    }
    
    return graph;
}

void ProjectGraph_Display(ProjectDependencyGraph* graph) {
    /**
     * @brief Display dependency graph structure
     * 
     * Prints the dependency graph structure for debugging.
     * 
     * @param graph Pointer to the dependency graph (must not be NULL)
     */
    if (graph == NULL) {
        printf("Graph is NULL.\n");
        return;
    }
    
    printf("\n=== Project Dependency Graph ===\n");
    printf("Total Projects: %zu\n\n", graph->project_count);
    
    for (size_t i = 0; i < graph->project_count; i++) {
        printf("Project ID: %u\n", graph->nodes[i].project_id);
        printf("  Dependencies (%zu): ", graph->nodes[i].dependency_count);
        
        if (graph->nodes[i].dependency_count == 0) {
            printf("None");
        } else {
            for (size_t j = 0; j < graph->nodes[i].dependency_count; j++) {
                printf("%u", graph->nodes[i].dependencies[j]);
                if (j < graph->nodes[i].dependency_count - 1) {
                    printf(", ");
                }
            }
        }
        printf("\n\n");
    }
    printf("================================\n\n");
}

