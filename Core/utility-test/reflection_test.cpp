// Created by BlurringShadow at 2021-02-28-上午 12:16
#include <boost/test/unit_test.hpp>

#include "reflection.h"

template<typename>
void f(int, char) {}

struct my_struct
{
    int v{};

    template<typename>
    void f(int, char) const {}
};

BOOST_AUTO_TEST_SUITE(reflection_test)

BOOST_AUTO_TEST_CASE(reflection)
{
    utility::reflection::type<my_struct>
        .MEMBER_FUNC(&my_struct::f<int>)
        .MEMBER_VAR(&my_struct::v);

    const auto id = entt::to_hashed(MEMBER_FUNC_NAME(&my_struct::f<int>)).value();
    const auto type_info = entt::resolve<my_struct>();
    const auto requested_id = (*type_info.func().begin()).id();
    BOOST_CHECK_EQUAL(id, requested_id);
}

BOOST_AUTO_TEST_SUITE_END()
