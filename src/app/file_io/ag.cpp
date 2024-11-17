#include "ag.h"

#include "big_endian.h"
#include "string_operations.h"
#include "general.h"
#include "../spectrum/spectrum.h"

#include <fstream>
#include <vector>
#include <complex>
#include <cstddef>
#include <filesystem>
#include <bitset>
#include <cstring>
#include <string>
#include <stdexcept>
#include <optional>

#include <unordered_map>

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
        int bbytes; // ntraces * tbytes + nbheaders*28
        short vers_id; // version of VnmrJ which operated device
        std::bitset<16> status; // explained later
        int nbheaders; // number of block headers per block, 1 in case of simple experiments, 2 in 2D hypercomplex
    };

    //! data from block header; seems not really relevant, will be cut off in release versions
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

    /*!
     * @brief read_file_header creates FileHeader out of vector of big endian bytes
     * @param buffer vector containing big endian bytes corresponding to header, read from fid file of Ag type
     * @param begin position from which reading of header starts
     * @return FileHeader struct with information from file header
     */
    FileHeader read_file_header(const std::vector<std::byte>& buffer, size_t begin)
    {
        using namespace BigEndian;
        return FileHeader{
        .nblocks   = to_int(buffer, begin + 0),
        .ntraces   = to_int(buffer, begin + 4),
        .np        = to_int(buffer, begin + 8),
        .ebytes    = to_int(buffer, begin + 12),
        .tbytes    = to_int(buffer, begin + 16),
        .bbytes    = to_int(buffer, begin + 20),
        .vers_id   = to_short(buffer, begin + 24),
        .status    = std::bitset<16>(to_short(buffer, begin+26)),
        .nbheaders = to_int(buffer, begin + 28)
        };
    }

    /*!
     * @brief read_block_header creates BlockHeader out of vector of big endian bytes, will be cut off in release
     * version as those informations doesnt seem important
     * @param buffer vector containing big endian bytes corresponding to header, read from fid file of Ag type
     * @param begin position from which reading of header starts
     * @return BlockHeader struct with information from file header
     */
    BlockHeader read_block_header(const std::vector<std::byte>& buffer, size_t begin)
    {
        using namespace BigEndian;
        return BlockHeader{
        .scale   = to_short(buffer, begin + 0),
        .status  = to_short(buffer, begin + 2),
        .index   = to_short(buffer, begin + 4),
        .mode    = to_short(buffer, begin + 6),
        .ctcount = to_int(buffer, begin + 8),
        .lpval   = to_float(buffer, begin + 12),
        .rpval   = to_float(buffer, begin + 16),
        .lvl     = to_float(buffer, begin + 20),
        .tlt     = to_float(buffer, begin + 24)
        };
    }

    std::optional<SpectrumInfo> parse_procpar(std::ifstream& procpar_file)
    {
        std::unordered_map<std::string, std::string> params{
            {"solvent", {}},
            {"samplename", {}},
            {"tn", {}},
            {"sw", {}},
            {"sfrq", {}},
            {"at", {}},
            {"reffrq", {}},
            {"np", {}}
        };

        std::string current_line{};
        std::vector<std::string> words{};
        bool found_param = false;
        std::string current_key{};

        while(std::getline(procpar_file, current_line)) {
            words = StringOperations::split(current_line, " ");
            if (found_param) {
                params[current_key] = StringOperations::remove_chars(words[1], R"(")");
                found_param = false;
            } else {
                if (params.count(words[0])) {
                    found_param = true;
                    current_key = words[0];
                }
            }

        }

        double zero_freq{};
        double obs_nucleus_freq{};
        double spectral_width{};
        double elements_number{};
        double acquisition_time{};

        try{
            zero_freq = std::stod(params["reffrq"]);
            obs_nucleus_freq = std::stod(params["sfrq"]);
            spectral_width  = std::stod(params["sw"]);
            elements_number = std::stod(params["np"]) / 2;
            acquisition_time = std::stod(params["at"]);
        } catch (const std::invalid_argument& e) {
            return {};
        } catch(const std::out_of_range& e) {
            return {};
        }

        double irradiation_freq = (zero_freq - obs_nucleus_freq) * -1000000;
        double plot_begin = irradiation_freq - spectral_width / 2;
        double plot_end = irradiation_freq + spectral_width / 2;

        SpectrumInfo info{
        .plot_begin_Hz = plot_begin,
        .plot_end_Hz = plot_end,
        .plot_begin_ppm = plot_begin / obs_nucleus_freq,
        .plot_end_ppm = plot_end / obs_nucleus_freq,
        .spectral_width = spectral_width,
        .acquisition_time = acquisition_time,
        .obs_nucleus_freq = obs_nucleus_freq,
        .dwell_time = acquisition_time / elements_number,
        .group_delay = 0.0,
        .trimmed = 0.0,
        };

        return info;
    }

    std::vector<std::vector<std::complex<double>>> read_fid_file(std::ifstream& fid_file)
    {
        std::vector<std::byte> buffer(FILE_HEADER_SIZE, std::byte{0});
        fid_file.read(reinterpret_cast<char*>(buffer.data()), FILE_HEADER_SIZE);
        FileHeader file_header = read_file_header(buffer, 0);
        enum class DataType {short16, int32, float32};
        DataType data_type{};
        if (file_header.status[3]){
            data_type = DataType::float32;
        } else if (file_header.status[2]){
            data_type = DataType::int32;
        } else {
            data_type = DataType::short16;
        }

        std::vector<std::vector<std::complex<double>>> fids{};

        const size_t fid_array_byte_size = file_header.np * file_header.ebytes;

        for (size_t i = 0; i < file_header.nblocks; i++) {
            buffer.resize(BLOCK_HEADER_SIZE);
            fid_file.read(reinterpret_cast<char*>(buffer.data()), BLOCK_HEADER_SIZE);
            BlockHeader block_header = read_block_header(buffer, 0);
            if (file_header.nbheaders == 2) {
                buffer.resize(BLOCK_HEADER_SIZE);
                fid_file.read(reinterpret_cast<char*>(buffer.data()), BLOCK_HEADER_SIZE);
                BlockHeader additional_block_header = read_block_header(buffer, 0);
            }
            std::vector<std::complex<double>> fid{};
            buffer.resize(fid_array_byte_size);
            fid_file.read(reinterpret_cast<char*>(buffer.data()), fid_array_byte_size);

            switch (data_type) {
            case DataType::float32:
                fid = BigEndian::read_fid_array_float(buffer);
                break;
            case DataType::int32:
                fid = BigEndian::read_fid_array_int(buffer);
                break;
            case DataType::short16:
                fid = BigEndian::read_fid_array_short(buffer);
                break;
            }
            fids.push_back(fid);
        }
        return fids;
    }


} // end of namespace

FileIO::FileReadResult FileIO::ag_parse_experiment_folder(const std::filesystem::path& folder_path)
{
    std::ifstream fid_file{folder_path / "fid", std::ios::in | std::ios::binary};
    std::ifstream procpar_file{folder_path / "procpar", std::ios::in};

    FileReadResult result{};

    if (fid_file) {
        result.fids = read_fid_file(fid_file);
    }
    if (procpar_file) {
        std::optional<SpectrumInfo> info = parse_procpar(procpar_file);
        if (info) {
            result.info = info.value();
        } else {
            result.file_read_status = FileReadStatus::invalid_procpar;
        }
    }
    result.file_read_status = FileReadStatus::success_1D;

    return result;
}

