#include "general.h"
#include "ag.h"
#include "br.h"

using namespace FileIO;

FileType FileIO::check_type(std::filesystem::path& folder)
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

size_t FileIO::dataTypeSize(DataType t)
{
    using enum DataType;
    switch (t) {
    case unknown:
        return 0;
        break;
    case short16:
        return 2;
        break;
    case int32:
        return 4;
        break;
    case float32:
        return 4;
        break;
    case double64:
        return 8;
        break;
    default:
        return 0;
    }
}


FileReadResult FileIO::open_experiment(std::filesystem::path& input_path)
{
    std::filesystem::path folder_path = input_path.parent_path();
    FileType file_type = check_type(folder_path);

    FileReadResult result;

    switch (file_type){
    case FileType::Ag:
        result = ag_parse_experiment_folder(folder_path);
        break;
    case FileType::Br:
        result = brParseExperimentFolder(folder_path);
        break;
    default:
        result.file_read_status = FileReadStatus::unknown_format;
        break;
    }

#ifdef DEBUG__
    qDebug() << result.info;
#endif

    if (result.info.samplename.empty()) {
        result.info.samplename = result.info.nucleus + " experiment";
    }

    return result;
}
