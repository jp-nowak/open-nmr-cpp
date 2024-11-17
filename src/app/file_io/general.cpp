#include "general.h"
#include "ag.h"

FileIO::FileType FileIO::check_type(std::filesystem::path& folder)
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

FileIO::FileReadResult FileIO::open_experiment(std::filesystem::path& input_path)
{
    std::filesystem::path folder_path = input_path.parent_path();
    FileType file_type = check_type(folder_path);

    FileReadResult result{};

    switch (file_type){
    case FileType::Ag:
        result = ag_parse_experiment_folder(folder_path);
        break;

    default:
        result.file_read_status = FileReadStatus::unknown_format;
        break;
    }

    return result;
}
