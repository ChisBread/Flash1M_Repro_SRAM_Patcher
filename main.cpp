#include "flash1m.h"
#include "flash512.h"
#include "flash.h"
#include "eeprom.h"

#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>

#define HEX_BASE ( 16 )

std::vector<PatchSet> patchsets;

static void init()
{
    patchsets.push_back(*(new FLASH512()));

    patchsets.push_back(*(new FLASH1M_V102()));

    patchsets.push_back(*(new FLASH1M_V103()));

    patchsets.push_back(*(new EEPROM()));

    patchsets.push_back(*(new EEPROM_V124()));

    patchsets.push_back(*(new EEPROM_V126));

    patchsets.push_back(*(new FLASH_1()));

    patchsets.push_back(*(new FLASH_2()));

    patchsets.push_back(*(new FLASH_3()));
}

#ifdef _WIN32
#include <windows.h>
int ReadFile(const std::string& filename, std::vector<unsigned char>& buffer)
{
    HANDLE hFile = CreateFile(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    DWORD dwFileSize = GetFileSize(hFile, NULL);
    buffer.resize(dwFileSize);

    DWORD dwBytesRead;
    if (!ReadFile(hFile, buffer.data(), dwFileSize, &dwBytesRead, NULL))
    {
        CloseHandle(hFile);
        return -1;
    }

    CloseHandle(hFile);
    return 0;
}

int WriteFile(const std::string& filename, const std::vector<unsigned char>& buffer)
{
    HANDLE hFile = CreateFile(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    DWORD dwBytesWritten;
    if (!WriteFile(hFile, buffer.data(), buffer.size(), &dwBytesWritten, NULL))
    {
        CloseHandle(hFile);
        return -1;
    }

    CloseHandle(hFile);
    return 0;
}
#endif // WIN32


int main(int argc, char* argv[])
{
    std::string filepath = argv[1];
    init();
    int retCode;
#ifndef _WIN32
    if (argc < 2)
    {
        std::cout << "Error: Use Filename as argument!" << std::endl;
        return 1;
    }
    else if (!std::filesystem::exists(filepath))
    {
        std::cout << "Error: File does not exist! filepath: " << filepath << " current path: " << std::filesystem::current_path() << std::endl;
        return 1;
    }
    std::ifstream input( filepath, std::ios::binary );

    std::vector<unsigned char> file_buffer(std::istreambuf_iterator<char>(input), {});
#else
    std::vector<unsigned char> file_buffer;
    int ret = ReadFile(filepath, file_buffer);
    if (ret != 0)
    {
        std::cout << "Error: Could not read file!" << std::endl;
        return 1;
    }
#endif // WIN32

    bool applied = false;

    try {

        std::string applicableName;
        for (PatchSet patchset : patchsets)
        {
            if (patchset.isApplicable(file_buffer, &applicableName))
            {
                std::cout << "Applying patches for " << applicableName << std::endl;
                patchset.applyPatches(&file_buffer);
                applied = true;
                break;
            }
        }

        if (!applied)
        {
            throw std::string("Error: Could not find applicable patchset.");
        }

        std::filesystem::path path(filepath);

        std::stringstream ss;
        std::string split = "/";
        if ((path.parent_path().string()!= "" || path.parent_path().string() != ".") && path.string().find(split) == std::string::npos)
        {
            split = "\\";
        }
        if (applicableName.find("FLASH1M") != std::string::npos)
        {
            ss << path.parent_path().string() << split + "[1MSRAM]" << path.filename().string();
        }
        else 
        {

            ss << path.parent_path().string() << split + "[SRAM]" << path.filename().string();
        }
        std::string outputfilename = ss.str();

        std::cout << "Success! Wrote " << outputfilename << std::endl;
#ifndef _WIN32
        std::ofstream output(outputfilename, std::ios::out | std::ios::binary);
        output.write((char*) file_buffer.data(), file_buffer.size());
        output.close();
#else
        std::string winfilename = filepath;
        for (int i = filepath.size() - 1; i >= 0; i--)
        {
            if (filepath[i] == '/' || filepath[i] == '\\')
            {
                winfilename = filepath.substr(0, i + 1);
                if (applicableName.find("FLASH1M") != std::string::npos)
                {
                    winfilename += "[1MSRAM]";
                }
                else
                {
                    winfilename += "[SRAM]";
                }
                winfilename += filepath.substr(i + 1);
                break;
            }
        }
        ret = WriteFile(winfilename, file_buffer);
        if (ret != 0)
        {
            std::cout << "Error: Could not write file!" << std::endl;
            return 1;
        }
#endif // WIN32
        retCode = 0;

    } catch (std::string& message) {
        std::cout << message << std::endl;
        retCode = 2;
    }
    return retCode;
}
