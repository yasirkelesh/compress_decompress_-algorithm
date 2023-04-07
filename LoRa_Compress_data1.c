#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

int16_t to_signed(uint16_t x, int bit_count) {
    int16_t mask = 1 << (bit_count - 1);
    return (x & mask) ? (x - (mask << 1)) : x;
}

uint16_t to_unsigned(int16_t x, int bit_count) {
    int16_t mask = 1 << (bit_count - 1);
    return (x < 0) ? (x + (mask << 1)) : x;
}

void compress_data(int16_t id, int16_t pitch, int16_t roll, int16_t heading, uint8_t compressed_data[5]) {
    uint16_t uid = to_unsigned(id, 13);
    uint16_t upitch = to_unsigned(pitch, 9);
    uint16_t uroll = to_unsigned(roll, 9);
    uint16_t uheading = to_unsigned(heading, 9);

    compressed_data[0] = (uid >> 5) & 0xFF;
    compressed_data[1] = (uid << 3) | ((upitch >> 6) & 0x07);
    compressed_data[2] = (upitch << 2) | ((uroll >> 7) & 0x03);
    compressed_data[3] = (uroll << 1) | ((uheading >> 8) & 0x01);
    compressed_data[4] = uheading & 0xFF;
}

void decompress_data(uint8_t compressed_data[5], int16_t *id, int16_t *pitch, int16_t *roll, int16_t *heading) {
    uint16_t uid = (compressed_data[0] << 5) | (compressed_data[1] >> 3);
    uint16_t upitch = ((compressed_data[1] & 0x07) << 6) | (compressed_data[2] >> 2);
    uint16_t uroll = ((compressed_data[2] & 0x03) << 7) | (compressed_data[3] >> 1);
    uint16_t uheading = ((compressed_data[3] & 0x01) << 8) | compressed_data[4];

    *id = to_signed(uid, 13);
    *pitch = to_signed(upitch, 9);
    *roll = to_signed(uroll, 9);
    *heading = to_signed(uheading, 9);
}

int main() {
    int16_t id = -10;
    int16_t pitch = -256;
    int16_t roll = 255;
    int16_t heading = -256;

    uint8_t compressed_data[5];

    compress_data(id, pitch, roll, heading, compressed_data);

    int16_t decompressed_id;
    int16_t decompressed_pitch;
    int16_t decompressed_roll;
    int16_t decompressed_heading;

    decompress_data(compressed_data, &decompressed_id, &decompressed_pitch, &decompressed_roll, &decompressed_heading);

    printf("Decompressed Data:\n");
    printf("ID: %d\n", decompressed_id);
    printf("Pitch: %d\n", decompressed_pitch);
    printf("Roll: %d\n", decompressed_roll);
    printf("Heading: %d\n", decompressed_heading);

    return 0;
}