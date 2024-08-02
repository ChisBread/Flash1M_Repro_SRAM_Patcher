#include <filesystem>
#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <algorithm>


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
#endif // WIN32


std::vector<std::string> types;

static void init()
{
    types = std::vector<std::string> ({"FLASH1M_V102",
                                       "FLASH1M_V103",
                                       "FLASH512",
                                       "EEPROM_V111",
                                       "EEPROM_V120",
                                       "EEPROM_V121",
                                       "EEPROM_V122",
                                       "EEPROM_V124",
                                       "EEPROM_V126",
                                       "FLASH_V120",
                                       "FLASH_V121",
                                       "FLASH_V123",
                                       "FLASH_V124",
                                       "FLASH_V125",
                                       "FLASH_V126",
                                       "SRAM"});


}

int main(int argc, char* argv[])
{
    init();
    int retCode;
    std::string filepath = argv[1];
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

    std::string type = "None";
    for (std::string& name : types)
    {
        std::vector< unsigned char > pattern;
        for(unsigned char character : name)
        {
            pattern.push_back(character);
        }

        auto it = std::search(std::begin(file_buffer), std::end(file_buffer), std::begin(pattern), std::end(pattern));

        if (std::end(file_buffer) != it)
        {
            type = name;
            break;
        }
    }
    std::cout << type << std::endl;

    return retCode;
}
