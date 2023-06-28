#pragma once

#include <iostream>

#include <glm/glm.hpp>

class Entity {
public:
	Entity();
	Entity(int id, const std::string &name) : id(id), name(name) {};
	//Entity(int id, const std::string &name) {};
	Entity(Entity &e);
	virtual ~Entity();

	virtual void updatePosition() = 0;

	virtual void render(float dt) = 0;

	virtual void control() = 0;
protected:
	int id;
	std::string name;
};