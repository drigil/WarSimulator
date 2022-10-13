#include "ParticleGenerator.h"

// Variables deciding the rate at which new particles are added to the simulation
float particleRate = 10;
float timeToAddParticles = particleRate;
int newParticlesPerCycle = 0;

float lifespan = 100;
float size = 30;
float velocityConst = 0.5;

bool isWind = false;

// The number of threads
#define numThreads 12

float computeArea(Vertex A, Vertex B, Vertex C) {
	float side1 = (A - B).magnitude();
	float side2 = (B - C).magnitude();
	float side3 = (C - A).magnitude();

	float s = (side1 + side2 + side3) / 2;
	float area = sqrt(s * (s - side1) * (s - side2) * (s - side3));

	return area;
}

void moveParticles(float timestep, int lowerIndex, int upperIndex, ParticleGenerator* particleGenerator) {
	
	Vertex averagePosTeam1(0, 0, 0);
	Vertex averagePosTeam2(0, 0, 0);
	int countTeam1 = 0;
	int countTeam2 = 0;


	for (int j = 0; j < particleGenerator->totalCapacity; j++) {
		if (particleGenerator->characterTeam[j] == 0) {
			averagePosTeam1 = averagePosTeam1 + particleGenerator->positionVector[j];
			countTeam1++;
		}
		else {
			averagePosTeam2 = averagePosTeam2 + particleGenerator->positionVector[j];
			countTeam2++;
		}
	}

	averagePosTeam1 = averagePosTeam1 / countTeam1;
	averagePosTeam2 = averagePosTeam2 / countTeam2;

	//printf("Average positions are %f, %f, %f and %f, %f, %f \n", averagePosTeam1.x, averagePosTeam1.y, averagePosTeam1.z, averagePosTeam2.x, averagePosTeam2.y, averagePosTeam2.z);


	for (int i = lowerIndex; i < upperIndex; i++) {

		if (particleGenerator->isDeadVector[i] == true) {
			continue;
		}

		float particleAccX = 0;
		float particleAccY = 0;
		float particleAccZ = 0;

		// Push away from the boundaries
		Vertex position = particleGenerator->positionVector[i];
		float boundaryConstantX = 20;
		float boundaryConstantY = 20;

		if (position.x > 0) {
			particleAccX = particleAccX - (1 / powf(particleGenerator->gridWidth - abs(position.x), 2)) * boundaryConstantX;
		}
		else {
			particleAccX = particleAccX + (1 / powf(particleGenerator->gridWidth - abs(position.x), 2)) * boundaryConstantX;
		}

		if (position.y > 0) {
			particleAccY = particleAccY - (1 / powf(20 - abs(position.y), 2)) * boundaryConstantY;
		}
		else {
			particleAccY = particleAccY + (1 / powf(20 - abs(position.y), 2)) * boundaryConstantY;
		}


		// Push particles towards mean of the other team
		if (particleGenerator->characterTeam[i] == 0) {
			Vertex meanForce = particleGenerator->positionVector[i] - averagePosTeam2;
			meanForce = (meanForce / meanForce.magnitude()) * (1 / powf(meanForce.magnitude(), 1));
			float meanForceConstant = -5.0f;
			meanForce = meanForce * meanForceConstant;

			particleAccX = particleAccX + meanForce.x;
			particleAccY = particleAccY + meanForce.y;
		}

		else if (particleGenerator->characterTeam[i] == 1) {
			Vertex meanForce = particleGenerator->positionVector[i] - averagePosTeam1;
			meanForce = (meanForce / meanForce.magnitude()) * (1 / powf(meanForce.magnitude(), 1));
			float meanForceConstant = -5.0f;
			meanForce = meanForce * meanForceConstant;

			particleAccX = particleAccX + meanForce.x;
			particleAccY = particleAccY + meanForce.y;
		}

		// Inter character forces on the same team
		int gridPosX = particleGenerator->positionVector[i].x * particleGenerator->gridScale + particleGenerator->gridWidth * particleGenerator->gridScale;
		int gridPosY = particleGenerator->positionVector[i].y * particleGenerator->gridScale + particleGenerator->gridHeight * particleGenerator->gridScale;
		for (int xPos = gridPosX - particleGenerator->interParticleForceRange; xPos < gridPosX + particleGenerator->interParticleForceRange; xPos++) {
			for (int yPos = gridPosY - particleGenerator->interParticleForceRange; yPos < gridPosY + particleGenerator->interParticleForceRange; yPos++) {
				for (int particleIndex = 0; particleIndex < particleGenerator->totalCapacity; particleIndex++) {
					// Check whether particle is alive
					if ((particleGenerator->isDeadVector[particleIndex] == true) || ((particleGenerator->isDeadVector[i] == true))) {
						continue;
					}

					// Check if same particle
					if (particleIndex == i) {
						continue;
					}

					// Check whether position is inside grid
					if ((xPos > 0) && (xPos < 2 * particleGenerator->gridWidth * particleGenerator->gridScale) && (yPos > 0) && (yPos < 2 * particleGenerator->gridHeight * particleGenerator->gridScale)) {
						if (particleGenerator->grid[xPos][yPos][particleIndex] == 1) {
							Vertex otherParticlePos = particleGenerator->positionVector[particleIndex];
							Vertex currParticlePos = particleGenerator->positionVector[i];

							// Take damage if enemy is in the same block as you
							if ((abs(xPos - gridPosX) <= 1) && (abs(yPos - gridPosY) <= 1) && (particleGenerator->characterTeam[i] != particleGenerator->characterTeam[particleIndex])) {
								//if (particleGenerator->hasAttacked[particleIndex] == false) { // Check if enemy has already attacked this turn
									float damage = particleGenerator->generateNoise(0, 0.2);
									particleGenerator->healthVector[i] = particleGenerator->healthVector[i] - damage;
									particleGenerator->hasAttacked[particleIndex] = true;
									if (particleGenerator->healthVector[i] <= 0) {
										particleGenerator->isDeadVector[i] = true;
										continue;
									}
							}

							if ((particleGenerator->characterClassType[particleIndex] == 1) && (particleGenerator->characterTeam[i] != particleGenerator->characterTeam[particleIndex])) {

								float damage = particleGenerator->generateNoise(0, 0.2);
								particleGenerator->healthVector[i] = particleGenerator->healthVector[i] - damage;
								particleGenerator->hasAttacked[particleIndex] = true;
								if (particleGenerator->healthVector[i] <= 0) {
									particleGenerator->isDeadVector[i] = true;
									continue;
								}
							}

							Vertex interparticleVector = (currParticlePos - otherParticlePos);

							// Avoidance
							if (particleGenerator->characterTeam[i] == particleGenerator->characterTeam[particleIndex]) {
							//if(particleGenerator->characterClassType[i] != 1) {
								float repulsiveForceConstant = 0.8f;
								Vertex repulsiveForceVector = (interparticleVector / (interparticleVector.magnitude() * powf(interparticleVector.magnitude(), 2))) * repulsiveForceConstant;

								particleAccX = particleAccX + repulsiveForceVector.x;
								particleAccY = particleAccY + repulsiveForceVector.y;

							}

							// Velocity Matching
							if (particleGenerator->characterTeam[i] == particleGenerator->characterTeam[particleIndex]) {
								Vertex vmForce = particleGenerator->velocityVector[particleIndex] - particleGenerator->velocityVector[i];
								float vmForceConstant = 0.5f;
								vmForce = vmForce * vmForceConstant;

								particleAccX = particleAccX + vmForce.x;
								particleAccY = particleAccY + vmForce.y;

							}

							// Centering - Follow the leader
							if ((particleGenerator->characterClassType[particleIndex] == 1) && (particleGenerator->characterClassType[i] == 0) && (particleGenerator->characterTeam[i] == particleGenerator->characterTeam[particleIndex])) {
								float centeringConstant = -0.8f;
								Vertex centeringForce = (interparticleVector / interparticleVector.magnitude()) * interparticleVector.magnitude() * centeringConstant;
								particleAccX = particleAccX + centeringForce.x;
								particleAccY = particleAccY + centeringForce.y;

							}

							//// Centering - Leaders maintain shape
							//if ((particleGenerator->characterClassType[particleIndex] == 1) && (particleGenerator->characterClassType[i] == 1) && (particleGenerator->characterTeam[i] == particleGenerator->characterTeam[particleIndex])) {
							//	float centeringConstant = 0.08f;
							//	Vertex centeringForce = (interparticleVector / interparticleVector.magnitude()) * dist * centeringConstant;
							//	particleAccX = particleAccX + centeringForce.x;
							//	particleAccY = particleAccY + centeringForce.y;
							//}

							// Attractive force - To the other team
							if (particleGenerator->characterTeam[i] != particleGenerator->characterTeam[particleIndex]) {
								float attractiveConstant = -0.1f;
								//Vertex centeringForce = (interparticleVector / interparticleVector.magnitude()) * (1/powf(dist, 2)) * attractiveConstant;
								Vertex centeringForce = particleGenerator->velocityVector[i] * attractiveConstant;

								particleAccX = particleAccX + centeringForce.x;
								particleAccY = particleAccY + centeringForce.y;

							}

						}
					}
				}
			}
		}

		// Avoid obstacles
		for (int j = 0; j < particleGenerator->obstacleList.size(); j++) {
			Vertex obstacleAvoidanceForce = particleGenerator->positionVector[i] - particleGenerator->obstacleList[j].centre;
			obstacleAvoidanceForce = (obstacleAvoidanceForce / obstacleAvoidanceForce.magnitude()) * (1 / powf(obstacleAvoidanceForce.magnitude(), 2));
			float forceConstant = 0.5;
			obstacleAvoidanceForce = obstacleAvoidanceForce * forceConstant;

			particleAccX = particleAccX + obstacleAvoidanceForce.x;
			particleAccY = particleAccY + obstacleAvoidanceForce.y;

		}

		//printf("Particle accelerations are %f, %f and %f \n", particleAccX, particleAccY, particleAccZ);

		Vertex acceleration(particleAccX, particleAccY, particleAccZ);
		if (isWind) {
			acceleration = acceleration + Vertex(0.1, 0, 0);
		}

		// Compute new velocities along the x, y and z positions
		float newVelX = particleGenerator->velocityVector[i].x + acceleration.x * timestep;
		float newVelY = particleGenerator->velocityVector[i].y + acceleration.y * timestep;
		float newVelZ = particleGenerator->velocityVector[i].z + acceleration.z * timestep;

		// Compute new positions along the x,y and z positions
		float newPosX = particleGenerator->positionVector[i].x + ((particleGenerator->velocityVector[i].x + newVelX) / 2) * timestep;
		float newPosY = particleGenerator->positionVector[i].y + ((particleGenerator->velocityVector[i].y + newVelY) / 2) * timestep;
		float newPosZ = particleGenerator->positionVector[i].z + ((particleGenerator->velocityVector[i].z + newVelZ) / 2) * timestep;

		float posX = particleGenerator->positionVector[i].x;
		float posY = particleGenerator->positionVector[i].y;
		float posZ = particleGenerator->positionVector[i].z;

		float velX = particleGenerator->velocityVector[i].x;
		float velY = particleGenerator->velocityVector[i].y;
		float velZ = particleGenerator->velocityVector[i].z;

		// If particle outside arena, kill it
		if ((newPosX < -particleGenerator->gridWidth) || ((newPosX > particleGenerator->gridWidth)) || (newPosY < -particleGenerator->gridHeight) || (newPosY > particleGenerator->gridHeight)) {
			particleGenerator->isDeadVector[i] = true;
			return;
		}

		particleGenerator->updateGridPosition(i, newPosX, newPosY, newPosZ);

		// Replace the new positions and velocities
		particleGenerator->positionVector[i].x = newPosX;
		particleGenerator->positionVector[i].y = newPosY;
		particleGenerator->positionVector[i].z = newPosZ;

		particleGenerator->velocityVector[i].x = newVelX;
		particleGenerator->velocityVector[i].y = newVelY;
		particleGenerator->velocityVector[i].z = newVelZ;

	}
}

ParticleGenerator::ParticleGenerator() {
	this->numParticles = 0;
	this->totalCapacity = 0;
	srand(static_cast <unsigned> (time(0)));
}

ParticleGenerator::ParticleGenerator(int numParticles, Vertex generatorPosition, Vertex generatorVelocity) {
	this->numParticles = numParticles;
	this->totalCapacity = numParticles * 8;
	this->generatorPosition = generatorPosition;
	this->generatorVelocity = generatorVelocity;
	srand(static_cast <unsigned> (time(0)));
}

void ParticleGenerator::setParams(int numParticles, Vertex generatorPosition, Vertex generatorVelocity) {
	this->numParticles = numParticles;
	this->totalCapacity = numParticles;
	this->generatorPosition = generatorPosition;
	this->generatorVelocity = generatorVelocity;

	for (int i = 0; i < 2 * gridWidth * gridScale; i++) {
		std::vector<std::vector<int>> vec1;
		for (int j = 0; j < 2 * gridHeight * gridScale; j++) {
			std::vector<int> vec2;
			for (int k = 0; k < totalCapacity; k++) {
				vec2.push_back(0);
			}
			vec1.push_back(vec2);
		}
		grid.push_back(vec1);
	}
}

void ParticleGenerator::setCollisionVector(std::vector<TriangularFace>& collisionVector) {
	this->collisionVector = collisionVector;
}

void ParticleGenerator::updateGridPosition(int index, float newPosX, float newPosY, float newPosZ) {
	int x = (positionVector[index].x * gridScale) + (gridWidth*gridScale);
	int y = (positionVector[index].y * gridScale) + (gridHeight*gridScale);

	int newX = (newPosX * gridScale) + (gridWidth * gridScale);
	int newY = (newPosY * gridScale) + (gridHeight * gridScale);

	grid[x][y][index] = 0;
	grid[newX][newY][index] = 1;

}

void ParticleGenerator::generateParticles(int generatorType) {
	// Point source
	if (generatorType == 0) {
		for (int particleIndex = 0; particleIndex < totalCapacity; particleIndex++) {
			if (particleIndex < numParticles) {
				positionVector.push_back(generateNoiseVertex(-1.0f, 1.0f) + generatorPosition);
				velocityVector.push_back(Vertex(0, 1.0f, 0) + generatorVelocity);
				healthVector.push_back(100);
				hasAttacked.push_back(false);
				isDeadVector.push_back(false);
				rotationVector.push_back(0);
				walkingAnimationState.push_back(rand() % 20);
				//rotationVector.push_back(180);
			}
			else {
				positionVector.push_back(generateNoiseVertex(-1.0f, 1.0f) + generatorPosition);
				velocityVector.push_back(Vertex(0, 1.0f, 0) + generatorVelocity);
				healthVector.push_back(100);
				hasAttacked.push_back(false);
				isDeadVector.push_back(true);
				rotationVector.push_back(0);
				walkingAnimationState.push_back(rand() % 20);
				//rotationVector.push_back(180);


				deadParticles.push(particleIndex);
			}
		}
	}

	// Opposing teams
	else if (generatorType == 1) {
		for (int particleIndex = 0; particleIndex < totalCapacity; particleIndex++) {
			
			int teamType = rand() % 2;
			int classType = rand() % 20;

			if (particleIndex < numParticles) {
				positionVector.push_back(generateNoiseVertex(-10.0f, 10.0f) + Vertex(10 * (2*teamType - 1), 0, 0));
				velocityVector.push_back(Vertex(-(2*teamType - 1) * velocityConst, 0, 0) + generatorVelocity);
				healthVector.push_back(100);
				isDeadVector.push_back(false);
				hasAttacked.push_back(false);
				rotationVector.push_back(0.05 * generateNoise(-180, 180));
				walkingAnimationState.push_back(rand() % 20);
				characterTeam.push_back(teamType);
				characterClassType.push_back((classType == 0) ? 1 : 0); // 0: Knife man, 1: Leader
				
			}
			else {
				positionVector.push_back(generateNoiseVertex(-10.0f, 10.0f) + Vertex(10 * (2*teamType - 1), 0, 0));
				velocityVector.push_back(Vertex(-(2*teamType - 1) * velocityConst, 0.0f, 0) + generatorVelocity);
				healthVector.push_back(100);
				hasAttacked.push_back(false);
				isDeadVector.push_back(true);
				rotationVector.push_back(0.05 * generateNoise(-180, 180));
				walkingAnimationState.push_back(rand() % 20);
				characterTeam.push_back(teamType);
				characterClassType.push_back((classType == 0) ? 1 : 0); // 0: Knife man, 1: Leader

				deadParticles.push(particleIndex);
			}
		}
	}
}

void ParticleGenerator::moveParticlesWithThreads(float timestep) {

	generatorPosition = generatorPosition + (generatorVelocity * timestep);
	std::thread threadHandleVector[numThreads];

	for (int i = 0; i < numThreads; i++) {
		int chunkSize = totalCapacity / numThreads;
		int lower = i * chunkSize;
		int upper = (i != (numThreads - 1)) ? ((i + 1) * chunkSize) : totalCapacity;

		threadHandleVector[i] = std::thread(moveParticles, timestep, lower, upper, this);
	}
	
	for (int i = 0; i < numThreads; i++) {
		threadHandleVector[i].join();
	}

	// Add new points to replace the dead ones
	timeToAddParticles = timeToAddParticles - timestep;

	if (timeToAddParticles < 0) {
		timeToAddParticles = particleRate;
		
		for (int i = 0; i < newParticlesPerCycle; i++) {
			int index = this->deadParticles.front();
			this->deadParticles.pop();

			this->positionVector[index] = this->generateNoiseVertex(-5.0f, 5.0f) + Vertex(5 * (2*characterTeam[index] - 1), 0, 0);
			this->velocityVector[index] = this->generatorVelocity + Vertex(-(2*characterTeam[index] - 1) * velocityConst, 0, 0);
			this->healthVector[index] = 100;
			this->hasAttacked[index] = false;
			this->isDeadVector[index] = false;
		}
	}

	//printf("Thread waiting completed \n");

}


float ParticleGenerator::generateNoise(float lowerLimit, float upperLimit) {
	float num = lowerLimit + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (upperLimit - lowerLimit)));
	return num;
}

Vertex ParticleGenerator::generateNoiseVertex(float lowerLimit, float upperLimit) {
	float noise1 = generateNoise(lowerLimit, upperLimit);
	float noise2 = generateNoise(lowerLimit, upperLimit);
	float noise3 = generateNoise(lowerLimit, upperLimit);

	return Vertex(noise1, noise2, 0);

}

float ParticleGenerator::linearInterpolate(float lower, float upper, float t) {
	return lower + t * (upper - lower);
}