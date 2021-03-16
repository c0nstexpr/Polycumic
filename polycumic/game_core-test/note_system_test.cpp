// Created by BlurringShadow at 2021-03-14-下午 9:46

#include <boost/test/unit_test.hpp>

#include "systems/note/note_system.h"

using namespace polycumic::game_core::systems::note;


BOOST_AUTO_TEST_CASE(note_system_test)
{
    note_system system{};
    const auto id = system.create_from_json(R"(
        [
            {
                "locator":
                {
                    "timestamp": 5,
                    "speed": 1,
                    "surface": "front",
                    "horizontal_coordinate": "left",
                    "vertical_coordinate": "right"
                },
                "direction": "none"
            }
        ]
    )"_padded).front();

    const auto& locator = 
        polycumic::game_core::registry.get<polycumic::game_core::components::note::locator>(id);

    BOOST_CHECK_EQUAL(locator.timestamp.count(), 5);
    BOOST_CHECK_EQUAL(locator.speed, 1);
    BOOST_TEST((locator.surface == polycumic::game_core::components::note::surface::front));
    BOOST_TEST((locator.horizontal_coordinate == polycumic::game_core::components::note::coordinate::left));
    BOOST_TEST((locator.vertical_coordinate == polycumic::game_core::components::note::coordinate::right));
}
