#ifndef BR_H
#define BR_H

#include"general.h"
#include"../spectrum/spectrum.h"

namespace FileIO
{
/*!
     * \brief FileIO::ag_parse_experiment_folder function that handles opening and reading of Ag experiment folder
     * containing fid and procpar files
     * \param folder_path
     * \return FileReadResult, containing extracted informations. If error occured FileReadResult.file_read_status is set to
     * corresponding error enum.
     */
FileReadResult brParseExperimentFolder(const std::filesystem::path& folderPath);
}



#endif // BR_H
