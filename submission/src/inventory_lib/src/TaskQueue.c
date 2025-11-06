/**
 * @file TaskQueue.c
 * @brief Implementation of task queue management functions using Queue (FIFO) data structure.
 */

#include "../header/TaskQueue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Disable MSVC warnings for deprecated functions
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Global timestamp counter for tasks
static uint32_t g_task_timestamp_counter = 1;

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

TaskQueue* TaskQueue_Create(size_t capacity) {
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
    TaskQueue* queue = (TaskQueue*)safe_malloc(sizeof(TaskQueue));
    if (queue == NULL) {
        return NULL;
    }
    
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    queue->capacity = capacity; // 0 means unlimited
    queue->next_id = 1;
    
    return queue;
}

void TaskQueue_Destroy(TaskQueue* queue) {
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
    if (queue == NULL) {
        return;
    }
    
    // Remove all nodes
    while (queue->front != NULL) {
        TaskQueueNode* temp = queue->front;
        queue->front = queue->front->next;
        free(temp);
    }
    
    free(queue);
}

uint32_t TaskQueue_Enqueue(TaskQueue* queue, TaskType type, TaskPriority priority, const char* description) {
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
    if (queue == NULL || description == NULL) {
        return 0;
    }
    
    // Check capacity
    if (queue->capacity > 0 && queue->size >= queue->capacity) {
        return 0; // Queue is full
    }
    
    // Create new task node
    TaskQueueNode* new_node = (TaskQueueNode*)safe_malloc(sizeof(TaskQueueNode));
    if (new_node == NULL) {
        return 0; // Memory allocation failed
    }
    
    // Initialize task
    new_node->task.id = queue->next_id++;
    new_node->task.type = type;
    new_node->task.priority = priority;
    new_node->task.status = 0; // Pending
    new_node->task.created_timestamp = g_task_timestamp_counter++;
    
    // Copy description (with bounds checking)
    strncpy(new_node->task.description, description, sizeof(new_node->task.description) - 1);
    new_node->task.description[sizeof(new_node->task.description) - 1] = '\0';
    
    new_node->next = NULL;
    
    // Add to rear of queue
    if (queue->rear == NULL) {
        // Queue is empty
        queue->front = queue->rear = new_node;
    } else {
        // Queue has elements
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    
    queue->size++;
    
    return new_node->task.id;
}

int TaskQueue_Dequeue(TaskQueue* queue, Task* task) {
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
    if (queue == NULL || task == NULL || queue->front == NULL) {
        return -1;
    }
    
    // Get front node
    TaskQueueNode* front_node = queue->front;
    
    // Copy task data
    *task = front_node->task;
    
    // Update front pointer
    queue->front = queue->front->next;
    
    // If queue becomes empty, update rear
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    
    // Free the node
    free(front_node);
    
    queue->size--;
    
    return 0;
}

int TaskQueue_Peek(TaskQueue* queue, Task* task) {
    /**
     * @brief Peek at the first task without removing it
     * 
     * @param queue Pointer to the task queue (must not be NULL)
     * @param task Pointer to store the task data (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters or empty queue)
     * 
     * @note Time complexity: O(1)
     */
    if (queue == NULL || task == NULL || queue->front == NULL) {
        return -1;
    }
    
    *task = queue->front->task;
    return 0;
}

int TaskQueue_IsEmpty(TaskQueue* queue) {
    /**
     * @brief Check if the queue is empty
     * 
     * @param queue Pointer to the task queue (must not be NULL)
     * @return 1 if empty, 0 if not empty, -1 if NULL
     * 
     * @note Time complexity: O(1)
     */
    if (queue == NULL) {
        return -1;
    }
    
    return (queue->front == NULL) ? 1 : 0;
}

size_t TaskQueue_GetSize(TaskQueue* queue) {
    /**
     * @brief Get the number of tasks in the queue
     * 
     * @param queue Pointer to the task queue (must not be NULL)
     * @return Number of tasks, or 0 if NULL or empty
     * 
     * @note Time complexity: O(1)
     */
    if (queue == NULL) {
        return 0;
    }
    
    return queue->size;
}

int TaskQueue_RemoveTask(TaskQueue* queue, uint32_t task_id, Task* task) {
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
    if (queue == NULL || queue->front == NULL) {
        return -1;
    }
    
    TaskQueueNode* current = queue->front;
    TaskQueueNode* prev = NULL;
    
    // Find the task
    while (current != NULL) {
        if (current->task.id == task_id) {
            // Found the task
            if (task != NULL) {
                *task = current->task;
            }
            
            // Remove from queue
            if (prev == NULL) {
                // Task is at front
                queue->front = current->next;
                if (queue->front == NULL) {
                    queue->rear = NULL;
                }
            } else {
                // Task is in middle or rear
                prev->next = current->next;
                if (current == queue->rear) {
                    queue->rear = prev;
                }
            }
            
            free(current);
            queue->size--;
            return 0;
        }
        
        prev = current;
        current = current->next;
    }
    
    return -1; // Task not found
}

int TaskQueue_FindTask(TaskQueue* queue, uint32_t task_id, Task* task) {
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
    if (queue == NULL || task == NULL) {
        return -1;
    }
    
    TaskQueueNode* current = queue->front;
    
    while (current != NULL) {
        if (current->task.id == task_id) {
            *task = current->task;
            return 0;
        }
        current = current->next;
    }
    
    return -1; // Task not found
}

void TaskQueue_Display(TaskQueue* queue, size_t max_count) {
    /**
     * @brief Display all tasks in the queue (from front to rear)
     * 
     * @param queue Pointer to the task queue (must not be NULL)
     * @param max_count Maximum number of tasks to display (0 for all)
     * 
     * @note Time complexity: O(n) where n is the number of tasks
     */
    if (queue == NULL || queue->front == NULL) {
        printf("Task queue is empty.\n");
        return;
    }
    
    printf("\n=== Task Queue (FIFO) ===\n");
    printf("%-5s %-20s %-10s %-15s %-10s %-30s\n",
           "ID", "Type", "Priority", "Status", "Timestamp", "Description");
    printf("--------------------------------------------------------------------------------------------------------\n");
    
    TaskQueueNode* current = queue->front;
    size_t count = 0;
    
    while (current != NULL && (max_count == 0 || count < max_count)) {
        printf("%-5u %-20s %-10s %-15s %-10u %-30s\n",
               current->task.id,
               TaskQueue_GetTypeString(current->task.type),
               TaskQueue_GetPriorityString(current->task.priority),
               TaskQueue_GetStatusString(current->task.status),
               current->task.created_timestamp,
               current->task.description);
        
        current = current->next;
        count++;
    }
    
    printf("--------------------------------------------------------------------------------------------------------\n");
    printf("Total Tasks: %zu (Displayed: %zu)\n", queue->size, count);
    printf("================================\n\n");
}

const char* TaskQueue_GetTypeString(TaskType type) {
    /**
     * @brief Get task type string representation
     * 
     * @param type Task type enumeration value
     * @return String representation of the task type
     */
    switch (type) {
        case TASK_TYPE_MATERIAL_ORDER:
            return "Material Order";
        case TASK_TYPE_NOTIFICATION:
            return "Notification";
        case TASK_TYPE_REPORT_GENERATION:
            return "Report Generation";
        case TASK_TYPE_DATA_BACKUP:
            return "Data Backup";
        case TASK_TYPE_CUSTOM:
            return "Custom";
        default:
            return "Unknown";
    }
}

const char* TaskQueue_GetPriorityString(TaskPriority priority) {
    /**
     * @brief Get task priority string representation
     * 
     * @param priority Task priority enumeration value
     * @return String representation of the priority
     */
    switch (priority) {
        case TASK_PRIORITY_LOW:
            return "Low";
        case TASK_PRIORITY_NORMAL:
            return "Normal";
        case TASK_PRIORITY_HIGH:
            return "High";
        case TASK_PRIORITY_URGENT:
            return "Urgent";
        default:
            return "Unknown";
    }
}

const char* TaskQueue_GetStatusString(int status) {
    /**
     * @brief Get task status string representation
     * 
     * @param status Task status value (0-3)
     * @return String representation of the status
     */
    switch (status) {
        case 0:
            return "Pending";
        case 1:
            return "Processing";
        case 2:
            return "Completed";
        case 3:
            return "Failed";
        default:
            return "Unknown";
    }
}

int TaskQueue_SaveToFile(TaskQueue* queue, const char* filename) {
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
    if (queue == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write queue metadata
    size_t count = queue->size;
    if (fwrite(&count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    if (fwrite(&queue->next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all tasks
    TaskQueueNode* current = queue->front;
    while (current != NULL) {
        if (fwrite(&current->task, sizeof(Task), 1, file) != 1) {
            fclose(file);
            return -1;
        }
        current = current->next;
    }
    
    fclose(file);
    return 0;
}

TaskQueue* TaskQueue_LoadFromFile(TaskQueue* queue, const char* filename) {
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
    if (filename == NULL) {
        return NULL;
    }
    
    // Destroy existing queue if provided
    if (queue != NULL) {
        TaskQueue_Destroy(queue);
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        // File doesn't exist, create new queue
        return TaskQueue_Create(0);
    }
    
    // Read queue metadata
    size_t count;
    uint32_t next_id;
    
    if (fread(&count, sizeof(size_t), 1, file) != 1 ||
        fread(&next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return NULL;
    }
    
    // Create new queue
    queue = TaskQueue_Create(0);
    if (queue == NULL) {
        fclose(file);
        return NULL;
    }
    
    queue->next_id = next_id;
    
    // Read all tasks
    for (size_t i = 0; i < count; i++) {
        Task task;
        if (fread(&task, sizeof(Task), 1, file) != 1) {
            TaskQueue_Destroy(queue);
            fclose(file);
            return NULL;
        }
        
        // Enqueue task
        if (TaskQueue_Enqueue(queue, task.type, task.priority, task.description) == 0) {
            TaskQueue_Destroy(queue);
            fclose(file);
            return NULL;
        }
    }
    
    fclose(file);
    return queue;
}

