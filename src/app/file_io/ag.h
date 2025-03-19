#ifndef AG_H
#define AG_H

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
FileReadResult ag_parse_experiment_folder(const std::filesystem::path& folder_path);
}

#endif // AG_H
