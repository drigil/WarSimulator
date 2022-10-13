#include <iostream>

#include <vector>
#include <Windows.h>

#include "simulationVariables.h"
#include "Ball.h"
#include "BoundingBox.h"
#include "ParticleGenerator.h"
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>

#include <chrono>
#include <thread>
#include <string>

BoundingBox boundingBox; //Bounding Box in the scene

// Time related variables
float totalTime = 0.0f; // Simulation elapsed time
std::chrono::steady_clock::time_point startTime, endTime;
float elapsedTime; // Display elapsed time

// Walking time
float walkUpdateTime = 0.0;
float walkSimTime = 0;

// Ball related variables
int numBalls = 1;
std::vector<Ball> ballArray; //Vector holding all the current balls in the scene

// Particle Generator
ParticleGenerator particleGenerator;

// Set of collision object
std::vector<TriangularFace> collisionObjects;

// Sprite animation management
GLuint walkingTexture; // Array of textures
std::vector<unsigned char*> walkingData; // Array of image data
std::vector<int> walkingWidth;
std::vector<int> walkingHeight;
std::vector<int> walkingNrChannels;

std::vector<unsigned char*> knifeAttackData; // Array of image data
std::vector<int> knifeAttackWidth;
std::vector<int> knifeAttackHeight;
std::vector<int> knifeAttackNrChannels;

std::vector<unsigned char*> rifleData; // Array of image data
std::vector<int> rifleWidth;
std::vector<int> rifleHeight;
std::vector<int> rifleNrChannels;

std::vector<unsigned char*> rifleShootData; // Array of image data
std::vector<int> rifleShootWidth;
std::vector<int> rifleShootHeight;
std::vector<int> rifleShootNrChannels;


unsigned char* backgroundData; // Array of image data
int backgroundWidth;
int backgroundHeight;
int backgroundNrChannels;

unsigned char* deathData; // Array of image data
int deathWidth;
int deathHeight;
int deathNrChannels;

std::vector<unsigned char*> obstacleData; // Array of image data
std::vector<int> obstacleWidth;
std::vector<int> obstacleHeight;
std::vector<int> obstacleNrChannels;


std::vector<Obstacle> constructObstacleList(int numObstacles) {
    std::vector<Obstacle> obstacleList;
    for (int i = 0; i < numObstacles; i++) {
        int obstacleType = particleGenerator.generateNoise(0, 19);
        //Vertex position = particleGenerator.generateNoiseVertex(-max(particleGenerator.gridHeight, particleGenerator.gridWidth), max(particleGenerator.gridHeight, particleGenerator.gridWidth));
        Vertex position = particleGenerator.generateNoiseVertex(-30, 30);

        float radius = 3;
        Obstacle obstacle(position, 3, obstacleType);
        obstacleList.push_back(obstacle);
    }
    return obstacleList;
}

void constructBoundingBox() {

    // Construct a bounding box
    std::vector<Face> faces;

    // Face 1 - Bottom
    Vertex v0(-10, -10, -10);
    Vertex v1(10, -10, -10);
    Vertex v2(10, -10, 10);
    Vertex v3(-10, -10, 10);
    std::vector<Vertex> vertices_f0{ v0, v1, v2, v3 };

    Face f0(vertices_f0);

    faces.push_back(f0);

    // Face 2 - left
    Vertex v4(-10, -10, -10);
    Vertex v5(-10, 10, -10);
    Vertex v6(-10, 10, 10);
    Vertex v7(-10, -10, 10);
    std::vector<Vertex> vertices_f1{ v4, v5, v6, v7 };

    Face f1(vertices_f1);

    faces.push_back(f1);

    // Face 3 - right
    Vertex v8(10, 10, -10);
    Vertex v9(10, -10, -10);
    Vertex v10(10, -10, 10);
    Vertex v11(10, 10, 10);
    std::vector<Vertex> vertices_f2{ v8, v9, v10, v11 };

    Face f2(vertices_f2);

    faces.push_back(f2);

    // Face 4 - top
    Vertex v12(-10, 10, -10);
    Vertex v13(10, 10, -10);
    Vertex v14(10, 10, 10);
    Vertex v15(-10, 10, 10);
    std::vector<Vertex> vertices_f3{ v12, v13, v14, v15 };

    Face f3(vertices_f3);

    faces.push_back(f3);

    // Face 5 - front
    Vertex v16(-10, 10, 10);
    Vertex v17(10, 10, 10);
    Vertex v18(10, -10, 10);
    Vertex v19(-10, -10, 10);
    std::vector<Vertex> vertices_f4{ v16, v17, v18, v19 };

    Face f4(vertices_f4);

    faces.push_back(f4);

    // Face 6 - back
    Vertex v20(-10, 10, -10);
    Vertex v21(-10, -10, -10);
    Vertex v22(10, -10, -10);
    Vertex v23(10, 10, -10);
    std::vector<Vertex> vertices_f5{ v20, v21, v22, v23 };

    Face f5(vertices_f5);

    faces.push_back(f5);

    boundingBox.setFaces(faces);
}

float getNoise() {
    float noise = rand() % 200 + 1;
    noise = noise - 100;
    noise = noise / 10.0f;

    return noise;
}

void constructBall() {

    // Spawn a ball
    // Set its position, velocity, radius and mass
    float posX = 0;
    float posY = 0;
    float posZ = 0;

    float velX = 6;
    float velY = 12;
    float velZ = 5;
        
    float radius = 1.0f;
    float mass = 20;

    if (numBalls == 1) {
        Ball ball;
        ball.setParams(0, posX, posY, posZ, velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball);
    }

    else if (numBalls == 2) {
        Ball ball;
        velX = 5;
        velY = 6;
        velZ = 0;

        ball.setParams(0, posX, posY, posZ, velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball);

        Ball ball2;
        ball2.setParams(1, posX + 5, posY, posZ, -velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball2);
    }

    else {
        Ball ball;
        ball.setParams(0, posX, posY, posZ, velX, velY, velZ, radius, mass, boundingBox, &ballArray);
        ballArray.push_back(ball);

        for (int i = 1; i < numBalls; i++) {
            Ball ball_i;
            ball_i.setParams(i, posX + getNoise(), posY + getNoise(), posZ + getNoise(), velX + getNoise(), velY + getNoise(), velZ + getNoise(), radius, mass, boundingBox, &ballArray);
            ballArray.push_back(ball_i);
        }
    }

}

void constructCollisionObject() {
    std::vector<Vertex> faceVertices;
    Vertex shift(7, 0, 0);
    float height = 2;
    float side = 7;
    Vertex v1(-side, height, -side);
    Vertex v2(-side, height, side);
    Vertex v3(side, height, side);
    faceVertices.push_back(v1 + shift);
    faceVertices.push_back(v2 + shift);
    faceVertices.push_back(v3 + shift);

    std::vector<Vertex> faceVertices2;
    Vertex v4(-side, height, -side);
    Vertex v5(side, height, side);
    Vertex v6(side, height, -side);
    faceVertices2.push_back(v4 + shift);
    faceVertices2.push_back(v5 + shift);
    faceVertices2.push_back(v6 + shift);


    TriangularFace face1(faceVertices);
    TriangularFace face2(faceVertices2);

    collisionObjects.push_back(face1);
    collisionObjects.push_back(face2);

}

void constructParticleGenerator() {
    int numParticles = 150;
    Vertex position = Vertex(0, 0.0f, 0);
    Vertex velocity = Vertex(0.0f, 0, 0);

    particleGenerator.setParams(numParticles, position, velocity);

    int generatorType = 1;
    particleGenerator.generateParticles(generatorType);

}

void display()
{   

    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Draw Background
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, backgroundWidth, backgroundHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, backgroundData);
    glGenerateMipmap(GL_TEXTURE_2D);

    int grassWidth = 50;
    int grassHeight = 50;
    glBegin(GL_QUADS);

    float repetitions = 100;
    glTexCoord2f(0.0, repetitions);
    glVertex3f(-grassWidth, grassHeight, 0);
    glTexCoord2f(repetitions, repetitions);
    glVertex3f(grassWidth, grassHeight, 0);
    glTexCoord2f(repetitions, 0.0);
    glVertex3f(grassWidth, -grassHeight, 0);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-grassWidth, -grassHeight, 0);

    glEnd();

    // Draw Characters
    for (int i = 0; i < particleGenerator.totalCapacity; i++) {

        if (particleGenerator.isDeadVector[i] == true) {
            //particleGenerator.deadParticles.push(i);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, deathWidth, deathHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, deathData);
            glGenerateMipmap(GL_TEXTURE_2D);

            float scaleFactor = -0.25;
            glBegin(GL_QUADS);

            glTexCoord2f(0.0, 0.0);
            glVertex3f(particleGenerator.positionVector[i].x - 0.5 - scaleFactor, particleGenerator.positionVector[i].y + 0.548 + scaleFactor, particleGenerator.positionVector[i].z);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(particleGenerator.positionVector[i].x + 0.5 + scaleFactor, particleGenerator.positionVector[i].y + 0.548 + scaleFactor, particleGenerator.positionVector[i].z);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(particleGenerator.positionVector[i].x + 0.5 + scaleFactor, particleGenerator.positionVector[i].y - 0.548 - scaleFactor, particleGenerator.positionVector[i].z);
            glTexCoord2f(0.0, 1.0);
            glVertex3f(particleGenerator.positionVector[i].x - 0.5 - scaleFactor, particleGenerator.positionVector[i].y - 0.548 - scaleFactor, particleGenerator.positionVector[i].z);

            glEnd();
            continue;
        }

        glPushMatrix();

        walkSimTime = walkSimTime + timestep;

        Vertex yAxis(0, 1, 0);
        Vertex orientationVector = particleGenerator.velocityVector[i] / particleGenerator.velocityVector[i].magnitude();
        float dot = yAxis.x * orientationVector.x + yAxis.y * orientationVector.y;
        float det = yAxis.y * orientationVector.x - yAxis.x * orientationVector.y;
        float angleRotation = atan2(det, dot) * (180.0 / 3.141592653589793238463) - 90;

        glTranslatef(particleGenerator.positionVector[i].x, particleGenerator.positionVector[i].y, particleGenerator.positionVector[i].z);
        glRotatef(angleRotation, 0, 0, 1);
        particleGenerator.rotationVector[i] = angleRotation;
        glTranslatef(-particleGenerator.positionVector[i].x, -particleGenerator.positionVector[i].y, -particleGenerator.positionVector[i].z);

        if (particleGenerator.hasAttacked[i] == true) {
            
            if (particleGenerator.characterClassType[i] == 0) {
                particleGenerator.walkingAnimationState[i] = ((particleGenerator.walkingAnimationState[i] + 1) < 15) ? (particleGenerator.walkingAnimationState[i] + 1) : 0;

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, knifeAttackWidth[particleGenerator.walkingAnimationState[i]], knifeAttackHeight[particleGenerator.walkingAnimationState[i]], 0, GL_RGBA, GL_UNSIGNED_BYTE, knifeAttackData[particleGenerator.walkingAnimationState[i]]);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            else if (particleGenerator.characterClassType[i] == 1) {
                particleGenerator.walkingAnimationState[i] = ((particleGenerator.walkingAnimationState[i] + 1) < 3) ? (particleGenerator.walkingAnimationState[i] + 1) : 0;

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rifleShootWidth[particleGenerator.walkingAnimationState[i]], rifleShootHeight[particleGenerator.walkingAnimationState[i]], 0, GL_RGBA, GL_UNSIGNED_BYTE, rifleShootData[particleGenerator.walkingAnimationState[i]]);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            float scaleFactor = 0.15;
            glBegin(GL_QUADS);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(particleGenerator.positionVector[i].x - 0.5 - scaleFactor, particleGenerator.positionVector[i].y + 0.548 + scaleFactor, particleGenerator.positionVector[i].z);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(particleGenerator.positionVector[i].x + 0.5 + scaleFactor, particleGenerator.positionVector[i].y + 0.548 + scaleFactor, particleGenerator.positionVector[i].z);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(particleGenerator.positionVector[i].x + 0.5+ scaleFactor, particleGenerator.positionVector[i].y - 0.548 - scaleFactor, particleGenerator.positionVector[i].z);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(particleGenerator.positionVector[i].x - 0.5 - scaleFactor, particleGenerator.positionVector[i].y - 0.548 - scaleFactor, particleGenerator.positionVector[i].z);

            glEnd();
            particleGenerator.hasAttacked[i] = false;
        }

        else {
            particleGenerator.walkingAnimationState[i] = ((particleGenerator.walkingAnimationState[i] + 1) < 20) ? (particleGenerator.walkingAnimationState[i] + 1) : 0;

            if (particleGenerator.characterClassType[i] == 0) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, walkingWidth[particleGenerator.walkingAnimationState[i]], walkingHeight[particleGenerator.walkingAnimationState[i]], 0, GL_RGBA, GL_UNSIGNED_BYTE, walkingData[particleGenerator.walkingAnimationState[i]]);
                glGenerateMipmap(GL_TEXTURE_2D);
            }

            else if (particleGenerator.characterClassType[i] == 1) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rifleWidth[particleGenerator.walkingAnimationState[i]], rifleHeight[particleGenerator.walkingAnimationState[i]], 0, GL_RGBA, GL_UNSIGNED_BYTE, rifleData[particleGenerator.walkingAnimationState[i]]);
                glGenerateMipmap(GL_TEXTURE_2D);
            }


            glBegin(GL_QUADS);

            glTexCoord2f(0.0, 1.0);
            glVertex3f(particleGenerator.positionVector[i].x - 0.5, particleGenerator.positionVector[i].y + 0.548, particleGenerator.positionVector[i].z);
            glTexCoord2f(1.0, 1.0);
            glVertex3f(particleGenerator.positionVector[i].x + 0.5, particleGenerator.positionVector[i].y + 0.548, particleGenerator.positionVector[i].z);
            glTexCoord2f(1.0, 0.0);
            glVertex3f(particleGenerator.positionVector[i].x + 0.5, particleGenerator.positionVector[i].y - 0.548, particleGenerator.positionVector[i].z);
            glTexCoord2f(0.0, 0.0);
            glVertex3f(particleGenerator.positionVector[i].x - 0.5, particleGenerator.positionVector[i].y - 0.548, particleGenerator.positionVector[i].z);

            glEnd();
        }

        glPopMatrix();
    }


    // Draw Obstacles
    glPushMatrix();
    for (int i = 0; i < particleGenerator.obstacleList.size(); i++) {
        int obstacleIndex = particleGenerator.obstacleList[i].obstacleType;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, obstacleWidth[obstacleIndex], obstacleHeight[obstacleIndex], 0, GL_RGBA, GL_UNSIGNED_BYTE, obstacleData[obstacleIndex]);
        glGenerateMipmap(GL_TEXTURE_2D);

        float obstacleSize = 0.8;
        glBegin(GL_QUADS);

        glTexCoord2f(0.0, 0.0);
        glVertex3f(particleGenerator.obstacleList[i].centre.x - obstacleSize, particleGenerator.obstacleList[i].centre.y + obstacleSize, 0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(particleGenerator.obstacleList[i].centre.x + obstacleSize, particleGenerator.obstacleList[i].centre.y + obstacleSize, 0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(particleGenerator.obstacleList[i].centre.x + obstacleSize, particleGenerator.obstacleList[i].centre.y - obstacleSize, 0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(particleGenerator.obstacleList[i].centre.x - obstacleSize, particleGenerator.obstacleList[i].centre.y - obstacleSize, 0);

        glEnd();

    }
    glPopMatrix();

    glDisable(GL_BLEND);

    // Draw Collision Objects
  /*  glPushMatrix();
    GLfloat colorObject[] = { 1.0f, 0.0f, 0.0f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, colorObject);

    for (int i = 0; i < collisionObjects.size(); i++) {
        glBegin(GL_TRIANGLES);
        glVertex3f(collisionObjects[i].faceVertices[0].x, collisionObjects[i].faceVertices[0].y, collisionObjects[i].faceVertices[0].z);
        glVertex3f(collisionObjects[i].faceVertices[1].x, collisionObjects[i].faceVertices[1].y, collisionObjects[i].faceVertices[1].z);
        glVertex3f(collisionObjects[i].faceVertices[2].x, collisionObjects[i].faceVertices[2].y, collisionObjects[i].faceVertices[2].z);
        glEnd();
    }
    glPopMatrix();*/


    glutSwapBuffers();
}

void idle() {

    // Running the simulation

    if (totalTime >= (1.0f / simulationFPS)) { // Checks simulation time

        endTime = std::chrono::steady_clock::now();
        elapsedTime = (std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime)).count() / pow(10, 6);

        if (elapsedTime >= (1.0f / displayFPS)) { // Checks display time
            startTime = std::chrono::steady_clock::now();
            totalTime = 0;
            glutPostRedisplay();
        }

        else {
            return;
        }
    }

    particleGenerator.moveParticlesWithThreads(timestep);
    totalTime = totalTime + timestep;
    //printf("moved particles \n");
}

void myInit()
{   
    glewExperimental = GL_TRUE;

    glClearColor(1.0, 1.0, 1.0, 1.0);
    //glColor3f(1.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(75.0f, 1.0f, 0.1f, 100.0f);

    int width = 30;
    int height = 20;
    glOrtho(-width, width, -height, height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    gluLookAt(0, 0, 25, 0, 0, 0, 0, 1, 0);

    /*glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);*/

   /* glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); */
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    
    glDisable(GL_DEPTH_TEST);
    glShadeModel(GLU_FLAT);

    // Setup light
   /* GLfloat lightpos[] = { 0, -10, 5, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    GLfloat lightColor[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, lightColor);*/

    constructParticleGenerator();
    particleGenerator.setCollisionVector(collisionObjects);

    int numObstacles = 40;
    particleGenerator.obstacleList = constructObstacleList(numObstacles);

    startTime = std::chrono::steady_clock::now();

    // Setup the texture
    glEnable(GL_TEXTURE_2D);

    // Character Images
    // Load in all the walking images
    int numImages = 20;

    for (int i = 0; i < numImages; i++) {
        int width, height, nrChannels;

        // Get file path
        std::string prefixStr = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/Top_Down_Survivor/knife/move/survivor-move_knife_";
        std::string tmp = std::to_string(i);
        std::string finalString = prefixStr + tmp + ".png";

        unsigned char* data = stbi_load(finalString.data(), &width, &height, &nrChannels, 4);
        if (data == NULL) {
            printf("Image not loaded correctly\n");
        }
        else {
            printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, nrChannels);
            walkingData.push_back(data);
            walkingWidth.push_back(width);
            walkingHeight.push_back(height);
            walkingNrChannels.push_back(nrChannels);

        }

    }

    numImages = 15;

    for (int i = 0; i < numImages; i++) {
        int width, height, nrChannels;

        // Get file path
        std::string prefixStr = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/Top_Down_Survivor/rifle/meleeattack/survivor-meleeattack_rifle_";
        std::string tmp = std::to_string(i);
        std::string finalString = prefixStr + tmp + ".png";

        unsigned char* data = stbi_load(finalString.data(), &width, &height, &nrChannels, 4);
        if (data == NULL) {
            printf("Image not loaded correctly\n");
        }
        else {
            printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, nrChannels);
            rifleShootData.push_back(data);
            rifleShootWidth.push_back(width);
            rifleShootHeight.push_back(height);
            rifleShootNrChannels.push_back(nrChannels);

        }

    }

    numImages = 15;

    for (int i = 0; i < numImages; i++) {
        int width, height, nrChannels;

        // Get file path
        std::string prefixStr = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/Top_Down_Survivor/knife/meleeattack/survivor-meleeattack_knife_";
        std::string tmp = std::to_string(i);
        std::string finalString = prefixStr + tmp + ".png";

        unsigned char* data = stbi_load(finalString.data(), &width, &height, &nrChannels, 4);
        if (data == NULL) {
            printf("Image not loaded correctly\n");
        }
        else {
            printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, nrChannels);
            knifeAttackData.push_back(data);
            knifeAttackWidth.push_back(width);
            knifeAttackHeight.push_back(height);
            knifeAttackNrChannels.push_back(nrChannels);

        }

    }

    numImages = 20;
    for (int i = 0; i < numImages; i++) {
        int width, height, nrChannels;

        // Get file path
        std::string prefixStr = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/Top_Down_Survivor/rifle/move/survivor-move_rifle_";
        std::string tmp = std::to_string(i);
        std::string finalString = prefixStr + tmp + ".png";

        unsigned char* data = stbi_load(finalString.data(), &width, &height, &nrChannels, 4);
        if (data == NULL) {
            printf("Image not loaded correctly\n");
        }
        else {
            printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, nrChannels);
            rifleData.push_back(data);
            rifleWidth.push_back(width);
            rifleHeight.push_back(height);
            rifleNrChannels.push_back(nrChannels);

        }

    }


    // Obstacle Images
    // Load in all the walking images
    numImages = 20;

    for (int i = 0; i < numImages; i++) {
        int width, height, nrChannels;

        // Get file path
        std::string prefixStr = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/Object_";
        std::string tmp = std::to_string(i + 1);
        std::string finalString = prefixStr + tmp + ".png";

        unsigned char* data = stbi_load(finalString.data(), &width, &height, &nrChannels, 4);
        if (data == NULL) {
            printf("Image not loaded correctly\n");
        }
        else {
            printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, nrChannels);
            obstacleData.push_back(data);
            obstacleWidth.push_back(width);
            obstacleHeight.push_back(height);
            obstacleNrChannels.push_back(nrChannels);

        }

    }

    // Get file path
    int grassWidth, grassHeight, grassNrChannels;
    std::string filepath = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/sand.png";
    unsigned char* data = stbi_load(filepath.data(), &grassWidth, &grassHeight, &grassNrChannels, 4);
    if (data == NULL) {
        printf("Image not loaded correctly\n");
    }
    else {
        backgroundData = data;
        backgroundWidth = grassWidth;
        backgroundHeight = grassHeight;
        backgroundNrChannels = grassNrChannels;
    }

    int death_width, death_height, death_nrChannels;
    std::string death_filepath = "C:/Users/anshu/Documents/Physically Based Modelling/BallBounce/BallBounce/Top_Down_Survivor/tombstone.png";
    unsigned char* death_data = stbi_load(death_filepath.data(), &death_width, &death_height, &death_nrChannels, 4);
    if (data == NULL) {
        printf("Image not loaded correctly\n");
    }
    else {
        deathData = death_data;
        deathWidth = death_width;
        deathHeight = death_height;
        deathNrChannels = death_nrChannels;
    }



    glGenTextures(1, &walkingTexture);

    glBindTexture(GL_TEXTURE_2D, walkingTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   
}


int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("Ball Bounce");

    glewInit();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    myInit();
    glutMainLoop();

}
