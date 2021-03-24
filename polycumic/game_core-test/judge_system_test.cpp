// Created by BlurringShadow at 2021-03-13-обнГ 12:10

#include <boost/test/unit_test.hpp>

#include "systems/note/judge_system.h"

using namespace polycumic::game_core::systems::note;

BOOST_AUTO_TEST_CASE(judge_system_test)
{
    judge_system system{};
    system.judge_from_input(
        std::chrono::milliseconds{}, 
        judge_system::coordinate_t::left,
        judge_system::coordinate_t::left,
        judge_system::surface_t::front,
        judge_system::direction_t::none
    );
}
