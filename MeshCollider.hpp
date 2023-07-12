#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RectangularPrism.hpp"
#include "Simplex.hpp"
#include "ManifoldPoints.hpp"

#define GJK_EPA_MAX_ITER 32

class MeshCollider {
public:
	MeshCollider() {}

	MeshCollider(RectangularPrism *_rp) {
		
	}

	~MeshCollider() {}

	ManifoldPoints epa(
		Simplex &simplex,
		RectangularPrism *rp1,
		RectangularPrism *rp2
	) {
		std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
		std::vector<unsigned int> faces(rp1->getIndexArray().size());
		std::copy(rp1->getIndexArray().begin(), rp1->getIndexArray().end(),
			faces.begin());

		// list: vec4(normal, distance), index:min distance
		auto [normals, minFace] = getFaceNormals(polytope, faces);

		glm::vec3 minNormal;
		float minDistance = FLT_MAX;

		size_t iterations = 0;
		while (minDistance == FLT_MAX) {
			minNormal = glm::vec3(
				normals[minFace].x,
				normals[minFace].y,
				normals[minFace].z);
			minDistance = normals[minFace].w;

			if (iterations++ > GJK_EPA_MAX_ITER) {
				break;
			}

			glm::vec3 supportVec = support(rp1, rp2, minNormal);
			float sDistance = glm::dot(minNormal, supportVec);

			if (abs(sDistance - minDistance) > 0.001f) {
				minDistance = FLT_MAX;

				std::vector<std::pair<size_t, size_t>> uniqueEdges;

				for (size_t i = 0; i < normals.size(); ++i) {
					if (sameDirection(normals[i], supportVec)) {
						size_t f = i * 3;

						addIfUniqueEdge(uniqueEdges, faces, f, f + 1);
						addIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
						addIfUniqueEdge(uniqueEdges, faces, f + 2, f);

						faces[f + 2] = faces.back(); faces.pop_back();
						faces[f + 1] = faces.back(); faces.pop_back();
						faces[f] = faces.back(); faces.pop_back();

						normals[i] = normals.back(); normals.pop_back();

						--i;
					}
				}


				if (uniqueEdges.size() == 0) {
					break;
				}

				// Now that we have a list of unique edges, we can add the new
				// faces to a list and add the supporting point to the polytope.
				// Storing the new faces in their own list allows us to
				// calculate only the normals of these faces
				std::vector<unsigned int> newFaces;
				for (auto [edgeIndex1, edgeIndex2] : uniqueEdges) {
					newFaces.push_back(edgeIndex1);
					newFaces.push_back(edgeIndex2);
					newFaces.push_back(polytope.size());
				}
				polytope.push_back(supportVec);

				auto [newNormals, newMinFace] = getFaceNormals(polytope, newFaces);

				// after we calculate the new normals, we need to find the new
				// closest face. To eke out a little more performance, we will
				// only iterate over the old normals, then we'll compare the closest
				// one to the closest face of the new normals. Finally, we can add
				// these new faces and normals to the end of their respective lists
				float oldMinDistance = FLT_MAX;
				for (size_t i = 0; i < normals.size(); ++i) {
					if (normals[i].w < oldMinDistance) {
						oldMinDistance = normals[i].w;
						minFace = i;
					}
				}

				if (newNormals[newMinFace].w < oldMinDistance) {
					minFace = newMinFace + normals.size();
				}

				faces.insert(faces.end(), newFaces.begin(), newFaces.end());
				normals.insert(normals.end(), newNormals.begin(), newNormals.end());
			}
		}

		if (minDistance == FLT_MAX) {
			//return;
		}

	

		// Once a supporting point isn't found further from the closest face,
		// we'll return that face's normal and its distance
		ManifoldPoints points;
		points.normal = glm::vec3(0.0f);
		//points.penetrationDepth = minDistance + 0.001f;
		points.hasCollision = true;
		return points;
	}

	// getFaceNormals() is just a slightly more complex version of a 2d implementaiton
	// instead of i and j, we now get 3 vertices by first looking up their index
	// in the faces list.  in 3d the normal is found by taking the cross
	// product of the vectors between the face's vertices.  the winding
	// order is now controlled by the index, instead of where we put
	// some negative sign.  even though its well defined, we don't check
	// when adding new faces, so we still need the check here. determining
	// the winding involves find the normal so there is no reason not to
	// have this check here
	std::pair<std::vector<glm::vec4>, size_t> getFaceNormals(
		std::vector<glm::vec3> &polytope,
		std::vector<unsigned int> faces
	) {
		std::vector<glm::vec4> normals;
		size_t minTriangle = 0;
		float minDistance = FLT_MAX;

		for (size_t i = 0; i < faces.size(); i += 3) {
			glm::vec3 a = polytope[faces[i]];
			glm::vec3 b = polytope[faces[i + 1]];
			glm::vec3 c = polytope[faces[i + 2]];

			glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
			float distance = glm::dot(normal, a);

			if (distance < 0) {
				normal *= -1;
				distance *= -1;
			}

			normals.emplace_back(normal, distance);

			if (distance < minDistance) {
				minTriangle = i / 3;
				minDistance = distance;
			}
		}
	}

	// addIfUniqueEdge() tests if the reverse of an edge already exists in
	// the list and if so, removes it.  if you look at how the winding works out,
	// if a neighboring face shares an edge, it will be in reverse order.
	// remember, we only want to store the eges that we are going to save
	// because every edge gets removed first, then we repair
	void addIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>> &edges,
		std::vector<unsigned int> &faces,
		size_t a, 
		size_t b
	) {

		//    0--<--3
		//   / \ B /   B: 0-2
		//  / A \ /    A: 2-0
		// 1-->--2
		auto reverse = std::find(
			edges.begin(),
			edges.end(),
			std::make_pair(faces[b], faces[a])
		);

		if (reverse != edges.end()) {
			edges.erase(reverse);
		} else {
			edges.emplace_back(faces[a], faces[b]);
		}
	}

	// Finding the furthest vertex is done by iterating the set of vertices,
	// and finding the one with the largest dot product in a given direction
	// we can accomplish this using:
	// max(dot(direction, (A - B)))
	// we can distribute the dot product into the difference
	// max(dot(direction,B) - dot(direction,B))
	// and distribute the max function
	// max(dot(direction,A)) - max(dot(-direction,B))
	// if you look at the second term, we have reversed the direction for
	// dot(-direction,B), as we want to subtract the least extreme vertex 
	// from B, to subtract from the most extreme vertex from A.
	// These vertices are called supporting points.
	glm::vec3 findFurthestPoint(RectangularPrism *rp, glm::vec3 direction) {
		// This function finds the support point in the given direction

		direction = glm::normalize(direction);
		glm::vec3 maxPoint;
		float maxDistance = -FLT_MAX; // least float possible

		for (int i = 0; i < 8; ++i) {
			glm::vec3 vertex = rp->getBBVertex(i);

			// take the dot product of every vector and our random direction
			float distance = glm::dot(vertex, direction);

			// pick the vector that is most similar to the random direction vec
			if (distance > maxDistance) {
				maxDistance = distance;
				maxPoint = vertex;
			}
		}

		return maxPoint;
	}

	glm::vec3 support(
		RectangularPrism *rp1,
		RectangularPrism *rp2,
		glm::vec3 direction) {
		// This function takes two collidable object and a given direction,
		// and return the vertex on the minkowski difference

		return findFurthestPoint(rp1, direction)
			- findFurthestPoint(rp2, -direction);
	}


	// We only have the support function which gives us one vertex at a time
	// (we are not using the whole polygon, just the hull)
	// we need to iteravely search and build up hwat's referred to as a 
	// 'simplex' around the origin
	// a simplex is a shape that has n+1 number of vertices, with n being
	// the number of dimensions in the space we are working in.
	//
	// we use these simplex to perform simple tests to find out which
	// vertex, edge or face is closest to the origin
	// depending on which feature is closest, we will remove, add, or swap
	// points to make the simplex closer to the origin.
	// we get the vertices for the simplex from our support() function,
	// so we need to find the direction to the origin from the closest feature.
	// if we find that the closest feature is already the closest possible,
	// but the origin is not contained, we know there is no collision.
	bool gjk(RectangularPrism *rp1, RectangularPrism *rp2) {
		// The goal of the GJK algorith is to determine if the origin
		// lies within the minkowski difference

		// we choose the unitX vector for no reason
		glm::vec3 unitX = glm::vec3(1.0f, 0.0f, 0.0f);

		// Get initial support in any direction
		glm::vec3 supportVec = support(rp1, rp2, unitX);

		// now that we have one point, we can add it to the simplex,
		// and set our search direction towards the origin
		Simplex points; // Simplex is an array of points, max count is 4
		points.pushFront(supportVec);
		glm::vec3 direction = -supportVec; // New direction is towards the origin

		// In a loop, we will add another point
		while (true) {
			// Exit condition for this loop is that the new point
			// is not in front of the search direction.
			// this would exit if the support function returns a point
			// that was already the furthest along the direciton.

			supportVec = support(rp1, rp2, direction);
			

			if (glm::dot(supportVec, direction) <= 0) {
				return false; // no collision
			}

			points.pushFront(supportVec);

			// now that we have a line, we will feed it into a function
			// that updates the simplex in the search direction, and will
			// return true/false to signify a collision
			if (nextSimplex(points, direction)) {
				return true;
			}
		}
	}

	bool nextSimplex(Simplex &points, glm::vec3 &direction) {
		switch (points.size()) {
		case 2: return line(points, direction);
		case 3: return triangle(points, direction);
		case 4: return tetrahedron(points, direction);
		}
		return false; // this should never execute
	}

	// There are 3 possible regions that the origin could be in, but 
	// realistically only 2. we start with point B, and search in the direction
	// of A. this leaves us with one check between the vector AB, and AO.
	// if AO is inside the region, we move on. if not, we will come back
	// to the line case, but B will be replaced. 
	bool line(Simplex &points, glm::vec3 &direction) {
		glm::vec3 a = points[0];
		glm::vec3 b = points[1];

		glm::vec3 ab = b - a;
		glm::vec3 ao = -a;

		if (sameDirection(ab, ao)) {
			direction = glm::cross(glm::cross(ab, ao), ab);
		} else {
			points = { a };
			direction = ao;
		}

		return false;
	}

	// The triangle case has 7 regions, but again we can cull out some impossibilities.
	// the new point we added was A, meaning that the origin cannot be in the direction
	// of the BC face, so we have 4 regions we need to check. if the origin is outside
	// the triangle on the AC face, we will check if its also in the direction of AC. 
	// if it is, the we will remove B from the simplex and move on. if not, 
	// we will do a line case between AB. if the origin was not in the direction
	// of the AC face, we will check the AB face. if its there, we will do the same
	// line case between AB.
	// Finally, if both check fail, then it is inside the triangle.
	// so we need to check if its above or below the triangle (tetrahedron)
	bool triangle(Simplex &points, glm::vec3 &direction) {
		glm::vec3 a = points[0];
		glm::vec3 b = points[1];
		glm::vec3 c = points[2];

		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		glm::vec3 ao = -a; // The vector A that points back at the origin

		glm::vec3 abc = glm::cross(ab, ac);

		if (sameDirection(glm::cross(abc, ac), ao)) {
			if (sameDirection(ac, ao)) {
				points = { a,c };
				direction = glm::cross(glm::cross(ac, ao), ac);
			} else {
				return line(points = { a,b }, direction);
			}
		
		} else {
			if (sameDirection(glm::cross(ab, abc), ao)) {
				return line(points = { a,b }, direction);
			} else {
				if (sameDirection(abc, ao)) {
					direction = abc;
				} else {
					points = { a,c,b };
					direction = -abc;
				}
			}
		}

		return false;
	}


	// the tetrahedron case is the most complex, but made of triangle cases.
	// we don't need to test if the origin is below the tetrahedron for the same
	// reasons as before.  we onnly need to determine which face, if any,
	// the origin is in the direction of.  if there is one, we will go back
	// to the triangle case, but with that face as the Simplex. but if not,
	// then we know we must be inside the tetrahedron, and the function will
	// return true (collision detected). with that final case, the GJK is done.
	bool tetrahedron(Simplex &points, glm::vec3 &direction) {
		glm::vec3 a = points[0];
		glm::vec3 b = points[1];
		glm::vec3 c = points[2];
		glm::vec3 d = points[3];

		glm::vec3 ab = b - a;
		glm::vec3 ac = c - a;
		glm::vec3 ad = d - a;
		glm::vec3 ao = -a;

		glm::vec3 abc = glm::cross(ab, ac);
		glm::vec3 acd = glm::cross(ac, ad);
		glm::vec3 adb = glm::cross(ad, ab);

		if (sameDirection(abc, ao)) {
			return triangle(points = { a,b,c }, direction);
		}

		if (sameDirection(acd, ao)) {
			return triangle(points = { a,c,d }, direction);
		}

		if (sameDirection(adb, ao)) {
			return triangle(points = { a,d,b }, direction);
		}

		return true;
	}

	bool sameDirection(glm::vec3 direction, glm::vec3 ao) {
		return glm::dot(direction, ao) > 0;
	}

private:

};