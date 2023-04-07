
#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    int pitch;
    int roll;
    int heading;
    int temperature;
    unsigned int pressure;
    int latitude;
    int longitude;
    unsigned int altitude;
    unsigned int speed;
    unsigned int satellite_count;
    unsigned int id;
} SensorData;

void print_data(const SensorData *data) {
    printf("Pitch: %d\n", data->pitch);
    printf("Roll: %d\n", data->roll);
    printf("Heading: %d\n", data->heading);
    printf("Temperature: %d\n", data->temperature);
    printf("Pressure: %u\n", data->pressure);
    printf("Latitude: %d\n", data->latitude);
    printf("Longitude: %d\n", data->longitude);
    printf("Altitude: %u\n", data->altitude);
    printf("Speed: %u\n", data->speed);
    printf("Satellite_count: %u\n", data->satellite_count);
    printf("ID: %u\n", data->id);
}
void compress_data(const SensorData *data, uint8_t *compressed_data) {
    uint32_t temp_data;

    temp_data = (uint32_t)(data->pitch + 180) & 0x1FF;
    temp_data |= ((uint32_t)(data->roll + 90) & 0x1FF) << 9;
    temp_data |= ((uint32_t)(data->heading + 180) & 0x1FF) << 18;
    memcpy(compressed_data, &temp_data, 4);

    temp_data = (uint32_t)(data->temperature + 40) & 0x7F;
    temp_data |= ((uint32_t)data->pressure & 0x3FFF) << 7; // 14 bits for pressure
    memcpy(compressed_data + 4, &temp_data, 3);

    temp_data = (uint32_t)(data->latitude - 360000) & 0xFFFF;
    temp_data |= ((uint32_t)(data->longitude - 260000) & 0xFFFF) << 16;
    memcpy(compressed_data + 7, &temp_data, 4);

    temp_data = (uint32_t)data->altitude & 0x3FF;
    temp_data |= ((uint32_t)data->speed & 0x3F) << 10;
    temp_data |= ((uint32_t)data->satellite_count & 0x1F) << 16;
    temp_data |= ((uint32_t)data->id & 0x3FF) << 21;
    memcpy(compressed_data + 11, &temp_data, 4);
}

void decompress_data(const uint8_t *compressed_data, SensorData *data) {
    uint32_t temp_data;

    memcpy(&temp_data, compressed_data, 4);
    data->pitch = (temp_data & 0x1FF) - 180;
    data->roll = ((temp_data >> 9) & 0x1FF) - 90;
    data->heading = ((temp_data >> 18) & 0x1FF) - 180;

    memcpy(&temp_data, compressed_data + 4, 3);
    data->temperature = (temp_data & 0x7F) - 40;
    data->pressure = (temp_data >> 7) & 0x3FFF; // 14 bits for pressure

    memcpy(&temp_data, compressed_data + 7, 4);
    data->latitude = (temp_data & 0xFFFF) + 360000;
    data->longitude = ((temp_data >> 16) & 0xFFFF) + 260000;

    memcpy(&temp_data, compressed_data + 11, 4);
    data->altitude = temp_data & 0x3FF;
    data->speed = (temp_data >> 10) & 0x3F;
    data->satellite_count = (temp_data >> 16) & 0x1F;
    data->id = (temp_data >> 21) & 0x3FF;
}

int main() {
    SensorData original_data = {
        .pitch = -45,
        .roll = 30,
        .heading = 150,
        .temperature = 25,
        .pressure = 8000,
        .latitude = 394576,
        .longitude = 284590,
        .altitude = 500,
        .speed = 25,
        .satellite_count = 10,
        .id = 1023
    };

    uint8_t compressed_data[15];
    SensorData decompressed_data;

    compress_data(&original_data, compressed_data);
    decompress_data(compressed_data, &decompressed_data);

    printf("Original data:\n");
    print_data(&original_data);
    printf("\n");

    printf("Decompressed data:\n");
    print_data(&decompressed_data);
    printf("\n");

    return 0;
}





/* pitch:      -180 ile +180 arası
 roll:    -90 ile +90 arası
 heading:  -180 ile +180 arası
 temperature: -40 ile +85 arası
 pressure:    0 ile +900 arası
latitude:    360000 ile 420000arası
longitude:   260000 ile 450000 arası
 altitude:     0 ile +1024 arası
speed:      0 ile 55 arası
satellite_count: 0 ile +26 arası
id:            kalan bit */



