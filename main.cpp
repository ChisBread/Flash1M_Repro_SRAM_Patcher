#include "flashid.h"

#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <string>
#include <filesystem>


int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Error: Use Filename as argument!" << std::endl;
        return 1;
    }

    const std::filesystem::path filepath(argv[1]);
    if (!std::filesystem::exists(filepath))
    {
        std::cout << "Error: File does not exist! filepath: " << filepath.string()
                  << " current path: " << std::filesystem::current_path().string() << std::endl;
        return 1;
    }

    std::ifstream input(filepath, std::ios::binary);
    if (!input)
    {
        std::cout << "Error: Could not read file!" << std::endl;
        return 1;
    }

    std::vector<unsigned char> file_buffer(std::istreambuf_iterator<char>(input), {});
    std::vector<PatchSet> patchsets = createFlashIdBypassPatchsets();

    bool applied = false;
    int retCode = 0;

    try {

        std::string applicableName;
        for (PatchSet& patchset : patchsets)
        {
            if (patchset.isApplicable(file_buffer, &applicableName))
            {
                std::cout << "Applying Flash ID bypass for " << applicableName << std::endl;
                patchset.applyPatches(&file_buffer);
                applied = true;
                break;
            }
        }

        if (!applied)
        {
            throw std::string("Error: Could not find applicable Flash ID bypass patchset.");
        }

        const std::filesystem::path outputPath = filepath.parent_path() / ("[FLASHID]" + filepath.filename().string());
        std::ofstream output(outputPath, std::ios::out | std::ios::binary);
        if (!output)
        {
            throw std::string("Error: Could not write file!");
        }
        output.write(reinterpret_cast<const char*>(file_buffer.data()), file_buffer.size());
        output.close();

        if (!output)
        {
            throw std::string("Error: Could not write file!");
        }

        std::cout << "Success! Wrote " << outputPath.string() << std::endl;
        retCode = 0;

    } catch (const std::string& message) {
        std::cout << message << std::endl;
        retCode = 2;
    }
    return retCode;
}
