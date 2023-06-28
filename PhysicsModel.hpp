#pragma once

#include <iostream>
#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "Object.hpp"

class PhysicsModel {
public:
	PhysicsModel();
	~PhysicsModel();

	void addObject(Object *object);
	void removeObject(Object *object);

	// dt is some deltaTime, usually will be passed from the game loop
	void Step(float dt) {
		for (Object *obj : objects) {
			// Apply a force
			obj->force += obj->mass * gravity;

			obj->velocity += obj->force / obj->mass * dt;
			obj->position += obj->velocity * dt;

			// reset net force at the end
			obj->force = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
private:
	std::vector<Object *> objects;
	const glm::vec3 gravity;

};