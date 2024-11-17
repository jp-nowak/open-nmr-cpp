#include "big_endian.h"

#include<cstring>
#include<bit>

short BigEndian::to_short(std::vector<std::byte> buffer, size_t begin)
{
    short i{};
    std::memcpy(&i, &buffer[begin], sizeof(short));
    return std::byteswap(i);
}

int BigEndian::to_int(std::vector<std::byte> buffer, size_t begin)
{
    int i{};
    std::memcpy(&i, &buffer[begin], sizeof(int));
    return std::byteswap(i);
}

float BigEndian::to_float(std::vector<std::byte> buffer, size_t begin)
{
    int i{};
    std::memcpy(&i, &buffer[begin], sizeof(int));
    i = std::byteswap(i);
    return std::bit_cast<float>(i);
}

std::vector<std::complex<double>> BigEndian::read_fid_array_float(const std::vector<std::byte>& buffer)
{
    std::vector<std::complex<double>> fid{};
    size_t number_of_complex_elements = buffer.size()/8;
    fid.resize(number_of_complex_elements);

    for (int b_i = 0, f_i = 0; ((b_i + 7) <= buffer.size() - 1) & (f_i < number_of_complex_elements); b_i+=8, f_i++) {
        float real = 0;
        std::byte real_bytes[] {buffer[b_i+7], buffer[b_i+6], buffer[b_i+5], buffer[b_i+4]};
        std::memcpy(&real, &real_bytes, 4);
        float imaginary = 0;
        std::byte imaginary_bytes[] {buffer[b_i+3], buffer[b_i+2], buffer[b_i+1], buffer[b_i+0]};
        std::memcpy(&imaginary, &imaginary_bytes, 4);

        fid[f_i] = std::complex<double>{real, imaginary};
    }
    return fid;
}

std::vector<std::complex<double>> BigEndian::read_fid_array_int(const std::vector<std::byte>& buffer)
{
    std::vector<std::complex<double>> fid{};
    size_t number_of_complex_elements = buffer.size()/8;
    fid.resize(number_of_complex_elements);

    for (int b_i = 0, f_i = 0; ((b_i + 7) <= buffer.size() - 1) & (f_i < number_of_complex_elements); b_i+=8, f_i++) {
        int real = 0;
        std::byte real_bytes[] {buffer[b_i+7], buffer[b_i+6], buffer[b_i+5], buffer[b_i+4]};
        std::memcpy(&real, &real_bytes, 4);
        int imaginary = 0;
        std::byte imaginary_bytes[] {buffer[b_i+3], buffer[b_i+2], buffer[b_i+1], buffer[b_i+0]};
        std::memcpy(&imaginary, &imaginary_bytes, 4);

        fid[f_i] = std::complex<double>(real, imaginary);
    }
    return fid;
}

std::vector<std::complex<double>> BigEndian::read_fid_array_short(const std::vector<std::byte>& buffer)
{
    std::vector<std::complex<double>> fid{};
    size_t number_of_complex_elements = buffer.size()/4;
    fid.resize(number_of_complex_elements);

    for (int b_i = 0, f_i = 0; ((b_i + 3) <= buffer.size() - 1) & (f_i < number_of_complex_elements); b_i+=4, f_i++) {
        short real = 0;
        std::byte real_bytes[] {buffer[b_i+3], buffer[b_i+2]};
        std::memcpy(&real, &real_bytes, 2);
        short imaginary = 0;
        std::byte imaginary_bytes[] {buffer[b_i+1], buffer[b_i+0]};
        std::memcpy(&imaginary, &imaginary_bytes, 2);

        fid[f_i] = std::complex<double>(real, imaginary);
    }
    return fid;
}
