#include "br.h"

#include "big_endian.h"
#include "little_endian.h"
#include "string_operations.h"
#include "general.h"
#include "../spectrum/spectrum_info.h"

#include <utility>
#include <fstream>
#include <vector>
#include <complex>
#include <cstddef>
#include <filesystem>
#include <cstring>
#include <string>
#include <stdexcept>
#include <optional>
#include <unordered_map>

#include <QDebug>

namespace
{

using namespace FileIO;

struct BrFidInfo {
    bool bigEndian;
    DataType dataType;
    size_t elemN;
};

std::optional<std::pair<SpectrumInfo, BrFidInfo>> parseAcqus(std::ifstream& acqusFile)
{
    std::unordered_map<std::string, std::string> params{
        {"$O1", {}}, // center of spectrum Hz
        {"$SW_h", {}}, // spectral width Hz
        {"$BF1", {}}, // observed nucleus frequency
        {"$TD", {}}, // simple elements number
        {"$SFO1", {}}, // irradiation frequency
        {"$SW", {}}, // spectral width ppm
        {"$DTYPA", {}}, // data type
        {"$GRPDLY", {}}, // group delay
        {"$SOLVENT", {}}, // solvent
        {"$NUC1", {}},
        {"$BYTORDA", {}},        // observed nucleus
    };

    std::string currentLine{};
    std::vector<std::string> words{};

    std::string currentKey{};

    while(std::getline(acqusFile, currentLine)) {
        words = StringOperations::split(currentLine, " ");
        currentKey = StringOperations::remove_chars(words[0], R"(#=)");
        if (params.count(currentKey)) {
            params[currentKey] = StringOperations::remove_chars(words[1], R"("<>)");
        }
    }

    // checks whether any key remained empty
    if (params.contains(std::string{})) {
        qDebug() << "1";
        return {};
    }

    double spectrumCenter{}, spectralWidthHz{}, spectralWidthPpm{}, observedNucleusFreq{},
        plotBegin{}, plotEnd{}, irradiationFreq{}, dwellTime{}, groupDelay{};
    int elementsNumber;
    bool bigEndian;

    // if conversions fail file is assumed to be corrupt and empty optional is returned
    try{
        spectrumCenter = std::stod(params["$O1"]);
        spectralWidthHz = std::stod(params["$SW_h"]);
        spectralWidthPpm = std::stod(params["$SW"]);
        observedNucleusFreq = std::stod(params["$BF1"]);
        elementsNumber = std::stoi(params["$TD"]) / 2;
        irradiationFreq = std::stod(params["$SFO1"]);
        bigEndian = static_cast<bool>(std::stoi(params["$BYTORDA"]));
        groupDelay = std::stod(params["$GRPDLY"]);
    } catch (const std::invalid_argument& e) {
        qDebug() << "2";
        return {};
    } catch(const std::out_of_range& e) {
        qDebug() << "3";
        return {};
    }

    dwellTime = 1 / spectralWidthPpm / irradiationFreq;

    size_t elemN;

    if (elementsNumber > 0) {
        elemN = static_cast<size_t>(elementsNumber);
    } else {
        qDebug() << "4";
        return {};
    }

    DataType dataType;

    if (params["$DTYPA"] == "0") {
        dataType = DataType::int32;
    } else if (params["$DTYPA"] == "2") {
        dataType = DataType::double64;
    } else {
        dataType = DataType::unknown;
    }

    double plotBeginHz = spectrumCenter - spectralWidthHz / 2;
    double plotEndHz = spectrumCenter + spectralWidthHz / 2;

    SpectrumInfo info{
        .plot_right_Hz = plotBeginHz,
        .plot_left_Hz = plotEndHz,
        .plot_right_ppm = plotBegin,
        .plot_left_ppm = plotEnd,
        .spectral_width = spectralWidthHz,
        .acquisition_time = elementsNumber * dwellTime,
        .obs_nucleus_freq = observedNucleusFreq,
        .dwell_time = dwellTime,
        .group_delay = groupDelay,
        .trimmed = 0.0,
        .samplename = {},
        .nucleus = params["$NUC1"],
        .solvent = params["$SOLVENT"]
    };

    BrFidInfo fidInfo{
        .bigEndian = bigEndian,
        .dataType = dataType,
        .elemN = elemN,
    };

    return std::make_pair(info, fidInfo);
}

std::optional<std::vector<std::vector<std::complex<double>>>> readFidFile(std::ifstream& fidFile, BrFidInfo& fidInfo)
{
    using enum DataType;
    using namespace FileIO;
    const size_t fidSize = fidInfo.elemN * dataTypeSize(fidInfo.dataType) * 2;

    std::vector<std::complex<double>> fid{};
    std::vector<std::byte> buffer(fidSize, std::byte{0});

    if (not fidFile.read(reinterpret_cast<char*>(buffer.data()), fidSize)) {
        return {};
    }

    if (fidInfo.bigEndian){
        using namespace BigEndian;
        switch (fidInfo.dataType) {
        case int32:
            fid = read_fid_array_int(buffer);
            break;
        case double64:
            fid = read_fid_array_double(buffer);
            break;
        default:
            return {};
        }
    } else {
        using namespace LittleEndian;
        switch (fidInfo.dataType) {
        case int32:
            fid = read_fid_array_int(buffer);
            break;
        case double64:
            fid = read_fid_array_double(buffer);
            break;
        default:
            return {};
        }
    }
        std::vector<std::vector<std::complex<double>>> fids{};
        fids.push_back(fid);
        return fids;
}

} // end of namespace

FileIO::FileReadResult FileIO::brParseExperimentFolder(const std::filesystem::path& folderPath)
{
    std::ifstream fidFile{folderPath / "fid", std::ios::in | std::ios::binary};
    std::ifstream acqusFile{folderPath / "acqus", std::ios::in};

    FileReadResult result{};

    if (acqusFile) {
        std::optional<std::pair<SpectrumInfo, BrFidInfo>> info = parseAcqus(acqusFile);
        if (info) {
            auto [spectrumInfo, fidInfo] = *info;
            result.info = spectrumInfo;
            if (auto fids = readFidFile(fidFile, fidInfo)) {
                result.fids = *fids;
            } else {
                result.file_read_status = FileReadStatus::invalid_fid;
            }
        } else {
            result.file_read_status = FileReadStatus::invalidAcqus;
            return result;
        }
    } else {
        result.file_read_status = FileReadStatus::invalidAcqus;
        return result;
    }


    result.file_read_status = FileReadStatus::success_1D;

    return result;
}
