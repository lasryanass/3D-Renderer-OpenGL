#include <GL/glut.h>
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <ctime>

// Variables for camera movement
float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 5.0f;

// Variables for object rotation
float rotationX = 0.0f;
float rotationY = 0.0f;

// Variables for storing object data
std::vector<float> vertices;
std::vector<float> normals;
std::vector<float> texCoords;
std::vector<int> indices;

// Lighting parameters
GLfloat lightPosition[] = {1.0f, 1.0f, 1.0f, 0.0f};
GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat shininess[] = {50.0f};

// Function to handle key presses
void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
        // Camera movement controls
        case 'w':
            cameraZ -= 0.1f; // Move camera forward
            break;
        case 's':
            cameraZ += 0.1f; // Move camera backward
            break;
        case 'a':
            cameraX -= 0.1f; // Move camera left
            break;
        case 'd':
            cameraX += 0.1f; // Move camera right
            break;
        case 'q':
            cameraY += 0.1f; // Move camera up
            break;
        case 'e':
            cameraY -= 0.1f; // Move camera down
            break;

        // Object rotation controls
        case 'r':
            rotationX += 1.0f; // Rotate object around x-axis
            break;
        case 'f':
            rotationX -= 1.0f; // Rotate object around x-axis
            break;
        case 't':
            rotationY += 1.0f; // Rotate object around y-axis
            break;
        case 'g':
            rotationY -= 1.0f; // Rotate object around y-axis
            break;

        case 27: // Escape key
            exit(0); // Exit the program
    }
    glutPostRedisplay(); // Mark the window for redrawing
}

// Function to parse the .obj file
void parseObjFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;

        iss >> prefix;

        if (prefix == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        } else if (prefix == "vn") {
            float nx, ny, nz;
            iss >> nx >> ny >> nz;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        } else if (prefix == "vt") {
            float u, v;
            iss >> u >> v;
            texCoords.push_back(u);
            texCoords.push_back(v);
        } else if (prefix == "f") {
            std::string face;
            while (iss >> face) {
                std::replace(face.begin(), face.end(), '/', ' ');
                std::istringstream faceStream(face);
                int vIndex, tIndex, nIndex;
                faceStream >> vIndex >> tIndex >> nIndex;
                indices.push_back(vIndex - 1);
                indices.push_back(tIndex - 1);
                indices.push_back(nIndex - 1);
            }
        }
    }

    file.close();
}

// Function to calculate the surface normal for a triangle
void calculateSurfaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float& nx, float& ny, float& nz) {
    float u1 = x2 - x1;
    float u2 = y2 - y1;
    float u3 = z2 - z1;
    float v1 = x3 - x1;
    float v2 = y3 - y1;
    float v3 = z3 - z1;

    nx = u2 * v3 - u3 * v2;
    ny = u3 * v1 - u1 * v3;
    nz = u1 * v2 - u2 * v1;

    float length = std::sqrt(nx * nx + ny * ny + nz * nz);
    nx /= length;
    ny /= length;
    nz /= length;
}

// Function to draw the 3D scene
void renderScene() {
    static clock_t startTime = clock(); // Start time

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glLoadIdentity(); // Reset the model-view matrix

    // Set the camera position and orientation
    gluLookAt(cameraX, cameraY, cameraZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glRotatef(rotationX, 1.0f, 0.0f, 0.0f); // Rotate around x-axis
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f); // Rotate around y-axis

    // Set up lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularLight);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    // Draw the object
    glBegin(GL_TRIANGLES);

    for (size_t i = 0; i < indices.size(); i += 3) {
        int vIndex = indices[i] * 3;
        int tIndex = indices[i + 1] * 2;
        int nIndex = indices[i + 2] * 3;

        float nx = normals[nIndex];
        float ny = normals[nIndex + 1];
        float nz = normals[nIndex + 2];

        // Set the surface normal
        glNormal3f(nx, ny, nz);

        // Calculate the lighting intensity based on the surface normal
        float dotProduct = nx * lightPosition[0] + ny * lightPosition[1] + nz * lightPosition[2];
        float absDotProduct = std::abs(dotProduct);
        float intensity = std::max(0.2f, absDotProduct);

        // Set the color based on the lighting intensity
        glColor3f(intensity, intensity, intensity);

        // Set the texture coordinates
        glTexCoord2f(texCoords[tIndex], texCoords[tIndex + 1]);

        // Set the vertex position
        glVertex3f(vertices[vIndex], vertices[vIndex + 1], vertices[vIndex + 2]);
    }

    glEnd();

    glutSwapBuffers(); // Swap the buffers to display the rendered scene

    // Calculate and print the elapsed time
    clock_t endTime = clock();
    float elapsedTime = static_cast<float>(endTime - startTime) / CLOCKS_PER_SEC;
    std::cout << "Time: " << elapsedTime << " seconds" << std::endl;
}

// Function to handle window resizing
void changeSize(int width, int height) {
    if (height == 0) {
        height = 1;
    }

    float ratio = static_cast<float>(width) / height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, width, height);
    gluPerspective(45, ratio, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: ./obj_viewer <obj_file.obj>" << std::endl;
        return 1;
    }

    parseObjFile(argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL OBJ Viewer");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(handleKeypress);

    glutMainLoop();

    return 0;
}
