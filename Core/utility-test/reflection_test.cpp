// Created by BlurringShadow at 2021-02-28-上午 12:16
#include <boost/test/unit_test.hpp>

#include "reflection.h"

#include "string_conversion.h"

using namespace polycumic::utility;
using namespace literals;

struct my_struct
{
    int v{};

    template<typename>
    void f(int, char) const {}
};

template<traits::member_of<my_struct> auto Ptr>
requires std::is_member_function_pointer_v<decltype(Ptr)>
void foo() {}

BOOST_AUTO_TEST_SUITE(reflection_test)

BOOST_AUTO_TEST_CASE(my_test)
{
    std::set<std::string_view> names{"int", "void", "char", "my_struct", "v"};

    using meta = reflection::type_meta<my_struct>;
    meta::MEMBER_DATA(&my_struct::v);
    meta::MEMBER_FUNC(&my_struct::f<int>);

    for(const auto& str : entt::hash_traits::all_str)
    {
        BOOST_TEST_MESSAGE(str);
        if(names.contains(str)) names.erase(str);
    }

    BOOST_TEST(names.empty());
}

BOOST_AUTO_TEST_SUITE_END()
