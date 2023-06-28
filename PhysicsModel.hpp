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

		}
	}
private:
	std::vector<Object *> objects;
	const glm::vec3 gravity;

};