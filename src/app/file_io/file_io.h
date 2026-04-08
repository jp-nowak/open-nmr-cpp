#ifndef FILE_IO_H
#define FILE_IO_H

#include "../typedefs.h"

#include <algorithm>
#include <filesystem>
#include <cstddef>
#include <string_view>
#include <unordered_map>
#include <type_traits>


namespace IO
{

struct SHash;
using Dict = std::unordered_map<String, String, SHash, std::equal_to<>>;

enum class DataType{int16, int32, int64, float32, float64};
enum class Endian{little, big};

//! returns size of DataType
size_t dataTypeSize(DataType);

template <typename Container> //! reads contents of file to buffer, returns bool indicating success
[[nodiscard]] bool readFileTo(Container& buffer, const std::filesystem::path& path);

/*!
 * \brief returns value of type ValueT created from big endian bytes
 * \param buffer contiguous container or array
 * \param begin position of first byte to be used in creating return value
 * \return
 */
template <typename Value>
Value beTo(const Buffer& buffer, size_t begin)
{
    char temp[sizeof(Value)];
    memcpy(&temp, &buffer[begin], sizeof(Value));
    std::ranges::reverse(temp);
    Value value;
    memcpy(&value, &temp, sizeof(Value));
    if constexpr (std::is_same_v<Value, Complex>) {
        return Value{value.imag, value.real};
    }
    if constexpr (std::is_aggregate_v<Value>) {
        std::ranges::reverse(value);
    }
    return value;
}



/*!
 * \brief returns value of type ValueT created from little endian bytes
 * \param buffer contiguous container or array
 * \param begin position of first byte to be used in creating return value
 * \return
 */
template <typename Value>
Value leTo(const Buffer& buffer, size_t begin)
{
    Value value;
    memcpy(&value, &buffer[begin], sizeof(Value));
    return value;
}

template <typename Value>
Value bytesTo(const Buffer& buffer, size_t begin, bool bigEndian)
{
    if (bigEndian) return beTo<Value>(buffer, begin);
    else return leTo<Value>(buffer, begin);
}

//! reads binary array of complex values
ComplexVector readComplexArray(Endian endian, DataType type, const Buffer& buffer, size_t begin, size_t cElemN);

template <typename ValueT>
ComplexVector readBeComplexArray(const Buffer& buffer, size_t begin, size_t cElemN);

template <typename ValueT>
ComplexVector readLeComplexArray(const Buffer& buffer, size_t begin, size_t cElemN);

//! reads binary array of simple values
Vector<NumericValueType> readSimpleArray(Endian endian, DataType type, const Buffer& buffer, size_t begin, size_t elemN);

template <typename ValueT>
Vector<NumericValueType> readBeSimpleArray(const Buffer& buffer, size_t begin, size_t elemN);

template <typename ValueT>
Vector<NumericValueType> readLeSimpleArray(const Buffer& buffer, size_t begin, size_t elemN);

/*!
 * \brief removes in place all characters specified in chars from string s
 * \param s string from which chars are removed
 * \param chars to be removed, each separately not as a sequence
 * \return ref to string s
 */
String& removeCharsInPlace(String& s,  StringView chars);

/*!
 * \brief removeCharsCopy creates copy of s with removed chars
 * \param s string from which copy chars are removed
 * \param chars to be removed, each separately not as a sequence
 * \return new string with removed chars
 */
String removeCharsCopy(String s, StringView chars);

Vector<StringView> split(StringView s, StringView delimiter);

Vector<String> splitCopy(StringView s, StringView delimiter);

StringView getWord(StringView s, size_t n, StringView delimiter);

String join(Vector<StringView> s, StringView delimiter);

String join(Vector<String> s, StringView delimiter);

typedef StringView (*sv_callback) (StringView);

String join(Vector<StringView> s, StringView delimiter, sv_callback f);

String join(Vector<String> s, StringView delimiter, sv_callback f);

StringView rstrip(StringView s, StringView chars);

StringView lstrip(StringView s, StringView chars);

StringView strip(StringView s, StringView chars);

StringView strip(StringView s);

bool xisDigit(char c);

bool xisNumber(StringView s);

char ASCItoUpper(char c);

String ASCItoUpper(StringView sv);

char ASCItoLower(char c);

String ASCItoLower(StringView sv);

template <auto X>
bool xis(decltype(X) x)
{
    return x == X;
}

struct SHash
{
using is_transparent = void;
size_t operator()(char const* rhs) const;

size_t operator()(std::string_view rhs) const;

size_t operator()(std::string const& rhs) const;
};

template <typename CharT>
class SplitIterator
{
    public:
    SplitIterator(std::basic_string_view<CharT> string, std::basic_string_view<CharT> delimiter);

    class Iterator;
    Iterator begin();
    Iterator end();

    private:
    std::basic_string_view<CharT> string;
    std::basic_string_view<CharT> delimiter;

    public:
    class Iterator
    {
        public:

        using iterator_category = std::input_iterator_tag;
        using value_type = std::basic_string_view<CharT>;
        using element_type = value_type;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = std::ptrdiff_t;

        Iterator(SplitIterator<CharT>* splitIterator, size_t position, std::basic_string_view<CharT> delimiter);

        Iterator& operator++();
        Iterator operator++(int);

        bool operator==(const Iterator& b) const;
        std::basic_string_view<CharT> operator*() const;

        const SplitIterator<CharT>* splitIterator;
        size_t position;
        const std::basic_string_view<CharT> delimiter;
        size_t delimiterPos;
    };

};

}
#endif // FILE_IO_H


