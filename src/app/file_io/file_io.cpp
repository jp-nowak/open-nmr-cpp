#include "file_io.h"


#include <fstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <algorithm>
#include <array>
#include <cassert>

namespace IO
{

size_t dataTypeSize(DataType t)
{
    using enum DataType;
    switch(t) {
    case int16: return 2;
    case int32: return 4;
    case int64: return 8;
    case float32: return 4;
    case float64: return 8;
    }
    assert(false);
}

template <typename Container>
bool readFileTo(Container& buffer, const std::filesystem::path& path)
{
    using ByteT = Container::value_type;
    std::error_code ec;
    const std::uintmax_t fileSize = std::filesystem::file_size(path, ec);
    if (not ec) {
        size_t size = fileSize / sizeof(ByteT);
        if (fileSize % sizeof(ByteT)) size++;
        buffer.resize(size + 1);
        std::ifstream file{path, std::ios::in | std::ios::binary};
        if (file) {
            buffer[size] = static_cast<ByteT>('\0');
            buffer[size - 1] = static_cast<ByteT>('\0');
            if (file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) return true;
        }
    }
    return false;
}
template bool readFileTo<String>(String& buffer, const std::filesystem::path& path);
template bool readFileTo<Buffer>(Buffer& buffer, const std::filesystem::path& path);

template <typename Value>
Value beTo(const Buffer& buffer, size_t begin)
{
    char temp[sizeof(Value)];
    std::memcpy(&temp, &buffer[begin], sizeof(Value));
    std::ranges::reverse(temp);
    Value value;
    std::memcpy(&value, &temp, sizeof(Value));
    return value;
}
#define declare(x) template x beTo<x>(const Buffer& buffer, size_t begin);
declare(short)
declare(int)
declare(long long)
declare(float)
declare(double)
declare(char16_t)
declare(char32_t)
#undef declare

template <typename Value>
Value leTo(const Buffer& buffer, size_t begin)
{
    Value value;
    std::memcpy(&value, &buffer[begin], sizeof(Value));
    return value;
}
#define declare(x) template x leTo<x>(const Buffer& buffer, size_t begin);
declare(short)
declare(int)
declare(long long)
declare(float)
declare(double)
declare(char16_t)
declare(char32_t)
#undef declare

ComplexVector readComplexArray(Endian endian, DataType type, const Buffer& buffer, size_t begin, size_t cElemN)
{
    switch (endian) {
    case Endian::little:
        switch (type) {
        case DataType::int16:
            return readLeComplexArray<short>(buffer, begin, cElemN);
        case DataType::int32:
            return readLeComplexArray<int>(buffer, begin, cElemN);
        case DataType::int64:
            return readLeComplexArray<long long>(buffer, begin, cElemN);
        case DataType::float32:
            return readLeComplexArray<float>(buffer, begin, cElemN);
        case DataType::float64:
            return readLeComplexArray<double>(buffer, begin, cElemN);
        } break;
    case Endian::big:
        switch (type) {
        case DataType::int16:
            return readBeComplexArray<short>(buffer, begin, cElemN);
        case DataType::int32:
            return readBeComplexArray<int>(buffer, begin, cElemN);
        case DataType::int64:
            return readBeComplexArray<long long>(buffer, begin, cElemN);
        case DataType::float32:
            return readBeComplexArray<float>(buffer, begin, cElemN);
        case DataType::float64:
            return readBeComplexArray<double>(buffer, begin, cElemN);
        }
    }
    assert(false);
}

template <typename Value>
ComplexVector readBeComplexArray(const Buffer& buffer, size_t begin, size_t cElemN)
{
    ComplexVector cArray{};
    using ArrayValue = ComplexVector::value_type;
    if (cElemN * 2 * sizeof(Value) > buffer.size() - begin) return cArray;
    cArray.resize(cElemN);
    for (size_t i = 0; i < cElemN; i++) {
        std::array<char, sizeof(Value)> real, imag;
        std::memcpy(&imag, &buffer[begin + i * 2 * sizeof(Value)], sizeof(Value));
        std::memcpy(&real, &buffer[begin + i * 2 * sizeof(Value) + sizeof(Value)], sizeof(Value));
        std::ranges::reverse(real);
        std::ranges::reverse(imag);
        cArray[i] = ArrayValue(std::bit_cast<Value>(real), std::bit_cast<Value>(imag));
    }
    return cArray;
}
#define declare(x) template ComplexVector readBeComplexArray<x>(const Buffer& buffer, size_t begin, size_t ElemN);
declare(short)
declare(int)
declare(long long)
declare(float)
declare(double)
#undef declare

template <typename Value>
ComplexVector readLeComplexArray(const Buffer& buffer, size_t begin, size_t cElemN)
{
    ComplexVector cArray{};
    using ArrayValue = ComplexVector::value_type;
    if (cElemN * 2 * sizeof(Value) > buffer.size() - begin) return cArray;
    cArray.resize(cElemN);
    for (size_t i = 0; i < cElemN; i++) {
        Value real, imag;
        std::memcpy(&real, &buffer[begin + i * 2 * sizeof(Value)], sizeof(Value));
        std::memcpy(&imag, &buffer[begin + i * 2 * sizeof(Value) + sizeof(Value)], sizeof(Value));
        cArray[i] = ArrayValue(real, imag);
    }
    return cArray;
}
#define declare(x) template ComplexVector readLeComplexArray<x>(const Buffer& buffer, size_t begin, size_t ElemN);
declare(short)
declare(int)
declare(long long)
declare(float)
declare(double)
#undef declare

//---------------String Utilities----------------------------------------------------------------------------------------------------------------------------------

String& removeCharsInPlace(String& s, StringView chars) {
    s.erase(std::remove_if(s.begin(), s.end(), [&chars](const char& c) {
        return chars.find(c) != std::string::npos;
    }), s.end());
    return s;
}

String removeCharsCopy(String s, StringView chars) {
    s.erase(std::remove_if(s.begin(), s.end(), [&chars](const char& c) {
        return chars.find(c) != std::string::npos;
    }), s.end());
    return s;
}

Vector<StringView> split(StringView s, StringView delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    StringView token;
    Vector<StringView> res;

    while ((pos_end = s.find(delimiter, pos_start)) != StringView::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr (pos_start));
    return res;
}

Vector<String> splitCopy(StringView s, StringView delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    String token;
    Vector<String> res;

    while ((pos_end = s.find(delimiter, pos_start)) != String::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back(String(s.substr(pos_start)));
    return res;
}

StringView getWord(StringView s, size_t n, StringView delimiter)
{
    size_t index = 0;
    if (n == 0) return s.substr(0, s.find(delimiter));
    for (size_t i = 0; i < n; i++) {
        if (size_t pos = s.find(delimiter, index); pos != StringView::npos) {
            index = pos + delimiter.size();
        } else return {};
    }
    size_t endPos = s.find(delimiter, index);
    if (endPos == StringView::npos) return s.substr(index);
    return s.substr(index, endPos - index);
}

namespace
{
String join_impl(auto s, StringView delimiter)
{
    String res;
    for (auto& i : s) {
        if (i.empty()) continue;
        res.append(i.data(), i.size());
        res.append(delimiter.data(), delimiter.size());
    }
    return res;
}

String join_callback_impl(auto s, StringView delimiter, auto modifier)
{
    (void) 0;
    String res;
    for (auto& j : s) {
        auto i = modifier(j);
        if (i.empty()) continue;
        res.append(i.data(), i.size());
        res.append(delimiter.data(), delimiter.size());
    }
    return res;
}
}

String join(Vector<StringView> s, StringView delimiter)
{
    return join_impl(s, delimiter);
}

String join(Vector<StringView> s, StringView delimiter, sv_callback f)
{
    return join_callback_impl(s, delimiter, f);
}

String join(Vector<String> s, StringView delimiter)
{
    return join_impl(s, delimiter);
}

String join(Vector<String> s, StringView delimiter, sv_callback f)
{
    return join_callback_impl(s, delimiter, f);
}

StringView rstrip(StringView s, StringView chars)
{
    auto n = s.find_last_not_of(chars);
    return (n == StringView::npos) ? StringView{} : s.substr(0, n + 1);
}

StringView lstrip(StringView s, StringView chars)
{
    auto n = s.find_first_not_of(chars);
    return (n == StringView::npos) ? StringView{} : s.substr(n);
}

StringView strip(StringView s, StringView chars)
{
    return lstrip(rstrip(s, chars), chars);
}

StringView strip(StringView s)
{
    return strip(s, R"("' )""\n\t\r");
}

bool xisDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool xisNumber(StringView s)
{
    if (s.empty()) return false;
    for (auto c : s) {
        if (not xisDigit(c)) return false;
    }
    return true;
}

//---------------SHash---------------------------------------------------------------------------------------------------------------------------------------------

size_t SHash::operator()(char const* rhs) const
{
    return std::hash<std::string_view>{}(rhs);
}
size_t SHash::operator()(std::string_view rhs) const
{
    return std::hash<std::string_view>{}(rhs);
}
size_t SHash::operator()(std::string const& rhs) const
{
    return std::hash<std::string>{}(rhs);
}

//---------------SplitIterator-------------------------------------------------------------------------------------------------------------------------------------

template <typename CharT>
SplitIterator<CharT>::SplitIterator(std::basic_string_view<CharT> string, std::basic_string_view<CharT> delimiter)
: string{string}
, delimiter{delimiter}
{

}

template <typename CharT>
SplitIterator<CharT>::Iterator SplitIterator<CharT>::begin()
{
    return Iterator{this, 0, delimiter};
}

template <typename CharT>
SplitIterator<CharT>::Iterator SplitIterator<CharT>::end()
{
    return Iterator{this, string.size(), delimiter};
}

template <typename CharT>
SplitIterator<CharT>::Iterator::Iterator(SplitIterator<CharT>* splitIterator, size_t position, std::basic_string_view<CharT> delimiter)
: splitIterator{splitIterator}
, position{position}
, delimiter{delimiter}
, delimiterPos{position}
{
    if (delimiter.size() == 0) {
        this->delimiterPos = splitIterator->string.size();
    }
    ++(*this);
    if (position != splitIterator->string.size()) this->position = 0;
}

template <typename CharT>
SplitIterator<CharT>::Iterator& SplitIterator<CharT>::Iterator::operator++()
{
    position = delimiterPos + delimiter.size();
    auto newPosition = splitIterator->string.find(delimiter, position);
    if (newPosition != decltype(splitIterator->string)::npos) {
        delimiterPos = newPosition;
    } else {
        delimiterPos = splitIterator->string.size();
        position = delimiterPos;
    }
    return *this;
}

template <typename CharT>
SplitIterator<CharT>::Iterator SplitIterator<CharT>::Iterator::operator++(int)
{
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

template <typename CharT>
bool SplitIterator<CharT>::Iterator::operator==(const Iterator& b) const
{
    return (this->splitIterator == b.splitIterator)
       and (this->delimiterPos == b.delimiterPos)
       and (this->position == b.position);
}

template <typename CharT>
std::basic_string_view<CharT> SplitIterator<CharT>::Iterator::operator*() const
{
    size_t start = position;
    return splitIterator->string.substr(start, delimiterPos - start);
}

template class SplitIterator<char>;
}
