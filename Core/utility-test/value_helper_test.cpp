// Created by BlurringShadow at 2021-02-28-上午 12:16

#include <boost/test/unit_test.hpp>

#include "value_getter.h"
#include "value_setter.h"

using namespace polycumic::utility;

BOOST_AUTO_TEST_SUITE(value_helper_test)

BOOST_AUTO_TEST_CASE(value_helper)
{
    struct my_struct
    {
        int v;
    };

    my_struct m{5};
    value_setter setter{m.v};
    const value_getter getter{m.v};

    setter(7);
    BOOST_CHECK_EQUAL(getter(), 7);
}

BOOST_AUTO_TEST_SUITE_END()
