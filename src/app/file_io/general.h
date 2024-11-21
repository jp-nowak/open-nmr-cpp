#ifndef FGENERAL_H
#define FGENERAL_H

#include "../spectrum/spectrum.h"

#include <vector>
#include <complex>
#include <filesystem>

namespace FileIO
{
    /*!
     * \brief The FileReadStatus enum contains possible results of attempt of opening experiment
     */
    enum class FileReadStatus{unknown_failure, success_1D, success_2D, unknown_format, invalid_fid, invalid_procpar};

    /*!
     * \brief The FileType enum: types of experiment files
     */
    enum class FileType{U, Ag, Br};

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
