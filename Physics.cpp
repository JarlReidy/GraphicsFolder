#include "stdafx.h"
#include "Physics.h"
#include "timer.h"



std::vector<ContactInfo> Physics::CollideWithWorld(const glm::vec3& spherePos, float sphereRad) 
{
	std::vector<ContactInfo> contacts;
	
	for (auto c : staticColliders)
	{
		
		glm::vec3 normal;
		float depth;
		if (c->Collide(spherePos, sphereRad, normal, depth))//checking if current pos has collide
		{
			contacts.push_back(ContactInfo{ normal, depth });
			collisionTests++;
		}
	}
	return contacts;
}



void Physics:: Update(float deltaTime)
{
	collisionTests = 0;

	for (auto p : projectiles)
	{
		//update position and velocity
		p->pos += p->vel * deltaTime;
		p->vel += gravity * deltaTime;
		std::vector<ContactInfo> contacts = CollideWithWorld(p->pos, p->radius);
		for (auto c : contacts)
		{
			//collision?
			p->pos += c.normal * c.depth;
			if (glm::dot(p->vel, c.normal) < 0.0f)
			{
				p->vel -= c.normal * dot(c.normal, p->vel) * (1.0f + p->restitution);
			
			}
		}
	}
	
}