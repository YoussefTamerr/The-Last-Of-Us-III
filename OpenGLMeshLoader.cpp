#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <random>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <windows.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <random>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <windows.h>
#include <string.h>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

int WIDTH = 1280;
int HEIGHT = 720;
int food = 0;
int ammo = 0;
char lastKey;
int level = 1;

GLuint tex;
char title[] = "The Last of Us Part III";

// 3D Projection Options
GLdouble fovy = 60.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100000;

float playerX = 0;
float playerZ = 13;
int playerRot = 180;
char cameraView = 'f';

bool isTakingDamage = false;
int damageDelay = 1;

float collectible1Position[9][3] = { {12, 0, 3}, {3, 0, 7}, {-8, 0, 12}, {9, 0, 8}, {-5, 0, -5}, {6, 0, 2}, {-2, 0, 9}, {4, 0, -8}, {7, 0, -10} };
float obstacle1Position[9][3] = { {7, 0, 2}, {-3, 0, -9}, {-11, 0, 5}, {7, 0, 4}, {-8, 0, 4}, {6, 0, -2}, {-2, 0, -5}, {1, 0, -3}, {5, 0, -1} };

float collectible2Position[9][3] = {
    {5.0f, 0.0f, -8.0f},
    {10.0f, 0.0f, 1.0f},
    {-3.0f, 0.0f, 10.0f},
    {2.0f, 0.0f, -11.0f},
    {8.0f, 0.0f, 7.0f},
    {-6.0f, 0.0f, -5.0f},
    {11.0f, 0.0f, -9.0f},
    {-10.0f, 0.0f, 3.0f},
    {4.0f, 0.0f, -10.0f}
};

float obstacle2Position[9][3] = {
    {-5.0f, 0.0f, 6.0f},
    {3.0f, 0.0f, -2.0f},
    {7.0f, 0.0f, 4.0f},
    {-9.0f, 0.0f, 9.0f},
    {1.0f, 0.0f, -4.0f},
    {-11.0f, 0.0f, 11.0f},
    {-4.0f, 0.0f, -6.0f},
    {6.0f, 0.0f, -8.0f},
    {-7.0f, 0.0f, 2.0f}
};

float d = 0.25;

GLfloat lightIntensity[] = { -0.3f, -0.3f, -0.3f, 1.0f };
GLfloat lightPosition[] = { -20.0f, 50.0f, 0.0f, 1.0f };

void updateSunPosition() {
    lightPosition[0] += 1.0f;

    if (lightPosition[0] >= 0.0f) {
        lightIntensity[0] -= 0.03f;
        lightIntensity[1] -= 0.03f;
        lightIntensity[2] -= 0.03f;
    }
    else {
        lightIntensity[0] += 0.03f;
        lightIntensity[1] += 0.03f;
        lightIntensity[2] += 0.03f;
    }


    if (lightPosition[0] > 20.0f) {
        lightPosition[0] = -20.0f;
        lightIntensity[0] = -0.3f;
        lightIntensity[1] = -0.3f;
        lightIntensity[2] = -0.3f; //-0.3f;
    }
    printf("lightPosition: %f\n", lightIntensity[0]);
}

int counter = 0;

void updateLightBulb() {
    float pos1[] = { 5.0f, 3.0f, 0.0f, 1.0f };
    float pos2[] = { -5.0f, 3.0f, 0.0f, 1.0f };

    srand(time(NULL));
    int random_number = rand() % 2;

    if (lightIntensity[0] < 0.0f) {
        lightIntensity[0] = 0.4f;
        lightIntensity[1] = 0.4f;
        lightIntensity[2] = 0.4f;
        counter++;
    }
    else {
        lightIntensity[0] = -0.4f;
        lightIntensity[1] = -0.4f;
        lightIntensity[2] = -0.4f;
    }
    if (random_number) {
        lightPosition[0] = pos1[0];
        lightPosition[1] = pos1[1];
        lightPosition[2] = pos1[2];
    }
    else {
        lightPosition[0] = pos2[0];
        lightPosition[1] = pos2[1];
        lightPosition[2] = pos2[2];
    }


    printf("counter: %i\n", random_number);

}

//=======================================================================
// Vector3f Class
//=======================================================================

class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(Vector3f& v) {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(Vector3f& v) {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() {
        return *this / sqrt(x * x + y * y + z * z);
    }

    Vector3f cross(Vector3f v) {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
    }

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
};

Camera camera(playerX, 2, playerZ, 0, 2, 0, 0, 1, 0);
void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, aspectRatio, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}

class Vector
{
public:
    GLdouble x, y, z;
    Vector() {}
    Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
    //================================================================================================//
    // Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
    // Here we are overloading the += operator to add a given value to all vector coordinates.        //
    //================================================================================================//
    void operator +=(float value)
    {
        x += value;
        y += value;
        z += value;
    }
};

//=======================================================================
// Printing Text
//=======================================================================

void print(int x, int y, char* string, int font) {
    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
        if (font == 12) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
        }
        else if (font == 18) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
        }
        else if (font == 24) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        }
    }
}

//=======================================================================
// Display Score
//=======================================================================

void displayScore() {

    glDisable(GL_LIGHTING);

    std::string text1 = "Food: " + std::to_string(food);
    std::string text2 = "Ammo: " + std::to_string(ammo);

    std::string text3 = "x: " + std::to_string(playerX);
    std::string text4 = "z: " + std::to_string(playerZ);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int x = WIDTH - 110;
    int y1 = HEIGHT - 28;

    glColor3f(1, 0, 0);

    print(x, y1, const_cast<char*>(text1.c_str()), 18);
    int y2 = HEIGHT - 58;
    //print(x - 600, y1, const_cast<char*>(text3.c_str()), 18);
    //print(x - 600, y2, const_cast<char*>(text4.c_str()), 18);


    if (level > 1) {
        print(x, y2, const_cast<char*>(text2.c_str()), 18);
    }
    glColor3f(1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glEnable(GL_LIGHTING);
}

Vector Eye(playerX, 5, playerZ);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;

Model_3DS model_ammo;
Model_3DS model_food;

Model_3DS model_skeli;
Model_3DS model_swampGhoul;

Model_3DS model_player1;
Model_3DS model_player2;

Model_3DS model_goal1;
Model_3DS model_goal2;

Model_3DS model_wall;
// Textures
GLTexture tex_ground;

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
    // Enable Lighting for this OpenGL Program
    glEnable(GL_LIGHTING);

    // Enable Light Source number 0
    // OpengL has 8 light sources
    glEnable(GL_LIGHT0);

    // Define Light source 0 ambient light
    GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    // Define Light source 0 diffuse light
    GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    // Define Light source 0 Specular light
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Finally, define light source 0 position in World Space
    GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
    // Enable Material Tracking
    glEnable(GL_COLOR_MATERIAL);

    // Sich will be assigneet Material Properties whd by glColor
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Set Material's Specular Color
    // Will be applied to all objects
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

    // Set Material's Shine value (0->128)
    GLfloat shininess[] = { 96.0f };
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    gluPerspective(fovy, aspectRatio, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();

    gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

    InitLightSource();

    InitMaterial();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================

void RenderGround()
{
    //glDisable(GL_LIGHTING); // Disable lighting

    glColor3f(0.6, 0.6, 0.6);       // Dim the ground texture a bit

    glEnable(GL_TEXTURE_2D);        // Enable 2D texturing

    glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);    // Bind the ground texture

    glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);    // Set quad normal direction.
    glTexCoord2f(0, 0);             // Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
    glVertex3f(-15, 0, -15);
    glTexCoord2f(5, 0);
    glVertex3f(15, 0, -15);
    glTexCoord2f(5, 5);
    glVertex3f(15, 0, 15);
    glTexCoord2f(0, 5);
    glVertex3f(-15, 0, 15);
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHTING);  // Enable lighting again for other entites coming throung the pipeline.

    glColor3f(1, 1, 1);     // Set material back to white instead of grey used for the ground texture.
}

void setupLights() {

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
}

void drawCollectible1(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y + 0.4, z);
    glRotatef(90, 1, 0, 0);
    glScalef(1.5, 1.5, 1.5);
    model_food.Draw();
    glPopMatrix();
}

void drawObstacle1(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(1.25, 1.25, 1.25);
    model_skeli.Draw();
    glPopMatrix();
}

void drawCollectible2(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(0, 1, 0, 0);
    glScalef(0.04, 0.04, 0.04);
    model_ammo.Draw();
    glPopMatrix();
}

void drawObstacle2(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y + 1.2, z);
    glScalef(1.25, 1.25, 1.25);
    model_swampGhoul.Draw();
    glPopMatrix();
}

//=======================================================================
// Player Collision
//=======================================================================

void playerCollide() {
    const float playerWidth = 0.5;
    const float collectible1Width = 0.5;
    const float collectible2Width = 0.5;
    const float collectible2Length = 1.5;
    const float obstacle1Width = 0.5;
    const float obstacle2Width = 0.5;
    const float goalWidth = 0.5;

    float leftBoundary = playerX - (playerWidth / 2);
    float rightBoundary = playerX + (playerWidth / 2);
    float frontBoundary = playerZ - (playerWidth / 2);
    float backBoundary = playerZ + (playerWidth / 2);

    float leftBoundaryGoal = 0 - (goalWidth / 2);
    float rightBoundaryGoal = 0 + (goalWidth / 2);
    float frontBoundaryGoal = -13 - (goalWidth / 2);
    float backBoundaryGoal = -13 + (goalWidth / 2);

    std::cout << playerX << ", " << playerZ << std::endl;
    std::cout << leftBoundary << ", " << rightBoundary << ", " << frontBoundary << ", " << backBoundary << "\n" << std::endl;

    if (level == 1) {
        for (int i = 0; i < 9; i++) {
            float* position = collectible1Position[i];
            float tempX = position[0];
            float tempZ = position[2];

            float leftBoundaryCol = tempX - (collectible1Width / 2);
            float rightBoundaryCol = tempX + (collectible1Width / 2);
            float frontBoundaryCol = tempZ - (collectible1Width / 2);
            float backBoundaryCol = tempZ + (collectible1Width / 2);

            if (leftBoundary <= rightBoundaryCol && rightBoundary >= leftBoundaryCol && frontBoundary <= backBoundaryCol && backBoundary >= frontBoundaryCol) {
                mciSendString(TEXT("play col.wav"), NULL, 0, 0);
                food += 1;
                collectible1Position[i][0] = 100;
                collectible1Position[i][1] = 1000;
                collectible1Position[i][2] = 100;
            }
        }

        for (int i = 0; i < 9; i++) {
            float* position = obstacle1Position[i];
            float tempX = position[0];
            float tempZ = position[2];

            float leftBoundaryCol = tempX - (obstacle1Width / 2);
            float rightBoundaryCol = tempX + (obstacle1Width / 2);
            float frontBoundaryCol = tempZ - (obstacle1Width / 2);
            float backBoundaryCol = tempZ + (obstacle1Width / 2);

            if (leftBoundary <= rightBoundaryCol && rightBoundary >= leftBoundaryCol && frontBoundary <= backBoundaryCol && backBoundary >= frontBoundaryCol) {
                if (lastKey == 'w') {
                    mciSendString(TEXT("play skele.mp3"), NULL, 0, 0);
                    isTakingDamage = true;
                    playerZ -= 2 * d * cos(DEG2RAD(playerRot));
                    playerX -= 2 * d * sin(DEG2RAD(playerRot));
                    if (cameraView == 't') {
                        float offset = 5.0;
                        camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                    else {
                        camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                }
                else {
                    playerZ += 2 * d * cos(DEG2RAD(playerRot));
                    playerX += 2 * d * sin(DEG2RAD(playerRot));
                    if (cameraView == 't') {
                        float offset = 5.0;
                        camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                    else {
                        camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                }
            }
        }

        if (leftBoundary <= rightBoundaryGoal && rightBoundary >= leftBoundaryGoal && frontBoundary <= backBoundaryGoal && backBoundary >= frontBoundaryGoal) {
            std::cout << "level 1 done" << std::endl;
            mciSendString(TEXT("play victory.mp3"), NULL, 0, 0);
            level = 2;
            playerX = 0;
            playerZ = 13;
            playerRot = 180;
            if (cameraView == 't') {
                float offset = 5.0;
                camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            }
            else {
                camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            }
            //asdf
            lightIntensity[0] = 0.4f;
            lightIntensity[1] = 0.4f;
            lightIntensity[2] = 0.4f;

            lightPosition[0] = 15.0f;
            lightPosition[1] = 5.0f;
            lightPosition[2] = 0.0f;
        }
    }
    else if (level == 2) {
        for (int i = 0; i < 9; i++) {
            float* position = collectible2Position[i];
            float tempX = position[0];
            float tempZ = position[2];

            float leftBoundaryCol = tempX - (collectible2Width / 2);
            float rightBoundaryCol = tempX + (collectible2Width / 2);
            float frontBoundaryCol = tempZ - (collectible2Length / 2);
            float backBoundaryCol = tempZ + (collectible2Length / 2);

            if (leftBoundary <= rightBoundaryCol && rightBoundary >= leftBoundaryCol && frontBoundary <= backBoundaryCol && backBoundary >= frontBoundaryCol) {
                mciSendString(TEXT("play col.wav"), NULL, 0, 0);
                ammo += 1;
                collectible2Position[i][0] = 100;
                collectible2Position[i][1] = 1000;
                collectible2Position[i][2] = 100;
            }
        }

        for (int i = 0; i < 9; i++) {
            float* position = obstacle2Position[i];
            float tempX = position[0];
            float tempZ = position[2];

            float leftBoundaryCol = tempX - (obstacle2Width / 2);
            float rightBoundaryCol = tempX + (obstacle2Width / 2);
            float frontBoundaryCol = tempZ - (obstacle2Width / 2);
            float backBoundaryCol = tempZ + (obstacle2Width / 2);

            if (leftBoundary <= rightBoundaryCol && rightBoundary >= leftBoundaryCol && frontBoundary <= backBoundaryCol && backBoundary >= frontBoundaryCol) {
                if (lastKey == 'w') {
                    mciSendString(TEXT("play zomb.mp3"), NULL, 0, 0);
                    isTakingDamage = true;
                    playerZ -= 2 * d * cos(DEG2RAD(playerRot));
                    playerX -= 2 * d * sin(DEG2RAD(playerRot));
                    if (cameraView == 't') {
                        float offset = 5.0;
                        camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                    else {
                        camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                }
                else {
                    playerZ += 2 * d * cos(DEG2RAD(playerRot));
                    playerX += 2 * d * sin(DEG2RAD(playerRot));
                    if (cameraView == 't') {
                        float offset = 5.0;
                        camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                    else {
                        camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                    }
                }
            }
        }

        if (leftBoundary <= rightBoundaryGoal && rightBoundary >= leftBoundaryGoal && frontBoundary <= backBoundaryGoal && backBoundary >= frontBoundaryGoal) {
            std::cout << "level 2 done" << std::endl;
            mciSendString(TEXT("play victory.mp3"), NULL, 0, 0);
            level = 3;
        }
    }
}

bool first = false;

void gameEnd() {

    glDisable(GL_LIGHTING);

    if (!first) {
        camera = Camera(playerX - 5 * sin(DEG2RAD(playerRot)), 4, playerZ - 5 * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        first = true;
    }

    glPushMatrix();
    glLoadIdentity();
    mciSendString(TEXT("pause background.mp3"), NULL, 0, 0);
    mciSendString(TEXT("play plinplon.mp3"), NULL, 0, 0);
    glColor3f(1, 1, 1);

    char plural1;
    char plural2;

    if (food == 1) plural1 = ' ';
    else plural1 = 's';

    if (ammo == 1) plural2 = ' ';
    else plural2 = 's';

    std::string text = "You have collected: ";
    std::string text1 = "Food: " + std::to_string(food) + " Can" + plural1;
    std::string text2 = "Ammo: " + std::to_string(ammo) + " Crate" + plural2;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, 0, HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text.c_str());
    int textWidth1 = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text1.c_str());
    int textWidth2 = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text2.c_str());

    glColor3f(0, 0, 0);
    print((WIDTH / 2) - textWidth / 2, (HEIGHT / 2) + 25, const_cast<char*>(text.c_str()), 18);
    print((WIDTH / 2) - textWidth1 / 2, HEIGHT / 2, const_cast<char*>(text1.c_str()), 18);
    print((WIDTH / 2) - textWidth2 / 2, (HEIGHT / 2) - 25, const_cast<char*>(text2.c_str()), 18);
    glColor3f(1, 1, 1);


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

//=======================================================================
// Display Function
//=======================================================================

void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    setupCamera();
    setupLights();

    if (level == 1) {
        glPushMatrix();
        glTranslated(lightPosition[0], lightPosition[1], lightPosition[2]);
        glColor3f(1, 1, 0);
        glutSolidSphere(3, 20, 20);
        glColor3f(1, 1, 1);
        glPopMatrix();
    }

    // Draw Ground
    RenderGround();

    //// Draw Clicker Model

    if (level != 3) {
        displayScore();
    }

    if (isTakingDamage) {
        if (damageDelay <= 0) {
            isTakingDamage = false;
        }
        else {
            damageDelay--;
        }
        glColor3f(220.0 / 255.0, 74.0 / 255.0, 59.0 / 255.0);
    }
    else {
        glColor3f(1, 1, 1);
    }

    if (level == 1) {
        for (int i = 0; i < 9; i++) {
            drawCollectible1(collectible1Position[i][0], collectible1Position[i][1], collectible1Position[i][2]);
        }

        for (int i = 0; i < 9; i++) {
            drawObstacle1(obstacle1Position[i][0], obstacle1Position[i][1], obstacle1Position[i][2]);
        }

        glPushMatrix();
        glTranslatef(0, 1.1, -13);
        glScalef(1.25, 1.25, 1.25);
        model_goal1.Draw();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(playerX, 0, playerZ);
        glScalef(1.25, 1.25, 1.25);
        glRotatef(playerRot, 0, 1, 0);

        if (cameraView == 't') {
            model_player1.Draw();
        }

        glPopMatrix();
    }
    else if (level == 2) {
        for (int i = 0; i < 9; i++) {
            drawCollectible2(collectible2Position[i][0], collectible2Position[i][1], collectible2Position[i][2]);
        }

        for (int i = 0; i < 9; i++) {
            drawObstacle2(obstacle2Position[i][0], obstacle2Position[i][1], obstacle2Position[i][2]);
        }

        glPushMatrix();
        glTranslatef(0, 0, -13);
        glScalef(0.23, 0.23, 0.23);
        model_goal2.Draw();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(playerX, 0, playerZ);
        glScalef(0.5, 0.5, 0.5);
        glScalef(0.23, 0.23, 0.23);
        glRotatef(playerRot, 0, 1, 0);

        if (cameraView == 't') {
            model_player2.Draw();
        }

        glPopMatrix();
    }
    else {
        gameEnd();
    }

    if (level == 2) {
        glPushMatrix();
        glTranslatef(0, 2, 15.6); //da elly yadoob
        //glTranslatef(5, 2, 22); // da over
        glScalef(0.3, 2, 0.1);
        model_wall.Draw();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, 0);
        glScalef(5.4, 2, 8.6); // da elly yadoob
        //glScalef(9, 2, 12); // da over
        glRotatef(90, 1, 0, 0);
        model_house.Draw();
        glPopMatrix();
    }

    // Draw house Model
    //glPushMatrix();
    //glRotatef(90.f, 1, 0, 0);
    //model_abby.Draw();
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);
    //glPopMatrix();

    //sky box

    glPushMatrix();

    GLUquadricObj* qobj;
    qobj = gluNewQuadric();
    glTranslated(50, 0, 0);
    glRotated(90, 1, 0, 1);
    glBindTexture(GL_TEXTURE_2D, tex);
    gluQuadricTexture(qobj, true);
    gluQuadricNormals(qobj, GL_SMOOTH);
    gluSphere(qobj, 100, 100, 100);
    gluDeleteQuadric(qobj);

    glPopMatrix();
    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'u':
        camera.moveZ(d);
        break;
    case 'j':
        camera.moveZ(-d);
        break;
    case 'h':
        camera.moveX(d);
        break;

    case 'k':
        camera.moveX(-d);
        break;
    case 'y':
        camera.moveY(d);
        break;
    case 'i':
        camera.moveY(-d);
        break;

    case 'z':
        // Top view
        camera = Camera(-0.605281f, 4.16508f, -0.276107f, -0.605958f, 3.17864f, -0.440224f, -0.00407153f, 0.164118f, -0.986432f);
        break;
    case 'x':
        // Side view
        camera = Camera(4.22272f, 1.44851f, -0.476684f, 3.22275f, 1.4433f, -0.471141f, -0.00525828f, 0.999951f, -0.00838759f);
        break;
    case 'c':
        // front view
        camera = Camera(-0.489235f, 1.24757f, 3.61104f, -0.493357f, 1.30398f, 2.61264f, 0.000232926f, 0.998408f, 0.056412f);
        break;
    case 'w':
        playerZ += d * cos(DEG2RAD(playerRot));
        playerX += d * sin(DEG2RAD(playerRot));
        mciSendString(TEXT("play dash.mp3"), NULL, 0, 0);
        if (playerZ >= 14.5) {
            playerZ = 14.5;
            isTakingDamage = true;
        }
        if (playerX >= 14.5) {
            playerX = 14.5;
            isTakingDamage = true;
        }
        if (playerZ <= -14.5) {
            playerZ = -14.5;
            isTakingDamage = true;
        }
        if (playerX <= -14.5) {
            playerX = -14.5;
            isTakingDamage = true;
        }
        if (cameraView == 't') {
            float offset = 5.0;
            camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            if (level == 2) {
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) >= 14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, 14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) >= 14.5) camera = Camera(14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) <= -14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, -14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) <= -14.5) camera = Camera(-14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            }
        }
        else {
            camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        }
        break;
    case 's':
        playerZ -= d * cos(DEG2RAD(playerRot));
        playerX -= d * sin(DEG2RAD(playerRot));
        if (playerZ >= 14.5) playerZ = 14.5;
        if (playerX >= 14.5) playerX = 14.5;
        if (playerZ <= -14.5) playerZ = -14.5;
        if (playerX <= -14.5) playerX = -14.5;
        if (cameraView == 't') {
            float offset = 5.0;
            camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            if (level == 2) {
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) >= 14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, 14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) >= 14.5) camera = Camera(14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) <= -14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, -14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) <= -14.5) camera = Camera(-14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            }
        }
        else {
            camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        }
        break;
    case 'd':
        if (playerRot == 0) {
            playerRot = 359;
        }
        else {
            playerRot -= 1;
        }
        if (cameraView == 't') {
            float offset = 5.0;
            camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            if (level == 2) {
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) >= 14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, 14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) >= 14.5) camera = Camera(14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) <= -14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, -14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) <= -14.5) camera = Camera(-14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            }
        }
        else {
            camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        }
        break;
    case 'a':
        if (playerRot == 359) {
            playerRot = 0;
        }
        else {
            playerRot += 1;
        }
        if (cameraView == 't') {
            float offset = 5.0;
            camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            if (level == 2) {
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) >= 14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, 14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) >= 14.5) camera = Camera(14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerZ - offset * cos(DEG2RAD(playerRot))) <= -14.5) camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, -14.5, 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
                if ((playerX - offset * sin(DEG2RAD(playerRot))) <= -14.5) camera = Camera(-14.5, 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
            }
        }
        else {
            camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        }
        break;
    case GLUT_KEY_ESCAPE:
        exit(EXIT_SUCCESS);
    }

    lastKey = key;
    playerCollide();
    glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
    //y = HEIGHT - y;

    //if (cameraZoom - y > 0)
    //{
    //    Eye.x += -0.1;
    //    Eye.z += -0.1;
    //}
    //else
    //{
    //    Eye.x += 0.1;
    //    Eye.z += 0.1;
    //}

    //cameraZoom = y;

    glLoadIdentity();       //Clear Model_View Matrix

    gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);     //Setup Camera with modified paramters

    GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glutPostRedisplay();    //Re-draw scene
}


//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
    if (h == 0) {
        h = 1;
    }

    WIDTH = w;
    HEIGHT = h;

    // set the drawable region of the window
    glViewport(0, 0, w, h);

    // set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

    // go back to modelview matrix so we can move the objects about
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
    // Loading Model files
    model_house.Load("models/house/house.3DS");
    model_tree.Load("models/tree/Tree1.3ds");

    //obstacles
    model_swampGhoul.Load("models/swampGhoul/swampGhoul.3ds");
    model_skeli.Load("Models/skeleton/skeleton3.3ds");

    //collectables
    model_ammo.Load("models/ammo/box_3DS.3ds");
    model_food.Load("models/food/Beets.3ds");

    //players
    model_player1.Load("Models/player1/Girl croft manor sport N050313.3ds");
    model_player2.Load("Models/player2/M_Voleuse.3ds");

    //goals
    model_goal1.Load("Models/goal1/lumberJack.3ds");
    model_goal2.Load("Models/goal2/Woman N241016.3ds");

    model_wall.Load("Models/wall/Wall decor N251018.3ds");

    // Loading texture files
    tex_ground.Load("textures/ground.bmp");
    loadBMP(&tex, "textures/blu-sky-3.bmp", true);


}

//=======================================================================
// Main Function
//=======================================================================
void Special(int key, int x, int y) {
    float a = 1.0;

    switch (key) {
    case GLUT_KEY_UP:
        camera.rotateX(a);
        break;
    case GLUT_KEY_DOWN:
        camera.rotateX(-a);
        break;
    case GLUT_KEY_LEFT:
        camera.rotateY(a);
        break;
    case GLUT_KEY_RIGHT:
        camera.rotateY(-a);
        break;
    }

    glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (cameraView == 'f') {
            cameraView = 't';
            float offset = 5.0;
            camera = Camera(playerX - offset * sin(DEG2RAD(playerRot)), 4, playerZ - offset * cos(DEG2RAD(playerRot)), 1000 * sin(DEG2RAD(playerRot)), 1, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        }
        else {
            cameraView = 'f';
            camera = Camera(playerX, 2, playerZ, 1000 * sin(DEG2RAD(playerRot)), 2, 1000 * cos(DEG2RAD(playerRot)), 0, 1, 0);
        }
    }

    glutPostRedisplay();
}

void Timer(int value) {

    int t = 1000;
    if (level == 1)
        updateSunPosition();
    if (level == 2) {
        t = 2000;
        updateLightBulb();
    }
    if (level == 3) {
        t = 50;
        camera.rotateY(1);
    }



    glutPostRedisplay();

    glutTimerFunc(t, Timer, 0);
}

void main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 150);
    glutCreateWindow(title);
    glutDisplayFunc(myDisplay);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(Special);
    glutMotionFunc(myMotion);
    glutReshapeFunc(myReshape);
    glutMouseFunc(Mouse);
    glutTimerFunc(0, Timer, 0);
    myInit();
    mciSendString(TEXT("play background.mp3"), NULL, 0, 0);
    LoadAssets();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);

    glutMainLoop();
}