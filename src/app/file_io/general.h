#ifndef FGENERAL_H
#define FGENERAL_H

#include "../spectrum/spectrum_info.h"

#include <vector>
#include <complex>
#include <filesystem>

namespace FileIO
{
    /*!
     * \brief The FileReadStatus enum contains possible results of attempt of opening experiment
     */
    enum class FileReadStatus{unknown_failure, success_1D, success_2D, unknown_format, invalid_fid, invalid_procpar, invalidAcqus};

    /*!
     * \brief The FileType enum: types of experiment files
     */
    enum class FileType{U, Ag, Br};

    struct FileReadResult
    {
        FileReadStatus file_read_status{FileReadStatus::unknown_failure};
        FileType file_type{FileType::U};
        SpectrumInfo info{};
        std::vector<std::vector<std::complex<double>>> fids;

    };

    enum class DataType{unknown, short16, int32, float32, double64};

    size_t dataTypeSize(DataType t);
    FileType check_type(std::filesystem::path& folder);
    FileReadResult open_experiment(std::filesystem::path& input_path);
}



#endif // GENERAL_H
