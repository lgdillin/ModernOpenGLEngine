#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class GlfwCamera {
public:


	GlfwCamera(
		glm::vec3 _position = glm::vec3(8.0f, 15.0f, 0.0f),
		glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f),
		GLfloat _yaw = -90.0f,
		GLfloat _pitch = 0.0f,
		GLfloat _movementSpeed = 5.0f,
		GLfloat _turnSpeed = 0.4f
	) {
		position = _position;
		worldUp = _up;
		yaw = _yaw;
		pitch = _pitch;
		movementSpeed = _movementSpeed;
		turnSpeed = _turnSpeed;

		front = glm::vec3(0.0f, 0.0f, -1.0f);

		//position = glm::vec3(0.0f, 3.0f, 2.0f);
		lastPosition = position;

		update();
	}

	~GlfwCamera() {

	}

	glm::vec3 getCameraPosition() {
		return position;
	}

	glm::mat4 calculateViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void setLastPosition(glm::vec3 last) {
		lastPosition = last;
	}

	void setXPosition(GLfloat x) {
		position.x = x;
	}

	void setYPosition(GLfloat y) {
		position.y = y;
	}

	void setZPosition(GLfloat z) {
		position.z = z;
	}

	glm::vec3 getLastPosition() {
		return lastPosition;
	}

	void setCurrentPosition(glm::vec3 current) {
		position = current;
	}

	void setToggleGravity(bool g) {
		toggleGravity = g;
	}

	bool getToggleGravity() { return toggleGravity; }

	void bumpCamera(glm::vec3 bump) {
		// Moves the camera by a small amount (collision detection
		lastPosition = position;
		position += bump;
	}

	void keyControl(bool* keys, bool* keyHit, GLfloat deltaTime) {
		GLfloat velocity = movementSpeed * deltaTime;

		if (keys[GLFW_KEY_W]) {
			position += front * velocity;
		} else if (keys[GLFW_KEY_S]) {
			position -= front * velocity;
		}

		if (keys[GLFW_KEY_D]) {
			position += right * velocity;
		} else if (keys[GLFW_KEY_A]) {
			position -= right * velocity;
		}

		if (keys[GLFW_KEY_G]) {
			if (keyHit[GLFW_KEY_G] == false) {
				keyHit[GLFW_KEY_G] = true;
				setToggleGravity(!toggleGravity);
			}
		} else {
			keyHit[GLFW_KEY_G] = false;
		}


		if (keys[GLFW_KEY_SPACE]) {
			bumpCamera(glm::vec3(0.0f, 5.0f, 0.0f) * velocity);
		}
	}

	void mouseControl(GLfloat xChange, GLfloat yChange) {
		//std::cout << "yaw: " << yaw << " pitch: " << pitch << std::endl;
		xChange *= turnSpeed;
		yChange *= turnSpeed;

		yaw += xChange;
		pitch += yChange;

		if (pitch > 89.0f) {
			pitch = 89.0f;
		}

		if (pitch < -89.0f) {
			pitch = -89.0f;
		}

		update();
	}

private:
	// We should be using vec4's for position
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up; // y-axis relative to camera's rotation
	glm::vec3 worldUp; // true global y-axis
	glm::vec3 right;

	glm::vec3 lastPosition;

	GLfloat yaw; // look left/right
	GLfloat pitch; // look up/down
	GLfloat roll; // tilt head to the side

	GLfloat movementSpeed;
	GLfloat turnSpeed;

	bool toggleGravity;

	void update() {
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);

		// cross(a,b) can be visualized using the right hand rule
		// where a is the index finger, and b is the middle finger.
		// the result of cross(a,b) is the thumb
		right = glm::normalize(glm::cross(front, worldUp));

		up = glm::normalize(glm::cross(right, front));


	}
};