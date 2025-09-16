#include "br.h"

#include "general.h"
#include "../spectrum/spectrum_info.h"

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

#include "file_io.h"

using namespace IO;

namespace
{

struct FidInfo {
    Endian endian;
    DataType type; // type of simple element
    size_t elemN; // number of complex elements
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
    } catch (...) {
        return {};
    }
    if (groupDelay == 0.0) return {};
    return groupDelay;
} // end of namespace

#ifndef NDEBUG
[[maybe_unused]] Dict readFullAcqus(const String& acqusFile)
{
    Dict params{};
    StringView currentParam;

    for (auto line : SplitIterator<char>(acqusFile, "\n")) {
        if (xis<'#'>(line[0])) {
            StringView currentParam = strip(getWord(line, 0, " "), "#=");
            params.emplace(std::make_pair(String(currentParam), strip(line.substr(line.find(" ") + 1), R"("'<> )""\n\t\r")));
        } else {
            params.find(currentParam)->second.append(line);
        }
    }
    return params;
}
#endif

Dict readSelectAcqus(const String& acqusFile)
{
    Dict params{
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
        {"$DECIM", {}}, // value needed to calc group delay if it is not stated in $GRPDLY
        {"$DSPFVS", {}}, // value needed to calc group delay if it is not stated in $GRPDLY
    };

    StringView currentParam;
    bool hasParam = false;

    for (auto line : SplitIterator<char>(acqusFile, "\n")) {
        if (xis<'#'>(line[0])) {
            StringView currentParam = strip(getWord(line, 0, " "), "#=");
            if (auto element = params.find(currentParam); element != params.end()) {
                element->second = strip(line.substr(line.find(" ") + 1), R"("'<> )""\n\t\r");
                hasParam = true;
            } else hasParam = false;
        } else if (hasParam) {
            params.find(currentParam)->second.append(line);
        }
    }
    return params;
}

std::optional<std::pair<SpectrumInfo, FidInfo>> paramsToInfo(const Dict& params)
{

    // establishing group delay
    double groupDelay;
    bool validGroupDelay = false;
    try{
        groupDelay = std::stod(params.at("$GRPDLY"));
        validGroupDelay = (groupDelay < 0.0) ? false : true;
    } catch (...) {validGroupDelay = false;}

    if (not validGroupDelay) {
        int decim, dspfvs;
        try{
            decim = std::stoi(params.at("$DECIM"));
            dspfvs = std::stoi(params.at("$DSPFVS"));
        } catch(...) {return {};}
        if (std::optional<double> gd = checkGroupDelay(decim, dspfvs)) {
            groupDelay = *gd;
        } else return {};
    }

    double spectrumCenter, spectralWidthHz, spectralWidthPpm, observedNucleusFreq,
           irradiationFreq, dwellTime;
    int elementsNumber;
    bool bigEndian;

    // if conversions fail file is assumed to be corrupt and empty optional is returned
    try{
        spectrumCenter = std::stod(params.at("$O1"));
        spectralWidthHz = std::stod(params.at("$SW_h"));
        spectralWidthPpm = std::stod(params.at("$SW"));
        observedNucleusFreq = std::stod(params.at("$BF1"));
        elementsNumber = std::stoi(params.at("$TD")) / 2;
        irradiationFreq = std::stod(params.at("$SFO1"));
        bigEndian = static_cast<bool>(std::stoi(params.at("$BYTORDA")));
    } catch (const std::invalid_argument& e) {
        return {};
    } catch(const std::out_of_range& e) {
        return {};
    }

    dwellTime = 1 / spectralWidthPpm / irradiationFreq; // time between data points

    size_t elemN;

    if (elementsNumber > 0) {
        elemN = static_cast<size_t>(elementsNumber);
    } else {
        return {};
    }

    DataType dataType;

    if (params.at("$DTYPA") == "0") {
        dataType = DataType::int32;
    } else if (params.at("$DTYPA") == "2") {
        dataType = DataType::float64;
    } else {
        return {};
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
        .samplename = {}, // read later from other file
        .nucleus = params.at("$NUC1"),
        .solvent = params.at("$SOLVENT")
    };

    FidInfo fidInfo{
        .endian = (bigEndian) ? Endian::big : Endian::little,
        .type = dataType,
        .elemN = elemN,
    };

    return std::make_pair(info, fidInfo);
}

std::optional<std::pair<SpectrumInfo, FidInfo>> parseAcqus(const String& acqus)
{
    Dict params = readSelectAcqus(acqus);
    return paramsToInfo(params);
}

Vector<ComplexVector> readFid(const Buffer& file, const FidInfo& info)
{
    const size_t fidSize =  info.elemN * dataTypeSize(info.type) * 2;
    constexpr size_t fidN = 1;
    if (fidSize * fidN > file.size()) return {};
    Vector<ComplexVector> fids{};
    for (size_t i = 0; i < fidN; i++) {
        fids.push_back(readComplexArray(info.endian, info.type, file, i * fidSize, info.elemN));
        if (fids[i].empty()) return {};
    }
    return fids;
}

std::string findAndReadTitle(const std::filesystem::path& folderPath)
{
    if (not std::filesystem::exists(folderPath / "pdata")) return {};
    auto pdataIter = std::filesystem::recursive_directory_iterator{folderPath / "pdata"};

    for (auto& i : pdataIter) {
        if (i.is_regular_file() && i.path().filename() == "title") {
            std::string title;
            if (not readFileTo(title, i.path())) return {};
            removeCharsInPlace(title, "\r");
            std::replace(title.begin(), title.end(), '\n', ' ');
            return title;
        }
    }

    return {};
}

} // end of namespace

FileReadResult openExperimentBr(const std::filesystem::path& fidPath)
{
    FileReadResult r{.type = FileType::Br};
    FidInfo fidInfo;
    { // reading acqus ant title
        String string;
        if (not readFileTo(string, fidPath.parent_path() / "acqus")) {r.status = ReadStatus::invalidAcqus; return r;}
        if (auto i = parseAcqus(string); i) std::tie(r.info, fidInfo) = *i;
        else {r.status = ReadStatus::invalidAcqus; return r;}
        r.info.samplename = findAndReadTitle(fidPath.parent_path());
    }
    { // reading fid
        Buffer buffer;
        if (not readFileTo(buffer, fidPath)) {r.status = ReadStatus::invalid_fid; return r;}
        r.fids = readFid(buffer, fidInfo);
        if (r.fids.empty()){ r.status = ReadStatus::invalid_fid; return r;}
    }
    r.status = ReadStatus::success_1D;
    return r;
}
