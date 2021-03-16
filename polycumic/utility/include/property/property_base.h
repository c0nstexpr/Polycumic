// Created by BlurringShadow at 2021-03-08-上午 12:29

#pragma once

#include "getter/getter.h"
#include "setter/setter.h"
#include "traits/object.h"
#include "validator/validator.h"

namespace polycumic::utility
{
    CPP_template(typename T, typename Setter)(requires(setter_object<Setter, T>))
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

    CPP_template(
        typename T,
        typename Getter,
        typename Setter,
        typename Validator = void       
    )(
        requires(
            getter_object<Getter, T> && 
            setter_object<Setter, T> && 
            (concepts::same_as<Validator, void> || validator<Validator, T>)
        )
    )
    struct property_trait : traits::unique_object_trait, boost::type_identity<T>
    {
        using getter_t = Getter;
        using setter_t = Setter;
        using validator_t = std::conditional_t<
            concepts::same_as<Validator, void>,
            decltype(equality_validator<traits::value_type<Getter>>),
            Validator
        >;

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
        ) noexcept(concepts::move_constructible<Getter> && concepts::move_constructible<Setter>) :
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
        template<typename Property>
        CPP_requires(
            property_trait_requires,
            requires(
                typename Property::getter_t,
                typename Property::setter_t,
                typename Property::validator_t
            )
            (
                getter_object<Property, traits::value_type<Property>> &&
                setter_object<Property, traits::value_type<Property>> &&
                traits::type_queryable<Property>
            )
        );

        CPP_template(typename Property)(
            requires(CPP_requires_ref(property_trait_requires, Property))
        )
        struct property_trait
        {
            using value_t = traits::value_type<Property>;
            using base_trait = utility::property_trait<
                value_t,
                typename Property::getter_t,
                typename Property::setter_t,
                typename Property::validator_t
            >;

            template<typename T>
            CPP_requires(
                constructible,
                requires(T& t)(
                    concepts::derived_from<Property, base_trait> &&
                    concepts::same_as<
                    typename Property::setter_result_t,
                    typename base_trait::setter_result_t
                    > &&
                    concepts::same_as<typename Property::getter_result_t, decltype(t.get())>,
                    concepts::same_as<
                    typename Property::setter_result_t,
                    decltype(t.set(std::declval<value_t>()))
                    >
                )
            );

            constexpr CPP_ctor(property_trait)
            ()(requires CPP_requires_ref(constructible, Property)) {}
        };

        template<typename Property>
        CPP_requires(is_property_type, requires(details::property_trait<Property>)(true));
    }

    template<typename Property>
    CPP_concept property_type = CPP_requires_ref(details::is_property_type, Property);

    template<typename T, typename Getter, typename Setter, typename Validator>
    class property_base : public property_trait<T, Getter, Setter, Validator>
    {
    public:
        using base = property_trait<Getter, Setter, Validator, Validator>;
        using base::base;

    private:
        Validator validator_;

    public:
        explicit constexpr property_base(
            Getter getter = {},
            Setter setter = {},
            Validator validator = {}
        ) noexcept(concepts::move_constructible<Getter> && concepts::move_constructible<Setter>) :
            base(std::move(getter), std::move(setter)),
            validator_(move(validator)) {}

        [[nodiscard]] constexpr auto& get_validator() { return validator_; }

        [[nodiscard]] constexpr auto& get_validator() const { return validator_; }
    };

    template<typename T, typename Getter, typename Setter>
    class property_base<T, Getter, Setter, void> : public property_trait<T, Getter, Setter, void>
    {
    public:
        using base = property_trait<T, Getter, Setter, void>;
        using base::base;

        explicit constexpr property_base(
            Getter getter = {},
            Setter setter = {}
        ) noexcept(concepts::move_constructible<Getter> && concepts::move_constructible<Setter>) :
            base(std::move(getter), std::move(setter)) {}

        [[nodiscard]] static constexpr const auto& get_validator()
        {
            return utility::equality_validator<typename property_base::type>;
        }
    };
}
