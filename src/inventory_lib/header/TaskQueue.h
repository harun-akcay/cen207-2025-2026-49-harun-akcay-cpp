/**
 * @file TaskQueue.h
 * 
 * @brief Provides functions for task queue management using Queue (FIFO) data structure
 * 
 * This module implements a FIFO (First In, First Out) queue for managing tasks
 * such as material orders, notifications, and report generation.
 */

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup TaskQueue Task Queue Implementation
 * @brief Queue (FIFO) data structure for task management
 * @{
 */

/**
 * @brief Task priority enumeration
 */
typedef enum {
    TASK_PRIORITY_LOW = 0,
    TASK_PRIORITY_NORMAL = 1,
    TASK_PRIORITY_HIGH = 2,
    TASK_PRIORITY_URGENT = 3
} TaskPriority;

/**
 * @brief Task type enumeration
 */
typedef enum {
    TASK_TYPE_MATERIAL_ORDER = 0,
    TASK_TYPE_NOTIFICATION = 1,
    TASK_TYPE_REPORT_GENERATION = 2,
    TASK_TYPE_DATA_BACKUP = 3,
    TASK_TYPE_CUSTOM = 4
} TaskType;

/**
 * @brief Task structure for queue
 * 
 * Represents a task in the task queue system.
 * Stores task details including type, priority, and description.
 */
typedef struct {
    /** @brief Task ID (unique identifier) */
    uint32_t id;
    /** @brief Task type */
    TaskType type;
    /** @brief Task priority */
    TaskPriority priority;
    /** @brief Task description string (maximum 255 characters + null terminator) */
    char description[256];
    /** @brief Task creation timestamp */
    uint32_t created_timestamp;
    /** @brief Task status (0 = pending, 1 = processing, 2 = completed, 3 = failed) */
    int status;
} Task;

/**
 * @brief Queue node structure for tasks
 * 
 * Represents a single entry in the task queue.
 * Each node contains a Task and a pointer to the next node.
 */
typedef struct TaskQueueNode {
    /** @brief Task data stored in this node */
    Task task;
    /** @brief Pointer to the next node in the queue (NULL if last node) */
    struct TaskQueueNode* next;
} TaskQueueNode;

/**
 * @brief Queue structure for task management
 * 
 * FIFO (First In, First Out) data structure for managing tasks.
 * The first task added will be the first task processed.
 * 
 * @note Time complexity: O(1) for enqueue/dequeue, O(n) for traversal
 * @note Space complexity: O(n)
 */
typedef struct {
    /** @brief Pointer to the front of the queue (first task, NULL if empty) */
    TaskQueueNode* front;
    /** @brief Pointer to the rear of the queue (last task, NULL if empty) */
    TaskQueueNode* rear;
    /** @brief Current number of tasks in the queue */
    size_t size;
    /** @brief Maximum capacity of the queue (0 for unlimited) */
    size_t capacity;
    /** @brief Next available task ID */
    uint32_t next_id;
} TaskQueue;

/**
 * @}
 */

// Task Queue Functions

/**
 * @brief Create a new task queue
 * 
 * Allocates memory for a new task queue and initializes it.
 * The queue is ready to use after creation.
 * 
 * @param capacity Maximum number of tasks (0 for unlimited)
 * @return Pointer to the newly created queue, or NULL on memory allocation failure
 * 
 * @note Time complexity: O(1)
 * @note The caller is responsible for destroying the queue using TaskQueue_Destroy()
 * to avoid memory leaks.
 */
TaskQueue* TaskQueue_Create(size_t capacity);

/**
 * @brief Destroy a task queue and free all memory
 * 
 * Frees all nodes in the queue and then frees the queue structure itself.
 * This function is safe to call with NULL pointer (no-op).
 * 
 * @param queue Pointer to the task queue to destroy (can be NULL)
 * 
 * @note Time complexity: O(n) where n is the number of tasks
 * @note After calling this function, the queue pointer becomes invalid and should not be used.
 */
void TaskQueue_Destroy(TaskQueue* queue);

/**
 * @brief Add a new task to the queue (enqueue)
 * 
 * Adds a new task to the rear of the queue (FIFO).
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param type Task type
 * @param priority Task priority
 * @param description Task description string (max 255 characters)
 * @return Task ID on success, 0 on error (NULL parameters, memory allocation failure, or queue full)
 * 
 * @note Time complexity: O(1)
 * @note The task is added to the rear of the queue
 */
uint32_t TaskQueue_Enqueue(TaskQueue* queue, TaskType type, TaskPriority priority, const char* description);

/**
 * @brief Remove and return the first task from the queue (dequeue)
 * 
 * Removes the front task (oldest task) from the queue and returns it.
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param task Pointer to store the removed task (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters or empty queue)
 * 
 * @note Time complexity: O(1)
 */
int TaskQueue_Dequeue(TaskQueue* queue, Task* task);

/**
 * @brief Peek at the first task without removing it
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param task Pointer to store the task data (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters or empty queue)
 * 
 * @note Time complexity: O(1)
 */
int TaskQueue_Peek(TaskQueue* queue, Task* task);

/**
 * @brief Check if the queue is empty
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @return 1 if empty, 0 if not empty, -1 if NULL
 * 
 * @note Time complexity: O(1)
 */
int TaskQueue_IsEmpty(TaskQueue* queue);

/**
 * @brief Get the number of tasks in the queue
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @return Number of tasks, or 0 if NULL or empty
 * 
 * @note Time complexity: O(1)
 */
size_t TaskQueue_GetSize(TaskQueue* queue);

/**
 * @brief Remove a specific task by ID from the queue
 * 
 * Searches for and removes a task with the given ID from the queue.
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param task_id Task ID to remove
 * @param task Pointer to store the removed task (can be NULL)
 * @return 0 on success, -1 on error (NULL queue, task not found, or empty queue)
 * 
 * @note Time complexity: O(n) where n is the number of tasks
 */
int TaskQueue_RemoveTask(TaskQueue* queue, uint32_t task_id, Task* task);

/**
 * @brief Find a task by ID without removing it
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param task_id Task ID to search for
 * @param task Pointer to store the task data (must not be NULL)
 * @return 0 on success, -1 on error (NULL parameters, task not found)
 * 
 * @note Time complexity: O(n) where n is the number of tasks
 */
int TaskQueue_FindTask(TaskQueue* queue, uint32_t task_id, Task* task);

/**
 * @brief Display all tasks in the queue (from front to rear)
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param max_count Maximum number of tasks to display (0 for all)
 * 
 * @note Time complexity: O(n) where n is the number of tasks
 */
void TaskQueue_Display(TaskQueue* queue, size_t max_count);

/**
 * @brief Get task type string representation
 * 
 * @param type Task type enumeration value
 * @return String representation of the task type
 */
const char* TaskQueue_GetTypeString(TaskType type);

/**
 * @brief Get task priority string representation
 * 
 * @param priority Task priority enumeration value
 * @return String representation of the priority
 */
const char* TaskQueue_GetPriorityString(TaskPriority priority);

/**
 * @brief Get task status string representation
 * 
 * @param status Task status value (0-3)
 * @return String representation of the status
 */
const char* TaskQueue_GetStatusString(int status);

/**
 * @brief Save the task queue to a binary file
 * 
 * Saves all tasks to a binary file for persistence.
 * 
 * @param queue Pointer to the task queue (must not be NULL)
 * @param filename Filename to save to (must not be NULL)
 * @return 0 on success, -1 on error
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 * @note Time complexity: O(n) where n is the number of tasks
 */
int TaskQueue_SaveToFile(TaskQueue* queue, const char* filename);

/**
 * @brief Load the task queue from a binary file
 * 
 * Loads tasks from a binary file.
 * 
 * @param queue Pointer to existing queue (will be destroyed if not NULL), or NULL to create new.
 * @param filename Filename to load from (must not be NULL)
 * @return Pointer to the loaded queue, or NULL on error
 * 
 * @note Uses binary format (fwrite/fread) as required by project specifications.
 * @note Time complexity: O(n) where n is the number of tasks
 */
TaskQueue* TaskQueue_LoadFromFile(TaskQueue* queue, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // TASK_QUEUE_H

