// Created by BlurringShadow at 2021-03-14-下午 9:46

#include <boost/test/unit_test.hpp>

#include "systems/note/note_system.h"
#include "systems/note/judge_system.h"
#include "systems/note/locator_system.h"

using namespace polycumic::game_core;

BOOST_AUTO_TEST_CASE(note_system_test)
{
    systems::note::note_system system{};
    {
        const auto id = system.create_from_json(
            R"(
        [
            {
                "locator": {
                    "timestamp": 2589,
                    "speed": 1,
                    "surface": "front",
                    "horizontal_coordinate": "left",
                    "vertical_coordinate": "right"
                },
                "direction": "none"
            },
            {
                "locator": {
                    "timestamp": 2752,
                    "speed": 1,
                    "surface": "front",
                    "horizontal_coordinate": "left",
                    "vertical_coordinate": "right"
                },
                "direction": "none"
            },
            {
                "locator": {
                    "timestamp": 2589,
                    "speed": 1,
                    "surface": "front",
                    "horizontal_coordinate": "left",
                    "vertical_coordinate": "right"
                },
                "direction": "none"
            },
            {
                "locator": {
                    "timestamp": 2915,
                    "speed": 1,
                    "surface": "front",
                    "horizontal_coordinate": "left",
                    "vertical_coordinate": "right"
                },
                "direction": "none"
            },
            {
                "locator": {
                    "timestamp": 3078,
                    "speed": 1,
                    "surface": "front",
                    "horizontal_coordinate": "left",
                    "vertical_coordinate": "right"
                },
                "direction": "none"
            }
        ]
    )"_padded
        ).front();

        {
            const auto& locator =
                registry.get<components::note::locator>(id);

            BOOST_CHECK_EQUAL(locator.timestamp.count(), 2589);
            BOOST_CHECK_EQUAL(locator.speed, 1);
            BOOST_TEST((locator.surface == components::note::surface::front));
            BOOST_TEST(
                (locator.horizontal_coordinate == components::note::coordinate::
                    left)
            );
            BOOST_TEST(
                (locator.vertical_coordinate == components::note::coordinate::
                    right)
            );
        }
    }


    systems::note::locator_system locator_system;
    systems::note::judge_system judge_system;

    for(const auto t : {78ms, 2589ms, 2915ms, 3078ms})
    {
        locator_system.set_timestamp(t);

        locator_system.each_transform(
            [](const auto entities)
            {
                const auto& view = registry.view<
                    const components::note::direction,
                    const components::transform>();
                for(const auto entity : entities)
                {
                    BOOST_TEST_MESSAGE(
                        nameof::nameof_enum(view.get<const components::note::direction>(entity))
                    );
                }
            }
        );

        judge_system.judge_from_timestamp(t);
        judge_system.flush(
            [](const auto entities)
            {
                const auto& view = registry.view<
                    const components::note::direction,
                    const components::transform>();
                for(const auto [entity, result] : entities)
                {
                    BOOST_TEST_MESSAGE(
                        (fmt::format(
                                "id:{}, result:{} direction:{}",
                                polycumic::utility::to_underlying(entity),
                                polycumic::utility::to_underlying(result),
                                polycumic::utility::to_underlying(
                                    view.get<const components::note::direction>(entity)
                                )
                            )
                        )
                    );
                    registry.destroy(entity);
                }
            }
        );
    }
}
