#ifndef FGENERAL_H
#define FGENERAL_H

#include "../spectrum/spectrum_info.h"
#include "../spectrum/value_typedefs.h"

#include <vector>
#include <complex>
#include <filesystem>
#include <expected>


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

FileType check_type(const std::filesystem::path& folder);

ReadResult open_experiment_(const std::filesystem::path& input_path);

#endif // GENERAL_H
