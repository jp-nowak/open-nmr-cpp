#ifndef FGENERAL_H
#define FGENERAL_H

#include "../spectrum/spectrum_info.h"
#include "../spectrum/value_typedefs.h"

#include <vector>
#include <complex>
#include <filesystem>
#include <expected>


/*!
* \brief The ReadStatus enum contains possible results of attempt of opening experiment
*/
enum class ReadStatus{unknown_failure, success_1D, success_2D, unknown_format, invalid_fid, invalid_procpar, invalidAcqus, invalidJDF};

/*!
* \brief The FileType enum: types of experiment files
*/
enum class FileType{U, Ag, Br, I};

enum class ReadError{unknownFailure, unknownFormat, noParamsFile, invalidFid, invalidProcpar, invalidAcqus, invalidJDF};

struct NMRExperiment
{
    SpectrumInfo info;
    Vector<ComplexVector> fids;
};

using ReadResult = std::expected<NMRExperiment, ReadError>;

struct FileReadResult
{
    ReadStatus status{ReadStatus::unknown_failure};
    FileType type{FileType::U};
    SpectrumInfo info{};
    std::vector<std::vector<std::complex<double>>> fids{};
};

FileType check_type(const std::filesystem::path& folder);
FileReadResult open_experiment(const std::filesystem::path& input_path);

ReadResult open_experiment_(const std::filesystem::path& input_path);

#endif // GENERAL_H
