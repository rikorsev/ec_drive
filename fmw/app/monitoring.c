#define EGL_MODULE_NAME "MONITOR"

#include <stdbool.h>
#include "egl_lib.h"
#include "ecd_bsp.h"
#include "cmd_handler.h"

#define PERIOD 10 // 10 milliseconds
#define OUT_LEN (sizeof(monitor_entry_t) + 6) // +6 bytes of meta data (2 for command ID, 2 for length and 2 for CRC)

#pragma pack(push, 1)
typedef struct
{
    uint32_t timestamp;
    uint16_t speed;
    uint16_t load;
}monitor_entry_t;
#pragma pack(pop)

static bool is_monitoring_active = false;

void monitoring_start(void)
{
    is_monitoring_active = true;
}

void monitoring_stop(void)
{
    is_monitoring_active = false;
}

static egl_result_t encode(monitor_entry_t *data, uint8_t *out)
{
    egl_result_t result = EGL_SUCCESS;
    size_t out_len = OUT_LEN;

    /* setup protocol to encode data with command ID 0xC006 */
    result = egl_ptc_setup(spi_llp(), CMD_MODITORING_DATA_ID, sizeof(CMD_MODITORING_DATA_ID));
    if(result != EGL_SUCCESS)
    {
        EGL_TRACE_ERROR("Protocol setup - fail. Result %s", EGL_RESULT());
        return result;
    }

    /* Encode data */
    result = egl_ptc_encode(spi_llp(), data, sizeof(monitor_entry_t), out, &out_len);
    if(result != EGL_SUCCESS)
    {
        EGL_TRACE_ERROR("Protocol encode - fail. Result %s", EGL_RESULT());
        return result;
    }

    if(out_len != OUT_LEN)
    {
        EGL_TRACE_ERROR("Out length %d, expected %d", out_len, OUT_LEN);
        return EGL_FAIL;
    }

    return EGL_SUCCESS;
}

static egl_result_t send(uint8_t *out)
{
    size_t write_len      = OUT_LEN;
    egl_result_t result   = EGL_SUCCESS;

    /* send monitoring data troug spi */
    result = egl_itf_write(spi(), out, &write_len);
    if(result != EGL_SUCCESS)
    {
        EGL_TRACE_ERROR("Data send - fail. Result %s", EGL_RESULT());
        return result;
    }

    if(write_len != OUT_LEN)
    {
        EGL_TRACE_ERROR("Write len %d, expected %d", write_len, OUT_LEN);
        return EGL_FAIL;
    }

    return result;
}

egl_result_t monitoring_update(void)
{
    static uint32_t target_timestamp = 0;
    uint32_t current_timestamp       = egl_counter_get(ms);
    egl_result_t result              = EGL_SUCCESS;

    if(target_timestamp < current_timestamp && is_monitoring_active == true)
    {
        uint8_t out[OUT_LEN]  = {0};
        monitor_entry_t data = 
        {
            .timestamp = current_timestamp,
            .load      = egl_bldc_get_load(motor()),
            .speed     = egl_bldc_get_speed(motor())
        };

        EGL_TRACE_INFO("Monitoring");

        /* update target timestamp */
        target_timestamp = current_timestamp + PERIOD;

        result = encode(&data, out);
        if(result == EGL_SUCCESS)
        {
            result = send(out);
        }
    }

    return result;
}