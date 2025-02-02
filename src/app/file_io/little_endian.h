#ifndef LITTLE_ENDIAN_H
#define LITTLE_ENDIAN_H

#include <vector>
#include <complex>
#include <cstddef>

namespace LittleEndian
{
std::vector<std::complex<double>> read_fid_array_double(const std::vector<std::byte>& buffer);

std::vector<std::complex<double>> read_fid_array_int(const std::vector<std::byte>& buffer);
}




#endif // LITTLE_ENDIAN_H
