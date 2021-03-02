#pragma once

#include <glm/ext.hpp>

namespace game_core
{
    template<glm::length_t L>
    using vec_d = glm::vec<L, double>;

    template<glm::length_t C, glm::length_t R>
    using mat_d = glm::mat<C, R, double>;

    using transform_t = mat_d<4, 4>;
}
