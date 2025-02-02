#ifndef BIG_ENDIAN_H
#define BIG_ENDIAN_H

#include <vector>
#include <cstddef>
#include <complex>

namespace BigEndian
{
    /*!
     * \brief to_short takes vector of big endian bytes and returns short corresponfing to them
     * \param buffer: contains input bytes
     * \param begin: position in buffer at which reading starts
     * \return short: value corresponding to bytes
     */
    short to_short(std::vector<std::byte> buffer, size_t begin);

    /*!
     * \brief to_int takes vector of big endian bytes and returns int corresponfing to them
     * \param buffer: contains input bytes
     * \param begin: position in buffer at which reading starts
     * \return int: value corresponding to bytes
     */
    int to_int(std::vector<std::byte> buffer, size_t begin);

    /*!
     * \brief to_float takes vector of big endian bytes and returns float corresponfing to them
     * it is not checked whether resulting float is valid
     * \param buffer: contains input bytes
     * \param begin: position in buffer at which reading starts
     * \return float: value corresponding to bytes
     */
    float to_float(std::vector<std::byte> buffer, size_t begin);

    std::vector<std::complex<double>> read_fid_array_double(const std::vector<std::byte>& buffer);

    /*!
     * \brief read_fid_array_float reads array of complex numbers from big endian bytes, they are promoted to double
     * it is assumed that array structure is:
     * float real 1 | float imaginary 1 | float real 2 | float imaginary 2 | ... | float real n | floar imaginary n
     * \param buffer: contains bytes from which fid is read, number of values to read is determined by its size
     * \return obtained values as vector of complex <double>
     */
    std::vector<std::complex<double>> read_fid_array_float(const std::vector<std::byte>& buffer);
\
    /*!
     * \brief read_fid_array_int reads array of complex numbers from big endian bytes, they are promoted to double
     * it is assumed that array structure is:
     * int real 1 | int imaginary 1 | int real 2 | int imaginary 2 | ... | int real n | int imaginary n
     * \param buffer: contains bytes from which fid is read, number of values to read is determined by its size
     * \return obtained values as vector of complex <double>
     */
    std::vector<std::complex<double>> read_fid_array_int(const std::vector<std::byte>& buffer);

    /*!
     * \brief read_fid_array_short reads array of complex numbers from big endian bytes, they are promoted to double
     * it is assumed that array structure is:
     * short real 1 | short imaginary 1 | short real 2 | short imaginary 2 | ... | short real n | short imaginary n
     * \param buffer: contains bytes from which fid is read, number of values to read is determined by its size
     * \return obtained values as vector of complex <double>
     */
    std::vector<std::complex<double>> read_fid_array_short(const std::vector<std::byte>& buffer);
}


#endif // BIG_ENDIAN_H
