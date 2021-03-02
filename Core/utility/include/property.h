#pragma once

#include "validator.h"
#include "value_getter.h"
#include "value_setter.h"

namespace polycumic::utility
{
    template<getter_object Getter,
        setter_object Setter,
        validator<value_type<Getter>> Validator = decltype(equality_validator<value_type<Getter>>)>
    requires std::same_as<value_type<Setter>, value_type<Getter>>
    struct property_trait : unique_object_trait, std::type_identity<value_type<Getter>>
    {
        using getter_t = Getter;
        using setter_t = Setter;
        using validator_t = Validator;

        using getter_result_t = getter_result_t<getter_t>;
        using setter_result_t = expected<typename property_trait::type, validation_result>;
    };

    namespace details
    {
        template<typename Property> requires getter_object<Property> && setter_object<Property> &&
        requires
        {
            typename Property::getter_t;
            typename Property::setter_t;
            typename Property::validator_t;

            std::derived_from<property_trait<typename Property::getter_t,
                typename Property::setter_t,
                typename Property::validator_t>>;
        }
        struct property_trait
        {
            constexpr property_trait() requires requires(Property t)
            {
                t.get();
                std::same_as<typename Property::getter_result_t, decltype(t.get())>;

                t.set(std::declval<t>());
                std::same_as<typename Property::setter_result_t, decltype(t.set(std::declval<t>()))>;
            } {}
        };
    }

    template<typename Property>
    concept property_type = requires { details::property_trait<Property>{}; };

    /**
     * \brief provide functionality like C# property
     */
    template<typename Getter, typename Setter, typename Validator>
    class property_base : property_trait<Getter, Setter, Validator>
    {
    public:
        using base = property_trait<Getter, Setter, Validator>;
        using base::base;

    private:
        Validator validator_;

    protected:
        Getter getter_;
        Setter setter_;

        [[nodiscard]] constexpr auto& get_validator() { return validator_; }

    public:
        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr property_base(
            Getter getter = {},
            Setter setter = {},
            Validator validator = {}
        ) noexcept(std::move_constructible<Getter> && std::move_constructible<Setter>) :
            validator_(move(validator)),
            getter_(move(getter)),
            setter_(move(setter)) {}

        [[nodiscard]] constexpr auto& get_validator() const { return validator_; }
    };

    template<typename Getter, typename Setter>
    class property_base<Getter, Setter, void> : public property_trait<Getter, Setter>
    {
    public:
        using base = property_trait<Getter, Setter>;
        using typename base::type;
        using base::base;

    protected:
        Getter getter_;
        Setter setter_;

    public:
        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr property_base(
            Getter getter = {},
            Setter setter = {}
        ) noexcept(std::move_constructible<Getter> && std::move_constructible<Setter>) :
            getter_(move(getter)),
            setter_(move(setter)) {}

        [[nodiscard]] static constexpr auto& get_validator()
        {
            return utility::equality_validator<type>;
        }
    };

    /**
     * \brief provide functionality like C# property
     */
    template<typename Getter, typename Setter, typename Validator>
    class property : public property_base<Getter, Setter, Validator>
    {
    public:
        using base = property_base<Getter, Setter, Validator>;
        using base::base;
        using typename base::type;
        using typename base::getter_result_t;
        using typename base::setter_result_t;
        using base::get_validator;

    private:
        using base::getter_;
        using base::setter_;

    public:
        [[nodiscard]] constexpr decltype(auto) get() const { return getter_(); }

        [[nodiscard]] constexpr decltype(auto) operator()() const { return get(); }

        // ReSharper disable once CppNonExplicitConversionOperator
        [[nodiscard]] constexpr operator typename property::getter_result_t() const
        {
            return get();
        }

        template<typename... Args> requires std::constructible_from<type, Args...>
        constexpr setter_result_t set(Args&&... args)
        {
            type new_value(std::forward<Args>(args)...);
            auto old_value = get();
            const auto res = get_validator()(old_value, new_value);
            if(res == validation_result::success)
            {
                setter_(new_value);
                return std::move(old_value);
            }
            return typename setter_result_t::unexpected_type(res);
        }

        template<typename First = type, typename... Args>
        constexpr auto operator()(First&& first, Args&&... args)
        {
            return set(std::forward<First>(first), std::forward<Args>(args)...);
        }

        template<typename U>
        constexpr property& operator=(U&& arg)
        {
            if(!set(std::forward<U>(arg))) throw std::invalid_argument("validation failed");
            return *this;
        }
    };

    template<typename Getter, typename Setter>
    property(Getter, Setter) -> property<Getter, Setter, void>;

    template<typename Getter, typename Setter, typename Validator>
    property(Validator, Getter, Setter) -> property<Getter, Setter, Validator>;

    namespace details
    {
        struct generate_property_obj
        {
            template<typename T>
            constexpr auto operator()(T& v) const
            {
                return property(value_getter(v), value_setter(v));
            }
        };
    }

    inline constexpr details::generate_property_obj generate_value_property;
}
