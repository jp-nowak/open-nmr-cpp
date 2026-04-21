#include "general.h"
#include "ag.h"
#include "br.h"
#include "i.h"

FileType check_type(const std::filesystem::path& file)
{
    using enum FileType;

    if (file.extension() == ".jdf") return I;

    if (file.filename() == "fid"){
        std::filesystem::path folder = file.parent_path();
        if (std::filesystem::exists(folder / "procpar")){
            return Ag;
        } else if (std::filesystem::exists(folder / "acqus")){
            return Br;
        }
    }
    return U;
}

ReadResult open_experiment_(const std::filesystem::path& inputPath)
{
    using enum FileType;

    switch (check_type(inputPath)) {
    case Ag: return openExperimentAg_(inputPath);
    case Br: return openExperimentBr_(inputPath);
    case I : return openExperimentI_(inputPath);
    case U : return std::unexpected(ReadError::unknownFormat);
    default: throw std::runtime_error("Unexpected switch value");
    }
}

