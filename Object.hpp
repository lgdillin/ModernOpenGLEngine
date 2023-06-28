#pragma once

// The physical simulation of an object within the model.
// considered the most super class as far as simulating a data model

#include <glm/glm.hpp>

struct Object {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;
	float mass;
};