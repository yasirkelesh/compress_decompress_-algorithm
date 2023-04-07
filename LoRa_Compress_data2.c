#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int32_t to_signed(uint32_t x, int bit_count) {
    int32_t mask = 1 << (bit_count - 1);
    return (x & mask) ? (x - (mask << 1)) : x;
}

uint32_t to_unsigned(int32_t x, int bit_count) {
    int32_t mask = 1 << (bit_count - 1);
    return (x < 0) ? (x + (mask << 1)) : x;
}

void compress_data(uint8_t id, int16_t barometer, int16_t temperature, int32_t latitude, int32_t longitude, uint8_t compressed_data[7]) {
    uint16_t ubarometer = to_unsigned(barometer, 10);
    uint16_t utemperature = to_unsigned(temperature, 9);
    uint32_t ulatitude = to_unsigned(latitude, 17);
    uint32_t ulongitude = to_unsigned(longitude, 17);

    compressed_data[0] = (id << 5) | ((ubarometer >> 5) & 0x1F);
    compressed_data[1] = (ubarometer << 3) | ((utemperature >> 6) & 0x07);
    compressed_data[2] = (utemperature << 2) | ((ulatitude >> 15) & 0x03);
    compressed_data[3] = (ulatitude >> 7) & 0xFF;
    compressed_data[4] = (ulatitude << 1) | ((ulongitude >> 16) & 0x01);
    compressed_data[5] = (ulongitude >> 8) & 0xFF;
    compressed_data[6] = ulongitude & 0xFF;
}

void decompress_data(uint8_t compressed_data[7], uint8_t *id, int16_t *barometer, int16_t *temperature, int32_t *latitude, int32_t *longitude) {
    *id = compressed_data[0] >> 5;
    uint16_t ubarometer = ((compressed_data[0] & 0x1F) << 5) | (compressed_data[1] >> 3);
    uint16_t utemperature = ((compressed_data[1] & 0x07) << 6) | (compressed_data[2] >> 2);
    uint32_t ulatitude = ((compressed_data[2] & 0x03) << 15) | (compressed_data[3] << 7) | (compressed_data[4] >> 1);
    uint32_t ulongitude = ((compressed_data[4] & 0x01) << 16) | (compressed_data[5] << 8) | compressed_data[6];

    *barometer = to_signed(ubarometer, 10);
    *temperature = to_signed(utemperature, 9);
    *latitude = to_signed(ulatitude, 17);
    *longitude = to_signed(ulongitude, 17);
}

int main() {
    uint8_t id = 7;
    int16_t barometer = 511;
    int16_t temperature = 254;
    int32_t latitude = 65534;
    int32_t longitude = -65535;

    uint8_t compressed_data[7];

    compress_data(id, barometer, temperature, latitude, longitude, compressed_data);

uint8_t decompressed_id;
int16_t decompressed_barometer;
int16_t decompressed_temperature;
int32_t decompressed_latitude;
int32_t decompressed_longitude;
decompress_data(compressed_data, &decompressed_id, &decompressed_barometer, &decompressed_temperature, &decompressed_latitude, &decompressed_longitude);

printf("Decompressed Data:\n");
printf("ID: %u\n", decompressed_id);
printf("Barometer: %d\n", decompressed_barometer);
printf("Temperature: %d\n", decompressed_temperature);
printf("Latitude: %d\n", decompressed_latitude);
printf("Longitude: %d\n", decompressed_longitude);

return 0;
}


