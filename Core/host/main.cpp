// Created by BlurringShadow at 2021-02-25-下午 11:40

#include <fmt/format.h>
#include "reflection.h"

using namespace polycumic::utility;

struct my_struct
{
    int v{};

    template<typename>
    void f(int, char) const {}
};

int main()
{
    using meta = reflection::type_meta<my_struct>;
    //meta::data<MEMBER_DATA_AS_ARG(&my_struct::v)>();
    meta::func<MEMBER_FUNC_AS_ARG(&my_struct::f<int>)>();

    return 0;
}
