// Created by BlurringShadow at 2021-03-02-下午 2:41
#include "property/property.h"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>

using namespace polycumic::utility;
using namespace boost::unit_test;

using data_type = int;

struct my_struct
{
    data_type v{};

    AUTO_MEMBER(v_prop, generate_value_property(v));
};

BOOST_AUTO_TEST_SUITE(property_test);

static std::uniform_int_distribution<data_type> distribution{
    std::numeric_limits<data_type>::min()
};

using validator_t = decltype(equality_validator<data_type>);

static_assert(validator<validator_t, data_type>);

using setter_t = value_setter<data_type>;
using getter_t = value_getter<data_type>;

static_assert(getter_object<getter_t, data_type>);
static_assert(setter_object<setter_t, data_type>);

using trait = property_trait<data_type, getter_t, setter_t>;
using property_base_t = property_base<data_type, getter_t, setter_t, void>;

static_assert(std::derived_from<property_base_t, trait>);

using property_t = property<data_type, getter_t, setter_t, void>;

static_assert(std::derived_from<property_t, property_base_t>);

std::array<data_type, 10> data_set;

BOOST_DATA_TEST_CASE(
    property_test,
    data::make(
        data_set |= ranges::actions::transform(
            [&](const auto&) { return distribution(get_random_engine()); }
        )
    ),
    value
)
{
    my_struct m;

    BOOST_CHECK_EQUAL(m.v_prop(), 0);

    m.v_prop = value;
    BOOST_CHECK_EQUAL(m.v_prop(), value);

    m.v_prop({});
    BOOST_CHECK_EQUAL(m.v_prop(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
