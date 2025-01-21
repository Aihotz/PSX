#ifndef TRANSFORMATIONCOMPONENT_HPP
#define TRANSFORMATIONCOMPONENT_HPP

#include "Component.hpp"
#include "Transformation.hpp"
#include <glm/glm.hpp>

class TransformationComponent : public Component
{
		friend class GameObject;

		void SetParent(TransformationComponent* parent);

		Transformation			 mLocalTransformation;
		Transformation			 mWorldTransformation;
		glm::mat4				 mWorldMatrix;
		TransformationComponent* mParent;

	public:
		TransformationComponent();

		virtual void AddToSystem() override;
		virtual void Initialize() override;
		virtual void Update() override;
		virtual void Shutdown() override;
		virtual void RemoveFromSystem() override;

		void RotateAxis(float angle, glm::vec3 axis);
		void LookAt(glm::vec3 target);

		void SetLocalRotation(const Rotation& rotation);
		void SetLocalRotation(glm::vec3 forward, glm::vec3 up, glm::vec3 right);
		void SetLocalPosition(glm::vec3 position);
		void SetLocalScale(glm::vec3 scale);

		void SetWorldRotation(const Rotation& rotation);
		void SetWorldRotation(glm::vec3 forward, glm::vec3 up, glm::vec3 right);
		void SetWorldPosition(glm::vec3 position);
		void SetWorldScale(glm::vec3 scale);

		glm::vec3 GetLocalPosition() const;
		glm::vec3 GetLocalScale() const;
		Rotation  GetLocalRotation() const;

		glm::vec3 GetWorldPosition() const;
		glm::vec3 GetWorldScale() const;
		Rotation  GetWorldRotation() const;

		const Transformation& GetLocalTransformation() const;
		const glm::mat4&	  GetWorldMatrix() const;
		const Transformation& GetWorldTransformation() const;
};

#endif