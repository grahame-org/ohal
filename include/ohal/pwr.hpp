#ifndef OHAL_PWR_HPP
#define OHAL_PWR_HPP

namespace ohal::pwr
{

template <typename Instance>
struct Controller
{
    static_assert(sizeof(Instance) == 0,
                  "ohal: pwr::Controller is not implemented for the selected MCU. "
                  "Ensure -DOHAL_FAMILY_* and -DOHAL_MODEL_* are set correctly.");
};

} // namespace ohal::pwr

#endif // OHAL_PWR_HPP
