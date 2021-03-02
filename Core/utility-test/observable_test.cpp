// Created by BlurringShadow at 2021-03-01-下午 1:55
#include <boost/test/unit_test.hpp>

#include "observable/observable_property.h"

using namespace polycumic::utility;

class my_class
{
    int v_{};

    //AUTO_RVALUE_MEMBER(v_prop_, generate_value_property(v_));

    //AUTO_RVALUE_MEMBER(observable_v_, observable::observable_property{v_prop_});

public:
    /*
    constexpr auto& v() const { return observable_v_; }

    constexpr auto& v() { return observable_v_; }
    */
};

BOOST_AUTO_TEST_SUITE(observable_property_test)

BOOST_AUTO_TEST_CASE(observable_property)
{
    
}

BOOST_AUTO_TEST_SUITE_END()
