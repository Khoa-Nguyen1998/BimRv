// ModelData.h
#pragma once

#include <vector>
#include <string>

namespace DEMO {
	// Define a Point class to represent a 3D point with x, y, z coordinates.
	class Point {
	public:
		double x, y, z;

		Point(double x = 0.0, double y = 0.0, double z = 0.0) : x(x), y(y), z(z) {}
	};

	// Define an Edge class, which is a collection of Points.
	class Edge {
	public:
		std::vector<Point> points;

		// Constructor to initialize an empty list of points
		Edge() = default;
	};

	// Define an EdgeLoop class, which is a collection of Edges.
	class EdgeLoop {
	public:
		std::vector<Edge> edges;

		// Constructor to initialize an empty list of edges
		EdgeLoop() = default;
	};

	// Define a Face class, which is a collection of EdgeLoops.
	class Face {
	public:
		std::vector<EdgeLoop> edgeLoops;

		// Constructor to initialize an empty list of edge loops
		Face() = default;
	};

	// Define a Geometry class, which is a collection of Faces.
	class Geometry {
	public:
		std::vector<Face> faces;

		// Constructor to initialize an empty list of faces
		Geometry() = default;
	};

	// Define an Element class, which contains an id, a category, and a Geometry object.
	class Element {
	public:
		int id;
		std::string category;
		Geometry geometry;

		Element(int id = 0, const std::string& category = "") : id(id), category(category) {}
	};

	// Define a Model class, which is a collection of Elements.
	class Model {
	public:
		std::vector<Element> elements;

		// Constructor to initialize an empty list of elements
		Model() = default;
	};
}