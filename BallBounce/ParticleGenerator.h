#pragma once

#include <vector>
#include "Vertex.h"
#include <ctime>
#include <queue>
#include <thread>
#include "Face.h"

class ParticleGenerator
{
	public:
		int totalCapacity;
		int numParticles;
		Vertex generatorPosition;
		Vertex generatorVelocity;

		int gridHeight = 50;
		int gridWidth = 50;
		float gridScale = 1.0f;
		int interParticleForceRange = 3.0f;

		std::vector<std::vector<std::vector<int>>> grid;

		std::vector<Obstacle> obstacleList;

		std::vector<Vertex> positionVector; // Contains the positions of all the points
		std::vector<Vertex> velocityVector; // Contains the velocities of all the points
		std::vector<float> healthVector; // Contains the lifespans of all the particles
		std::vector<float> rotationVector; // Contains the rotation in degrees of each particle
		std::vector<bool> isDeadVector; // If particle is dead or not
		std::vector<int> walkingAnimationState; // Contains the state of the walking animation
		std::vector<int> characterClassType; // Contains the character class - 0:Knife class
		std::vector<int> characterTeam; // Contains the character class
		std::vector<bool> hasAttacked;

		std::vector<TriangularFace> collisionVector; // Contains the triangles the points can collide with

		std::queue<int> deadParticles; // Keep track of all the dead particles in the scene

		ParticleGenerator();
		ParticleGenerator(int numParticles, Vertex generatorPosition, Vertex generatorVelocity);
		void setParams(int numParticles, Vertex generatorPosition, Vertex generatorVelocity);
		void setCollisionVector(std::vector<TriangularFace>& collisionVector);
		void generateParticles(int generatorType);
		void moveParticlesWithThreads(float timestep);

		void updateGridPosition(int index, float newPosX, float newPosY, float newPosZ);

		void setObstacleList(std::vector<Obstacle> obstacleList) {
			this->obstacleList = obstacleList;
		}

		float generateNoise(float lowerLimit, float upperLimit);
		float linearInterpolate(float lower, float upper, float t);
		Vertex generateNoiseVertex(float lowerLimit, float upperLimit);


};



struct thread_data
{
	float timestep;
	int lowerIndex;
	int upperIndex;
	ParticleGenerator *particleGenerator;
};


