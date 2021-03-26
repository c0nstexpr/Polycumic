#pragma once

#include "utility_core.h"

#include "property/property.h"

namespace polycumic::utility::observable
{
    CPP_template(typename PropertyT)(requires(property_type<PropertyT>))
    class observable_property :
        public traits::unique_object_trait,
        public boost::type_identity<traits::value_type<PropertyT>>
    {
    public:
        using property_t = PropertyT;
        using typename observable_property::type;
        using const_ref_t = add_const_lvalue_ref_t<type>;
        using on_next_t = std::pair<const_ref_t, typename property_t::getter_result_t>;

    private:
        property_t& property_;

        rxcpp::subjects::subject<type> previous_;
        rxcpp::subjects::subject<on_next_t> next_;
     
    public:
        explicit observable_property(property_t& prop) : property_(prop) {}

        [[nodiscard]] constexpr auto observe_before_changed() const
        {
            return previous_.get_observable();
        }

        [[nodiscard]] constexpr auto observe_after_changed() const
        {
            return next_.get_observable();
        }

        CPP_template(typename... Args)(requires concepts::constructible_from<type>)
        constexpr void set(Args&&... args)
        {
            auto&& pre_subscriber = previous_.get_subscriber();
            auto&& next_subscriber = next_.get_subscriber();
            try
            {
                auto&& setter_expected = property_.set(std::forward<Args>(args)...);
                if(setter_expected)
                {
                    type pre = property_;
                    pre_subscriber.on_next(std::as_const(pre));
                    (*setter_expected)();
                    next_subscriber.on_next(on_next_t{pre, property_});
                }
                else
                {
                    const validation_result res = setter_expected.error();
                    if(res != validation_result::ignored) throw validation_error{""};
                }
            }
            catch(...)
            {
                const auto exception_ptr = std::current_exception();
                pre_subscriber.on_error(exception_ptr);
                next_subscriber.on_error(exception_ptr);
            }
        }

        [[nodiscard]] constexpr decltype(auto) value() const { return property_(); }

        [[nodiscard]] constexpr decltype(auto) value() { return property_(); }
    };

    CPP_template(typename PropertyT)(requires(property_type<PropertyT>))
    observable_property(PropertyT&) -> observable_property<PropertyT>;
}
