#include <string.h>

#include "egl_ringbuf.h"
#include "unity.h"

void setUp(void)
{
    /* Do nothing */
}

void tearDown(void)
{
    /* Do nothing */
}

void test_empty_buffer_id001(void)
{
    EGL_DECLARE_RINGBUF(ringbuf, 128);

    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_size(&ringbuf),      "Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_free_size(&ringbuf), "Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,   egl_ringbuf_get_full_size(&ringbuf), "Wrong RingBufer FULL size");
}

void test_write_read_byte_id002(void)
{
    uint8_t data_in  = 0x55;
    uint8_t data_out = 0;
    size_t writen    = 0;
    size_t read      = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 128);

    writen = egl_ringbuf_write(&ringbuf, &data_in, sizeof(data_in));

    TEST_ASSERT_EQUAL_INT_MESSAGE(1,   writen,                              "WRITE: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_size(&ringbuf),      "WRITE: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(127, egl_ringbuf_get_free_size(&ringbuf), "WRITE: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,   egl_ringbuf_get_full_size(&ringbuf), "WRITE: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, &data_out, sizeof(data_out));

    TEST_ASSERT_EQUAL_INT_MESSAGE(1,    read,                                "READ: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0x55, data_out,                            "READ: Wrong data");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128,  egl_ringbuf_get_size(&ringbuf),      "READ: Wrong RingBufer size not");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128,  egl_ringbuf_get_free_size(&ringbuf), "READ: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,    egl_ringbuf_get_full_size(&ringbuf), "READ: Wrong RingBufer FULL size");
}

void test_write_read_eight_bytes_id003(void)
{
    uint64_t data_in  = 0x12345678;
    uint64_t data_out = 0;
    size_t writen     = 0;
    size_t read       = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 128);

    writen = egl_ringbuf_write(&ringbuf, &data_in, sizeof(data_in));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8,   writen,                              "WRITE: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_size(&ringbuf),      "WRITE: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(120, egl_ringbuf_get_free_size(&ringbuf), "WRITE: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8,   egl_ringbuf_get_full_size(&ringbuf), "WRITE: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, &data_out, sizeof(data_out));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8,          read,                                "READ: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0x12345678, data_out,                            "READ: Wrong data");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128,        egl_ringbuf_get_size(&ringbuf),      "READ: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128,        egl_ringbuf_get_free_size(&ringbuf), "READ: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,          egl_ringbuf_get_full_size(&ringbuf), "READ: Wrong RingBufer FULL size");
}

void test_write_read_full_buffer_id004(void)
{
    uint8_t data_in[128]  = {0};
    uint8_t data_out[128] = {0};
    size_t writen         = 0;
    size_t read           = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 128);

    /* Set up in buffer */
    for(int i = 0; i < 128; i++)
    {
        data_in[i] = i;
    }

    writen = egl_ringbuf_write(&ringbuf, data_in, sizeof(data_in));

    TEST_ASSERT_EQUAL_INT_MESSAGE(128, writen,                              "WRITE: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_size(&ringbuf),      "WRITE: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,   egl_ringbuf_get_free_size(&ringbuf), "WRITE: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_full_size(&ringbuf), "WRITE: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, data_out, sizeof(data_out));

    TEST_ASSERT_EQUAL_INT_MESSAGE(128, read,                                "READ: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_size(&ringbuf),      "READ: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(128, egl_ringbuf_get_free_size(&ringbuf), "READ: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,   egl_ringbuf_get_full_size(&ringbuf), "READ: Wrong RingBufer FULL size");

    /* Check output */
    for(int i = 0; i < 128; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, data_out[i], "READ: Wrong data out");
    }
}

void test_write_read_more_then_buf_size_id005(void)
{
    uint8_t data_in[9]  = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    uint8_t data_out[9] = {0};
    size_t writen       = 0;
    size_t read         = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 8);

    writen = egl_ringbuf_write(&ringbuf, data_in, sizeof(data_in));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8, writen,                              "WRITE: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_size(&ringbuf),      "WRITE: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, egl_ringbuf_get_free_size(&ringbuf), "WRITE: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_full_size(&ringbuf), "WRITE: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, data_out, sizeof(data_out));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8, read,                                "READ: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_size(&ringbuf),      "READ: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_free_size(&ringbuf), "READ: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, egl_ringbuf_get_full_size(&ringbuf), "READ: Wrong RingBufer FULL size");

    /* Check output */
    for(int i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, data_out[i], "READ: Wrong data out");
    }

    /* Check last byte. It shouldn't be modified */
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, data_out[8], "READ: Wrong data out");
}

void test_write_two_messages_read_one_id006(void)
{
    uint8_t data_in_1[]  = {0, 1, 2, 3};
    uint8_t data_in_2[]  = {4, 5, 6, 7, 8, 9, 10, 11};
    uint8_t data_out[12] = {0};
    size_t writen        = 0;
    size_t read          = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 32);

    writen = egl_ringbuf_write(&ringbuf, data_in_1, sizeof(data_in_1));

    TEST_ASSERT_EQUAL_INT_MESSAGE(4, writen,                               "WRITE1: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_size(&ringbuf),      "WRITE1: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(28, egl_ringbuf_get_free_size(&ringbuf), "WRITE1: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, egl_ringbuf_get_full_size(&ringbuf),  "WRITE1: Wrong RingBufer FULL size");

    writen = egl_ringbuf_write(&ringbuf, data_in_2, sizeof(data_in_2));
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, writen,                               "WRITE2: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_size(&ringbuf),      "WRITE2: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(20, egl_ringbuf_get_free_size(&ringbuf), "WRITE2: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, egl_ringbuf_get_full_size(&ringbuf), "WRITE2: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, data_out, sizeof(data_out));
    
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, read,                                "READ: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_size(&ringbuf),      "READ: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_free_size(&ringbuf), "READ: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, egl_ringbuf_get_full_size(&ringbuf),  "READ: Wrong RingBufer FULL size");

    /* Check output */
    for(int i = 0; i < 12; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, data_out[i], "READ: Wrong data out");
    }
}

void test_write_one_messages_read_two_id007(void)
{
    uint8_t data_in[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    uint8_t data_out_1[4] = {0};
    uint8_t data_out_2[8] = {0};
    size_t writen         = 0;
    size_t read           = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 32);

    writen = egl_ringbuf_write(&ringbuf, data_in, sizeof(data_in));

    TEST_ASSERT_EQUAL_INT_MESSAGE(12, writen,                              "WRITE: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_size(&ringbuf),      "WRITE: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(20, egl_ringbuf_get_free_size(&ringbuf), "WRITE: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(12, egl_ringbuf_get_full_size(&ringbuf), "WRITE: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, data_out_1, sizeof(data_out_1));

    TEST_ASSERT_EQUAL_INT_MESSAGE(4,  read,                                "READ1: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_size(&ringbuf),      "READ1: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(24, egl_ringbuf_get_free_size(&ringbuf), "READ1: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8,  egl_ringbuf_get_full_size(&ringbuf), "READ1: Wrong RingBufer FULL size");

    for(int i = 0; i < 4; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(i, data_out_1[i], "READ1: Wrong data out");
    }

    read = egl_ringbuf_read(&ringbuf, data_out_2, sizeof(data_out_2));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8,  read,                                "READ2: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_size(&ringbuf),      "READ2: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(32, egl_ringbuf_get_free_size(&ringbuf), "READ2: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,  egl_ringbuf_get_full_size(&ringbuf), "READ2: Wrong RingBufer FULL size");

    for(int i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(i + 4, data_out_2[i], "READ2: Wrong data out");
    }
}

void test_read_write_two_chuncks_id008(void)
{
    uint32_t data_in_1  = 0x12345678;
    uint8_t data_in_2[] = {8, 7, 6, 5, 4, 3, 2, 1};
    uint8_t data_out[8] = {0};
    size_t writen       = 0;
    size_t read         = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 8);

    writen = egl_ringbuf_write(&ringbuf, &data_in_1, sizeof(data_in_1));

    TEST_ASSERT_EQUAL_INT_MESSAGE(4, writen,                              "WRITE1: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_size(&ringbuf),      "WRITE1: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, egl_ringbuf_get_free_size(&ringbuf), "WRITE1: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(4, egl_ringbuf_get_full_size(&ringbuf), "WRITE1: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, data_out, 4);

    TEST_ASSERT_EQUAL_INT_MESSAGE(4, read,                                "READ1: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_size(&ringbuf),      "READ1: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_free_size(&ringbuf), "READ1: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, egl_ringbuf_get_full_size(&ringbuf), "READ1: Wrong RingBufer FULL size");

    writen = egl_ringbuf_write(&ringbuf, data_in_2, sizeof(data_in_2));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8, writen,                              "WRITE2: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_size(&ringbuf),      "WRITE2: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, egl_ringbuf_get_free_size(&ringbuf), "WRITE2: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_full_size(&ringbuf), "WRITE2: Wrong RingBufer FULL size");

    read = egl_ringbuf_read(&ringbuf, data_out, sizeof(data_in_2));

    TEST_ASSERT_EQUAL_INT_MESSAGE(8, read,                                "READ1: Wrong length");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_size(&ringbuf),      "READ1: Wrong RingBufer size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(8, egl_ringbuf_get_free_size(&ringbuf), "READ1: Wrong RingBufer FREE size");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, egl_ringbuf_get_full_size(&ringbuf), "READ1: Wrong RingBufer FULL size");

    /* Check output */
    for(int i = 0; i < 8; i++)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(8 - i, data_out[i], "READ: Wrong data out");
    }
}

void test_read_write_loop_id009(void)
{
    uint8_t data_in[]   = {0, 1, 2};
    uint8_t data_out[3] = {0};
    size_t writen       = 0;
    size_t read         = 0;

    EGL_DECLARE_RINGBUF(ringbuf, 8);

    for(int i = 0; i < 100; i++)
    {
        memset(data_out, 0, sizeof(data_out));

        writen = egl_ringbuf_write(&ringbuf, data_in, sizeof(data_in));
        
        TEST_ASSERT_EQUAL_INT_MESSAGE(3, writen, "WRITE: Wrong length");

        read = egl_ringbuf_read(&ringbuf, data_out, sizeof(data_out));

        TEST_ASSERT_EQUAL_INT_MESSAGE(3, read, "READ: Wrong length");

        /* Check output */
        for(int i = 0; i < 3; i++)
        {   
            TEST_ASSERT_EQUAL_INT_MESSAGE(i, data_out[i], "READ: Wrong data out");
        }
    }
}

int main(void)
{
    UnityBegin("EGL:RING_BUFFER");

    RUN_TEST(test_empty_buffer_id001);
    RUN_TEST(test_write_read_byte_id002);
    RUN_TEST(test_write_read_eight_bytes_id003);
    RUN_TEST(test_write_read_full_buffer_id004);
    RUN_TEST(test_write_read_more_then_buf_size_id005);
    RUN_TEST(test_write_two_messages_read_one_id006);
    RUN_TEST(test_write_one_messages_read_two_id007);
    RUN_TEST(test_read_write_two_chuncks_id008);
    RUN_TEST(test_read_write_loop_id009);

    return (UnityEnd());
}