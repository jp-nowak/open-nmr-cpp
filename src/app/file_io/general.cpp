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

FileReadResult open_experiment(const std::filesystem::path& input_path)
{
    using enum FileType;
    FileType type = check_type(input_path);

    FileReadResult result;

    switch (type){
    case Ag:
        result = openExperimentAg(input_path);
        break;
    case Br:
        result = openExperimentBr(input_path);
        break;
    case I:
        result = openExperimentI(input_path);
        break;
    default:
        result.status = ReadStatus::unknown_format;
        break;
    }

    if (result.info.samplename.empty()) {
        result.info.samplename = result.info.nucleus + " experiment";
    }

    return result;
}
