#include "Entity.h"
#include <iostream>

void Entity::SetPosition(const glm::vec3& newPosition)
{
	m_position = newPosition;
}
void Entity::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void Entity::SetRotation(const glm::fquat& newRotation)
{
	m_rotation = newRotation;
}

void Entity::SetRotation(const glm::vec3& newRotation)
{
	m_rotation = glm::fquat(newRotation);
}
void Entity::SetRotation(float x, float y, float z)
{
	m_rotation = glm::fquat(glm::vec3(x, y, z));
}

void Entity::LookAt(const glm::vec3& target, const glm::vec3& up)
{
	m_rotation = glm::quatLookAt(glm::normalize(target - m_position), up);
}

const glm::vec3& Entity::GetPosition()
{
	return m_position;
}

const glm::fquat& Entity::GetRotation()
{
	return m_rotation;
}

glm::vec3 Entity::Forward()
{
	return m_rotation * glm::vec3(0, 0, -1);
}

glm::vec3 Entity::Right()
{
	return m_rotation * glm::vec3(1, 0, 0);
}

glm::vec3 Entity::Up()
{
	return m_rotation * glm::vec3(0, 1, 0);
}