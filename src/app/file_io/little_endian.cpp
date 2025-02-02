#include "little_endian.h"

#include<cstring>
#include<bit>

std::vector<std::complex<double>> LittleEndian::read_fid_array_double(const std::vector<std::byte>& buffer)
{
    std::vector<std::complex<double>> fid{};
    const size_t number_of_complex_elements = buffer.size()/8;
    fid.resize(number_of_complex_elements);

    for (size_t b_i = 0, f_i = 0; ((b_i + 15) <= buffer.size() - 1) & (f_i < number_of_complex_elements); b_i+=16, f_i++) {
        double real = 0;
        std::memcpy(&real, buffer.data() + b_i, 8);
        double imaginary = 0;
        std::memcpy(&imaginary, buffer.data() + b_i + 8, 8);

        fid[f_i] = std::complex<double>{real, imaginary};
    }
    return fid;
}

std::vector<std::complex<double>> LittleEndian::read_fid_array_int(const std::vector<std::byte>& buffer)
{
    std::vector<std::complex<double>> fid{};
    const size_t number_of_complex_elements = buffer.size()/8;
    fid.resize(number_of_complex_elements);

    for (size_t b_i = 0, f_i = 0; ((b_i + 7) <= buffer.size() - 1) & (f_i < number_of_complex_elements); b_i+=8, f_i++) {
        int real = 0;
        std::memcpy(&real, buffer.data() + b_i, 4);
        int imaginary = 0;
        std::memcpy(&imaginary, buffer.data() + b_i + 4, 4);

        fid[f_i] = std::complex<double>{static_cast<double>(real), static_cast<double>(imaginary)};
    }
    return fid;
}
