#ifndef AG_H
#define AG_H

#include"general.h"
#include"../spectrum/spectrum.h"

namespace FileIO
{
    FileReadResult ag_parse_experiment_folder(const std::filesystem::path& folder_path);
}

#endif // AG_H
