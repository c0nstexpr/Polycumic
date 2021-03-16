// Created by BlurringShadow at 2021-03-11-下午 2:24

#pragma once

#include "core.h"

namespace polycumic::game_core::systems
{
    template<typename Component>
    class component_observable
    {
    public:
        using observable_t =
        std::unordered_map<entt::entity, std::reference_wrapper<const Component>>;

        auto observable() const { return subject_.get_observable(); }

        void flush(observable_t list = {})
        {
            const auto& view = registry.view<const Component>();

            list.reserve(list.size() + observer_.size());

            observer_.each(
                [&](const entt::entity id)
                {
                    list.emplace(
                        id,
                        view.template get<const Component>(id)
                    );
                }
            );

            subject_.get_subscriber().on_next(std::move(list));
        }

    private:
        entt::observer observer_{registry, entt::collector.update<Component>()};

        rxcpp::subjects::subject<observable_t> subject_;
    };
}
