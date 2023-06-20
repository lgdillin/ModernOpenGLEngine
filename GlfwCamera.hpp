#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class GlfwCamera {
public:


	GlfwCamera(
		glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
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

		position = glm::vec3(0.0f, 3.0f, 2.0f);

		update();
	}

	~GlfwCamera() {

	}

	glm::mat4 calculateViewMatrix() {
		return glm::lookAt(position, position + front, up);
	}

	void keyControl(bool* keys, GLfloat deltaTime) {
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

	GLfloat yaw; // look left/right
	GLfloat pitch; // look up/down
	GLfloat roll; // tilt head to the side

	GLfloat movementSpeed;
	GLfloat turnSpeed;

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