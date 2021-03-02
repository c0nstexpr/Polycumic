// Created by BlurringShadow at 2021-03-02-下午 2:41
#include <boost/test/unit_test.hpp>

#include "property.h"

using namespace polycumic::utility;

struct my_struct
{
    int v;
};


BOOST_AUTO_TEST_SUITE(property_test)

BOOST_AUTO_TEST_CASE(property_test)
{
    using validator_t = decltype(equality_validator<int>);

    static_assert(validator<validator_t, int>);

    using setter_t = value_setter<int>;
    using getter_t = value_getter<int>;

    static_assert(getter_object<getter_t>);
    static_assert(setter_object<setter_t>);

    using trait = property_trait<getter_t, setter_t>;
    using property_base_t = property_base<getter_t, setter_t, void>;

    static_assert(std::derived_from<property_base_t, trait>);

    using property_t = property<getter_t, setter_t, void>;

    static_assert(std::derived_from<property_t, property_base_t>);

    my_struct m{5};

    property prop(value_getter(m.v), value_setter(m.v));
    auto&& generated_prop = generate_value_property(m.v);

    static_assert(std::same_as<decltype(prop), property_t>);
    static_assert(std::same_as<std::decay_t<decltype(generated_prop)>, property_t>);

    BOOST_TEST(
        ranges::all_of(std::initializer_list{generated_prop() ,prop()}, [](const auto& v) {
            return v == 5;})
    );
    prop = 7;
    BOOST_TEST(
        ranges::all_of(std::initializer_list{generated_prop() ,prop()}, [](const auto& v) {
            return v == 7;})
    );
    generated_prop({});
    BOOST_TEST(
        ranges::all_of(std::initializer_list{generated_prop() ,prop()}, [](const auto& v) {
            return v == 0;})
    );
}

BOOST_AUTO_TEST_SUITE_END()
