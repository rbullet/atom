#pragma once
#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup util Util
* @{
*/

/**
* @defgroup helpers Helpers
* @ingroup util
* @brief Low-level helper macros providing pointer and register utilities.
* @{
*/

#define CONTAINER_OF(ptr, type, member) ((type*)((uintptr_t)(ptr) - offsetof(type, member)))

/** @cond INTERNAL */
/**
 * @brief Internal macro: concatenates two tokens (no expansion).
 *
 * @param a first token
 * @param b second token
 */
#define _CAT2(a, b) a##b

/**
 * @brief Internal macro: concatenates two tokens (with expansion).
 *
 * @param a first token
 * @param b second token
 */
#define _CAT(a, b) _CAT2(a, b)
/** @endcond */

/**
 * @brief Compute pointer to an element at a given index with an explicit item size.
 *
 * This is a low-level helper to perform pointer arithmetic on untyped memory.
 *
 * @param ptr        base pointer
 * @param index      index of the element
 * @param item_size  size (in bytes) of one element at that pointer
 *
 * @return (void*) pointer to element at given index
 */
#define PTR_OFFSET(ptr, index, item_size) ((void*)((uint8_t* )(ptr) + ((index) * (item_size))))

#define REG(base, offset)  (*(volatile uint32_t *)((volatile void *)((uintptr_t)(base) + (offset))))

/**
 * @brief Extract a bit-field from a register value.
 *
 * The field must provide `<field>_MASK` and `<field>_OFFSET` defines.
 *
 * @param reg   the register value
 * @param field field base name
 *
 * @return extracted field value (right-shifted)
 */
#define REG_GET_FIELD(reg, field)  (((reg) & ((uint32_t)field##_MASK)) >> (field##_OFFSET))

/**
 * @brief Write a field inside a register.
 *
 * The field must provide `<field>_MASK` and `<field>_OFFSET` defines.
 *
 * @param reg   register lvalue
 * @param field field base name
 * @param value value to store
 */
#define REG_SET_FIELD(reg, field, value) ((reg) = (((reg) & ~((uint32_t)field##_MASK)) | (((uint32_t)(value) << field##_OFFSET) & (uint32_t)field##_MASK)))

/** @} */ /* end of helpers */
/** @} */ /* end of util */

#ifdef __cplusplus
}
#endif
