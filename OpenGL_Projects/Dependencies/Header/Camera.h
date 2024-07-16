#pragma once

#include<GL/glew.h>
#include<glm.hpp>
#include<gtc/matrix_transform.hpp>

#include<vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default Camera Values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
	public:
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;

		// Euler Angles
		float Yaw;
		float Pitch;

		// Camera Options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		// Constructor with Vectors
		Camera() : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
		{
			Position = glm::vec3(0.0f, 0.0f, 5.0f);
			WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
			Yaw = YAW;
			Pitch = PITCH;
			updateCameraVectors();
		}

		glm::mat4 GetViewMatrix()
		{
			return glm::lookAt(Position, Position + Front, Up);
		}

		void ProcessKeyboard(Camera_Movement direction, float deltaTime)
		{
			float velocity = MovementSpeed * deltaTime;
			if (direction == FORWARD)
			{
				if (Position.z > 2) Position += Front * velocity;
			}
			if (direction == BACKWARD)
			{
				if (Position.z < 5) Position -= Front * velocity;
			}
			if (direction == LEFT)
			{
				if (Position.x > -2)Position -= Right * velocity;
			}
			if (direction == RIGHT)
			{
				if (Position.x < 2) Position += Right * velocity;
			}
			if (direction == UP)
			{
				if (Position.y < 2) {
					Position += Up * velocity;
				}
			}
			if (direction == DOWN)
			{
				if (Position.y > -0.3) {
					Position -= Up * velocity;
				}
			}
		}

		void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
		{
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}

			// update Front, Right and Up Vectors using the updated Euler angles
			updateCameraVectors();
		}

		void ProcessMouseScroll(float yoffset)
		{
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 45.0f)
				Zoom = 45.0f;
		}

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};


