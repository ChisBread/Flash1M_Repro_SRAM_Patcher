#include "flashid.h"
#include "Patterns.h"

#include <cstddef>

namespace
{
template <std::size_t N>
std::vector<unsigned char> bytes(const unsigned char (&data)[N])
{
    return std::vector<unsigned char>(data, data + N);
}
}

std::vector<PatchSet> createFlashIdBypassPatchsets()
{
    std::vector<PatchSet> patchsets;

    PatchSet flash1mV102("FLASH1M_V102");
    flash1mV102.addPatch(bytes(FLASH1M_V102_MARKER_3), bytes(FLASH1M_V102_REPLACE_3));
    patchsets.push_back(flash1mV102);

    PatchSet flash1mV103("FLASH1M_V103");
    flash1mV103.addPatch(bytes(FLASH1M_V103_MARKER_3), bytes(FLASH1M_V103_REPLACE_3));
    patchsets.push_back(flash1mV103);

    PatchSet flash512("FLASH512");
    flash512.addPatch(std::vector<unsigned char> {(0xff),(0xf7),(0x88),(0xfd),(0x00),(0x04),(0x03),(0x0c),
                                                  (0x03),(0x4a),(0x01),(0x24)},
                      std::vector<unsigned char> {(0xff),(0xf7),(0x88),(0xfd),(0x00),(0x04),(0x03),(0x0c),
                                                  (0x03),(0x4a),(0x00),(0x24)});
    patchsets.push_back(flash512);

    PatchSet flashV120(std::vector<std::string>({"FLASH_V120", "FLASH_V121"}));
    flashV120.addPatch(std::vector<unsigned char> {(0x90),(0xb5),(0x93),(0xb0),(0x6f),(0x46),(0x39),(0x1d),
                                                   (0x08),(0x1c),(0x00),(0xf0)},
                       std::vector<unsigned char> {(0x00),(0xb5),(0x3d),(0x20),(0x00),(0x02),(0x1f),(0x21),
                                                   (0x08),(0x43),(0x02),(0xbc),(0x08),(0x47)});
    patchsets.push_back(flashV120);

    PatchSet flashV123(std::vector<std::string>({"FLASH_V123", "FLASH_V124", "FLASH_V125", "FLASH_V126"}));
    flashV123.addPatch(std::vector<unsigned char> {(0xff),(0xf7),(0xaa),(0xff),(0x00),(0x04),(0x03),(0x0c)},
                       std::vector<unsigned char> {(0x1b),(0x23),(0x1b),(0x02),(0x32),(0x20),(0x03),(0x43)});
    patchsets.push_back(flashV123);

    return patchsets;
}
