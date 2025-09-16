#include "general.h"
#include "ag.h"
#include "br.h"

FileType check_type(std::filesystem::path& folder)
{
    if (std::filesystem::exists(folder / "fid")){
        if (std::filesystem::exists(folder / "procpar")){
            return FileType::Ag;
        } else if (std::filesystem::exists(folder / "acqus")){
            return FileType::Br;
        }
    }
    return FileType::U;
}

FileReadResult open_experiment(std::filesystem::path& input_path)
{
    std::filesystem::path folder_path = input_path.parent_path();
    FileType type = check_type(folder_path);

    FileReadResult result;

    switch (type){
    case FileType::Ag:
        result = openExperimentAg(input_path);
        break;
    case FileType::Br:
        result = openExperimentBr(input_path);
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
