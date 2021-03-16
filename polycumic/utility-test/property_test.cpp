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

    void set(const data_type value) { v = value; }

    data_type get() const { return v; }

    AUTO_MEMBER(v_prop, generate_value_property(v));

    property<
        data_type, member_getter<&my_struct::get>, member_setter<data_type, &my_struct::set>
    > v_member_prop = 
        generate_property_from_mem<&my_struct::get, &my_struct::set>(*this);
};

static std::uniform_int_distribution<data_type> distribution{
    std::numeric_limits<data_type>::min()
};

using validator_t = decltype(equality_validator<data_type>);

static_assert(validator<validator_t, data_type>);

using value_setter_t = value_setter<data_type>;
using value_getter_t = value_getter<data_type>;

static_assert(getter_object<value_getter_t, data_type>);
static_assert(setter_object<value_setter_t, data_type>);

using mem_setter_t = member_setter<data_type, &my_struct::set>;
using mem_getter_t = member_getter<&my_struct::get>;

static_assert(setter_object<mem_setter_t, data_type>);
static_assert(getter_object<mem_getter_t, data_type>);

using trait = property_trait<data_type, value_getter_t, value_setter_t>;
using property_base_t = property_base<data_type, value_getter_t, value_setter_t, void>;

static_assert(concepts::derived_from<property_base_t, trait>);

using property_t = property<data_type, value_getter_t, value_setter_t, void>;

static_assert(concepts::derived_from<property_t, property_base_t>);

BOOST_AUTO_TEST_SUITE(property_test);

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

    m.v_member_prop = value;
    BOOST_CHECK_EQUAL(m.v_member_prop(), value);

    m.v_member_prop({});
    BOOST_CHECK_EQUAL(m.v_member_prop(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
