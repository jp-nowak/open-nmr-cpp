#include "i.h"

#include "general.h"
#include "../spectrum/spectrum_info.h"
#include "file_io.h"

#include <utility>
#include <vector>
#include <cstddef>
#include <filesystem>
#include <cstring>
#include <string>
#include <stdexcept>
#include <optional>
#include <unordered_map>
#include <array>
#include <QDebug>
#include <string>
#include <array>
#include <cstddef>

using namespace IO;

namespace
{

struct FullHeader
{
    FixedString<8> fileIdentifier;
    i8 endian; // endian of params and data: 1 little 0 big, header is always big
    u8 majorVersion;
    u16 minorVersion;
    u8 dataDimensionNumber;
    u8 dataDimensionExist;
    u8 controlByte;
    i8 dataInstrument;
    Array<u8, 8> translate;
    Array<u8, 8> dataAxisType;
    Array<Byte, 16> axisUnits;
    FixedString<124> title; // name of experiment
    Array<Byte, 4> xAxisType;
    Array<u32, 8> elementNumber; // mV = 2: number of complex elements in fid
    Array<u32, 8> offsetStart;
    Array<u32, 8> offsetEnd;
    Array<f64, 8> axisStart; // mV = 2: always 0?
    Array<f64, 8> axisEnd; // mV = 2: acquisition time
    Array<Byte, 4> experimentDate;
    Array<Byte, 4> revisionDate;
    FixedString<16> nodeName;
    FixedString<128> site;
    FixedString<128> author;
    FixedString<128> comment;
    FixedString<256> axisTitles;
    Array<f64, 8> baseFreqs;
    Array<f64, 8> zeroPoints;
    Array<Byte, 8> reversed;
    Array<Byte, 3> SKIP; // three bytes not used in header
    u32 annotationOk;
    u32 historyUsed;
    u32 historyLength;
    u32 paramStart; // params start at this byte + 16
    u32 paramLength;
    Array<u32, 8> listStart;
    Array<u32, 8> listLength;
    u32 dataStart; // start of fid
    u64 dataLength;
    u64 contextStart;
    u32 contextLength;
    u64 annoteStart;
    u32 annoteLength;
    u64 totalSize;
    u32 unitLocation;
};

enum HeaderFields {
    fileIdentifier = 0,
    endian = 8,
    majorVersion = 9,
    minorVersion = 10,
    dataDimensionNumber = 12,
    dataDimensionExist = 13,
    controlByte = 14,
    dataInstrument = 15,
    translate = 16,
    dataAxisType = 24,
    axisUnits = 32,
    title = 48,
    xAxisType = 172,
    elementNumber = 176,
    offsetStart = 208,
    offsetEnd = 240,
    axisStart = 272,
    axisEnd = 336,
    experimentDate = 400,
    revisionDate = 404,
    nodeName = 408,
    site = 424,
    author = 552,
    comment = 680,
    axisTitles = 808,
    baseFreqs = 1064,
    zeroPoints = 1128,
    reversed = 1192,
    SKIP = 1200, // three bytes not used in header
    annotationOk = 1203,
    historyUsed = 1204,
    historyLength = 1208,
    paramStart = 1212,
    paramLength = 1216,
    listStart = 1220,
    listLength = 1252,
    dataStart = 1284,
    dataLength = 1288,
    contextStart = 1296,
    contextLength = 1304,
    annoteStart = 1308,
    annoteLength = 1316,
    totalSize = 1320,
    unitLocation = 1328,
};

constexpr size_t FILE_HEADER_SIZE = 1332;

struct Date
{
    u8 day;
    u8 month;
    int year;
};

std::optional<Date> readDate(Array<Byte, 4>& bytes)
{
    int day = bytes[2] & 0b00011111;
    int month = ((bytes[0] << 3) & 0b00001000) + (bytes[1] >> 5);
    int year = 1990 + (bytes[0] >> 1);
    if ((day > 31) or (month > 12)) return {};
    assert(year > 1990); assert(year < 1990 + 127);
    return Date{static_cast<u8>(day), static_cast<u8>(month), year};
}

using ParamValue = std::variant<i32, f64, Complex, String>;

enum Unit {
    U_None      = 0,
    Abundance   = 1,
    Ampere      = 2,
    Candela     = 3,
    Celsius     = 4,
    Coulomb     = 5,
    Degree      = 6,
    Electronvolt= 7,
    Farad       = 8,
    Sievert     = 9,
    Gram        = 10,
    Gray        = 11,
    Henry       = 12,
    Hertz       = 13,
    Kelvin      = 14,
    Joule       = 15,
    Liter       = 16,
    Lumen       = 17,
    Lux         = 18,
    Meter       = 19,
    Mole        = 20,
    Newton      = 21,
    Ohm         = 22,
    Pascal      = 23,
    Percent     = 24,
    Point       = 25,
    Ppm         = 26,
    Radian      = 27,
    Second      = 28,
    Siemens     = 29,
    Steradian   = 30,
    Tesla       = 31,
    Volt        = 32,
    Watt        = 33,
    Weber       = 34,
    Decibel     = 35,
    Dalton      = 36,
    Thompson    = 37,
    Ugeneric    = 38,
    LPercent    = 39,
    PPT         = 40,
    PPB         = 41,
    Index       = 42,
    U_COUNT,
    U_ERROR
};

Unit ToUnit(int value)
{
    if (value >= U_COUNT) return U_ERROR;
    if (value < U_None) return U_ERROR;
    return static_cast<Unit>(value);
}

enum Prefix : i8 {
    Yotta = -8,
    Zetta = -7,
    Exa   = -6,
    Pecta = -5,
    Tera  = -4,
    Giga  = -3,
    Mega  = -2,
    Kilo  = -1,
    P_None  = 0,
    Milli = 1,
    Micro = 2,
    Nano  = 3,
    Pico  = 4,
    Femto = 5,
    Atto  = 6,
    Zepto = 7,
    P_COUNT,
    P_ERROR,
};

Prefix ToPrefix(i8 value)
{
    if (value >= P_COUNT) return P_ERROR;
    if (value < Yotta) return P_ERROR;
    auto a = static_cast<Prefix>(value);
    return a;
}

struct FullUnit
{
    Prefix prefix;
    Unit unit;
    i8 power;
};

struct Units
{
    FullUnit u0;
    FullUnit u1;
    FullUnit u2;
    FullUnit u3;
    FullUnit u4;
};

FullUnit getUnit(const Buffer& buffer, size_t start)
{
    auto a = FullUnit{
        .prefix = ToPrefix(std::bit_cast<i8>(buffer[start]) >> 4),
        .unit = ToUnit(leTo<i8>(buffer, start + 1)),
        .power = static_cast<i8>(std::bit_cast<i8>(buffer[start]) & 0b00001111)
    };
    return FullUnit{
        .prefix = ToPrefix(std::bit_cast<i8>(buffer[start]) >> 4),
        .unit = ToUnit(leTo<i8>(buffer, start + 1)),
        .power = static_cast<i8>(std::bit_cast<i8>(buffer[start]) & 0b00001111)
    };
}

Units getUnits(const Buffer& buffer, size_t start)
{
    Units a;
    a.u0 = getUnit(buffer, start);
    a.u1 = getUnit(buffer, start + 2);
    a.u2 = getUnit(buffer, start + 4);
    a.u3 = getUnit(buffer, start + 6);
    a.u4 = getUnit(buffer, start + 8);
    return a;
}

struct Param
{
    ParamValue value;
    Units units;
    u16 scaler;
};

struct SHash2
{
    using is_transparent = void;
    size_t operator()(char const* rhs) const
    {
        return std::hash<std::string_view>{}(rhs);
    }
    size_t operator()(std::string_view rhs) const
    {
        return std::hash<std::string_view>{}(rhs);
    }
    size_t operator()(std::string const& rhs) const
    {
        return std::hash<std::string>{}(rhs);
    }
    size_t operator()(FixedString<28> const& rhs) const
    {
        return std::hash<std::string_view>{}(StringView{std::begin(rhs),
                    std::find(std::begin(rhs), std::end(rhs), '\0')});
    }
};

using ParamsDict = std::unordered_map<String, Param, SHash2, std::equal_to<>>;

[[maybe_unused]] std::optional<FullHeader> readFullFileHeader(const Buffer& buffer, size_t begin)
{
    if (buffer.size() < begin + FILE_HEADER_SIZE) return {};
    return FullHeader{
                      beTo<FixedString<8>>(buffer, begin + fileIdentifier),
                      beTo<i8>(buffer, begin + endian),
                      beTo<u8>(buffer, begin + majorVersion),
                      beTo<u16>(buffer, begin + minorVersion),
                      beTo<u8>(buffer, begin + dataDimensionNumber),
                      beTo<u8>(buffer, begin + dataDimensionExist),
                      beTo<u8>(buffer, begin + controlByte),
                      beTo<i8>(buffer, begin + dataInstrument),
                      beTo<Array<u8, 8>>(buffer, begin + translate),
                      beTo<Array<u8, 8>>(buffer, begin + dataAxisType),
                      beTo<Array<Byte, 16>>(buffer, begin + axisUnits),
                      beTo<FixedString<124>>(buffer, begin + title),
                      beTo<Array<Byte, 4>>(buffer, begin + xAxisType),
                      beTo<Array<u32, 8>>(buffer, begin + elementNumber),
                      beTo<Array<u32, 8>>(buffer, begin + offsetStart),
                      beTo<Array<u32, 8>>(buffer, begin + offsetEnd),
                      beTo<Array<f64, 8>>(buffer, begin + axisStart),
                      beTo<Array<f64, 8>>(buffer, begin + axisEnd),
                      beTo<Array<Byte, 4>>(buffer, begin + experimentDate),
                      beTo<Array<Byte, 4>>(buffer, begin + revisionDate),
                      beTo<FixedString<16>>(buffer, begin + nodeName),
                      beTo<FixedString<128>>(buffer, begin + site),
                      beTo<FixedString<128>>(buffer, begin + author),
                      beTo<FixedString<128>>(buffer, begin + comment),
                      beTo<FixedString<256>>(buffer, begin + axisTitles),
                      beTo<Array<f64, 8>>(buffer, begin + baseFreqs),
                      beTo<Array<f64, 8>>(buffer, begin + zeroPoints),
                      beTo<Array<Byte, 8>>(buffer, begin + reversed),
                      beTo<Array<Byte, 3>>(buffer, begin + SKIP),
                      beTo<u32>(buffer, begin + annotationOk),
                      beTo<u32>(buffer, begin + historyUsed),
                      beTo<u32>(buffer, begin + historyLength),
                      beTo<u32>(buffer, begin + paramStart),
                      beTo<u32>(buffer, begin + paramLength),
                      beTo<Array<u32, 8>>(buffer, begin + listStart),
                      beTo<Array<u32, 8>>(buffer, begin + listLength),
                      beTo<u32>(buffer, begin + dataStart),
                      beTo<u64>(buffer, begin + dataLength),
                      beTo<u64>(buffer, begin + contextStart),
                      beTo<u32>(buffer, begin + contextLength),
                      beTo<u64>(buffer, begin + annoteStart),
                      beTo<u32>(buffer, begin + annoteLength),
                      beTo<u64>(buffer, begin + totalSize),
                      beTo<u32>(buffer, begin + unitLocation)
    };
}



template <typename T, size_t N>
static QDebug printArray(QDebug os, const std::array<T, N>& arr) {
    os << "[";
    for (size_t i = 0; i < N; i++) {
        if constexpr (std::is_same_v<T, std::byte>)
            os << std::to_integer<int>(arr[i]);
        else
            os << arr[i];

        if (i + 1 < N) os << ", ";
    }
    os << "]";
    return os;
}

#define TO_STRING(x) strip(StringView{x.data(), x.size()})

QDebug operator<<(QDebug os, const FullHeader& h)
{
    os << "FullHeader {\n";

    os << "  fileIdentifier: \""     << TO_STRING(h.fileIdentifier) << "\"\n";
    os << "  endian: "                << int(h.endian) << "\n";
    os << "  majorVersion: "          << int(h.majorVersion) << "\n";
    os << "  minorVersion: "          << h.minorVersion << "\n";
    os << "  dataDimensionNumber: "   << int(h.dataDimensionNumber) << "\n";

    os << "  dataDimensionExist: "    << h.dataDimensionExist << "\n";
    os << "  controlByte: "           << h.controlByte << "\n";

    os << "  dataInstrument: "        << int(h.dataInstrument) << "\n";

    os << "  translate: ";            printArray(os, h.translate) << "\n";
    os << "  dataAxisType: ";         printArray(os, h.dataAxisType) << "\n";
    os << "  axisUnits: ";            printArray(os, h.axisUnits) << "\n";

    os << "  title: \""               << TO_STRING(h.title) << "\"\n";

    os << "  xAxisType: ";            printArray(os, h.xAxisType) << "\n";
    os << "  elementNumber: ";        printArray(os, h.elementNumber) << "\n";
    os << "  offsetStart: ";          printArray(os, h.offsetStart) << "\n";
    os << "  offsetEnd: ";            printArray(os, h.offsetEnd) << "\n";
    os << "  axisStart: ";            printArray(os, h.axisStart) << "\n";
    os << "  axisEnd: ";              printArray(os, h.axisEnd) << "\n";
    os << "  experimentDate: ";       printArray(os, h.experimentDate) << "\n";
    os << "  revisionDate: ";         printArray(os, h.revisionDate) << "\n";

    os << "  nodeName: \""            << TO_STRING(h.nodeName) << "\"\n";
    os << "  site: \""                << TO_STRING(h.site) << "\"\n";
    os << "  author: \""              << TO_STRING(h.author) << "\"\n";
    os << "  comment: \""             << TO_STRING(h.comment) << "\"\n";
    os << "  axisTitles: \""          << TO_STRING(h.axisTitles) << "\"\n";

    os << "  baseFreqs: ";            printArray(os, h.baseFreqs) << "\n";
    os << "  zeroPoints: ";           printArray(os, h.zeroPoints) << "\n";
    os << "  reversed: ";             printArray(os, h.reversed) << "\n";

    os << "  SKIP: ";                 printArray(os, h.SKIP) << "\n";

    os << "  annotationOk: "          << h.annotationOk << "\n";
    os << "  historyUsed: "           << h.historyUsed << "\n";
    os << "  historyLength: "         << h.historyLength << "\n";
    os << "  paramStart: "            << h.paramStart << "\n";
    os << "  paramLength: "           << h.paramLength << "\n";

    os << "  listStart: ";            printArray(os, h.listStart) << "\n";
    os << "  listLength: ";           printArray(os, h.listLength) << "\n";

    os << "  dataStart: "             << h.dataStart << "\n";
    os << "  dataLength: "            << h.dataLength << "\n";

    os << "  contextStart: "          << h.contextStart << "\n";
    os << "  contextLength: "         << h.contextLength << "\n";

    os << "  annoteStart: "           << h.annoteStart << "\n";
    os << "  annoteLength: "          << h.annoteLength << "\n";

    os << "  totalSize: "             << h.totalSize << "\n";
    os << "  unitLocation: "          << h.unitLocation << "\n";

    os << "}\n";

    return os;
}

enum Positions
{
    Pos_Scaler = 4,
    Pos_Unit = 6,
    Pos_Value = 16,
    Pos_Type = 32,
    Pos_Name = 36,
    Param_Size = 64
};

std::optional<ParamsDict> readParams(const Buffer& buffer, size_t paramNumber, size_t start, bool bigEndian)
{

//  |xxxx||scaler||       unit     ||          value                      || 32 bytes
//  |    type   ||                   name                                 || 32 bytes
//  xxxx - 4 empty bytes
//  scaler - u16
//  unit(s) - 10 bytes
//  value 16 bytes one of: FixedString<16>, f64, i32, Complex<double>
//  valuetype uint32
//  name FixedString<28>

    size_t pos = start;

    ParamsDict params{};

    for (size_t i = 0; i <= paramNumber; i++, pos += Param_Size) {
        if (i == 1) {
            qDebug () << 5;
        }
        u32 type = bytesTo<u32>(buffer, pos + Pos_Type, bigEndian);
        u16 scaler = bytesTo<u16>(buffer, pos + Pos_Scaler, bigEndian);
        Units units = getUnits(buffer, pos + Pos_Unit);

        StringView valueName{reinterpret_cast<const char*>(&buffer[pos + Pos_Name]), 28};
        valueName = strip(valueName);

        switch(type) {
        case 0: // char[16]
        {
            StringView value{reinterpret_cast<const char*>(&buffer[pos + Pos_Value]), 16};
            value = strip(value);
            params.emplace(std::make_pair(String{valueName},
                                          Param{
                                              .value = String{value},
                                              .units = units,
                                              .scaler = scaler}));
            break;
        }
        case 4: [[fallthrough]]; // inf?
        case 1: // i32
            params.emplace(std::make_pair(String{valueName},
                                          Param{
                                                .value = bytesTo<i32>(buffer, pos + Pos_Value, bigEndian),
                                                .units = units,
                                                .scaler = scaler}));
            break;
        case 2: // f64
            params.emplace(std::make_pair(String{valueName},
                                          Param{
                                                .value = bytesTo<f64>(buffer, pos + Pos_Value, bigEndian),
                                                .units = units,
                                                .scaler = scaler}));
            break;
        case 3: // Complex (f64, f64)
            params.emplace(std::make_pair(String{valueName},
                                          Param{
                                                .value = std::complex<double>{bytesTo<double>(buffer, pos + Pos_Value, bigEndian),
                                                    bytesTo<double>(buffer, pos + Pos_Value + sizeof(double), bigEndian)},
                                                .units = units,
                                                .scaler = scaler}));
            break;
        default:
            return {};
        }
    }
    return params;
}

#define PPM 1000000

std::optional<SpectrumInfo> paramsToInfo(const FullHeader& header, const ParamsDict& params)
{
    if (header.majorVersion != 1) return {};

    auto findCaseInsensitive = [&params](auto x) {
        if (auto p = params.find(x); p != params.end()) return p;
        if (auto p = params.find(ASCItoLower(x)); p != params.end()) return p;
        if (auto p = params.find(ASCItoUpper(x)); p != params.end()) return p;
        throw std::out_of_range("NO_PARAM");
    };

    auto paramValue = [&findCaseInsensitive](StringView x) -> double {
        auto p = findCaseInsensitive(x);
        auto valueVariant = p->second.value;
        if (std::holds_alternative<int>(valueVariant)) {
            return std::get<int>(valueVariant);
        } else if (std::holds_alternative<double>(valueVariant)) {
            return std::get<double>(valueVariant);
        }
        throw std::out_of_range("NO_NUMERIC_VAL");
    };

    SpectrumInfo info{};
    double xOffset_ppm;
    double xOffset_Hz;
    try {
        if (header.minorVersion == 1) {
            info.plot_left_ppm = header.axisStart[0];
            info.plot_right_ppm = header.axisEnd[0];
            xOffset_ppm = (info.plot_left_ppm - info.plot_right_ppm) / 2;
            info.obs_nucleus_freq = paramValue("x_freq");
            info.plot_left_Hz = info.plot_left_ppm * info.obs_nucleus_freq;
            info.plot_right_Hz = info.plot_right_ppm * info.obs_nucleus_freq;
            info.spectral_width = info.plot_left_Hz - info.plot_right_Hz;
            info.dwell_time = 1 / info.spectral_width;
            info.acquisition_time = info.dwell_time * header.elementNumber[0];
            info.group_delay = 0.0;
            info.trimmed = 0.0;
        } else if (header.minorVersion == 2) {
            xOffset_ppm = paramValue("X_OFFSET"); // [ppm] middle point of spectrum
            info.obs_nucleus_freq = paramValue("X_FREQ") / PPM; // [Hz] Larmor frequency of measured nucleus (0 ppm)
            xOffset_Hz = info.obs_nucleus_freq * xOffset_ppm;
            info.spectral_width = paramValue("X_SWEEP_CLIPPED"); // [Hz]
            info.plot_right_Hz = xOffset_Hz - info.spectral_width / 2;
            info.plot_left_Hz = xOffset_Hz + info.spectral_width / 2;
            info.plot_left_ppm = info.plot_left_Hz / info.obs_nucleus_freq;
            info.plot_right_ppm = info.plot_right_Hz / info.obs_nucleus_freq;
            if (header.axisStart[0] != 0) return {};
            else info.acquisition_time = header.axisEnd[0];
            info.dwell_time = info.acquisition_time / header.elementNumber[0];
            info.group_delay = 20.0;
        } else return {};

        info.solvent = strip(std::get<String>(params.at("solvent").value));
        info.solvent = strip(std::get<String>(findCaseInsensitive("solvent")->second.value));
        info.samplename = strip(StringView{header.title.data(), header.title.size()});
        info.nucleus = strip(std::get<String>(findCaseInsensitive("x_domain")->second.value));

    } catch (...) {
        return {};
    }

    return info;
}

ComplexVector readFid(const Buffer& buffer, FullHeader header)
{ // TODO rewrite in better way

    DataType elementType;
    if (header.controlByte & 0b00000001) {
        elementType = DataType::float64;
    } else if (header.controlByte & 0b01000000) {
        elementType = DataType::float32;
    } else return {};

    Endian endian = (header.endian) ? Endian::little : Endian::big;

    size_t arraySize = header.elementNumber[0] * dataTypeSize(elementType);
    double i = buffer.size() - header.dataStart;

    if (header.minorVersion == 2) {
        if (arraySize * 2 > buffer.size() - header.dataStart) return {};

        Vector<NumericValueType> imagFid = readSimpleArray(endian, elementType, buffer, header.dataStart, header.elementNumber[0]);
        Vector<NumericValueType> realFid = readSimpleArray(endian, elementType, buffer, header.dataStart + arraySize, header.elementNumber[0]);
        assert(realFid.size() == imagFid.size());
        ComplexVector fid;
        fid.resize(realFid.size());
        std::transform(realFid.begin(), realFid.end(), imagFid.begin(), fid.begin(), [](auto a, auto b){return Complex{a, b};});
        return fid;
    } else if (header.minorVersion == 1) {
        if (arraySize > buffer.size() - header.dataStart) return {};
        if (arraySize % 2 != 0) return {};
        ComplexVector fid = readComplexArray(endian, elementType, buffer, header.dataStart, header.elementNumber[0] / 2);
        return fid;
    } else return {};
}

QDebug operator<< (QDebug os, Complex const& m)
{
    return os << real(m) << imag(m);
}

QDebug operator<< (QDebug os, Date const& m)
{
    return os << "date: " << m.day << "." << m.month << "." << m.year;
}

QDebug operator<< (QDebug os, ParamsDict const& m)
{
    for (auto& i : m) {
        qDebug() << i.first;
        std::visit([](auto&& arg){qDebug() << arg;}, i.second.value);
        qDebug() << "\n";
    }
}

} // end of namespace

FileReadResult openExperimentI(const std::filesystem::path& filePath)
{
    FileReadResult r{.type = FileType::I};
    Buffer buffer;
    if (not readFileTo(buffer, filePath)) {r.status = ReadStatus::invalidJDF; return r;}

    auto header = readFullFileHeader(buffer, 0);
    if (not header) {r.status = ReadStatus::invalidJDF; return r;}


    bool bigEndian = !header->endian;
    u32 paramSize = bytesTo<u32>(buffer, header->paramStart, bigEndian);
    if (paramSize != 64) {r.status = ReadStatus::invalidJDF; return r;}
    u32 lowIndex = bytesTo<u32>(buffer, header->paramStart + 4, bigEndian);
    if (lowIndex != 0) {r.status = ReadStatus::invalidJDF; return r;}
    u32 highIndex = bytesTo<u32>(buffer, header->paramStart + 8, bigEndian);
    [[maybe_unused]] u32 paramTotalSize = bytesTo<u32>(buffer, header->paramStart + 12, bigEndian);

    auto params = readParams(buffer, highIndex, header->paramStart + 16, bigEndian);
    if (not params) {r.status = ReadStatus::invalidJDF; return r;}

    auto info = paramsToInfo(header.value(), params.value());
    if (not info) {r.status = ReadStatus::invalidJDF; return r;}

    auto fid = readFid(buffer, header.value());
    if (not fid.size()) {r.status = ReadStatus::invalidJDF; return r;}

    r.status = ReadStatus::success_1D;
    r.fids = {fid};
    r.info = info.value();
    r.type = FileType::I;
    return r;

}

// repair ppm shifts for jdf

