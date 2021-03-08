// Created by BlurringShadow at 2021-03-01-下午 1:55
#include <boost/test/unit_test.hpp>

#include "observable/observable_property.h"

using namespace polycumic::utility;

struct my_class
{
    int v{};

    AUTO_MEMBER(v_prop, generate_value_property(v));

    AUTO_MEMBER(observable_v, observable::observable_property(v_prop));
};

BOOST_AUTO_TEST_SUITE(observable_property_test)

BOOST_AUTO_TEST_CASE(observable_property_test)
{
    my_class c{.v = 1};
    auto& observable = c.observable_v;

    [[maybe_unused]] const auto& before_changed = observable.observe_before_changed().subscribe(
        [](const int v) { BOOST_CHECK_EQUAL(v, 1); }
    );
    [[maybe_unused]] const auto& after_changed = observable.observe_after_changed().subscribe(
        [](const auto pair)
        {
            const auto& [pre, cur] = pair;
            BOOST_CHECK_EQUAL(pre, 1);
            BOOST_CHECK_EQUAL(cur, 5);
        }
    );

    observable.set(5);

    BOOST_CHECK_EQUAL(c.v, 5);
}

BOOST_AUTO_TEST_SUITE_END()
