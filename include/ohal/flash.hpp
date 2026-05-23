#ifndef OHAL_FLASH_HPP
#define OHAL_FLASH_HPP

namespace ohal::flash
{

template <typename Instance>
struct Controller
{
    static_assert(sizeof(Instance) == 0,
                  "ohal: flash::Controller is not implemented for the selected MCU. "
                  "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::flash

#endif // OHAL_FLASH_HPP
