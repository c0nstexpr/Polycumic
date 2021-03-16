// Created by BlurringShadow at 2021-03-13-下午 12:10

#include <boost/test/unit_test.hpp>

#include "systems/note/locator_system.h"

using namespace polycumic::game_core::systems::note;


BOOST_AUTO_TEST_CASE(locator_system_test)
{
    locator_system system{};
    BOOST_TEST_MESSAGE((system.timestamp()().count()));
}
