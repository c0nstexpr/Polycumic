// Created by BlurringShadow at 2021-03-11-下午 4:42

#pragma once

#include "traits/member.h"
#include "traits/object.h"

namespace polycumic::utility
{
    namespace details
    {
        template<auto MemberPtr>
        struct member_getter_traits : traits::unique_object_trait
        {
            using traits = traits::member_function_pointer_traits<MemberPtr>;
            using class_t = typename traits::class_t;
            using type = typename traits::result_t;

        protected:
            static constexpr auto getter_ = [](const class_t& instance) { return (instance.*MemberPtr)(); };
        };
    }

    template<auto MemberPtr>
    class member_getter : public details::member_getter_traits<MemberPtr>
    {
    public:
        using base = details::member_getter_traits<MemberPtr>;
        using typename base::class_t;
        using typename base::type;

    private:
        using base::getter_;

        const class_t& instance_;
    public:
        explicit constexpr member_getter(const class_t& instance) : instance_(instance) {}

        [[nodiscard]] constexpr decltype(auto) operator()() const
        noexcept(noexcept(getter_(instance_))) { return getter_(instance_); }

        // ReSharper disable once CppNonExplicitConversionOperator
        [[nodiscard]] constexpr operator typename
        base::traits::result_t() const noexcept(noexcept(std::declval<member_getter>()()))
        {
            return (*this)();
        }
    };
}
