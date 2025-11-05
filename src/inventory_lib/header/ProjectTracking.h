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

#ifdef __cplusplus
}
#endif

#endif // PROJECT_TRACKING_H

