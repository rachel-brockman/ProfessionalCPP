#include "SrcMain.h"
#include <string>
#include "RleFile.h"

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Process argc and argv, and use to compress/decompress files
    if(argc == 2){
        RleFile inputFile;
        std::string fileName = argv[1];
        std::string fileExtension = fileName.substr(fileName.length() - 3);
        if(fileExtension == "rl1"){
            inputFile.ExtractArchive(fileName);
        }
        else{
            inputFile.CreateArchive(fileName);
        }
    }
}
