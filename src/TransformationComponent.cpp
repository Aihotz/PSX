#include "TransformationComponent.hpp"
#include "GameObject.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "HierarchyManager.hpp"

TransformationComponent::TransformationComponent()
	: mWorldMatrix { glm::identity<glm::mat4>() }, mParent { nullptr }
{
}

void TransformationComponent::SetParent(TransformationComponent* parent)
{
	mParent = parent;

	if (parent != nullptr)
	{
		// now this transformation is a child
		mLocalTransformation = parent->GetWorldTransformation().InverseConcatenate(mWorldTransformation);
	}
	else
	{
		// now this transformation is parent-most
		mLocalTransformation = mWorldTransformation;

		// only parent-most transformations are in the system
		AddToSystem();
	}

	Update();
}

void TransformationComponent::AddToSystem()
{
	HierarchyManager::GetInstance().AddComponent(this);
}

void TransformationComponent::Initialize()
{
	// only parent-most tranformations will be added to the system
	if (GameObject* parent = GetOwner()->GetParent())
	{
		SetParent(parent->GetComponent<TransformationComponent>());
	}
	else
	{
		AddToSystem();
	}
}

void TransformationComponent::Update()
{
	if (mParent != nullptr)
	{
		// this is a child
		mWorldTransformation = mParent->GetWorldTransformation() * mLocalTransformation;
	}
	else
	{
		mWorldTransformation = mLocalTransformation;
	}

	mWorldMatrix = mWorldTransformation.GetMatrix();
}

void TransformationComponent::Shutdown()
{
	RemoveFromSystem();
}

void TransformationComponent::RemoveFromSystem()
{
	HierarchyManager::GetInstance().RemoveComponent(this);
}

void TransformationComponent::RotateAxis(float angle, glm::vec3 axis)
{
	mLocalTransformation.rotation.RotateAxis(angle, axis);
	Update();
}

void TransformationComponent::LookAt(glm::vec3 target)
{
	mLocalTransformation.LookAt(target);
	Update();
}

void TransformationComponent::SetLocalRotation(const Rotation& rotation)
{
	mLocalTransformation.rotation = rotation;
	Update();
}

void TransformationComponent::SetLocalRotation(glm::vec3 forward, glm::vec3 up, glm::vec3 right)
{
	SetLocalRotation(Rotation(forward, up, right));
}

void TransformationComponent::SetLocalPosition(glm::vec3 position)
{
	mLocalTransformation.position = position;
	Update();
}

void TransformationComponent::SetLocalScale(glm::vec3 scale)
{
	mLocalTransformation.scale = scale;
	Update();
}

void TransformationComponent::SetWorldRotation(const Rotation& rotation)
{
	if (mParent == nullptr)
	{
		SetLocalRotation(rotation);
	}
	else
	{
		Transformation temp {};
		temp.rotation = rotation;

		SetLocalRotation(mParent->GetWorldTransformation().InverseConcatenate(temp).rotation);
	}
}

void TransformationComponent::SetWorldRotation(glm::vec3 forward, glm::vec3 up, glm::vec3 right)
{
	SetWorldRotation(Rotation(forward, up, right));
}

void TransformationComponent::SetWorldPosition(glm::vec3 position)
{
	if (mParent == nullptr)
	{	
		SetLocalPosition(position);
	}
	else
	{
		Transformation temp {};
		temp.position = position;

		SetLocalPosition(mParent->GetWorldTransformation().InverseConcatenate(temp).position);
	}
}

void TransformationComponent::SetWorldScale(glm::vec3 scale)
{
	if (mParent == nullptr)
	{
		SetLocalScale(scale);
	}
	else
	{
		Transformation temp {};
		temp.scale = scale;

		SetLocalScale(mParent->GetWorldTransformation().InverseConcatenate(temp).scale);
	}
}

glm::vec3 TransformationComponent::GetLocalPosition() const
{
	return mLocalTransformation.position;
}

glm::vec3 TransformationComponent::GetLocalScale() const
{
	return mLocalTransformation.scale;
}

Rotation TransformationComponent::GetLocalRotation() const
{
	return mLocalTransformation.rotation;
}

glm::vec3 TransformationComponent::GetWorldPosition() const
{
	return mWorldTransformation.position;
}

glm::vec3 TransformationComponent::GetWorldScale() const
{
	return mWorldTransformation.scale;
}

Rotation TransformationComponent::GetWorldRotation() const
{
	return mWorldTransformation.rotation;
}

const Transformation& TransformationComponent::GetLocalTransformation() const
{
	return mLocalTransformation;
}

const glm::mat4& TransformationComponent::GetWorldMatrix() const
{
	return mWorldMatrix;
}

const Transformation& TransformationComponent::GetWorldTransformation() const
{
	return mWorldTransformation;
}