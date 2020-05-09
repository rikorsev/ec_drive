#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "egl_chunk.h"

void egl_chunk_reset(egl_chunks_t *chunks)
{
    chunks->in = 0;
    chunks->out = 0;
}

egl_result_t egl_chunk_init(egl_chunks_t *chunks, char *buffer, size_t size)
{
    assert(chunks != NULL);
    assert(buffer != NULL);

    if(size == 0 || size % chunks->number != 0)
    {
        return EGL_INVALID_PARAM;
    }

    chunks->size = size / chunks->number;

    for(int i = 0; i < chunks->number; i++)
    {
        chunks->chunk[i].buf  = &buffer[i * chunks->size];
        chunks->chunk[i].size = 0;
    }

    egl_chunk_reset(chunks);

    return EGL_SUCCESS;
}

void egl_chunk_out_index_inc(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);
    chunks->out = (chunks->out + 1) % chunks->number;
}

void egl_chunk_in_index_inc(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);
    chunks->out = (chunks->in + 1) % chunks->number;
}

egl_result_t egl_chunk_write_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t size)
{
    egl_chunk_t *chunk = NULL;

    assert(chunks != NULL);
    assert(data   != NULL);

    if(index > chunks->number)
    {
        return EGL_INVALID_PARAM;
    }

    if(offset + size > chunks->size)
    {
        return EGL_OUT_OF_BOUNDARY;
    }

    chunk = &chunks->chunk[index];
    chunk->size = size;

    memcpy(chunk->buf + offset, data, size);

    return EGL_SUCCESS;
}

egl_result_t egl_chunk_write(egl_chunks_t *chunks, void *data, size_t size)
{
    egl_result_t result = egl_chunk_write_with_offset_and_index(chunks, chunks->in, data, 0, size);
    
    egl_chunk_in_index_inc(chunks);
    
    return result;
}

egl_result_t egl_chunk_read_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t *size)
{
    egl_chunk_t *chunk = NULL; 
    size_t read_size = 0;

    assert(chunks != NULL);
    assert(data   != NULL);
    assert(size   != NULL);

    if(index > chunks->number)
    {
        return EGL_INVALID_PARAM;
    }

    chunk = &chunks->chunk[index];

    if(offset > chunk->size)
    {
        return EGL_OUT_OF_BOUNDARY;
    } 

    read_size = chunk->size - offset;
    read_size = read_size < *size ? read_size : *size;

    memcpy(data, chunk->buf + offset, read_size);

    *size = read_size;

    return EGL_SUCCESS;
}

egl_result_t egl_chunk_read(egl_chunks_t *chunks, void *data, size_t *size)
{
    egl_result_t result = egl_chunk_read_with_offset_and_index(chunks, chunks->out, data, 0, size);

    egl_chunk_out_index_inc(chunks);

    return result;
}

egl_result_t egl_chunk_clear(egl_chunks_t *chunks, unsigned int index)
{
    egl_chunk_t *chunk = egl_chunk_by_index_get(chunks, index);
    if(chunk == NULL)
    {
        return EGL_INVALID_PARAM;
    }

    chunk->size = 0;

    return EGL_SUCCESS;
}

unsigned int egl_chunk_in_index_get(egl_chunks_t *chunks)
{
    return chunks->in;
}

unsigned int egl_chunk_out_index_get(egl_chunks_t *chunks)
{
    return chunks->out;
}

egl_result_t egl_chunk_read_and_clear(egl_chunks_t *chunks, void *data, size_t *size)
{
    unsigned int index  = egl_chunk_out_index_get(chunks);
    egl_result_t result = egl_chunk_read(chunks, data, size);

    if(result == EGL_SUCCESS)
    {
      result = egl_chunk_clear(chunks, index);
    }

    return result;
}

egl_chunk_t *egl_chunk_by_index_get(egl_chunks_t *chunks, unsigned int index)
{
    assert(chunks  != NULL);

    return index < chunks->number ? &chunks->chunk[index] : NULL;
}

egl_chunk_t *egl_chunk_out_current_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    egl_chunk_t *chunk = egl_chunk_by_index_get(chunks, chunks->out++);
    chunks->out %= chunks->number;

    return chunk;
}

egl_chunk_t *egl_chunk_in_current_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    egl_chunk_t *chunk = egl_chunk_by_index_get(chunks, chunks->in++);
    chunks->in %= chunks->number;

    return chunk;
}

egl_chunk_t *egl_chunk_in_previous_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    return egl_chunk_by_index_get(chunks, (chunks->in - 1) % chunks->number);
}

egl_chunk_t *egl_chunk_out_previous_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    return egl_chunk_by_index_get(chunks, (chunks->out - 1) % chunks->number);
}

egl_chunk_t *egl_chunk_in_next_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    return egl_chunk_by_index_get(chunks, (chunks->in + 1) % chunks->number);
}

egl_chunk_t *egl_chunk_out_next_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    return egl_chunk_by_index_get(chunks, (chunks->out + 1) % chunks->number);
}