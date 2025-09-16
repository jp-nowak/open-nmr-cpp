#ifndef FGENERAL_H
#define FGENERAL_H

#include "../spectrum/spectrum_info.h"

#include <vector>
#include <complex>
#include <filesystem>


/*!
* \brief The ReadStatus enum contains possible results of attempt of opening experiment
*/
enum class ReadStatus{unknown_failure, success_1D, success_2D, unknown_format, invalid_fid, invalid_procpar, invalidAcqus};

/*!
* \brief The FileType enum: types of experiment files
*/
enum class FileType{U, Ag, Br};

struct FileReadResult
{
    ReadStatus status{ReadStatus::unknown_failure};
    FileType type{FileType::U};
    SpectrumInfo info{};
    std::vector<std::vector<std::complex<double>>> fids{};

};

FileType check_type(std::filesystem::path& folder);
FileReadResult open_experiment(std::filesystem::path& input_path);




#endif // GENERAL_H
