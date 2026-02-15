#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include "src/data.pb.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

void process_data(void) {
    uint8_t buffer[64];

    SensorData source_data = SensorData_init_zero;
    source_data.uptime = k_uptime_get_32();
    source_data.temperature = 24.5f;
    source_data.active = true;

    LOG_INF("Original: Uptime=%d, Temp=%.2f, Active=%d",
            source_data.uptime, (double)source_data.temperature, source_data.active);

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, SensorData_fields, &source_data);
    const size_t message_length = stream.bytes_written;

    if (!status) {
        LOG_ERR("Encoding failed: %s", PB_GET_ERROR(&stream));
        return;
    }

    LOG_HEXDUMP_INF(buffer, message_length, "Serialized Data");

    SensorData dest_data = SensorData_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, message_length);
    status = pb_decode(&istream, SensorData_fields, &dest_data);

    if (!status) {
        LOG_ERR("Decoding failed: %s", PB_GET_ERROR(&istream));
        return;
    }

    LOG_INF("Decoded:  Uptime=%d, Temp=%.2f, Active=%d",
            dest_data.uptime, (double)dest_data.temperature, dest_data.active);
}

int main(void) {
    k_msleep(100);

    while (1) {
        process_data();
        k_msleep(5000);
    }
    return 0;
}
