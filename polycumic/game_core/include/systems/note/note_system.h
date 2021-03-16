// Created by BlurringShadow at 2021-03-11-下午 11:50

#pragma once
#include "core.h"
#include "components/note/direction.h"
#include "components/note/locator.h"

#include <magic_enum.hpp>

#pragma warning(push)
#pragma warning( disable : 4706 )
#include <simdjson.h>
#pragma warning(pop)
namespace polycumic::game_core::systems::note
{
    class note_system
    {
        simdjson::ondemand::parser parser_;

    public:
        std::vector<entt::entity> create_from_json(const simdjson::padded_string& str);
    };
}
