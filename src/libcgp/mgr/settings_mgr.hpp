#ifndef MGR_SETTINGS_MGR_HPP_
#define MGR_SETTINGS_MGR_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/intf.hpp>

#include <CxxUtils/static_singleton.hpp>
#include <CxxUtils/type_list.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <vector>

LIBGCP_DECL_START_
class SettingsMgrBase final : public CxxUtils::StaticSingletonHelper
{
    // ------------------------------
    // Class internals
    // ------------------------------

    static constexpr size_t kMaxSettings = 512;

    protected:
    static_assert(static_cast<size_t>(Setting::kLast) < kMaxSettings, "Too many settings");

    // ------------------------------
    // Object creation
    // ------------------------------

    public:
    explicit SettingsMgrBase(const setting_t &settings) noexcept;
    SettingsMgrBase() = delete;
    ~SettingsMgrBase() override;

    // ------------------------------
    // Class interaction
    // ------------------------------

    template <Setting kSetting, typename T>
    FAST_CALL SettingsMgrBase &SetSetting(const T value) noexcept
    {
        static_assert(
            std::is_same_v<T, typename SettingTypes<static_cast<size_t>(kSetting)>::type>, "Setting type mismatch"
        );

        return SetSetting(kSetting, value);
    }

    template <Setting kSetting, typename T>
    FAST_CALL T GetSetting() const noexcept
    {
        static_assert(
            std::is_same_v<T, typename SettingTypes<static_cast<size_t>(kSetting)>::type>, "Setting type mismatch"
        );

        return GetSetting<T>(kSetting);
    }

    template <typename T>
    FAST_CALL SettingsMgrBase &SetSetting(const Setting setting, const T value) noexcept
    {
        if (settings_[static_cast<size_t>(setting)].GetSetting<T>() == value) {
            return *this;
        }

        const uint64_t new_value = settings_[static_cast<size_t>(setting)].SetSetting(value);
        for (const auto &listener : listeners_[static_cast<size_t>(setting)]) {
            listener(new_value);
        }

        return *this;
    }

    template <typename T>
    NDSCRD FAST_CALL T GetSetting(const Setting setting) const noexcept
    {
        return settings_[static_cast<size_t>(setting)].GetSetting<T>();
    }

    FAST_CALL SettingsMgrBase &AddListener(const Setting setting, void (*listener)(uint64_t)) noexcept
    {
        listeners_[static_cast<size_t>(setting)].push_back(listener);

        return *this;
    }

    // ---------------------------------
    // Class implementation methods
    // ---------------------------------

    protected:
    // ------------------------------
    // Class fields
    // ------------------------------

    std::array<SettingContainer, kMaxSettings> settings_{};
    std::array<std::vector<void (*)(uint64_t)>, kMaxSettings> listeners_{};
};

using SettingsMgr = CxxUtils::StaticSingleton<SettingsMgrBase>;

LIBGCP_DECL_END_

#endif  // MGR_SETTINGS_MGR_HPP_
