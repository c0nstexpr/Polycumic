// Created by BlurringShadow at 2021-03-11-下午 5:19

#pragma once

#include "validator.h"
#include "traits/member.h"
#include "traits/object.h"

namespace polycumic::utility
{
    namespace details
    {
        template<auto MemberPtr>
        struct member_validator_traits : traits::unique_object_trait
        {
            using traits = traits::member_function_pointer_traits<MemberPtr>;
            using class_t = typename traits::class_t;
            using type = typename traits::args_t::template indexed_t<0>;

            static constexpr auto invocable = traits::args_t::size == 1 && 
                concepts::convertible_to<typename traits::result_t, validation_result>;
        };
    }

    template<auto MemberPtr>
    class member_validator : details::member_validator_traits<MemberPtr>
    {
        using base = details::member_validator_traits<MemberPtr>;
        static_assert(base::invocable);
        using typename base::traits;
        using typename base::class_t;
        using typename base::type;

        static constexpr auto validator_ = std::mem_fn(MemberPtr);

        class_t& instance_;

    public:
        explicit constexpr member_validator(class_t& instance) : instance_(instance) {}

        constexpr validation_result operator()(
            const type& t
        ) noexcept(noexcept(validator_(instance_, t)))
        {
            return validator_(instance_, t);
        }
    };
}
