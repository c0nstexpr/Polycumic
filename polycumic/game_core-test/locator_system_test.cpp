// Created by BlurringShadow at 2021-03-13-下午 12:10

#include <boost/test/unit_test.hpp>

#include "systems/note/locator_system.h"

using namespace polycumic::game_core;


BOOST_AUTO_TEST_CASE(locator_system_test)
{
    systems::note::locator_system system{};
    const auto id = registry.create();
    registry.emplace<components::note::locator>(
        id,
        2250ms,
        1.0,
        components::note::surface::front,
        components::note::coordinate::left,
        components::note::coordinate::left
    );

    const auto& transform = registry.emplace<components::transform>(id);
    registry.emplace<components::note::direction>(id);
    registry.emplace<components::note::judge_state>(id);

    system.set_timestamp(0ms);
    const auto excepted_res = scale(
        translate(
            glm::one<glm::dmat4>(),
            glm::dvec3{150, 100, 100} * 0.25
        ),
        mix({}, glm::one<glm::dvec3>(), 0.25)
    );

    const auto epsilon = 0.001;

    for(auto i = 0; i < excepted_res.length(); ++i)
        for(auto j = 0; j < glm::dvec3::length(); ++j)
        {
            BOOST_CHECK_CLOSE(excepted_res[i][j], transform.value[i][j], 0.001);
        }
}
