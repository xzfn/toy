#pragma once

#include <glm/vec3.hpp>


constexpr glm::vec3 COLOR_BLACK{ 0.0, 0.0, 0.0 };
constexpr glm::vec3 COLOR_WHITE{ 1.0, 1.0, 1.0 };

constexpr glm::vec3 COLOR_LIGHT_GRAY{ 0.827451, 0.827451, 0.827451 };
constexpr glm::vec3 COLOR_GRAY{ 0.5, 0.5, 0.5 };
constexpr glm::vec3 COLOR_DARK_GRAY{ 0.662745, 0.662745, 0.662745 };

constexpr glm::vec3 COLOR_RED{ 1.0, 0.0, 0.0 };
constexpr glm::vec3 COLOR_GREEN{ 0.0, 1.0, 0.0 };
constexpr glm::vec3 COLOR_BLUE{ 0.0, 0.0, 1.0 };

constexpr glm::vec3 COLOR_YELLOW{ 1.0, 1.0, 0.0 };
constexpr glm::vec3 COLOR_MAGENTA{ 1.0, 0.0, 1.0 };
constexpr glm::vec3 COLOR_CYAN{ 0.0, 1.0, 1.0 };


uint8_t color_component_to_byte(float c);
uint32_t rgb_to_hex(glm::vec3 color);
