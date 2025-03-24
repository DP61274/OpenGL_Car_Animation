// Compile the program : g++ cars_Brasenham.cpp -o cars_Brasenham -lGL -lGLU -lglut
// Run the executable : ./cars_Brasenham

#include <GL/glut.h>
#include <cmath>
#include <iostream>

// Global variables for animation states
float carX = -1.0f; // Car starts from the left
float pedestrianY = -0.5f; // Pedestrian starts at the bottom edge of the road
bool pedestrianMoving = true;
int lightState = 0; // 0 = Red, 1 = Yellow, 2 = Green
bool collisionOccurred = false; 
bool pedestrianBlink = false;
bool showAccidentText = false;

// Bresenham's Line Algorithm
void drawLine(int x1, int y1, int x2, int y2) {
int dx = abs(x2 - x1), dy = abs(y2 - y1);
int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
int err = dx - dy;

while (true) {
glVertex2f(x1 / 100.0, y1 / 100.0);
if (x1 == x2 && y1 == y2) break;
int e2 = 2 * err;
if (e2 > -dy) { err -= dy; x1 += sx; }
if (e2 < dx) { err += dx; y1 += sy; }
}
}
// Bresenham's Line Algorithm with thickness support
void drawThickLine(int x1, int y1, int x2, int y2, int thickness) {
for (int i = -thickness; i <= thickness; i++) {
int dx = abs(x2 - x1), dy = abs(y2 - y1);
int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
int err = dx - dy, e2;
int x = x1, y = y1;

while (true) {
glVertex2f((x + i) / 100.0, y / 100.0);
if (x == x2 && y == y2) break;
e2 = 2 * err;
if (e2 > -dy) { err -= dy; x += sx; }
if (e2 < dx) { err += dx; y += sy; }
}
}
}

// Bresenham's pole Line Algorithm 
void drawPoleLine(int x1, int y1, int x2, int y2, int thickness = 1) {
for (int t = -thickness / 2; t <= thickness / 2; t++) {
int dx = abs(x2 - x1), dy = abs(y2 - y1);
int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
int err = dx - dy;
int x = x1, y = y1;
while (true) {
glVertex2f((x + t) / 100.0, y / 100.0);
if (x == x2 && y == y2) break;
int e2 = 2 * err;
if (e2 > -dy) { err -= dy; x += sx; }
if (e2 < dx) { err += dx; y += sy; }
}
}
}

// Bresenham's Circle Algorithm with thickness support
void drawThickCircle(int centerX, int centerY, int radius, int thickness) {
for (int t = -thickness; t <= thickness; t++) {
int x = radius, y = 0;
int radiusError = 1 - x;
while (x >= y) {
glVertex2f((centerX + x + t) / 100.0, (centerY + y) / 100.0);
glVertex2f((centerX - x + t) / 100.0, (centerY + y) / 100.0);
glVertex2f((centerX + x + t) / 100.0, (centerY - y) / 100.0);
glVertex2f((centerX - x + t) / 100.0, (centerY - y) / 100.0);
glVertex2f((centerX + y + t) / 100.0, (centerY + x) / 100.0);
glVertex2f((centerX - y + t) / 100.0, (centerY + x) / 100.0);
glVertex2f((centerX + y + t) / 100.0, (centerY - x) / 100.0);
glVertex2f((centerX - y + t) / 100.0, (centerY - x) / 100.0);
y++;
if (radiusError < 0) {
radiusError += 2 * y + 1;
} else {
x--;
radiusError += 2 * (y - x) + 1;
}
}
}
}

//Function to Render Text
void renderText(float x, float y, const char* text) {
glColor3f(0.0f, 0.0f, 0.0f); 
glRasterPos2f(x, y);

// Loop through each character and draw it
for (const char* c = text; *c != '\0'; c++) {
glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}
}

// Function to draw the road
void drawRoad() {
glColor3f(0.5f, 0.5f, 0.5f);
glBegin(GL_QUADS);
glVertex2f(-1.0f, -0.5f);
glVertex2f(1.0f, -0.5f);
glVertex2f(1.0f, 0.0f);
glVertex2f(-1.0f, 0.0f);
glEnd();
// Draw lane divider using Bresenham's Algorithm
glColor3f(1.0f, 1.0f, 1.0f);
glBegin(GL_POINTS);
for (int x = -100; x <= 100; x += 10) {
drawLine(x, -25, x + 5, -25);
}
glEnd();
// Draw pedestrian crossing 
float shiftX = 0.5f;
for (float i = -0.5f; i <= 0.0f; i += 0.05f) {

glColor3f((int(i * 20) % 2 == 0) ? 0.1f : 0.8f, 
(int(i * 20) % 2 == 0) ? 0.1f : 0.8f, 
(int(i * 20) % 2 == 0) ? 0.1f : .8f);
glBegin(GL_QUADS);
glVertex2f(0.1f + shiftX, i); 
glVertex2f(0.4f + shiftX, i); 
glVertex2f(0.4f + shiftX, i + 0.05f); 
glVertex2f(0.1f + shiftX, i + 0.05f);
glEnd();
}
}

// Function to draw the pedestrian
void drawPedestrian() {
    if (collisionOccurred && pedestrianBlink) {
        glColor3f(1.0f, 0.0f, 0.0f); // Blink red on collision
    } else {
        glColor3f(0.72f, 0.45f, 0.2f); // Normal skin color
    }

    glBegin(GL_POINTS);
    
    // Head using Bresenham Circle with thickness
    drawThickCircle(77, pedestrianY * 100 + 15, 3, 1);

    // Shirt with Horizontal Lines
    glColor3f(0.0, 0.8, 0.0); 
    glBegin(GL_POINTS);
    for (int y = pedestrianY * 100; y <= pedestrianY * 100 + 10; y += 2) { 
        drawThickLine(74, y, 80, y, 1);
    }
    glEnd();

    // Arms
    glColor3f(0.72f, 0.45f, 0.2f);
    glBegin(GL_POINTS);
    drawThickLine(74, pedestrianY * 100 + 7, 72, pedestrianY * 100 + 7, 2); // Left Arm
    drawThickLine(80, pedestrianY * 100 + 7, 82, pedestrianY * 100 + 7, 2); // Right Arm
    glEnd();

    // Legs
    glColor3f(0.72f, 0.45f, 0.2f); 
    glBegin(GL_POINTS);
    drawThickLine(75, pedestrianY * 100 - 8, 75, pedestrianY * 100, 1); // Left Leg
    drawThickLine(79, pedestrianY * 100 - 8, 79, pedestrianY * 100, 1); // Right Leg
    glEnd();

    glEnd();
}

// Function to detect collision
bool checkCollision() {
    //car boundaries
float carLeft = carX - 0.15f; 
float carRight = carX + 0.15f;
    //pedestrian boundaries
float pedestrianLeft = 0.74f; 
float pedestrianRight = 0.8f; 

bool xOverlap = (carRight >= pedestrianLeft && carLeft <= pedestrianRight);
bool yOverlap = (pedestrianY >= -0.15f && pedestrianY <= -0.05f);
return xOverlap && yOverlap;
}

void pedestrianBlinkEffect(int value) {
if (collisionOccurred) {
pedestrianBlink = !pedestrianBlink;
showAccidentText = pedestrianBlink;
glutPostRedisplay(); 
glutTimerFunc(300, pedestrianBlinkEffect, 0);
}
}

// Function to draw the car outline using Bresenham's Algorithm
void drawCar() {
    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_POINTS);

    int baseX = carX * 100;  // Convert car position to integer space
    int baseY = -15;         // Set base Y position for the car

    // Car Body (Rectangle using Bresenham)
    drawLine(baseX - 15, baseY, baseX + 15, baseY);      
    drawLine(baseX - 15, baseY + 10, baseX + 15, baseY + 10); 
    drawLine(baseX - 15, baseY, baseX - 15, baseY + 10); 
    drawLine(baseX + 15, baseY, baseX + 15, baseY + 10); 

    // Car Roof (Smaller rectangle on top)
    drawLine(baseX - 10, baseY + 10, baseX + 10, baseY + 10); 
    drawLine(baseX - 10, baseY + 15, baseX + 10, baseY + 15);
    drawLine(baseX - 10, baseY + 10, baseX - 10, baseY + 15);
    drawLine(baseX + 10, baseY + 10, baseX + 10, baseY + 15);

    glEnd();

    // Draw Wheels using Bresenham's Circle Algorithm
    glColor3f(0.0f, 0.0f, 0.0f); 
    glBegin(GL_POINTS);
    drawThickCircle(baseX - 10, baseY - 5, 4, 1); // Left wheel
    drawThickCircle(baseX + 10, baseY - 5, 4, 1); // Right wheel
    glEnd();
}


// Function to draw the traffic light using Bresenham
void drawTrafficLight() {
// Draw Traffic Light Pole
glColor3f(0.3f, 0.3f, 0.3f);
glBegin(GL_POINTS);
drawPoleLine(37.5, 50, 37.5, 0, 3);
glEnd();

// Traffic light box
glColor3f(0.1f, 0.1f, 0.1f);
glBegin(GL_QUADS);
glVertex2f(0.3f, 0.8f);
glVertex2f(0.45f, 0.8f);
glVertex2f(0.45f, 0.5f);
glVertex2f(0.3f, 0.5f);
glEnd();

glColor3f(1.0f, 0.0f, 0.0f);
glBegin(GL_POINTS);
drawLine(30, 80, 45, 80); 
drawLine(30, 50, 45, 50); 
drawLine(30, 80, 30, 50); 
drawLine(45, 80, 45, 50); 
glEnd();
     // Active = bright, Inactive = dim
float brightRed = (lightState == 0) ? 1.0f : 0.3f; 
float brightYellow = (lightState == 1) ? 0.8f : 0.3f;
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

// Show "Accident" message if collision occurs and blinking effect is ON
if (collisionOccurred && showAccidentText) {
    glColor3f(1.0f, 0.0f, 0.0f);  
    glRasterPos2f(-0.1f, 0.4f); 
    const char* message = "ACCIDENT!";
    for (const char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

glutSwapBuffers();
}

// Keyboard input function
void handleKeys(unsigned char key, int x, int y) {
switch (key) {
    // Escape key
case 27: 
exit(0);
break;
     //space key
case ' ':
carX = -1.0f; // Reset car position
collisionOccurred = false; 
pedestrianBlink = false; 
showAccidentText = false;
pedestrianY = -0.5f; // Reset pedestrian
glutTimerFunc(500, pedestrianBlinkEffect, 0); // Restart blink timer
break;
}
}

// Special keys function (for arrow keys)
void handleSpecialKeys(int key, int x, int y) {
switch (key) {
case GLUT_KEY_RIGHT: // Move car right
carX += 0.05f;
break;
case GLUT_KEY_LEFT: // Move car left
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
if (pedestrianY < 0.3f) { // Pedestrian moves until they reach the other side
pedestrianY += 0.01f;
}
}
}

// Check for collision
if (checkCollision()) {
if (!collisionOccurred) { // Only trigger once
collisionOccurred = true;
pedestrianBlink = true;
showAccidentText = true; 
glutTimerFunc(500, pedestrianBlinkEffect, 0); 
}
}

glutPostRedisplay();
glutTimerFunc(50, update, 0);
}
// Function to change traffic light state
void changeLight(int value) {
lightState = (lightState + 1) % 3; // Cycle through 0 (Red) → 1 (Yellow) → 2 (Green)
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
glutTimerFunc(500, pedestrianBlinkEffect, 0); 
glutMainLoop();
return 0;
}
