#pragma once
#include "stdafx.h"
#include "Node.h"

struct SingleLight
{
	glm::vec4 color;
	float ambientLevel;
	float diffuseLevel;
	float specularLevel;
	float specularPower;
	float radius;
	glm::vec3 position;
	SingleLight(const glm::vec3& position, const glm::vec4& color, float ambientLevel, float diffuseLevel, float specularLevel, float specularPower, float radius ) :
		position(position), color(color), ambientLevel(ambientLevel), diffuseLevel(diffuseLevel), specularLevel(specularLevel), specularPower(specularPower), radius(radius) {}
};

class LightNode : public Node 
{
	// light with a position in local space which transformed by the current inherited transform in the graph
	// light affects all nodes below it in the graph, to a maximum of four lights.
public:
	SingleLight light;
	LightNode(const glm::vec3 &position, const glm::vec4& color, float ambientLevel, float diffuseLevel, float specularLevel, float specularPower, float radius) :
		light(SingleLight(position, color, ambientLevel, diffuseLevel, specularLevel, specularPower, radius)) {}
	virtual void Accept(Visitor* v);
};