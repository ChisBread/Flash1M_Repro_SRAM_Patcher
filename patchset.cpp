#include "patchset.h"
#include <algorithm>
#include <iostream>


void PatchSet::addPatch(const std::vector<unsigned char>& marker,
                        const std::vector<unsigned char>& replace,
                        const std::vector<bool>& maskMarker,
                        const std::vector<bool>& maskReplace )
{
    Patch patch = {marker, replace, maskMarker, maskReplace};

    this->patches.push_back(patch);
}


bool PatchSet::isApplicable(std::vector<unsigned char> &data, std::string* applicableName)
{
    for (std::string& name : this->names)
    {
        std::vector< unsigned char > pattern;
        for(unsigned char character : name)
        {
            pattern.push_back(character);
        }

        auto it = std::search(std::begin(data), std::end(data), std::begin(pattern), std::end(pattern));

        if (std::end(data) != it)
        {
            *applicableName = name;
            return true;
        }
    }

    return false;

}

int64_t PatchSet::findIndex(std::vector<unsigned char>* data, Patch* patch)
{
    bool useMaskBit = (patch->MaskMarker.size() == (patch->Marker.size()));

    int64_t index = -1;

    if (data->size() > patch->Marker.size())
    {
        uint64_t data_size = data->size();
        uint64_t marker_size = patch->Marker.size();

        uint64_t data_idx = marker_size - 1;
        uint64_t marker_idx = marker_size - 1;

        do {
            if (((*data)[data_idx] == patch->Marker[marker_idx])
                    || (useMaskBit && patch->MaskMarker[marker_idx]))
            {
                if (marker_idx == 0) {
                    index = data_idx;
                    break;
                }
                else
                {
                    data_idx--;
                    marker_idx--;
                }
            }
            else
            {
                uint64_t lastMatch_idx = 0U;

                for (lastMatch_idx = marker_size - 1; lastMatch_idx > 0; lastMatch_idx--)
                {
                    if ((patch->Marker[lastMatch_idx] == (*data)[data_idx])
                            || (useMaskBit && patch->MaskMarker[lastMatch_idx]))
                    {
                        break;
                    }
                }
                data_idx += marker_size - std::min(marker_idx, lastMatch_idx + 1);
                marker_idx = marker_size - 1;
            }

        } while (data_idx < data_size);
    }

    return index;
}

std::string toHex(unsigned char c)
{
    std::string hex = "0123456789ABCDEF";
    std::string result = "";
    result += hex[c >> 4];
    result += hex[c & 0x0F];
    return result;
}

void PatchSet::applyPatches(std::vector<unsigned char>* data)
{
    uint8_t index = 0U;
    for (Patch pair : this->patches)
    {
        int64_t index = this->findIndex(data, &pair);

        if ((index > 0) && ((data->size() - index) >= pair.Replace.size()))
        {
            for (auto replace_byte : pair.Replace)
            {
                (*data)[index] = replace_byte;
                index++;
            }
            std::string successmsg = "Patch applied successfully.\nPattern";
            for (auto byte : pair.Marker)
            {
                successmsg += " 0x" + toHex(byte);
            }
            successmsg += "\nReplace";
            for (auto byte : pair.Replace)
            {
                successmsg += " 0x" + toHex(byte);
            }
            std::cerr << successmsg << std::endl << std::endl;

        }
        else
        {
            std::string errormsg = "One of the patches could not be applied. Aborting.\nPattern";
            for (auto byte : pair.Marker)
            {
                errormsg += " 0x" + toHex(byte);
            }
            errormsg += "\nReplace";
            for (auto byte : pair.Replace)
            {
                errormsg += " 0x" + toHex(byte);
            }
            throw errormsg;
        }

    }
}
