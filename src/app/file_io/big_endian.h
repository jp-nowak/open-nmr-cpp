#ifndef BIG_ENDIAN_H
#define BIG_ENDIAN_H

#include <vector>
#include <cstddef>
#include <complex>

namespace BigEndian
{
    short to_short(std::vector<std::byte> buffer, size_t begin);
    int to_int(std::vector<std::byte> buffer, size_t begin);
    float to_float(std::vector<std::byte> buffer, size_t begin);
    std::vector<std::complex<double>> read_fid_array_float(const std::vector<std::byte>& buffer);
    std::vector<std::complex<double>> read_fid_array_int(const std::vector<std::byte>& buffer);
    std::vector<std::complex<double>> read_fid_array_short(const std::vector<std::byte>& buffer);
}


#endif // BIG_ENDIAN_H
