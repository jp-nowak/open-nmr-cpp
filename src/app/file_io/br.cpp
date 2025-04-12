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
#include <array>
#include <QDebug>

namespace
{

using namespace FileIO;

struct BrFidInfo {
    bool bigEndian;
    DataType dataType;
    size_t elemN;
};

std::optional<double> checkGroupDelay(int decim, int dspfvs)
{
    static const std::unordered_map<int, std::array<double, 4>> dspTable{
        // DECIM | DSPFVS 10       11          12          13
        {2,    {{44.7500,   46.0000,    46.3111,    2.7500}}},
        {3,    {{33.5000,   36.5000,    36.5300,    2.8330}}},
        {4,    {{66.6250,   48.0000, 	47.8700, 	2.8750}}},
        {6,    {{59.0833, 	50.1667, 	50.2290, 	2.9170}}},
        {8,    {{68.5625, 	53.2500, 	53.2890, 	2.9380}}},
        {12,   {{60.3750, 	69.5000, 	69.5510, 	2.9580}}},
        {16,   {{69.5313, 	72.2500, 	71.6000, 	2.9690}}},
        {24,   {{61.0208, 	70.1667, 	70.1840, 	2.9790}}},
        {32,   {{70.0156, 	72.7500, 	72.1380, 	2.9840}}},
        {48,   {{61.3438, 	70.5000, 	70.5280, 	2.9890}}},
        {64,   {{70.2578, 	73.0000, 	72.3480, 	2.9920}}},
        {96,   {{61.5052, 	70.6667, 	70.7000, 	2.9950}}},
        {128,  {{70.3789, 	72.5000, 	72.5240,    0.0   }}},
        {192,  {{61.5859,	71.3333, 	71.3333, 	0.0   }}},
        {256,  {{70.4395, 	72.2500, 	72.2500, 	0.0   }}},
        {384,  {{61.6263, 	71.6667, 	71.6667, 	0.0   }}},
        {512,  {{70.4697, 	72.1250, 	72.1250, 	0.0   }}},
        {768,  {{61.6465, 	71.8333, 	71.8333,    0.0   }}},
        {1024, {{70.4849, 	72.0625, 	72.0625, 	0.0   }}},
        {1536, {{61.6566, 	71.9167, 	71.9167, 	0.0   }}},
        {2048, {{70.4924, 	72.0313, 	72.0313, 	0.0   }}},
        };

    if ((dspfvs > 13) || (dspfvs < 10)) return {};

    double groupDelay = 0.0;
    try{
        groupDelay = dspTable.at(decim).at(dspfvs - 10);
    } catch (const std::out_of_range& e) {
        return {};
    }
    if (groupDelay == 0.0) return {};
    return groupDelay;
}

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
        {"$NUC1", {}}, // observed nucleus
        {"$BYTORDA", {}}, // byte order 1 - BE, 0 - LE
        {"$DECIM", {}},
        {"$DSPFVS", {}},
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

    int decim, dspfvs;
    double groupDelay;
    bool validGroupDelay;
    try{
        groupDelay = std::stod(params["$GRPDLY"]);
        validGroupDelay = true;
    } catch (const std::invalid_argument& e) {
        validGroupDelay = false;
    } catch(const std::out_of_range& e) {
        validGroupDelay = false;
    }
    if (groupDelay < 0.0) {
        validGroupDelay = false;
    }

    if (not validGroupDelay) {
        try{
            decim = std::stoi(params["$DECIM"]);
            dspfvs = std::stoi(params["$DSPFVS"]);
        } catch (const std::invalid_argument& e) {
            return {};
        } catch(const std::out_of_range& e) {
            return {};
        }
        if (std::optional<double> gd = checkGroupDelay(decim, dspfvs)) {
            groupDelay = *gd;
        } else return {};
    }

    double spectrumCenter{}, spectralWidthHz{}, spectralWidthPpm{}, observedNucleusFreq{},
        irradiationFreq{}, dwellTime{};
    int elementsNumber{};
    bool bigEndian{};

    // if conversions fail file is assumed to be corrupt and empty optional is returned
    try{
        spectrumCenter = std::stod(params["$O1"]);
        spectralWidthHz = std::stod(params["$SW_h"]);
        spectralWidthPpm = std::stod(params["$SW"]);
        observedNucleusFreq = std::stod(params["$BF1"]);
        elementsNumber = std::stoi(params["$TD"]) / 2;
        irradiationFreq = std::stod(params["$SFO1"]);
        bigEndian = static_cast<bool>(std::stoi(params["$BYTORDA"]));
    } catch (const std::invalid_argument& e) {
        return {};
    } catch(const std::out_of_range& e) {
        return {};
    }

    dwellTime = 1 / spectralWidthPpm / irradiationFreq;

    size_t elemN;

    if (elementsNumber > 0) {
        elemN = static_cast<size_t>(elementsNumber);
    } else {
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
        .plot_right_ppm = plotBeginHz / observedNucleusFreq,
        .plot_left_ppm = plotEndHz / observedNucleusFreq,
        .spectral_width = spectralWidthHz,
        .acquisition_time = elementsNumber * dwellTime,
        .obs_nucleus_freq = observedNucleusFreq,
        .dwell_time = dwellTime,
        .group_delay = groupDelay,
        .trimmed = 0.0,
        .samplename = {}, // TODO!!!
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

std::string readTitle(const std::filesystem::path& folderPath)
{
    std::string title{};
    if (not std::filesystem::exists(folderPath / "pdata")) return title;
    auto pdataIter = std::filesystem::recursive_directory_iterator{folderPath / "pdata"};

    for (auto& i : pdataIter) {
        if (i.is_regular_file() && i.path().filename() == "title") {
            std::ifstream titleFile{i.path()};
            if (not titleFile) return title;
            std::string currentLine{};
            while(std::getline(titleFile, currentLine)) {
                title.append(1, ' ').append(currentLine);
            }
            return title;
        }
    }

    return title;
}


} // end of namespace


FileIO::FileReadResult FileIO::brParseExperimentFolder(const std::filesystem::path& folderPath)
{

    FileReadResult result{};
    result.file_type = FileType::Br;
    result.file_read_status = FileReadStatus::unknown_failure;
    std::ifstream acqusFile{folderPath / "acqus", std::ios::in};

    if (acqusFile) {
        std::optional<std::pair<SpectrumInfo, BrFidInfo>> info = parseAcqus(acqusFile);
        if (info) {
            auto [spectrumInfo, fidInfo] = *info;
            result.info = spectrumInfo;
            std::ifstream fidFile{folderPath / "fid", std::ios::in | std::ios::binary};
            if (auto fids = readFidFile(fidFile, fidInfo)) {
                result.fids = *fids;    
                result.info.samplename = readTitle(folderPath);
                result.file_read_status = FileReadStatus::success_1D;
            } else {
                result.file_read_status = FileReadStatus::invalid_fid;
                return result;
            }
        } else {
            result.file_read_status = FileReadStatus::invalidAcqus;
            return result;
        }
    } else {
        result.file_read_status = FileReadStatus::invalidAcqus;
        return result;
    }



    return result;
}
