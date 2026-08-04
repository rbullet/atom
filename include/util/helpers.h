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
 * @brief Low-level helper macros for pointer, bit manipulation, and register operations.
 * @{
 */

/**
 * @brief Compute the address of the containing structure from a member pointer.
 *
 * Given a pointer to a structure member, returns a pointer to the enclosing
 * structure instance.
 *
 * @param ptr    Pointer to the structure member.
 * @param type   Type of the enclosing structure.
 * @param member Name of the member within @p type.
 *
 * @return Pointer to the enclosing structure.
 */
#define CONTAINER_OF(ptr, type, member) ((type*)((uintptr_t)(ptr) - offsetof(type, member)))

/** @cond INTERNAL */
/**
 * @brief Internal macro: concatenates two tokens without macro expansion.
 *
 * @param a First token.
 * @param b Second token.
 */
#define _CAT2(a, b) a##b

/**
 * @brief Internal macro: concatenates two tokens with macro expansion.
 *
 * @param a First token.
 * @param b Second token.
 */
#define _CAT(a, b) _CAT2(a, b)
/** @endcond */

/**
 * @brief Compute a pointer to an element at a given index.
 *
 * Performs pointer arithmetic on untyped memory using an explicit element size.
 *
 * @param ptr       Base pointer.
 * @param index     Zero-based element index.
 * @param item_size Size, in bytes, of each element.
 *
 * @return Pointer to the indexed element.
 */
#define PTR_OFFSET(ptr, index, item_size) ((void*)((uint8_t*)(ptr) + ((index) * (item_size))))

/**
 * @brief Set one or more bits.
 *
 * Sets all bits specified by @p mask in @p value.
 *
 * @param value Lvalue to modify.
 * @param mask  Bit mask of bits to set.
 */
#define BITS_SET(value, mask) ((value) |= (uint32_t)(mask))

/**
 * @brief Clear one or more bits.
 *
 * Clears all bits specified by @p mask in @p value.
 *
 * @param value Lvalue to modify.
 * @param mask  Bit mask of bits to clear.
 */
#define BITS_CLEAR(value, mask) ((value) &= ~(uint32_t)(mask))

/**
 * @brief Compute the address of a memory-mapped 32-bit register.
 *
 * Returns a pointer to a 32-bit volatile register located at the specified
 * base address and byte offset.
 *
 * @param base   Peripheral base address.
 * @param offset Register byte offset.
 *
 * @return Pointer to the register.
 */
#define REG(base, offset) ((volatile uint32_t *)((uintptr_t)(base) + (offset)))

/**
 * @brief Read a 32-bit memory-mapped register.
 *
 * @param reg Pointer to the register.
 *
 * @return Register value.
 */
#define REG_READ(reg) (*(reg))

/**
 * @brief Write a 32-bit memory-mapped register.
 *
 * @param reg   Pointer to the register.
 * @param value Value to write.
 */
#define REG_WRITE(reg, value) (*(reg) = (value))

/**
 * @brief Extract a bit-field from a register.
 *
 * Reads the field identified by @p field from the register pointed to by
 * @p reg. The field must define the macros `<field>_MASK` and
 * `<field>_OFFSET`.
 *
 * @param reg   Pointer to the register.
 * @param field Field base name.
 *
 * @return Field value, right-aligned.
 */
#define REG_GET_FIELD(reg, field) (((*(reg)) & ((uint32_t)(field##_MASK))) >> (field##_OFFSET))

/**
 * @brief Update a bit-field within a register.
 *
 * Performs a read-modify-write operation on the register pointed to by
 * @p reg. The field must define the macros `<field>_MASK` and
 * `<field>_OFFSET`.
 *
 * @note This macro performs a read-modify-write operation and should only
 * be used with registers that support it.
 *
 * @param reg   Pointer to the register.
 * @param field Field base name.
 * @param value Value to store in the field.
 */
#define REG_SET_FIELD(reg, field, value) ((*(reg)) = (((*(reg)) & ~((uint32_t)(field##_MASK))) | ((((uint32_t)(value)) << (field##_OFFSET)) & (uint32_t)(field##_MASK))))

/**
 * @brief Return the number of elements in a statically allocated array.
 *
 * Computes the element count of an array at compile time.
 *
 * This macro must only be used with actual arrays. It must not be used with
 * pointers, as `sizeof(pointer)` returns the pointer size rather than the
 * size of the referenced object.
 *
 * Example:
 *
 * @code
 * uint32_t values[] = {1, 2, 3, 4};
 *
 * size_t count = ARRAY_SIZE(values); // 4
 * @endcode
 *
 * @param array Array whose element count is requested.
 *
 * @return Number of elements contained in the array.
 */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/** @} */ /* end of helpers */
/** @} */ /* end of util */

#ifdef __cplusplus
}
#endif