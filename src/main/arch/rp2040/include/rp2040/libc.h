#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

// --- Callback type for reading bytes ---
typedef ssize_t (*libc_read_func_t)(uint8_t* ptr, size_t len);

// --- Callback type for writing bytes ---
typedef ssize_t (*libc_write_func_t)(uint8_t const* ptr, size_t len);

// --- Get current read callback ---
libc_read_func_t libc_get_read_callback(void);

// --- Set read callback ---
void libc_set_read_callback(libc_read_func_t read_func);

// --- Get current write callback ---
libc_write_func_t libc_get_write_callback(void);

// --- Set write callback ---
void libc_set_write_callback(libc_write_func_t write_func);

#ifdef __cplusplus
}
#endif
