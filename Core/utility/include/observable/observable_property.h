#pragma once

#include "utils.h"

#include "property.h"

namespace polycumic::utility::observable
{
    template<property_type PropertyT>
    class observable_property : traits::unique_object_trait, std::type_identity<traits::value_type<PropertyT>>
    {
    public:
        using property_t = PropertyT;
        using value_t = traits::value_type<property_t>;
        using const_ref_t = add_const_lvalue_ref_t<value_t>;

    private:
        property_t& property_;

        rxcpp::subjects::subject<const_ref_t> previous_;
        rxcpp::subjects::subject<std::pair<const_ref_t, const_ref_t>> next_;

    public:
        explicit observable_property(property_t& prop) : property_(prop) {}

        [[nodiscard]] constexpr decltype(auto) observe_before_changed() const
        {
            return previous_.get_observable();
        }

        [[nodiscard]] constexpr decltype(auto) observe_after_changed() const
        {
            return next_.get_observable();
        }

        template<typename... Args> requires std::constructible_from<value_t>
        constexpr void set(Args&&... args)
        {
            auto&& pre_subscriber = previous_.get_subscriber();
            auto&& next_subscriber = next_.get_subscriber();
            try
            {
                auto&& setter_expected = property_.set(std::forward<Args>(args)...);
                setter_expected.and_then(
                    [&](const auto& pre)
                    {
                        pre_subscriber.on_next(pre);
                        next_subscriber.on_next(std::pair{pre, property_});
                        return expected<void, void>();
                    }
                );
                setter_expected.or_else(
                    [](const validation_result res)
                    {
                        if(res != validation_result::equaled)
                            throw std::invalid_argument{"validation failed"};
                        return expected<void, void>();
                    }
                );
            }
            catch(...)
            {
                pre_subscriber.on_error();
                next_subscriber.on_error();
            }
        }

        [[nodiscard]] constexpr decltype(auto) value() const requires requires
        {
            std::as_const(property_)();
        } { return property_(); }

        [[nodiscard]] constexpr decltype(auto) value() { return value(); }
    };
}
