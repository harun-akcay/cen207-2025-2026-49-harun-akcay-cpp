/**
 * @file MaterialInventory.c
 * @brief Implementation of material inventory management functions using Double Linked List.
 */

#include "../header/MaterialInventory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

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

MaterialList* MaterialList_Create(void) {
    /**
     * @brief Create a new material list
     * 
     * Allocates memory for a new material list and initializes it to empty.
     * The material list is ready to use after creation.
     * 
     * @return Pointer to the newly created material list, or NULL on memory allocation failure
     * 
     * @note The caller is responsible for destroying the material list using MaterialList_Destroy()
     * to avoid memory leaks.
     */
    MaterialList* list = (MaterialList*)safe_malloc(sizeof(MaterialList));
    if (list == NULL) {
        return NULL;
    }
    
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->next_id = 1;
    
    return list;
}

void MaterialList_Destroy(MaterialList* list) {
    /**
     * @brief Destroy a material list and free all memory
     * 
     * Frees all nodes in the list and then frees the material list structure itself.
     * This function is safe to call with NULL pointer (no-op).
     * 
     * @param list Pointer to the material list to destroy (can be NULL)
     * 
     * @note After calling this function, the material list pointer becomes invalid
     * and should not be used.
     */
    if (list == NULL) {
        return;
    }
    
    MaterialNode* current = list->head;
    while (current != NULL) {
        MaterialNode* next = current->next;
        free(current);
        current = next;
    }
    
    free(list);
}

uint32_t MaterialList_AddMaterial(MaterialList* list, const char* name, 
                                   const char* category, uint32_t quantity,
                                   uint32_t unit_price_cents, const char* supplier) {
    /**
     * @brief Add a material to the list
     * 
     * Creates a new material entry and adds it to the end of the double linked list.
     * The material is assigned a unique ID automatically.
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @param name The material name string (max 127 characters, must not be NULL)
     * @param category The material category string (max 63 characters, must not be NULL)
     * @param quantity The quantity in stock
     * @param unit_price_cents The unit price in cents
     * @param supplier The supplier name string (max 63 characters, must not be NULL)
     * @return Material ID on success, 0 on error (NULL parameters or memory allocation failure)
     * 
     * @note Strings longer than maximum will be truncated.
     */
    if (list == NULL || name == NULL || category == NULL || supplier == NULL) {
        return 0;
    }
    
    // Create new node
    MaterialNode* new_node = (MaterialNode*)safe_malloc(sizeof(MaterialNode));
    if (new_node == NULL) {
        return 0;
    }
    
    // Initialize material data
    new_node->material.id = list->next_id++;
    new_node->material.quantity = quantity;
    new_node->material.unit_price_cents = unit_price_cents;
    
    // Copy name (ensure null termination)
    size_t name_len = strlen(name);
    size_t copy_len = (name_len < sizeof(new_node->material.name) - 1) 
                      ? name_len 
                      : sizeof(new_node->material.name) - 1;
    memcpy(new_node->material.name, name, copy_len);
    new_node->material.name[copy_len] = '\0';
    
    // Copy category (ensure null termination)
    size_t cat_len = strlen(category);
    copy_len = (cat_len < sizeof(new_node->material.category) - 1) 
               ? cat_len 
               : sizeof(new_node->material.category) - 1;
    memcpy(new_node->material.category, category, copy_len);
    new_node->material.category[copy_len] = '\0';
    
    // Copy supplier (ensure null termination)
    size_t supp_len = strlen(supplier);
    copy_len = (supp_len < sizeof(new_node->material.supplier) - 1) 
               ? supp_len 
               : sizeof(new_node->material.supplier) - 1;
    memcpy(new_node->material.supplier, supplier, copy_len);
    new_node->material.supplier[copy_len] = '\0';
    
    // Insert at the end of the list
    new_node->next = NULL;
    new_node->prev = list->tail;
    
    if (list->tail == NULL) {
        // First node in list
        list->head = new_node;
        list->tail = new_node;
    } else {
        // Append to end
        list->tail->next = new_node;
        list->tail = new_node;
    }
    
    list->size++;
    return new_node->material.id;
}

Material* MaterialList_FindMaterial(MaterialList* list, uint32_t id) {
    /**
     * @brief Find a material in the list by ID
     * 
     * Searches for a material with the given ID in the double linked list.
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @param id The material ID to search for
     * @return Pointer to the Material structure if found, NULL if not found or on error
     * 
     * @note The returned pointer is valid until the material is removed or the list is destroyed.
     */
    if (list == NULL) {
        return NULL;
    }
    
    MaterialNode* current = list->head;
    while (current != NULL) {
        if (current->material.id == id) {
            return &(current->material);
        }
        current = current->next;
    }
    
    return NULL; // Not found
}

Material* MaterialList_FindMaterialByName(MaterialList* list, const char* name) {
    /**
     * @brief Find a material in the list by name
     * 
     * Searches for a material with the given name in the double linked list.
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @param name The material name to search for (must not be NULL)
     * @return Pointer to the Material structure if found, NULL if not found or on error
     * 
     * @note The returned pointer is valid until the material is removed or the list is destroyed.
     */
    if (list == NULL || name == NULL) {
        return NULL;
    }
    
    MaterialNode* current = list->head;
    while (current != NULL) {
        if (strcmp(current->material.name, name) == 0) {
            return &(current->material);
        }
        current = current->next;
    }
    
    return NULL; // Not found
}

int MaterialList_UpdateMaterial(MaterialList* list, uint32_t id, 
                                 const char* name, const char* category,
                                 uint32_t quantity, uint32_t unit_price_cents,
                                 const char* supplier) {
    /**
     * @brief Update a material in the list
     * 
     * Updates the material with the given ID. Only non-NULL and non-UINT32_MAX
     * parameters are updated.
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @param id The material ID to update
     * @param name The new material name (NULL to keep unchanged)
     * @param category The new material category (NULL to keep unchanged)
     * @param quantity The new quantity (UINT32_MAX to keep unchanged)
     * @param unit_price_cents The new unit price (UINT32_MAX to keep unchanged)
     * @param supplier The new supplier name (NULL to keep unchanged)
     * @return 0 on success, -1 if material not found or on error
     */
    if (list == NULL) {
        return -1;
    }
    
    MaterialNode* current = list->head;
    while (current != NULL) {
        if (current->material.id == id) {
            // Update fields
            if (name != NULL) {
                size_t name_len = strlen(name);
                size_t copy_len = (name_len < sizeof(current->material.name) - 1) 
                                  ? name_len 
                                  : sizeof(current->material.name) - 1;
                memcpy(current->material.name, name, copy_len);
                current->material.name[copy_len] = '\0';
            }
            
            if (category != NULL) {
                size_t cat_len = strlen(category);
                size_t copy_len = (cat_len < sizeof(current->material.category) - 1) 
                                  ? cat_len 
                                  : sizeof(current->material.category) - 1;
                memcpy(current->material.category, category, copy_len);
                current->material.category[copy_len] = '\0';
            }
            
            if (quantity != UINT32_MAX) {
                current->material.quantity = quantity;
            }
            
            if (unit_price_cents != UINT32_MAX) {
                current->material.unit_price_cents = unit_price_cents;
            }
            
            if (supplier != NULL) {
                size_t supp_len = strlen(supplier);
                size_t copy_len = (supp_len < sizeof(current->material.supplier) - 1) 
                                  ? supp_len 
                                  : sizeof(current->material.supplier) - 1;
                memcpy(current->material.supplier, supplier, copy_len);
                current->material.supplier[copy_len] = '\0';
            }
            
            return 0;
        }
        current = current->next;
    }
    
    return -1; // Not found
}

int MaterialList_RemoveMaterial(MaterialList* list, uint32_t id) {
    /**
     * @brief Remove a material from the list
     * 
     * Finds and removes a material from the double linked list, freeing the associated memory.
     * Handles all cases: first node, last node, middle node, and single node.
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @param id The material ID to remove
     * @return 0 on success, -1 if material not found or on error
     */
    if (list == NULL) {
        return -1;
    }
    
    MaterialNode* current = list->head;
    while (current != NULL) {
        if (current->material.id == id) {
            // Found the node, remove it
            if (current->prev == NULL) {
                // First node
                list->head = current->next;
            } else {
                current->prev->next = current->next;
            }
            
            if (current->next == NULL) {
                // Last node
                list->tail = current->prev;
            } else {
                current->next->prev = current->prev;
            }
            
            free(current);
            list->size--;
            return 0;
        }
        current = current->next;
    }
    
    return -1; // Not found
}

size_t MaterialList_GetSize(MaterialList* list) {
    /**
     * @brief Get the number of materials in the list
     * 
     * Returns the current number of materials stored in the list.
     * This is a constant-time operation.
     * 
     * @param list Pointer to the material list (can be NULL)
     * @return Number of materials in the list, or 0 if list is NULL
     */
    if (list == NULL) {
        return 0;
    }
    return list->size;
}

MaterialNode* MaterialList_GetFirst(MaterialList* list) {
    /**
     * @brief Get the first material in the list (for iteration)
     * 
     * Returns a pointer to the first node in the list for iteration purposes.
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @return Pointer to the first MaterialNode, or NULL if list is empty
     */
    if (list == NULL) {
        return NULL;
    }
    return list->head;
}

MaterialNode* MaterialList_GetNext(MaterialNode* node) {
    /**
     * @brief Get the next material node (for iteration)
     * 
     * Returns a pointer to the next node in the list for iteration purposes.
     * 
     * @param node Pointer to the current MaterialNode (must not be NULL)
     * @return Pointer to the next MaterialNode, or NULL if at end
     */
    if (node == NULL) {
        return NULL;
    }
    return node->next;
}

int MaterialList_SaveToFile(MaterialList* list, const char* filename) {
    /**
     * @brief Save material list to binary file
     * 
     * Saves the material list to a binary file using fwrite().
     * The file format is:
     * 1. size_t size (number of materials)
     * 2. For each material: Material structure
     * 
     * @param list Pointer to the material list (must not be NULL)
     * @param filename The filename to save to (must not be NULL)
     * @return 0 on success, -1 on error (NULL parameters, file I/O error)
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     */
    if (list == NULL || filename == NULL) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        return -1;
    }
    
    // Write the number of materials first
    size_t material_count = list->size;
    if (fwrite(&material_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write next_id
    if (fwrite(&(list->next_id), sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        return -1;
    }
    
    // Write all materials sequentially
    MaterialNode* current = list->head;
    while (current != NULL) {
        // Write Material structure
        if (fwrite(&(current->material), sizeof(Material), 1, file) != 1) {
            fclose(file);
            return -1;
        }
        current = current->next;
    }
    
    fclose(file);
    return 0;
}

MaterialList* MaterialList_LoadFromFile(MaterialList* list, const char* filename) {
    /**
     * @brief Load material list from binary file
     * 
     * Loads the material list from a binary file using fread().
     * The file format is:
     * 1. size_t size (number of materials)
     * 2. uint32_t next_id
     * 3. For each material: Material structure
     * 
     * @param list Pointer to the material list (if NULL, a new list will be created)
     * @param filename The filename to load from (must not be NULL)
     * @return Pointer to material list on success, NULL on error
     * 
     * @note Uses binary format (fwrite/fread) as required by project specifications.
     * @note If list is NULL, a new list will be created and returned.
     * @note If list is not NULL, existing materials will be preserved and new materials will be added.
     */
    if (filename == NULL) {
        return NULL;
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL; // File doesn't exist or can't be opened
    }
    
    // Create material list if not provided
    if (list == NULL) {
        list = MaterialList_Create();
        if (list == NULL) {
            fclose(file);
            return NULL;
        }
    }
    
    // Read the number of materials
    size_t material_count;
    if (fread(&material_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        if (list != NULL && MaterialList_GetSize(list) == 0) {
            MaterialList_Destroy(list);
        }
        return NULL;
    }
    
    // Read next_id
    uint32_t next_id;
    if (fread(&next_id, sizeof(uint32_t), 1, file) != 1) {
        fclose(file);
        if (list != NULL && MaterialList_GetSize(list) == 0) {
            MaterialList_Destroy(list);
        }
        return NULL;
    }
    
    // Update next_id if loaded value is higher
    if (next_id > list->next_id) {
        list->next_id = next_id;
    }
    
    // Read all materials and add them to the list
    for (size_t i = 0; i < material_count; i++) {
        Material material;
        if (fread(&material, sizeof(Material), 1, file) != 1) {
            fclose(file);
            return list; // Return partial load
        }
        
        // Add material to list by creating a node
        MaterialNode* new_node = (MaterialNode*)safe_malloc(sizeof(MaterialNode));
        if (new_node == NULL) {
            fclose(file);
            return list; // Return partial load
        }
        
        // Copy material data
        new_node->material = material;
        
        // Insert at the end of the list
        new_node->next = NULL;
        new_node->prev = list->tail;
        
        if (list->tail == NULL) {
            // First node in list
            list->head = new_node;
            list->tail = new_node;
        } else {
            // Append to end
            list->tail->next = new_node;
            list->tail = new_node;
        }
        
        list->size++;
    }
    
    fclose(file);
    return list;
}



