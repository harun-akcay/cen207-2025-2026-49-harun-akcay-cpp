/**
 * @file MaterialInventory.h
 * 
 * @brief Provides functions for material inventory management using Double Linked List
 */

#ifndef MATERIAL_INVENTORY_H
#define MATERIAL_INVENTORY_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup MaterialInventory Material Inventory Implementation
 * @brief Double Linked List data structure for material inventory management
 * @{
 */

/**
 * @brief Material structure for inventory
 * 
 * Represents a material item in the inventory management system.
 * Stores material name, quantity, unit price, and category.
 */
typedef struct {
    /** @brief Material ID (unique identifier) */
    uint32_t id;
    /** @brief Material name string (maximum 127 characters + null terminator) */
    char name[128];
    /** @brief Material category string (maximum 63 characters + null terminator) */
    char category[64];
    /** @brief Quantity in stock */
    uint32_t quantity;
    /** @brief Unit price in cents (to avoid floating point issues) */
    uint32_t unit_price_cents;
    /** @brief Supplier name string (maximum 63 characters + null terminator) */
    char supplier[64];
} Material;

/**
 * @brief Double Linked List Node structure
 * 
 * Represents a single entry in the double linked list.
 * Each node contains a Material and pointers to the next and previous nodes.
 */
typedef struct MaterialNode {
    /** @brief Material data stored in this node */
    Material material;
    /** @brief Pointer to the previous node in the list (NULL if first node) */
    struct MaterialNode* prev;
    /** @brief Pointer to the next node in the list (NULL if last node) */
    struct MaterialNode* next;
} MaterialNode;

/**
 * @brief Double Linked List structure
 * 
 * Main double linked list data structure that stores materials.
 * Maintains pointers to both head and tail for efficient operations.
 */
typedef struct {
    /** @brief Pointer to the first node in the list (NULL if empty) */
    MaterialNode* head;
    /** @brief Pointer to the last node in the list (NULL if empty) */
    MaterialNode* tail;
    /** @brief Current number of materials stored in the list */
    size_t size;
    /** @brief Next available material ID */
    uint32_t next_id;
} MaterialList;

/**
 * @}
 */

// Material List Functions

/**
 * @brief Create a new material list.
 * @return Pointer to the material list, or NULL on error.
 */
MaterialList* MaterialList_Create(void);

/**
 * @brief Destroy a material list and free all memory.
 * @param list Pointer to the material list.
 */
void MaterialList_Destroy(MaterialList* list);

/**
 * @brief Add a material to the list.
 * @param list Pointer to the material list.
 * @param name The material name (max 127 characters).
 * @param category The material category (max 63 characters).
 * @param quantity The quantity in stock.
 * @param unit_price_cents The unit price in cents.
 * @param supplier The supplier name (max 63 characters).
 * @return Material ID on success, 0 on error.
 */
uint32_t MaterialList_AddMaterial(MaterialList* list, const char* name, 
                                   const char* category, uint32_t quantity,
                                   uint32_t unit_price_cents, const char* supplier);

/**
 * @brief Find a material in the list by ID.
 * @param list Pointer to the material list.
 * @param id The material ID to search for.
 * @return Pointer to the Material structure, or NULL if not found.
 */
Material* MaterialList_FindMaterial(MaterialList* list, uint32_t id);

/**
 * @brief Find a material in the list by name.
 * @param list Pointer to the material list.
 * @param name The material name to search for.
 * @return Pointer to the Material structure, or NULL if not found.
 */
Material* MaterialList_FindMaterialByName(MaterialList* list, const char* name);

/**
 * @brief Update a material in the list.
 * @param list Pointer to the material list.
 * @param id The material ID to update.
 * @param name The new material name (NULL to keep unchanged).
 * @param category The new material category (NULL to keep unchanged).
 * @param quantity The new quantity (UINT32_MAX to keep unchanged).
 * @param unit_price_cents The new unit price (UINT32_MAX to keep unchanged).
 * @param supplier The new supplier name (NULL to keep unchanged).
 * @return 0 on success, -1 if material not found.
 */
int MaterialList_UpdateMaterial(MaterialList* list, uint32_t id, 
                                 const char* name, const char* category,
                                 uint32_t quantity, uint32_t unit_price_cents,
                                 const char* supplier);

/**
 * @brief Remove a material from the list.
 * @param list Pointer to the material list.
 * @param id The material ID to remove.
 * @return 0 on success, -1 if material not found.
 */
int MaterialList_RemoveMaterial(MaterialList* list, uint32_t id);

/**
 * @brief Get the number of materials in the list.
 * @param list Pointer to the material list.
 * @return Number of materials.
 */
size_t MaterialList_GetSize(MaterialList* list);

/**
 * @brief Get the first material in the list (for iteration).
 * @param list Pointer to the material list.
 * @return Pointer to the first MaterialNode, or NULL if list is empty.
 */
MaterialNode* MaterialList_GetFirst(MaterialList* list);

/**
 * @brief Get the next material node (for iteration).
 * @param node Pointer to the current MaterialNode.
 * @return Pointer to the next MaterialNode, or NULL if at end.
 */
MaterialNode* MaterialList_GetNext(MaterialNode* node);

/**
 * @brief Save material list to binary file.
 * @param list Pointer to the material list.
 * @param filename The filename to save to.
 * @return 0 on success, -1 on error.
 * 
 * @note Uses binary format (fwrite/fread) as required.
 */
int MaterialList_SaveToFile(MaterialList* list, const char* filename);

/**
 * @brief Load material list from binary file.
 * @param list Pointer to the material list (will be created if NULL).
 * @param filename The filename to load from.
 * @return Pointer to material list on success, NULL on error.
 * 
 * @note Uses binary format (fwrite/fread) as required.
 */
MaterialList* MaterialList_LoadFromFile(MaterialList* list, const char* filename);

#ifdef __cplusplus
}
#endif

#endif // MATERIAL_INVENTORY_H



