/**
 * @file TaskQueue_test.cpp
 * @brief Unit tests for TaskQueue module (Queue/FIFO)
 * 
 * This file contains comprehensive unit tests for the TaskQueue module
 * using the Google Test framework. The tests cover queue operations, 
 * FIFO behavior, and error handling.
 */

#include "gtest/gtest.h"
#include "../../inventory_lib/header/TaskQueue.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

/**
 * @class TaskQueueTest
 * @brief Test fixture for TaskQueue unit tests
 */
class TaskQueueTest : public ::testing::Test {
protected:
	void SetUp() override {
		// Setup test data
	}

	void TearDown() override {
		// Clean up test data
	}
};

// Queue Creation and Destruction Tests

TEST_F(TaskQueueTest, TaskQueue_CreateDestroy) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	EXPECT_EQ(TaskQueue_GetSize(queue), 0);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_CreateWithZeroCapacity) {
	TaskQueue* queue = TaskQueue_Create(0);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_DestroyNull) {
	// Should not crash
	TaskQueue_Destroy(nullptr);
}

// Queue Enqueue Tests

TEST_F(TaskQueueTest, TaskQueue_EnqueueSingle) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	EXPECT_NE(id, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_EnqueueMultiple) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id1 = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	uint32_t id2 = TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	uint32_t id3 = TaskQueue_Enqueue(queue, TASK_TYPE_REPORT_GENERATION, TASK_PRIORITY_LOW, "Task3");
	
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_NE(id3, 0);
	EXPECT_NE(id1, id2);
	EXPECT_NE(id2, id3);
	EXPECT_EQ(TaskQueue_GetSize(queue), 3);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_EnqueueNull) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, nullptr);
	EXPECT_EQ(id, 0);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_EnqueueToNull) {
	uint32_t id = TaskQueue_Enqueue(nullptr, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	EXPECT_EQ(id, 0);
}

// Queue Dequeue Tests (FIFO)

TEST_F(TaskQueueTest, TaskQueue_DequeueSingle) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	EXPECT_NE(id, 0);
	
	Task task;
	int result = TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 0);
	EXPECT_EQ(task.id, id);
	EXPECT_EQ(task.type, TASK_TYPE_MATERIAL_ORDER);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_DequeueFIFO) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id1 = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	uint32_t id2 = TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	uint32_t id3 = TaskQueue_Enqueue(queue, TASK_TYPE_REPORT_GENERATION, TASK_PRIORITY_LOW, "Task3");
	
	// Dequeue should return first task (FIFO)
	Task task;
	int result = TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(task.id, id1); // First in, first out
	EXPECT_EQ(task.type, TASK_TYPE_MATERIAL_ORDER);
	
	// Dequeue second task
	result = TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(task.id, id2);
	EXPECT_EQ(task.type, TASK_TYPE_NOTIFICATION);
	
	// Dequeue third task
	result = TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(task.id, id3);
	EXPECT_EQ(task.type, TASK_TYPE_REPORT_GENERATION);
	
	EXPECT_EQ(TaskQueue_GetSize(queue), 0);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_DequeueFromEmpty) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	Task task;
	int result = TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_DequeueNull) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	
	int result = TaskQueue_Dequeue(queue, nullptr);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

// Queue Peek Tests

TEST_F(TaskQueueTest, TaskQueue_Peek) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	EXPECT_NE(id, 0);
	
	Task task;
	int result = TaskQueue_Peek(queue, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 1); // Size should not change
	EXPECT_EQ(task.id, id);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_PeekFromEmpty) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	Task task;
	int result = TaskQueue_Peek(queue, &task);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

// Queue IsEmpty Tests

TEST_F(TaskQueueTest, TaskQueue_IsEmpty) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	EXPECT_EQ(TaskQueue_IsEmpty(queue), 1);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	EXPECT_EQ(TaskQueue_IsEmpty(queue), 0);
	
	Task task;
	TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(TaskQueue_IsEmpty(queue), 1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_IsEmptyNull) {
	EXPECT_EQ(TaskQueue_IsEmpty(nullptr), -1);
}

// Queue GetSize Tests

TEST_F(TaskQueueTest, TaskQueue_GetSize) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	EXPECT_EQ(TaskQueue_GetSize(queue), 0);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	EXPECT_EQ(TaskQueue_GetSize(queue), 1);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	EXPECT_EQ(TaskQueue_GetSize(queue), 2);
	
	Task task;
	TaskQueue_Dequeue(queue, &task);
	EXPECT_EQ(TaskQueue_GetSize(queue), 1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_GetSizeNull) {
	EXPECT_EQ(TaskQueue_GetSize(nullptr), 0);
}

// Queue Remove Task Tests

TEST_F(TaskQueueTest, TaskQueue_RemoveTask) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id1 = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	uint32_t id2 = TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	uint32_t id3 = TaskQueue_Enqueue(queue, TASK_TYPE_REPORT_GENERATION, TASK_PRIORITY_LOW, "Task3");
	
	Task task;
	int result = TaskQueue_RemoveTask(queue, id2, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 2);
	EXPECT_EQ(task.id, id2);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_RemoveNonExistentTask) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	
	Task task;
	int result = TaskQueue_RemoveTask(queue, 999, &task);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

// Queue Find Task Tests

TEST_F(TaskQueueTest, TaskQueue_FindTask) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Test Task");
	EXPECT_NE(id, 0);
	
	Task task;
	int result = TaskQueue_FindTask(queue, id, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(task.id, id);
	EXPECT_EQ(TaskQueue_GetSize(queue), 1); // Size should not change
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_FindNonExistentTask) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	
	Task task;
	int result = TaskQueue_FindTask(queue, 999, &task);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

// Queue Capacity Tests

TEST_F(TaskQueueTest, TaskQueue_EnqueueCapacityFull) {
	TaskQueue* queue = TaskQueue_Create(2);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id1 = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	uint32_t id2 = TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	EXPECT_NE(id1, 0);
	EXPECT_NE(id2, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 2);
	
	// Queue is full, should fail
	uint32_t id3 = TaskQueue_Enqueue(queue, TASK_TYPE_REPORT_GENERATION, TASK_PRIORITY_LOW, "Task3");
	EXPECT_EQ(id3, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 2);
	
	TaskQueue_Destroy(queue);
}

// Queue Remove Task Edge Cases

TEST_F(TaskQueueTest, TaskQueue_RemoveTaskFromFront) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id1 = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	uint32_t id2 = TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	
	Task task;
	int result = TaskQueue_RemoveTask(queue, id1, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(task.id, id1);
	EXPECT_EQ(TaskQueue_GetSize(queue), 1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_RemoveTaskFromRear) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id1 = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	uint32_t id2 = TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	
	Task task;
	int result = TaskQueue_RemoveTask(queue, id2, &task);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(task.id, id2);
	EXPECT_EQ(TaskQueue_GetSize(queue), 1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_RemoveTaskNullTask) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	uint32_t id = TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	
	int result = TaskQueue_RemoveTask(queue, id, nullptr);
	EXPECT_EQ(result, 0);
	EXPECT_EQ(TaskQueue_GetSize(queue), 0);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_RemoveTaskFromEmpty) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	Task task;
	int result = TaskQueue_RemoveTask(queue, 1, &task);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

// Queue Display Tests

TEST_F(TaskQueueTest, TaskQueue_Display) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	
	// Display should not crash
	TaskQueue_Display(queue, 0);
	TaskQueue_Display(queue, 1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_DisplayEmpty) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	// Display empty queue should not crash
	TaskQueue_Display(queue, 0);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_DisplayNull) {
	// Display null queue should not crash
	TaskQueue_Display(nullptr, 0);
}

// Queue String Conversion Tests

TEST_F(TaskQueueTest, TaskQueue_GetTypeString) {
	EXPECT_STREQ(TaskQueue_GetTypeString(TASK_TYPE_MATERIAL_ORDER), "Material Order");
	EXPECT_STREQ(TaskQueue_GetTypeString(TASK_TYPE_NOTIFICATION), "Notification");
	EXPECT_STREQ(TaskQueue_GetTypeString(TASK_TYPE_REPORT_GENERATION), "Report Generation");
	EXPECT_STREQ(TaskQueue_GetTypeString(TASK_TYPE_DATA_BACKUP), "Data Backup");
	EXPECT_STREQ(TaskQueue_GetTypeString(TASK_TYPE_CUSTOM), "Custom");
	EXPECT_STREQ(TaskQueue_GetTypeString((TaskType)999), "Unknown");
}

TEST_F(TaskQueueTest, TaskQueue_GetPriorityString) {
	EXPECT_STREQ(TaskQueue_GetPriorityString(TASK_PRIORITY_LOW), "Low");
	EXPECT_STREQ(TaskQueue_GetPriorityString(TASK_PRIORITY_NORMAL), "Normal");
	EXPECT_STREQ(TaskQueue_GetPriorityString(TASK_PRIORITY_HIGH), "High");
	EXPECT_STREQ(TaskQueue_GetPriorityString(TASK_PRIORITY_URGENT), "Urgent");
	EXPECT_STREQ(TaskQueue_GetPriorityString((TaskPriority)999), "Unknown");
}

TEST_F(TaskQueueTest, TaskQueue_GetStatusString) {
	EXPECT_STREQ(TaskQueue_GetStatusString(0), "Pending");
	EXPECT_STREQ(TaskQueue_GetStatusString(1), "Processing");
	EXPECT_STREQ(TaskQueue_GetStatusString(2), "Completed");
	EXPECT_STREQ(TaskQueue_GetStatusString(3), "Failed");
	EXPECT_STREQ(TaskQueue_GetStatusString(999), "Unknown");
}

// Queue File Operations Tests

TEST_F(TaskQueueTest, TaskQueue_SaveToFile) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	
	int result = TaskQueue_SaveToFile(queue, "test_tasks.bin");
	EXPECT_EQ(result, 0);
	
	TaskQueue_Destroy(queue);
	
	// Clean up
	remove("test_tasks.bin");
}

TEST_F(TaskQueueTest, TaskQueue_SaveToFileNull) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	int result = TaskQueue_SaveToFile(nullptr, "test.bin");
	EXPECT_EQ(result, -1);
	
	result = TaskQueue_SaveToFile(queue, nullptr);
	EXPECT_EQ(result, -1);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_LoadFromFile) {
	TaskQueue* queue = TaskQueue_Create(10);
	EXPECT_NE(queue, nullptr);
	
	TaskQueue_Enqueue(queue, TASK_TYPE_MATERIAL_ORDER, TASK_PRIORITY_NORMAL, "Task1");
	TaskQueue_Enqueue(queue, TASK_TYPE_NOTIFICATION, TASK_PRIORITY_HIGH, "Task2");
	
	TaskQueue_SaveToFile(queue, "test_tasks.bin");
	TaskQueue_Destroy(queue);
	
	// Load from file
	TaskQueue* loaded = TaskQueue_LoadFromFile(nullptr, "test_tasks.bin");
	EXPECT_NE(loaded, nullptr);
	EXPECT_EQ(TaskQueue_GetSize(loaded), 2);
	
	// Check tasks by dequeueing (FIFO order)
	Task task;
	int result = TaskQueue_Dequeue(loaded, &task);
	EXPECT_EQ(result, 0);
	EXPECT_STREQ(task.description, "Task1");
	EXPECT_EQ(task.type, TASK_TYPE_MATERIAL_ORDER);
	EXPECT_EQ(task.priority, TASK_PRIORITY_NORMAL);
	
	result = TaskQueue_Dequeue(loaded, &task);
	EXPECT_EQ(result, 0);
	EXPECT_STREQ(task.description, "Task2");
	EXPECT_EQ(task.type, TASK_TYPE_NOTIFICATION);
	EXPECT_EQ(task.priority, TASK_PRIORITY_HIGH);
	
	TaskQueue_Destroy(loaded);
	
	// Clean up
	remove("test_tasks.bin");
}

TEST_F(TaskQueueTest, TaskQueue_LoadFromFileNonExistent) {
	TaskQueue* queue = TaskQueue_LoadFromFile(nullptr, "non_existent.bin");
	EXPECT_NE(queue, nullptr); // Should create new queue
	EXPECT_EQ(TaskQueue_GetSize(queue), 0);
	
	TaskQueue_Destroy(queue);
}

TEST_F(TaskQueueTest, TaskQueue_LoadFromFileNull) {
	TaskQueue* queue = TaskQueue_LoadFromFile(nullptr, nullptr);
	EXPECT_EQ(queue, nullptr);
}

