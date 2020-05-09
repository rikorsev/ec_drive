#include "egl_result.h"

typedef struct
{
    char *buf;
    size_t size;
}egl_chunk_t;

typedef struct
{
    egl_chunk_t *chunk;
    unsigned int number;
    unsigned int size;
    unsigned int in;
    unsigned int out;
}egl_chunks_t;

#define EGL_DECLARE_CHUNKS(NAME, NUMBER)                                  \
static egl_chunk_t NAME##_chunk[(NUMBER)] = {0};                          \
static egl_chunks_t NAME =                                                \
{                                                                         \
    .chunk  = NAME##_chunk,                                               \
    .number = (NUMBER),                                                   \
    .size   = 0,                                                          \
    .in     = 0,                                                          \
    .out    = 0                                                           \
}

egl_result_t egl_chunk_init(egl_chunks_t *chunks, char *buffer, size_t size);
void         egl_chunk_reset(egl_chunks_t *chunks);
egl_result_t egl_chunk_write(egl_chunks_t *chunks, void *data, size_t size);
egl_result_t egl_chunk_read(egl_chunks_t *chunks, void *data, size_t *size);
egl_result_t egl_chunk_write_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t size);
egl_result_t egl_chunk_read_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t *size);
egl_result_t egl_chunk_clear(egl_chunks_t *chunks, unsigned int index);
egl_result_t egl_chunk_read_and_clear(egl_chunks_t *chunks, void *data, size_t *size);


unsigned int egl_chunk_out_index_get(egl_chunks_t *chunks);
void         egl_chunk_out_index_inc(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_out_previous_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_out_current_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_out_next_get(egl_chunks_t *chunks);

unsigned int egl_chunk_in_index_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_in_previous_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_in_current_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_in_next_get(egl_chunks_t *chunks);
void         egl_chunk_in_index_inc(egl_chunks_t *chunks);

egl_chunk_t *egl_chunk_by_index_get(egl_chunks_t *chunks, unsigned int index);