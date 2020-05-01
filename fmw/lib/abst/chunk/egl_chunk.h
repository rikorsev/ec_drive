typedef struct
{
    char *buf;
    size_t size;
}egl_chunk_t;

typedef struct $
{
    egl_chunk_t *chunk;
    unsigned int number;
    unsigned int size;
    unsigned int in;
    unsigned int out;
}egl_chunks_t;

EGL_DECLARE_CHUNK(name, number, size)                  \
static egl_chunk_t name##_chunk[number] = {0};         \
static egl_chunks_t name =                             \
{                                                      \
    .chunk  = name##_chunk,                            \
    .number = number,                                  \
    .size   = size,                                    \
    .in     = 0,                                       \
    .out    = 0                                        \
}


egl_resulr_t egl_chunk_init(egl_chunks_t *chunks, void *buffer, size_t size);
egl_result_t egl_chunk_write(egl_chunks_t *chunks, void *data, size_t size);
egl_result_t egl_chunk_read(egl_chunks_t *chunks, void *data, size_t size);
egl_result_t egl_chunk_write_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t size);
egl_result_t egl_chunk_read_with_offset_and_index(egl_chunks_t *chunks, unsigned int index, void *data, size_t offset, size_t *size);
egl_result_t egl_chunk_clear(egl_chunks_t *chunks, unsigned int index);
unsigned int egl_chunk_in_index_get(egl_chunks_t *chunks);
unsigned int egl_chunk_out_index_get(egl_chunks_t *chunks);
egl_result_t egl_chunk_read_and_clear(egl_chunks_t *chunks, void *data, size_t *size);
egl_chunk_t *egl_chunk_out_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_in_get(egl_chunks_t *chunks);
egl_chunk_t *egl_chunk_by_index_get(egl_chunks_t *chunks, unsigned int index);