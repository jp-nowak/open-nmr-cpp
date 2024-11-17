#ifndef FGENERAL_H
#define FGENERAL_H

#include "../spectrum/spectrum.h"

#include <vector>
#include <complex>
#include <filesystem>

namespace FileIO
{
    enum class FileReadStatus{success_1D, success_2D, unknown_format, invalid_fid, invalid_procpar};
    enum class FileType{Ag, Br, U};

    struct FileReadResult
    {
        FileReadStatus file_read_status;
        FileType file_type;
        SpectrumInfo info;
        std::vector<std::vector<std::complex<double>>> fids;

    };

    FileType check_type(std::filesystem::path& folder);
    FileReadResult open_experiment(std::filesystem::path& input_path);
}



#endif // GENERAL_H
