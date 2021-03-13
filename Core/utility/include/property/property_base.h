// Created by BlurringShadow at 2021-03-08-上午 12:29

#pragma once

#include "getter/getter.h"
#include "setter/setter.h"
#include "traits/object.h"
#include "validator/validator.h"

namespace polycumic::utility
{
    template<typename T, setter_object<T> Setter>
    class validated_setter
    {
        static constexpr auto nothrow_move_constructible_ =
            std::is_nothrow_move_constructible_v<T>;

        T new_value_;
        Setter& setter_;

    public:
        constexpr validated_setter(
            Setter& setter,
            T&& new_value
        ) noexcept(nothrow_move_constructible_) :
            new_value_(std::move(new_value)),
            setter_(setter) {}

        void operator(
        )() noexcept(std::is_nothrow_invocable_v<Setter, T&&> && nothrow_move_constructible_)
        {
            setter_(std::move(new_value_));
        }
    };


    template<
        typename T,
        getter_object<T> Getter,
        setter_object<T> Setter,
        validator<traits::value_type<Getter>> Validator =
        decltype(equality_validator<traits::value_type<Getter>>)>
    struct property_trait : traits::unique_object_trait, std::type_identity<T>
    {
        using getter_t = Getter;
        using setter_t = Setter;
        using validator_t = Validator;

        using getter_result_t = std::invoke_result_t<getter_t>;

        using validated_t = validated_setter<T, Setter>;
        using setter_result_t = expected<validated_t, validation_result>;

    protected:
        Getter getter_;
        Setter setter_;

    public:
        constexpr property_trait(
            Getter getter,
            Setter setter
        ) noexcept(std::move_constructible<Getter> && std::move_constructible<Setter>) :
            getter_(std::move(getter)),
            setter_(std::move(setter)) {}

        constexpr validated_t make_validated_setter(
            Setter& setter,
            T&& new_value
        ) & noexcept(std::is_nothrow_constructible_v<validated_t, Setter&, T&&>)
        {
            return {setter, std::move(new_value)};
        }
    };

    namespace details
    {
        template<traits::type_queryable Property> requires requires
        {
            typename Property::getter_t;
            typename Property::setter_t;
            typename Property::validator_t;
            getter_object<Property, traits::value_type<Property>> &&
            setter_object<Property, traits::value_type<Property>>;
        }
        struct property_trait
        {
            using value_t = traits::value_type<Property>;
            using base_trait = utility::property_trait<
                value_t,
                typename Property::getter_t,
                typename Property::setter_t,
                typename Property::validator_t
            >;

            constexpr property_trait() requires requires(Property t)
            {
                std::derived_from<Property, base_trait>;

                std::same_as<
                    typename Property::setter_result_t,
                    typename base_trait::setter_result_t
                >;

                t.get();
                std::same_as<typename Property::getter_result_t, decltype(t.get())>;

                t.set(std::declval<value_t>());
                std::same_as<typename Property::setter_result_t, decltype(t.set(
                    std::declval<value_t>()
                ))>;
            } {}
        };
    }

    template<typename Property>
    concept property_type = requires { details::property_trait<Property>{}; };

    template<typename T, typename Getter, typename Setter, typename Validator>
    class property_base : public property_trait<T, Getter, Setter, Validator>
    {
    public:
        using base = property_trait<Getter, Setter, Validator>;
        using base::base;

    private:
        Validator validator_;

    public:
        explicit constexpr property_base(
            Getter getter = {},
            Setter setter = {},
            Validator validator = {}
        ) noexcept(std::move_constructible<Getter> && std::move_constructible<Setter>) :
            base(std::move(getter), std::move(setter)),
            validator_(move(validator)) {}

        [[nodiscard]] constexpr auto& get_validator() { return validator_; }

        [[nodiscard]] constexpr auto& get_validator() const { return validator_; }
    };

    template<typename T, typename Getter, typename Setter>
    class property_base<T, Getter, Setter, void> : public property_trait<T, Getter, Setter>
    {
    public:
        using base = property_trait<T, Getter, Setter>;
        using base::base;

        explicit constexpr property_base(
            Getter getter = {},
            Setter setter = {}
        ) noexcept(std::move_constructible<Getter> && std::move_constructible<Setter>) :
            base(std::move(getter), std::move(setter)) {}

        [[nodiscard]] static constexpr const auto& get_validator()
        {
            return utility::equality_validator<typename property_base::type>;
        }
    };
}
