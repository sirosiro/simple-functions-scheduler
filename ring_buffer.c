#include "ring_buffer.h"

/* For NULL pointer check */
#ifndef NULL
#define NULL ((void*)0)
#endif

/**
 * @brief Default internal data copy function.
 * @note This is a simple byte-by-byte copy loop to avoid dependency on string.h (memcpy).
 */
static void _default_copy(void* dest, const void* src, rb_size_t len) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (len--) {
        *d++ = *s++;
    }
}

rb_bool rb_init(rb_handle_t handle, void* buffer, rb_size_t size, rb_bool overwrite_on_full, rb_copy_func_t read_func, rb_copy_func_t write_func) {
    if (handle == NULL || buffer == NULL || size == 0) {
        return RB_FALSE;
    }

    handle->buffer = (unsigned char*)buffer;
    handle->size = size;
    handle->head = 0;
    handle->tail = 0;
    handle->is_full = RB_FALSE;
    handle->overwrite_on_full = overwrite_on_full;

    handle->read_from_ring = (read_func != NULL) ? read_func : _default_copy;
    handle->write_to_ring = (write_func != NULL) ? write_func : _default_copy;

    return RB_TRUE;
}

rb_size_t rb_get_used_space(rb_handle_t handle) {
    if (handle->is_full) {
        return handle->size;
    }
    if (handle->head >= handle->tail) {
        return handle->head - handle->tail;
    }
    return handle->size - (handle->tail - handle->head);
}

rb_size_t rb_get_free_space(rb_handle_t handle) {
    return handle->size - rb_get_used_space(handle);
}

rb_size_t rb_write(rb_handle_t handle, const void* data, rb_size_t length) {
    if (data == NULL || length == 0) {
        return 0;
    }

    rb_size_t free_space = rb_get_free_space(handle);

    if (!handle->overwrite_on_full && length > free_space) {
        return 0; /* Not enough space and overwrite is disabled */
    }
    
    /* If overwrite is enabled and we need more space than is free,
       we advance the tail pointer to "discard" the oldest data. */
    if (handle->overwrite_on_full && length > free_space) {
        rb_size_t space_to_create = length - free_space;
        handle->tail = (handle->tail + space_to_create) % handle->size;
    }
    
    rb_size_t bytes_to_write = length;
    
    /* The actual number of bytes we write should not exceed buffer size */
    if (bytes_to_write > handle->size) {
        data = (const unsigned char*)data + (bytes_to_write - handle->size);
        bytes_to_write = handle->size;
    }


    /* Perform the write */
    rb_size_t part1 = handle->size - handle->head;
    if (bytes_to_write > part1) {
        /* Wraps around */
        handle->write_to_ring(handle->buffer + handle->head, data, part1);
        handle->write_to_ring(handle->buffer, (const unsigned char*)data + part1, bytes_to_write - part1);
    } else {
        /* Does not wrap around */
        handle->write_to_ring(handle->buffer + handle->head, data, bytes_to_write);
    }

    handle->head = (handle->head + bytes_to_write) % handle->size;
    
    if (handle->head == handle->tail) {
        handle->is_full = RB_TRUE;
    }

    return length; /* Per manifest, return requested length on overwrite, not written length */
}

static rb_size_t _internal_read(rb_handle_t handle, void* buffer, rb_size_t length, rb_bool advance_tail) {
    if (buffer == NULL || length == 0) {
        return 0;
    }

    rb_size_t used_space = rb_get_used_space(handle);
    rb_size_t bytes_to_read = (length > used_space) ? used_space : length;

    if (bytes_to_read == 0) {
        return 0;
    }

    /* Perform the read */
    rb_size_t part1 = handle->size - handle->tail;
    if (bytes_to_read > part1) {
        /* Wraps around */
        handle->read_from_ring(buffer, handle->buffer + handle->tail, part1);
        handle->read_from_ring((unsigned char*)buffer + part1, handle->buffer, bytes_to_read - part1);
    } else {
        /* Does not wrap around */
        handle->read_from_ring(buffer, handle->buffer + handle->tail, bytes_to_read);
    }
    
    if (advance_tail) {
        handle->tail = (handle->tail + bytes_to_read) % handle->size;
        handle->is_full = RB_FALSE;
    }

    return bytes_to_read;
}

rb_size_t rb_read(rb_handle_t handle, void* buffer, rb_size_t length) {
    return _internal_read(handle, buffer, length, RB_TRUE);
}

rb_size_t rb_peek(rb_handle_t handle, void* buffer, rb_size_t length) {
    return _internal_read(handle, buffer, length, RB_FALSE);
}
