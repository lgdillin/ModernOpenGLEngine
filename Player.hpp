#pragma once

#include "GlfwCamera.hpp"
#include "Entity.hpp"

class Player : public Entity {
public:
	Player(
		int id, 
		const std::string &name,
		GlfwCamera *_glfwCamera
	) : Entity(id, name) {
		glfwCamera = _glfwCamera;
	}

	~Player() {}

	void updatePosition() {

	}

	void render() {}

	void control() {} // nothing yet.

private:
	GlfwCamera *glfwCamera; // allows the model (player) to send information to the view
};