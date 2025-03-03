#include <GL/glut.h>
#include <cmath>

float carX = -1.0f;  // Car starts from the left
float pedestrianY = -0.5f;  // Pedestrian starts at the bottom edge of the road
bool pedestrianMoving = true;
int lightState = 0;  // 0 = Red, 1 = Yellow, 2 = Green
bool collisionOccurred = false; // Track collision
bool pedestrianBlink  = false; // To make pedestrian blink

//Function to Render Text
void renderText(float x, float y, const char* text) {
    glColor3f(0.0f, 0.0f, 0.0f); // Set text color (black)
    glRasterPos2f(x, y); // Position the text

    // Loop through each character and draw it
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

// Function to draw the road
void drawRoad() {
    glColor3f(0.5f, 0.5f, 0.5f);  // Grey road
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -0.5f);
        glVertex2f(1.0f, -0.5f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(-1.0f, 0.0f);
    glEnd();
    
    // Draw lane divider
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex2f(-1.0f, -0.25f);
        glVertex2f(1.0f, -0.25f);
    glEnd();
    
    // Draw pedestrian crossing (black and white stripes only under pedestrian width)
    float shiftX = 0.5f; // Move the crossing to the right
    
    for (float i = -0.5f; i <= 0.0f; i += 0.05f) {
        glColor3f((int(i * 20) % 2 == 0) ? 0.0f : 1.0f, 
                  (int(i * 20) % 2 == 0) ? 0.0f : 1.0f, 
                  (int(i * 20) % 2 == 0) ? 0.0f : 1.0f);
    
        glBegin(GL_QUADS);
            glVertex2f(0.1f + shiftX, i);        // Left-bottom
            glVertex2f(0.4f + shiftX, i);         // Right-bottom
            glVertex2f(0.4f + shiftX, i + 0.05f); // Right-top
            glVertex2f(0.1f + shiftX, i + 0.05f);// Left-top
        glEnd();
    }
}

// Function to draw the pedestrian
void drawPedestrian() {
    if (collisionOccurred && pedestrianBlink) {
        glColor3f(1.0f, 0.0f, 0.0f);  // Blink red
    } else {
        glColor3f(0.76f, 0.52f, 0.30f);  // Normal skin color
    }

    // Head
    glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 10)
            glVertex2f(0.77f + 0.03f * cos(i * M_PI / 180), pedestrianY + 0.12f + 0.03f * sin(i * M_PI / 180));
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f); // Shirt
    glBegin(GL_QUADS);
        glVertex2f(0.74f, pedestrianY);
        glVertex2f(0.8f, pedestrianY);
        glVertex2f(0.8f, pedestrianY + 0.1f);
        glVertex2f(0.74f, pedestrianY + 0.1f);
    glEnd();

    glColor3f(0.76f, 0.52f, 0.30f); // Arms
    glBegin(GL_QUADS);
        glVertex2f(0.74f, pedestrianY + 0.07f);
        glVertex2f(0.72f, pedestrianY + 0.07f);
        glVertex2f(0.72f, pedestrianY + 0.03f);
        glVertex2f(0.74f, pedestrianY + 0.03f);
    glEnd();

    glBegin(GL_QUADS);
        glVertex2f(0.8f, pedestrianY + 0.07f);
        glVertex2f(0.82f, pedestrianY + 0.07f);
        glVertex2f(0.82f, pedestrianY + 0.03f);
        glVertex2f(0.8f, pedestrianY + 0.03f);
    glEnd();

    // Legs
    glColor3f(0.76f, 0.52f, 0.30f);
    glBegin(GL_QUADS);
        glVertex2f(0.76f, pedestrianY - 0.05f);
        glVertex2f(0.74f, pedestrianY - 0.05f);
        glVertex2f(0.74f, pedestrianY);
        glVertex2f(0.76f, pedestrianY);
    glEnd();
    
    glBegin(GL_QUADS);
        glVertex2f(0.8f, pedestrianY - 0.05f);
        glVertex2f(0.78f, pedestrianY - 0.05f);
        glVertex2f(0.78f, pedestrianY);
        glVertex2f(0.8f, pedestrianY);
    glEnd();
}

// Function to detect collision
bool checkCollision() {
    float carLeft = carX - 0.15f;  // Left boundary of car
    float carRight = carX + 0.15f; // Right boundary of car
    float pedestrianLeft = 0.74f;  // Left boundary of pedestrian
    float pedestrianRight = 0.8f;  // Right boundary of pedestrian

    bool xOverlap = (carRight >= pedestrianLeft && carLeft <= pedestrianRight); // X-axis overlap
    bool yOverlap = (pedestrianY >= -0.15f && pedestrianY <= -0.05f); // Y-axis overlap

    return xOverlap && yOverlap;  // True if both X and Y overlap
}

void pedestrianBlinkEffect(int value) {
    if (collisionOccurred) {
        pedestrianBlink = !pedestrianBlink;  // Toggle blinking
        glutPostRedisplay();  // Force redraw to reflect changes
        glutTimerFunc(300, pedestrianBlinkEffect, 0); // Blink every 300ms
    }
}


// Function to draw the car
void drawCar() {
    glColor3f(0.4f, 0.6f, 0.9f);  // Light blue truck body
    glBegin(GL_QUADS);
        glVertex2f(carX - 0.15f, -0.15f);
        glVertex2f(carX + 0.15f, -0.15f);
        glVertex2f(carX + 0.15f, -0.05f);
        glVertex2f(carX - 0.15f, -0.05f);
    glEnd();
    
    // Truck cabin
    glBegin(GL_QUADS);
        glVertex2f(carX + 0.1f, -0.05f);
        glVertex2f(carX + 0.15f, -0.05f);
        glVertex2f(carX + 0.15f, 0.02f);
        glVertex2f(carX + 0.1f, 0.02f);
    glEnd();

    // Draw wheels
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 10)
            glVertex2f(carX - 0.1f + 0.02f * cos(i * 3.14159 / 180), -0.18f + 0.02f * sin(i * 3.14159 / 180));
    glEnd();
    
    glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 10)
            glVertex2f(carX + 0.05f + 0.02f * cos(i * 3.14159 / 180), -0.18f + 0.02f * sin(i * 3.14159 / 180));
    glEnd();
}

// Function to draw the traffic light
void drawTrafficLight() {
    // Traffic light pole
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(0.35f, 0.9f);   // Move right
        glVertex2f(0.4f, 0.9f);    // Move right
        glVertex2f(0.4f, 0.0f);   // Move right
        glVertex2f(0.35f, 0.0f);  // Move right
    glEnd();

    // Traffic light box
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(0.3f, 0.8f);
        glVertex2f(0.45f, 0.8f);
        glVertex2f(0.45f, 0.5f);
        glVertex2f(0.3f, 0.5f);
    glEnd();

    float brightRed = (lightState == 0) ? 1.0f : 0.3f;      // Active = bright, Inactive = dim
    float brightYellow = (lightState == 1) ? 0.8f : 0.3f;  // Yellow light slightly dimmer
    float brightGreen = (lightState == 2) ? 1.0f : 0.3f;

    // Red Light
    glColor3f(brightRed, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        float cx = 0.375f, cy = 0.75f, r = 0.03f;
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * M_PI / 180;
            glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
        }
    glEnd();

    // Yellow Light
    glColor3f(brightYellow, brightYellow * 0.75f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        cy = 0.65f;
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * M_PI / 180;
            glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
        }
    glEnd();

    // Green Light
    glColor3f(0.0f, brightGreen, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        cy = 0.55f;
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * M_PI / 180;
            glVertex2f(cx + r * cos(angle), cy + r * sin(angle));
        }
    glEnd();
}



// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawRoad();
    drawTrafficLight();
    drawCar();
    drawPedestrian();

    // Display control instructions
    renderText(-0.9f, 0.9f, "Controls: ");
    renderText(-0.9f, 0.85f, "[Arrow Keys] Move Car");
    renderText(-0.9f, 0.8f, "[SPACE] Restart");
    renderText(-0.9f, 0.75f, "[ESC] Exit");

    glutSwapBuffers();
}

// Keyboard input function
void handleKeys(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Escape key
            exit(0);
            break;
        case ' ':
            carX = -1.0f;  // Reset car position
            collisionOccurred = false;  // Allow movement again
            pedestrianBlink = false;  // Stop blinking
            pedestrianY = -0.5f; // Reset pedestrian
            
            glutTimerFunc(500, pedestrianBlinkEffect, 0); // Restart blink timer
            break;
        
    }
}

// Special keys function (for arrow keys)
void handleSpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT:  // Move car right
            carX += 0.05f;
            break;
        case GLUT_KEY_LEFT:  // Move car left
            carX -= 0.05f;
            break;
    }
    glutPostRedisplay();
}

// Update function for animation
void update(int value) {

    // Move the pedestrian only when the light is red
    if (!collisionOccurred) { // Stop movement on collision
        if (lightState == 0) { 
            if (pedestrianY < 0.3f) {  // Pedestrian moves until they reach the other side
                pedestrianY += 0.01f;
            }
        }
    }

    // Check for collision
    if (checkCollision()) {
        if (!collisionOccurred) {  // Only trigger once
            collisionOccurred = true;
            pedestrianBlink = true;
            glutTimerFunc(500, pedestrianBlinkEffect, 0); // Start blinking effect
        }
    }
    

    glutPostRedisplay();
    glutTimerFunc(50, update, 0);
}
// Function to change traffic light state
void changeLight(int value) {
    lightState = (lightState + 1) % 3;  // Cycle through 0 (Red) → 1 (Yellow) → 2 (Green)
    glutTimerFunc(3000, changeLight, 0);
}

// Initialize OpenGL
void init() {
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-1, 1, -1, 1);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Car and Pedestrian Crossing");

    init();
    glutDisplayFunc(display);

    glutKeyboardFunc(handleKeys);
    glutSpecialFunc(handleSpecialKeys);

    glutTimerFunc(50, update, 0);
    glutTimerFunc(3000, changeLight, 0);
    glutTimerFunc(500, pedestrianBlinkEffect, 0); // Start blinking timer

    glutMainLoop();
    return 0;
}
