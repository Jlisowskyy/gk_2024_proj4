#ifndef MGR_SETTINGS_MGR_HPP_
#define MGR_SETTINGS_MGR_HPP_

#include <libcgp/defines.hpp>
#include <libcgp/settings.hpp>

#include <CxxUtils/singleton.hpp>

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <vector>

LIBGCP_DECL_START_
class SettingsMgr final : public CxxUtils::Singleton<SettingsMgr>
{
    // ------------------------------
    // Class internals
    // ------------------------------

    static constexpr size_t kMaxSettings = 128;

    public:
    enum class Setting : std::uint16_t {
        kCameraType,
        kLast,
    };

    struct SettingContainer {
        template <typename T>
        explicit SettingContainer(const T value) noexcept
        {
            SetSetting(value);
        }

        SettingContainer() = default;

        template <typename T>
        FAST_CALL uint64_t SetSetting(const T value) noexcept
        {
            static_assert(sizeof(T) <= sizeof(uint64_t), "Setting value is too large");
            static_assert(std::is_trivially_copyable<T>::value, "Setting type must be trivially copyable");

            *reinterpret_cast<T *>(&data_) = value;
            return data_;
        }

        template <typename T>
        NDSCRD FAST_CALL T GetSetting(const Setting setting) const noexcept
        {
            static_assert(sizeof(T) <= sizeof(uint64_t), "Setting value is too large");
            static_assert(std::is_trivially_copyable<T>::value, "Setting type must be trivially copyable");

            return *reinterpret_cast<T *>(&data_);
        }

        NDSCRD FAST_CALL uint64_t GetRaw() const noexcept { return data_; }

        protected:
        uint64_t data_{};
    };

    using setting_t = std::vector<std::tuple<Setting, SettingContainer>>;

    protected:
    static_assert(static_cast<size_t>(Setting::kLast) < kMaxSettings, "Too many settings");

    // ------------------------------
    // Object creation
    // ------------------------------

    explicit SettingsMgr(const setting_t &settings) noexcept;

    public:
    SettingsMgr() = delete;
    ~SettingsMgr();

    FAST_CALL static SettingsMgr &InitInstance(const setting_t &settings) noexcept
    {
        assert(!IsInited());

        Singleton::InitInstance(new SettingsMgr(settings));
        return GetInstance();
    }

    // ------------------------------
    // Class interaction
    // ------------------------------

    template <typename T>
    FAST_CALL SettingsMgr &SetSetting(const Setting setting, const T value) noexcept
    {
        const uint64_t new_value = settings_[static_cast<size_t>(setting)].SetSetting(value);
        for (const auto &listener : listeners_[static_cast<size_t>(setting)]) {
            listener(new_value);
        }

        return *this;
    }

    template <typename T>
    NDSCRD FAST_CALL T GetSetting(const Setting setting) const noexcept
    {
        return settings_[static_cast<size_t>(setting)].GetSetting<T>(setting);
    }

    FAST_CALL SettingsMgr &AddListener(const Setting setting, void (*listener)(uint64_t)) noexcept
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

LIBGCP_DECL_END_

#endif  // MGR_SETTINGS_MGR_HPP_
