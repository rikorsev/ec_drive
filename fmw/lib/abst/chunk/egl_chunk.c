#include <assert.h>

egl_resulr_t egl_chunk_init(egl_chunks_t *chunks, void *buffer, size_t size)
{
    assert(chunks != NULL);
    assert(data   != NULL);

    if(size < chunks->size * chunks->number)
    {
        return EGL_INVALID_PARAM;
    }

    for(int i = 0; i < chunks->num; i++)
    {
        chunks->chunk[i].buf = &buffer[i * chunks->size];
        chunks->chunk[i].size = 0;
    }
}

// egl_result_t egl_chunk_write(egl_chunks_t *chunks, void *data, size_t size)
// {
//     unsigned int index current_index = chunks->in;
//     unsigned int index num = (size / chunk->number + 1);
//     egl_chunk_t *chunk = NULL;

//     assert(chunks != NULL);
//     assert(data   != NULL);

//     /* Update write index */
//     chunks->in = num % chunk->size;

//      Copy data to chanks 
//     for(int i = 0; i < num; i++)
//     {
//         chunk = chunks->chunk[(current_index + i) % chunks->number];
        
//         assert(chunk != NULL);
//         assert(chunk->buff != NULL);
        
//         chunk->size = size > chunks->size ? chunks->size ? size;
//         memcpy(chunk->buf, data + i * chunk->size, chunk->size);
//     }

//     return EGL_SUCCESS;
// }

egl_result_t egl_chunk_write_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t size)
{
    egl_chunk_t *chunk = NULL;
    size_t write_size = 0;

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

    chunk = chunks->chunk[index];

    memcpy(chunk->buf + offset, data, size);

    return EGL_SUCCESS;
}

egl_result_t egl_chunk_write(egl_chunks_t *chunks, void *data, size_t size)
{
    egl_result_t result = egl_chunk_write_with_offset_and_index(chunks, chunks->in++, data, 0, size);
    
    chunks->in %= chunks->number;
    
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

    if(offset > chunk->size)
    {
        return EGL_OUT_OF_BOUNDARY;
    } 

    chunk = chunks->chunk[index];
    read_size = chunk->size - offset;
    read_size = read_size < size ? read_size : size;

    memcpy(data, chunk->buf + offset, read_size);

    *size = read_size;

    return EGL_SUCCESS;
}

egl_result_t egl_chunk_read(egl_chunks_t *chunks, void *data, size_t *size)
{
    egl_result_t result = egl_chunk_read_with_offset_and_index(chunks, chunks->out++, data, 0, size);

    chunks->out %= chunks->number;

    return result;
}

egl_result_t egl_chunk_clear(egl_chunks_t *chunks, unsigned int index)
{
    if(index > chunks->number)
    {
        return EGL_INVALID_PARAM;
    }

    chunks->chunk[index].size = 0;

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

// egl_result_t egl_chunk_read(egl_chunks_t *chunks, void *data, size_t *size)
// {
//     unsigned int index current_index = chunks->out;
//     unsigned int index num = (size / chunk->number + 1);
//     size_t read_size  = 0;
//     size_t offset = 0;
//     egl_chunk_t *chunk = NULL;

//     assert(chunks  != NULL);
//     assert(data    != NULL);

//     /* Update read index */
//     chunks->out = num % chunk->size;

//      Copy data from chanks  
//     for(int i = 0; i < num; i++)
//     {
//         chunk = chunks->chunk[(current_index + i) % chunks->number];
        
//         assert(chunk != NULL);
//         assert(chunk->buff != NULL);

//         read_size = size > chunks->size ? chunks->size ? size;
//         memcpy(data + offset, chunk->buf, read_size);
//         offset += read_size;
//     }

//     *size = offset;

//     return EGL_SUCCESS;
// }

egl_chunk_t *egl_chunk_out_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    egl_chunk_t *chunk = chunks->chunk[chunks->out++];
    chunks->out %= chunks->size;

    return chunk;
}

egl_chunk_t *egl_chunk_in_get(egl_chunks_t *chunks)
{
    assert(chunks  != NULL);

    egl_chunk_t *chunk = chunks->chunk[chunks->in++];
    chunks->in %= chunks->size;

    return chunk;
}

egl_chunk_t *egl_chunk_by_index_get(egl_chunks_t *chunks, unsigned int index)
{
    assert(chunks  != NULL);

    return index < chunks->number ? chunks->chunk[index] : NULL;
}