#pragma once

#include <glm/glm.hpp>

struct ManifoldPoints {
	glm::vec3 a, b, normal;
	float depth;
	bool hasCollision;

	ManifoldPoints() :
		a(0.0f),
		b(0.0f),
		normal(0.0f),
		depth(0.0f),
		hasCollision(false)
	{}

	ManifoldPoints(
		glm::vec2 _a,
		glm::vec2 _b
	) : a(_a, 0.0f),
		b(_b, 0.0f),
		hasCollision(true)
	{
		glm::vec2 ba = a - b;
		depth = glm::length(ba);
		if (depth > 0.00001f) {
			normal = glm::vec3(ba / depth, 0.0f);
		} else {
			normal = glm::vec3(0.0f, 1.0f, 0.0f);
			depth = 1;
		}
	}

	ManifoldPoints(
		glm::vec3 _a,
		glm::vec3 _b
	) : a(_a), b(_b), hasCollision(true) {

		glm::vec3 ba = a - b;
		depth = glm::length(ba);

		if (depth > 0.00001f) {
			normal = ba / depth;
		} else {
			normal = glm::vec3(0.0f, 1.0f, 0.0f);
			depth = 1.0f;
		}
	}

	ManifoldPoints(
		glm::vec3 _a,
		glm::vec3 _b,
		glm::vec3 _n,
		float d
	) : a(_a), b(_b), normal(_n), depth(d), hasCollision(true) {}

	ManifoldPoints(
		glm::vec2 _a,
		glm::vec2 _b,
		glm::vec2 _n,
		float d
	) : a(_a, 0.0f), b(_b, 0.0f), normal(_n, 0.0f), 
		depth(d), hasCollision(true) {}
	
	void swapPoints() {
		glm::vec3 t = a;
		a = b; 
		b = t;
		normal = -normal;
	}
};