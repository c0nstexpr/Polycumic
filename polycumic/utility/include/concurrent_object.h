// Created by BlurringShadow at 2021-03-12-下午 1:56

#pragma once

#include "utils.h"

#include  <shared_mutex>

namespace polycumic::utility
{
    template<typename T>
    class concurrent_object
    {
    public:
        CPP_template(typename... Args)(requires(concepts::constructible_from<T, Args...>))
        // ReSharper disable once CppNonExplicitConvertingConstructor
        constexpr concurrent_object(Args&&... args) :
            object_(std::forward<Args>(args)...) {}

        constexpr auto& raw() { return object_; }

        constexpr auto& raw() const { return object_; }

#ifdef __cpp_lib_concepts
        template<std::invocable<const T&> Func>
#endif
        template<typename Func>
        void read(Func func) const
        {
            std::shared_lock _(mutex_);
            func(object_);
        }

#ifdef __cpp_lib_concepts
        template<std::invocable<T&> Func>
#endif
        template<typename Func>
        void write(Func func)
        {
            std::unique_lock _(mutex_);
            func(object_);
        }

    private:
        T object_;

        mutable std::shared_mutex mutex_;
    };

    template<typename T>
    concurrent_object(T&&) -> concurrent_object<concepts::remove_cvref_t<T>>; 
}
