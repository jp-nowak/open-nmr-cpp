#include "ag.h"

#include "file_io.h"
#include <bitset>
#include <optional>

using namespace IO;

namespace
{
constexpr size_t FILE_HEADER_SIZE = 32;
constexpr size_t BLOCK_HEADER_SIZE = 28;

//! data from file headers in Ag fid file
struct FileHeader
{
    int nblocks; // number of blocks in file, if nblocks == 1 then spectrum is 1D
    int ntraces; // number of fids in one block, havent seen it being biger then 1
    int np; // number of simple elements in one fid, equal twice the number of complex elements
    int ebytes; // bytes per simple element
    int tbytes; // np * ebytes
    int bbytes; // ntraces * tbytes + nbheaders * 28
    short vers_id; // version of VnmrJ which operated device
    std::bitset<16> status; // explained later
    int nbheaders; // number of block headers per block, 1 in case of simple experiments, 2 in 2D hypercomplex
};

//! creates FileHeader from bytes
FileHeader readFileHeader(const Buffer& buffer, size_t begin)
{
    return FileHeader{
        .nblocks   = beTo<int>(buffer, begin),
        .ntraces   = beTo<int>(buffer, begin + 4),
        .np        = beTo<int>(buffer, begin + 8),
        .ebytes    = beTo<int>(buffer, begin + 12),
        .tbytes    = beTo<int>(buffer, begin + 16),
        .bbytes    = beTo<int>(buffer, begin + 20),
        .vers_id   = beTo<short>(buffer, begin + 24),
        .status    = beTo<char16_t>(buffer, begin + 26),
        .nbheaders = beTo<int>(buffer, begin + 28),
    };
}

//! meanings of bits of FileHeader status
enum FHStatus
{
    S_Data = 0,
    S_Spec = 1,
    S_32 = 2,
    S_Float = 3,
    S_Complex = 4,
    S_Hyper = 5,
    S_Acqpar = 7,
    S_Secnd = 8,
    S_Transf = 9,
    S_NP = 11,
    S_NF = 12,
    S_NI = 13,
    S_NI2 = 14,
};

#ifndef NDEBUG
[[maybe_unused]] void printFileHeader(const FileHeader& f)
{
    printf("FileHeader: %p \n", (void*)&f);
    printf("nblocks: %d \n", f.nblocks);
    printf("ntraces: %d \n", f.ntraces);
    printf("np: %d \n", f.np);
    printf("ebytes: %d \n", f.ebytes);
    printf("tbytes: %d \n", f.tbytes);
    printf("bbytes: %d \n", f.bbytes);
    printf("vers_id: %d \n", f.vers_id);
    printf("status: %s \n", f.status.to_string().c_str());
    printf("\t S_Data: %d \n", f.status[S_Data]);
    printf("\t S_Spec: %d \n", f.status[S_Spec]);
    printf("\t S_32: %d \n", f.status[S_32]);
    printf("\t S_Float: %d \n", f.status[S_Float]);
    printf("\t S_Complex: %d \n", f.status[S_Complex]);
    printf("\t S_Hyper: %d \n", f.status[S_Hyper]);
    printf("\t S_Acqpar: %d \n", f.status[S_Acqpar]);
    printf("\t S_Secnd: %d \n", f.status[S_Secnd]);
    printf("\t S_Transf: %d \n", f.status[S_Transf]);
    printf("\t S_NP: %d \n", f.status[S_NP]);
    printf("\t S_NF: %d \n", f.status[S_NF]);
    printf("\t S_NI: %d \n", f.status[S_NI]);
    printf("\t S_NI2: %d \n", f.status[S_NI2]);
    printf("nbheaders: %d \n", f.nbheaders);
}
#endif

//! data from block header; seems not really relevant
struct BlockHeader
{
    short scale;
    std::bitset<16> status;
    short index;
    short mode;
    int ctcount;
    float lpval;
    float rpval;
    float lvl;
    float tlt;
};

//! creates BlockHeader from bytes
BlockHeader readBlockHeader(const Buffer& buffer, size_t begin)
{
    return BlockHeader{
        .scale   = beTo<short>(buffer, begin),
        .status  = beTo<char16_t>(buffer, begin + 2),
        .index   = beTo<short>(buffer, begin + 4),
        .mode    = beTo<short>(buffer, begin + 6),
        .ctcount = beTo<int>(buffer, begin + 8),
        .lpval   = beTo<float>(buffer, begin + 16),
        .rpval   = beTo<float>(buffer, begin + 20),
        .lvl     = beTo<float>(buffer, begin + 24),
        .tlt     = beTo<float>(buffer, begin + 28),
    };
}

enum BHStatus
{
    B_Data = 0,
    B_Spec = 1,
    B_32 = 2,
    B_Float = 3,
    B_Complex = 4,
    B_Hyper = 5,
    More_blocks = 7,
    NP_Cmplx = 8,
    NF_Cmplx = 9,
    NI_Cmplx = 10,
    NI2_Cmplx = 11,
};

#ifndef NDEBUG
[[maybe_unused]] void printBlockHeader(const BlockHeader& x)
{
    printf("BlockHeader: %p \n", (void*)&x);
    printf("scale: %d \n", x.scale);
    printf("status: %s \n", x.status.to_string().c_str());
    printf("\t B_Data: %d \n", x.status[B_Data]);
    printf("\t B_Spec: %d \n", x.status[B_Spec]);
    printf("\t B_32: %d \n", x.status[B_32]);
    printf("\t B_Float: %d \n", x.status[B_Float]);
    printf("\t B_Complex: %d \n", x.status[B_Complex]);
    printf("\t B_Hyper: %d \n", x.status[B_Hyper]);
    printf("\t More_blocks: %d \n", x.status[More_blocks]);
    printf("\t NP_Cmplx: %d \n", x.status[NP_Cmplx]);
    printf("\t NF_Cmplx: %d \n", x.status[NF_Cmplx]);
    printf("\t S_NPNI_Cmplx %d \n", x.status[NI_Cmplx]);
    printf("\t NI2_Cmplx: %d \n", x.status[NI2_Cmplx]);
    printf("index: %d \n", x.index);
    printf("mode: %d \n", x.mode);
    printf("ctcount: %d \n", x.ctcount);
    printf("lpval: %f \n", x.lpval);
    printf("rpval: %f \n", x.rpval);
    printf("lvl: %f \n", x.lvl);
    printf("tlt: %f \n", x.tlt);
}
#endif

struct HyperCmplxHeader
{
    short s_spare1l;
    std::bitset<16> status;
    short s_spare2;
    short s_spare3;
    int l_spare1;
    float lpval1;
    float rpval1;
    float f_spare1;
    float f_spare2;
};

HyperCmplxHeader readHyperCmplxHeader(const Buffer& buffer, size_t begin)
{
    return HyperCmplxHeader{
        .s_spare1l = beTo<short>(buffer, begin),
        .status    = beTo<char16_t>(buffer, begin + 2),
        .s_spare2  = beTo<short>(buffer, begin + 4),
        .s_spare3  = beTo<short>(buffer, begin + 6),
        .l_spare1  = beTo<int>(buffer, begin + 8),
        .lpval1    = beTo<float>(buffer, begin + 16),
        .rpval1    = beTo<float>(buffer, begin + 20),
        .f_spare1  = beTo<float>(buffer, begin + 24),
        .f_spare2  = beTo<float>(buffer, begin + 28),
    };
}

enum HCHStatus
{
    NP_PHmode = 0,
    NP_AVmode = 1,
    NP_PWRmode = 2,
    NF_PHmode = 4,
    NF_AVmode = 5,
    NF_PWRmode = 6,
    NI_PHmode = 8,
    NI_AVmode = 9,
    NI_PWRmode = 10,
    NI2_PHmode = 12,
    NI2_AVmode = 13,
    NI2_PWRmode = 14,

};

Vector<ComplexVector> readFidFile(const Buffer& buffer)
{
    size_t pos = 0;
    auto fileHeader = readFileHeader(buffer, pos);
    pos += FILE_HEADER_SIZE;
    DataType dataType;
    if (fileHeader.status[S_Float]) {
        dataType = DataType::float32;
    } else if (fileHeader.status[S_32]) {
        dataType = DataType::int32;
    } else {
        dataType = DataType::int16;
    }
    size_t cElemN = fileHeader.np / 2;
    int traceSize = fileHeader.np * dataTypeSize(dataType);

    if ((traceSize != fileHeader.tbytes)
    or fileHeader.nbheaders > 1
    or (static_cast<int>(dataTypeSize(dataType)) != fileHeader.ebytes)
    or not fileHeader.status[S_Data]
    or fileHeader.status[S_Spec]
    or fileHeader.status[S_Hyper]
    or not fileHeader.status[S_Acqpar]
    or fileHeader.status[S_Secnd]
    or fileHeader.status[S_Transf]
    or fileHeader.status[S_NP]
    or fileHeader.status[S_NF]
    or fileHeader.status[S_NI]
    or fileHeader.status[S_NI2]
    or fileHeader.status[S_Complex]
    ) return {};

    Vector<ComplexVector> fids{};

    for (int i = 0; i < fileHeader.nblocks; i++) {
        [[maybe_unused]] auto blockHeader = readBlockHeader(buffer, pos);
        pos += BLOCK_HEADER_SIZE;
        if (fileHeader.nbheaders == 2) {
            [[maybe_unused]] auto hyperHeader = readHyperCmplxHeader(buffer,pos);
            pos += BLOCK_HEADER_SIZE;
        } else if (fileHeader.nbheaders != 1) return {};
        for (int j = 0; j < fileHeader.ntraces; j++) {
            ComplexVector trace = readComplexArray(Endian::big, dataType, buffer, pos, cElemN);
            if (trace.empty()) return {};
            pos += traceSize;
            fids.push_back(trace);
        }
    }
    return fids;
}

#ifndef NDEBUG
//! reads all params from String containing procpar file
[[maybe_unused]] Dict readFullProcpar(const String& procpar)
{
    auto lines = SplitIterator<char>(procpar, "\n");
    Dict params;
    StringView name_;

    for (auto line : lines) {
        Vector<StringView> words = split(line, " ");
        if (xisDigit(words[0][0]) or xis<'"'>(words[0][0])) {
            if (xisNumber(words[0])) words[0] = {};
            if (not params.contains(name_)) {
                params.emplace(std::make_pair(String(name_), join(words, " ", &strip)));
            } else {
                params.find(name_)->second.append(join(words, " ", &strip));
            }
        } else {
            name_ = words[0];
        }
    }
    return params;
}
#endif

//! reads params necessary for processing from String containing procpar file
Dict readSelectProcpar(const String& procpar)
{
    Dict params{
        {"solvent", {}},
        {"samplename", {}},
        {"tn", {}},
        {"sw", {}},
        {"sfrq", {}},
        {"at", {}},
        {"reffrq", {}},
        {"np", {}},
        {"samplename", {}},
        {"tn", {}},
    };

    bool foundParam = false;
    StringView name_;

    for (auto line : SplitIterator<char>(procpar, "\n")) {
        StringView firstWord = getWord(line, 0, " ");
        if (foundParam) {
            if (xisDigit(firstWord[0]) or xis<'"'>(firstWord[0])) {
                Vector<StringView> words = split(line, " ");
                if (xisNumber(words[0])) words[0] = {};
                if (auto& paramValue = params.find(name_)->second; paramValue.empty()) {
                    paramValue = join(words, " ", &strip);
                } else {
                    paramValue.append(join(words, " ", &strip));
                }
            } else foundParam = false;
        }
        if (params.contains(firstWord)) {
            foundParam = true;
            name_ = firstWord;
        }
    }
    return params;
}

std::optional<SpectrumInfo> paramsToInfo(const Dict& params)
{
    double zero_freq, obs_nucleus_freq, spectral_width, elements_number, acquisition_time;
    // if conversions fail file is assumed to be corrupt and empty optional is returned
    try{
        zero_freq = std::stod(params.at("reffrq"));
        obs_nucleus_freq = std::stod(params.at("sfrq"));
        spectral_width  = std::stod(params.at("sw"));
        elements_number = std::stod(params.at("np")) / 2;
        acquisition_time = std::stod(params.at("at"));
    } catch (...) {
        return {};
    }

    if (not params.contains("samplename")
    or not params.contains("tn")
    or not params.contains("solvent")
    or obs_nucleus_freq == 0.0
    or elements_number == 0.0) return {};

    double irradiation_freq = std::fabs((zero_freq - obs_nucleus_freq) * 1000000);
    double plot_begin = irradiation_freq - spectral_width / 2;
    double plot_end = irradiation_freq + spectral_width / 2;

    return SpectrumInfo{
        .plot_right_Hz = plot_begin,
        .plot_left_Hz = plot_end,
        .plot_right_ppm = plot_begin / obs_nucleus_freq,
        .plot_left_ppm = plot_end / obs_nucleus_freq,
        .spectral_width = spectral_width,
        .acquisition_time = acquisition_time,
        .obs_nucleus_freq = obs_nucleus_freq,
        .dwell_time = acquisition_time / elements_number,
        .group_delay = 0.0,
        .trimmed = 0.0,
        .samplename = params.at("samplename"),
        .nucleus = params.at("tn"),
        .solvent = params.at("solvent")
    };
}

} // end of namespace

FileReadResult openExperimentAg(const std::filesystem::path& fidPath)
{
    FileReadResult result{.type = FileType::Ag};
    {
        Buffer buffer{};
        if (readFileTo(buffer, fidPath)) {
            result.fids = readFidFile(buffer);
        }
        if (result.fids.empty()) return FileReadResult{.status = ReadStatus::invalid_fid};
    }
    {
        String buffer{};
        auto procparPath = fidPath.parent_path() / "procpar";
        if (not std::filesystem::exists(procparPath)) return FileReadResult{.status = ReadStatus::invalid_procpar};
        if (not readFileTo(buffer, procparPath)) return FileReadResult{.status = ReadStatus::invalid_procpar};
        Dict params = readSelectProcpar(buffer);
        if (auto info = paramsToInfo(params); info) result.info = *info;
        else return FileReadResult{.status = ReadStatus::invalid_procpar};
    }
    result.status = (result.fids.size() > 1) ? ReadStatus::success_2D : ReadStatus::success_1D;
    return result;
}
