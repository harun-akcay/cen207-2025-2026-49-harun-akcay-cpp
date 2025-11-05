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

