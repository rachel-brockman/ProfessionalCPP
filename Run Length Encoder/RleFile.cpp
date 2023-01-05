#include "RleFile.h"
#include "RleData.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

void RleFile::CreateArchive(const std::string& source)
{
    std::ifstream::pos_type size;
    char* memblock = nullptr;
    std::vector<int8_t> input;
    input.clear();
    // Open the file for input, in binary mode, and start ATE (at the end)
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg(); // Save the sie of the file
        memblock = new char[static_cast<unsigned int>(size)];
        file.seekg(0, std::ios::beg); // Seek back to start of file
        file.read(memblock, size);
        file.close();
        // File data is now in memblock array
        for(int i = 0; i < size; i++){
            input.push_back(memblock[i]);
        }
        //        std::cout << size << std::endl;
        //        input.insert(input.end(), &memblock[0], &memblock[sizeof(memblock)]);
        mRleData.Compress(input);
        // Make sure to clean up!
        delete[] memblock;
    }
    
    mHeader.mFileSize = static_cast<int>(size);
    mHeader.mFileNameLength = static_cast<unsigned char>(source.length());
    mHeader.mFileName = source;
    
    // Open the file for output, in binary mode, and overwrite an existing file
    std::ofstream arc(source + ".rl1", std::ios::out|std::ios::binary|std::ios::trunc);
    if (arc.is_open())
    {
        arc.write(mHeader.mSig, 4);
        arc.write(reinterpret_cast<char*>(&(mHeader.mFileSize)), 4);
        arc.write(reinterpret_cast<char*>(&(mHeader.mFileNameLength)), 1);
        arc.write(mHeader.mFileName.c_str(), static_cast<int>(mHeader.mFileNameLength));
        //This is objectively the issue
        for (int i = 0; i <mRleData.mData.size(); i++){
            arc.write(reinterpret_cast<char*>(&(mRleData.mData[i])), 1);
        }
        //        arc.write(reinterpret_cast<char*>(&(mRleData.mData)), mRleData.mData.size());
    }
}

void RleFile::ExtractArchive(const std::string& source)
{
    // Requires <fstream>
    std::ifstream::pos_type size;
    std::vector<int8_t> input;
    char* memblock = nullptr;
    // Open the file for input, in binary mode, and start ATE (at the end)
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg(); // Save the size of the file
        memblock = new char[static_cast<unsigned int>(size)];
        file.seekg(0, std::ios::beg); // Seek back to start of file
        file.read(memblock, size);
        file.close();
        // File data is now in memblock array
        //Get file size
        mHeader.mFileSize = *(reinterpret_cast<int*>(&memblock[4]));
        //Get the file name length
        mHeader.mFileNameLength = memblock[8];
        
        int endOfFileName = 9 + mHeader.mFileNameLength;
        char* fileName = new char[mHeader.mFileNameLength + 4];
        for(int i = 0; i< mHeader.mFileNameLength; i++){
            mHeader.mFileName += memblock[i+9];
        }
//        mHeader.mFileName = fileName;
        for(int i = endOfFileName; i < static_cast<int>(size); i++){
            input.push_back(memblock[i]);
        }
        mRleData.Decompress(input, (static_cast<int>(size)-(endOfFileName))*2);
        // Make sure to clean up!
        delete[] memblock;
        delete[] fileName;
    }
    // Open the file for output, in binary mode, and overwrite an existing file
    std::ofstream arc(mHeader.mFileName, std::ios::out|std::ios::binary|std::ios::trunc);
    if (arc.is_open())
    {
        for (int i = 0; i <mRleData.mData.size(); i++){
            arc.write(reinterpret_cast<char*>(&(mRleData.mData[i])), 1);
        }
    }
}
