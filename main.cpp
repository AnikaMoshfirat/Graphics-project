#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//anika's code
#include<windows.h>
#include<mmsystem.h>
#define PI 3.14159f
#include <algorithm>
 // For srand, rand
#include <ctime>   // For time

int lastClickTime = 0; // Store the last click time
const int DOUBLE_CLICK_TIME = 300; // Maximum time between clicks (in milliseconds)

int currentDisplay = 1;
int i;
float cloudOffset = -1.0f;
float planeX = 1.0f;
float planeY = -0.28f;
float speed = 0.006;
float angle = 0.0;
bool isAscending = false;
bool hasFlown = false;

void drawCircle(float x, float y, float radius)
{
    int numSegments = 100; // Smoothness of the circle
    float angle;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the circle
    for (int i = 0; i <= numSegments; i++)
    {
        angle = (i * 2.0f * PI )/ numSegments;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

// Function to draw the sky
/*void drawSky() {
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.8, 1.0); // Light blue
    glVertex2f(-1.0, 0.0);
    glVertex2f(1.0, 0.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);
    glEnd();
}*/
void drawSky() {
    glBegin(GL_QUADS);

    // Sky gradient - Light blue at the top, blending to a yellowish color near the sun

    // Top part of the sky (Light Blue)
    glColor3f(0.5, 0.8, 1.0);
    glVertex2f(-1.0, 0.0);
    //glVertex2f(1.0, 0.0);
    glVertex2f(-1.0, 1.0);
    glVertex2f(1.0, 1.0);

    // Transition area towards the sun (Yellowish blend)
    glColor3f(0.8, 0.7, 0.2); // Yellowish blend
    glVertex2f(1.0, 0.2); // Adjust 0.2 to control the gradient height/start
    glVertex2f(-1.0, 0.2);

    // Bottom part of the sky (more Yellowish close to horizon/sun)
    glColor3f(0.9, 0.8, 0.0); // More yellowish
    glVertex2f(-1.0, -0.2); // Adjust -0.2 to control the gradient height/end
    glVertex2f(1.0, -0.2);

    glEnd();
}

void drawGround() {
    glBegin(GL_QUADS);

    for (float x = -1.0; x < 1.0; x += 0.02) {
        for (float y = -1.0; y < 0.0; y += 0.02) {

            // Calculate varying green with a stronger light green effect
            float green = 0.5 + (x + 1.0) * 0.1 + (y + 1.0) * 0.1; // Reduced base green variation
            float lightGreenFactor = (1.0 - (y + 1.0)) * 0.3; // Increased light green influence
            green += lightGreenFactor;

            // Add a small amount of blue, more subtle
            float blue = (1.0 - (y + 1.0)) * 0.03; // Reduced blue amount

            // Clamp colors
            green = std::fmin(1.0f, std::fmax(0.0f, green));
            blue = std::fmin(1.0f, std::fmax(0.0f, blue));

            glColor3f(0.0, green, blue);

            glVertex2f(x, y);
            glVertex2f(x + 0.02, y);
            glVertex2f(x + 0.02, y + 0.02);
            glVertex2f(x, y + 0.02);
        }
    }
    glEnd();
}

// Function to draw a rectangle
void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function to display text on the screen
void renderBitmapString(float x, float y, float z, void *font, char *string) {
    char *c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Function to draw the terminal
void drawStripes(float x, float y, float width, float height, float stripeWidth, float r, float g, float b, float r2, float g2, float b2) {
    for (float i = 0; i < width; i += stripeWidth * 2) {
        drawRectangle(x + i, y, stripeWidth, height, r, g, b);
        drawRectangle(x + i + stripeWidth, y, stripeWidth, height, r2, g2, b2);
    }
}


// Function to draw the terminal
void drawTerminal() {
  // 1. Curved Canopy (Darker Brown)
    glColor3f(0.2, 0.15, 0.1);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0, 0.3);
    for (float angle = 0; angle <= PI; angle += PI / 200.0) {
        float x = 0.45 * cos(angle);
        float y = 0.3 * sin(angle) + 0.3;
        glVertex2f(x, y);
    }
    glEnd();

       // Central arch
    glBegin(GL_POLYGON);
    glColor3f(0.35, 0.25, 0.15); // Ash color
    for (float angle = 0; angle <= M_PI; angle += 0.01) {
        float x = 0.3 * cos(angle);
        float y = 0.3 * sin(angle) + 0.3;
        glVertex2f(x, y);
    }
    glEnd();

    // Main building
    drawRectangle(-0.8, 0.0, 1.6, 0.3, 1.9f, 1.9f, 1.9f);

    // Windows
    float windowX[] = {-0.64, -0.46, -0.27, 0.15, 0.34, 0.53};
    for (int i = 0; i < 6; i++) {
        drawStripes(windowX[i], 0.1, 0.1, 0.1, 0.003, 0.5, 0.8, 0.9, 0.2, 0.2, 0.4);
    }

    // Door
    drawRectangle(-0.1, 0.0, 0.19, 0.18, 0.5, 0.5, 0.5);

    // Side buildings
    drawRectangle(-0.8, 0.0, 0.15, 0.4, 0.698f, 0.745f, 0.710f);
    drawRectangle(0.7, 0.0, 0.1, 0.4, 0.698f, 0.745f, 0.710f);

    // Display the station name
    glColor3f(1.0, 1.0, 0.0);
    renderBitmapString(-0.18f, 0.32f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "BANGLADESH AIRPORT");
}
void drawBuilding(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void drawWindow(float x, float y, float width, float height) {
    glColor3f(1.0, 1.0, 1.0); // White color
    glBegin(GL_POLYGON);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}


void drawBuildingWithWindows(float x, float y, float width, float height) {
    drawBuilding(x, y, width, height, 0.0, 0.0, 0.0); // Black building

    // Draw the windows on the building
    float windowWidth = 0.05;
    float windowHeight = 0.1;
    float windowSpacingX = 0.08;
    float windowSpacingY = 0.15;

    glColor3f(1.0, 1.0, 1.0);
    for (int i = 0; i < 3; ++i) { // Rows of windows
        for (int j = 0; j < 3; ++j) { // Columns of windows
            float windowX = x + 0.05 + j * windowSpacingX; // Adjust offsets as needed
            float windowY = y + 0.1 + i * windowSpacingY; // Adjust offsets as needed
            drawWindow(windowX, windowY, windowWidth, windowHeight);
        }
    }
}

// Function to draw the control tower
void drawControlTower() {
    // Base of the tower (tapered rectangle)
    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.6, 0.7); // Light blue-gray
    glVertex2f(0.47, 0.3); // Bottom left
    glVertex2f(0.52, 0.3); // Bottom right
    glVertex2f(0.55, 0.55); // Top right (slightly narrower)
    glVertex2f(0.44, 0.55); // Top left (slightly narrower)
    glEnd();

    // Circular top of the tower
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.5, 0.5); // Light gray
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float x = 0.05 * cos(angle) + 0.5;
        float y = 0.05 * sin(angle) + 0.6;
        glVertex2f(x, y);
    }
    glEnd();

    // Antenna on top
    glBegin(GL_LINES);
    glColor3f(0.1, 0.1, 0.1); // Dark gray
    glVertex2f(0.5, 0.62); // Bottom of the antenna
    glVertex2f(0.5, 0.72); // Top of the antenna
    glEnd();

    // Base for the antenna
    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.3, 0.3); // Dark gray
    glVertex2f(0.48, 0.62);
    glVertex2f(0.52, 0.62);
    glVertex2f(0.52, 0.65);
    glVertex2f(0.48, 0.65);
    glEnd();
}

// Function to draw the traffic light
void drawTrafficLight() {
    float baseX = -0.2; // Adjust position near horizontal runway
    float baseY = -0.36;
    float lightWidth = 0.03; // Smaller size
    float lightHeight = 0.09; // Smaller size

    // Pole
    drawRectangle(baseX + 0.01, baseY - 0.2, 0.005, 0.2, 0.1, 0.1, 0.1);

    // Light housing
    drawRectangle(baseX, baseY, lightWidth, lightHeight, 0.1, 0.1, 0.1);

    // Red light
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(baseX + lightWidth / 2 + 0.005 * cos(angle), baseY + 0.06 + 0.005 * sin(angle));
    }
    glEnd();

    // Yellow light
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 0.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(baseX + lightWidth / 2 + 0.005 * cos(angle), baseY + 0.04 + 0.005 * sin(angle));
    }
    glEnd();

    // Green light
    glBegin(GL_POLYGON);
    glColor3f(0.0, 1.0, 0.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(baseX + lightWidth / 2 + 0.005 * cos(angle), baseY + 0.02 + 0.005 * sin(angle));
    }
    glEnd();
}

// Function to draw the runway
void drawRunway() {
    float offsetY = -0.3; // Adjust the entire runway downward
    float runwayWidth = 0.3; // Increased runway width

    // Vertical runway
    drawRectangle(-0.15, -0.7 + offsetY, runwayWidth, 1.0, 0.3, 0.3, 0.3); // Vertical tarmac

    // Horizontal runway
    drawRectangle(-1.0, -0.2 + offsetY, 2.0, runwayWidth, 0.3, 0.3, 0.3); // Horizontal tarmac

    // Runway centerlines (vertical dashed line)
    for (float y = -0.9; y <= -0.2; y += 0.2) { // Exclude overlapping region
        drawRectangle(-0.01, y + offsetY, 0.02, 0.1, 1.0, 1.0, 1.0);
    }
    for (float y = 0.09; y <= 0.1; y += 0.2) { // Above the overlap
        drawRectangle(-0.01, y + offsetY, 0.02, 0.1, 1.0, 1.0, 1.0);
    }

    // Runway centerlines (horizontal dashed line)
    for (float x = -0.9; x <= 0.9; x += 0.2) {
        drawRectangle(x, -0.03 + offsetY, 0.1, 0.04, 1.0, 1.0, 1.0);
    }

    // Side boundary lines (vertical)
    drawRectangle(-0.16, -0.7 + offsetY, 0.01, 0.5, 1.0, 1.0, 1.0); // Left boundary below overlap
    drawRectangle(0.14, -0.7 + offsetY, 0.01, 0.5, 1.0, 1.0, 1.0);  // Right boundary below overlap
    drawRectangle(-0.16, 0.1 + offsetY, 0.01, 0.2, 1.0, 1.0, 1.0);  // Left boundary above overlap
    drawRectangle(0.14, 0.1 + offsetY, 0.01, 0.2, 1.0, 1.0, 1.0);   // Right boundary above overlap

    // Side boundary lines (horizontal)
    drawRectangle(-1.0, -0.22 + offsetY, 0.85, 0.02, 1.0, 1.0, 1.0); // Left boundary before overlap
    drawRectangle(0.15, -0.22 + offsetY, 0.9, 0.02, 1.0, 1.0, 1.0);  // Right boundary after overlap
    drawRectangle(-1.0, 0.1 + offsetY, 0.85, 0.02, 1.0, 1.0, 1.0);   // Left boundary before overlap
    drawRectangle(0.15, 0.1 + offsetY, 0.9, 0.02, 1.0, 1.0, 1.0);    // Right boundary after overlap
}

// Function to draw clouds
void drawCloud(float x, float y) {
    x += cloudOffset;

    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.1 * cos(angle);
        float dy = 0.05 * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.04 * sin(angle);
        glVertex2f(x + 0.12 + dx, y + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.04 * sin(angle);
        glVertex2f(x - 0.12 + dx, y + dy);
    }
    glEnd();
}


// Function to draw trees
void drawTree(float x, float y) {
    // Trunk
    drawRectangle(x, y, 0.05, 0.15, 0.55, 0.27, 0.07);

    // Leaves (layered circles)
    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.8, 0.2);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.1 * cos(angle);
        float dy = 0.1 * sin(angle);
        glVertex2f(x + 0.025 + dx, y + 0.2 + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.7, 0.2);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.08 * sin(angle);
        glVertex2f(x + 0.025 + dx, y + 0.3 + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.2, 0.6, 0.2);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.06 * cos(angle);
        float dy = 0.06 * sin(angle);
        glVertex2f(x + 0.025 + dx, y + 0.38 + dy);
    }
    glEnd();
}


// Function to draw a single palm leaf (now a more elongated shape)
void drawPalmLeaf(float x, float y, float width, float height, float angle) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.0f, 0.6f, 0.0f); // Darker green

    glBegin(GL_QUADS);
    // Adjust these values to control leaf shape
    glVertex2f(0, 0); // Base
    glVertex2f(width * 0.2, height * 0.8); // Right side
    glVertex2f(0, height); // Top
    glVertex2f(-width * 0.2, height * 0.8); // Left side
    glEnd();

    glPopMatrix();
}

void PalmTree(float x, float y, float size) {
    // Trunk (Brown, Tapered)
    glColor3f(0.5f, 0.35f, 0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(x - size * 0.04, y);
    glVertex2f(x + size * 0.04, y);
    glVertex2f(x + size * 0.01, y + size * 0.6); // Tapered top
    glVertex2f(x - size * 0.01, y + size * 0.6); // Tapered top
    glEnd();


   // --- Leaves ---
    int numLeaves = 10;
    float leafWidth = size * 0.25;
    float leafHeight = size * 0.6;
    float angleStep = 360.0f / numLeaves;

    float crownRadius = size * 0.4;

    // Seed the random number generator ONCE at the beginning of your program
    static bool seeded = false; // Ensure seeding only happens once
    if (!seeded) {
        srand(time(0)); // Seed with the current time
        seeded = true;
    }

    for (int i = 0; i < numLeaves; ++i) {
        float angle = i * angleStep;

        // ... (Leaf position calculation remains the same)

        float leafAngle = angle + 90.0f;
float leafX = 0.0f; // Initialize to 0
        float leafY = 0.0f; // Initialize to 0

        // Calculate leaf position offset from center - on a circle
        leafX = x + crownRadius * cos(angle * M_PI / 180.0f);
        leafY = y + size * 0.7 + crownRadius * sin(angle * M_PI / 180.0f);
        // Add some randomness to the angle (NOW CORRECTED)
      //  float angleRandomness = 20.0f;
       // leafAngle += (rand() % (int)(2 * angleRandomness + 1)) - angleRandomness; // + 1 to avoid 0

        drawPalmLeaf(leafX, leafY, leafWidth, leafHeight, leafAngle);
    }
}


void treebig()
{

    /**....................................... Big Tree (Left) ..........................................**/


    /**....................................... Big Tree (Left) Stem ..........................................**/

    glPushMatrix();
    glTranslated(-1.66f,-0.19f,0.0f);
    glBegin(GL_QUADS);
    glColor3f(0.314, 0.184, 0.149);
    glVertex2f(0.73f, 0.19f);
    glVertex2f(0.744f, 0.472f);
    glVertex2f(0.754f, 0.472f);
    glVertex2f(0.764f, 0.19f);

    glVertex2f(0.738f, 0.255f);
    glVertex2f(0.736f, 0.274f);
    glVertex2f(0.684f, 0.303f);
    glVertex2f(0.680f, 0.298f);

    glVertex2f(0.759f, 0.303f);
    glVertex2f(0.759f, 0.319f);
    glVertex2f(0.815f, 0.350f);
    glVertex2f(0.824f, 0.347f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(0.314, 0.184, 0.149);
    glVertex2f(0.744f, 0.408f);
    glVertex2f(0.687f, 0.460f);
    glVertex2f(0.744f, 0.424f);

    glVertex2f(0.755f, 0.421f);
    glVertex2f(0.815f, 0.493f);
    glVertex2f(0.755f, 0.439f);
    glEnd();
    glPopMatrix();
}

void tree_leaf1()
{
    /**....................................... Big Tree (Left) Top Leaf ..........................................**/
    glPushMatrix();
    glTranslated(-1.7f,-0.14f,0.0f);

    drawCircle(0.644,0.532,0.06);

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.644,0.477,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.691,0.568,0.07);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.771,0.563,0.06);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.814,0.562,0.045);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.835,0.543,0.05);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.87,0.438,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.749,0.457,0.06);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.796,0.485,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.709,0.442,0.05);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.89,0.517,0.055);

    /**....................................... Big Tree (Left) Right Leaf ..........................................**/

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.855,0.276,0.035);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.896,0.290,0.025);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.836,0.306,0.03);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.881,0.327,0.03);


    /**....................................... Big Tree (Left) Left Leaf ..........................................**/

    glPushMatrix();
    glTranslated(-0.14f,-0.11f,0.0f);

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.865,0.376,0.035);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.896,0.390,0.025);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.835,0.406,0.03);

    glColor3f(0.694, 0.78, 0.294);
    drawCircle(0.802,0.384,0.03);

    glColor3f(0.549, 0.639, 0.153);
    drawCircle(0.836,0.366,0.035);

    glColor3f(0.647, 0.745, 0.184);
    drawCircle(0.881,0.408,0.03);

    glPopMatrix();

    glPopMatrix();

}

// Function to draw the sun
void drawSun(float x, float y, float radius) {
    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 0.5);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = radius * cos(angle);
        float dy = radius * sin(angle);
        glVertex2f(x + dx, y + dy);
    }

        glEnd();


    // Add a glow effect around the sun (optional, but adds a nice touch)
    glEnable(GL_BLEND); // Enable blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending

    for (float r = radius; r < radius + 0.04; r += 0.02) { // Larger radius for glow
        float alpha = 0.4 - (r - radius) / 0.2; // Alpha decreases with distance
        glColor4f(1.0, 1.0, 0.0, alpha); // Yellow with alpha (transparency)
        glBegin(GL_POLYGON);
        for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
            float dx = r * cos(angle);
            float dy = r * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
        glEnd();
    }
    glDisable(GL_BLEND);
}

// Drawing Of Birds
void drawCircle(float x, float y, float radius);

// Birds
void circle_bird(float x1, float y1, float r) {
    int i;

    GLfloat x = x1;
    GLfloat y = y1;
    GLfloat radius = r;
    int triangleAmount = 100; // # of lines used to draw circle

    // Get the aspect ratio of the window
    int width2 = glutGet(GLUT_WINDOW_WIDTH);
    int height2 = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat aspect2 = (GLfloat)height2 / (GLfloat)width2; // Aspect ratio (height/width)

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;

    glColor3ub(80, 80, 80);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {
        glVertex2f(x + (radius * cos(i * twicePi / triangleAmount) * aspect2),
                     y + (radius * sin(i * twicePi / triangleAmount)));
    }
    glEnd();
}

void circle(float x1, float y1, float r, char c) {
    int i;
    // Get the aspect ratio of the window
    int width2 = glutGet(GLUT_WINDOW_WIDTH);
    int height2 = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat aspect2 = (GLfloat)height2 / (GLfloat)width2; // Aspect ratio (height/width)

    GLfloat x = x1;
    GLfloat y = y1;
    GLfloat radius = r;
    int triangleAmount = 100; // # of lines used to draw circle

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;
    // glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {

        if (c == 's') {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                         y + (radius * sin(i * twicePi / triangleAmount)));

        } else {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                         y + (radius * sin(i * twicePi / triangleAmount)));

        }
    }
    glEnd();
}

// Function to draw a bird
void draw_bird(float xAdjust, float yAdjust, int birdType) {
    int wingColorR = 255, wingColorG = 140, wingColorB = 0;
    int bodyColorR = 255, bodyColorG = 255, bodyColorB = 0;
    int beakColorR = 190, beakColorG = 0, beakColorB = 0;

    if (birdType == 1) { // Green birds
        wingColorR = 0; wingColorG = 215; wingColorB = 0;
        bodyColorR = 0; bodyColorG = 100; bodyColorB = 0;
    } else if (birdType == 2) { // Black birds
        wingColorR = 0; wingColorG = 0; wingColorB = 0;
        bodyColorR = 50; bodyColorG = 50; bodyColorB = 50;
        beakColorR = 0; beakColorG = 0; beakColorB = 0;
    }

    glColor3ub(wingColorR, wingColorG, wingColorB);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f + xAdjust, 0.508f + yAdjust);
    glVertex2f(-0.5f + xAdjust, 0.492f + yAdjust);
    glVertex2f(-0.48f + xAdjust, 0.50f + yAdjust);
    glEnd();

    glColor3ub(bodyColorR, bodyColorG, bodyColorB);
    glBegin(GL_QUADS);
    glVertex2f(-0.51f + xAdjust, 0.49f + yAdjust);
    glVertex2f(-0.53f + xAdjust, 0.55f + yAdjust);
    glVertex2f(-0.53f + xAdjust, 0.52f + yAdjust);
    glVertex2f(-0.52f + xAdjust, 0.47f + yAdjust);
    glEnd();

    glColor3ub(0, 0, 0);
    circle_bird(-0.5 + xAdjust, 0.5 + yAdjust, 0.01);

    glColor3ub(beakColorR, beakColorG, beakColorB);
    circle(-0.52 + xAdjust, 0.49 + yAdjust, 0.015, 'l');

    glColor3ub(bodyColorR, bodyColorG, bodyColorB);
    glBegin(GL_POLYGON);
    glVertex2f(-0.52f + xAdjust, 0.49f + yAdjust);
    glVertex2f(-0.55f + xAdjust, 0.47f + yAdjust);
    glVertex2f(-0.535f + xAdjust, 0.465f + yAdjust);
    glVertex2f(-0.55f + xAdjust, 0.43f + yAdjust);
    glVertex2f(-0.515f + xAdjust, 0.485f + yAdjust);
    glEnd();
}

// Bird data
float birdX[8] = {-0.06f, -0.06f, -0.12f, -0.12f, -0.07f, -0.07f, -0.12f, 1.2f};  // Last bird starts from right
float birdY[8] = {0.05f, 0.09f, 0.03f, 0.07f, 0.25f, 0.29f, 0.24f, 0.21f};
int birdColorType[8] = {0, 1, 2, 0, 1, 2, 0, 1};
float birdMoveX[8] = {0.0055f, 0.0055f, 0.0035f, 0.0055f, 0.0035f, 0.0035f, 0.0055f, -0.007f}; // Last bird moves left
bool birdMovingRight[8] = {true, true, true, true, true, true, true, true};


// Function to draw the road light
void drawRoadLight(float x, float y) {
    // Base of the pole
    drawRectangle(x - 0.02, y, 0.04, 0.05, 0.2, 0.2, 0.2); // Dark gray

    // Pole
    drawRectangle(x - 0.005, y + 0.05, 0.01, 0.4, 0.5, 0.5, 0.5); // Light gray

    // Y-shaped structure
    glBegin(GL_QUADS);
    glColor3f(0.5, 0.5, 0.5); // light gray
    glVertex2f(x - 0.005, y + 0.4);
    glVertex2f(x + 0.005, y + 0.4);
    glVertex2f(x + 0.05, y + 0.55);
    glVertex2f(x + 0.04, y + 0.55);

    glVertex2f(x - 0.005, y + 0.4);
    glVertex2f(x + 0.005, y + 0.4);
    glVertex2f(x - 0.05, y + 0.55);
    glVertex2f(x - 0.04, y + 0.55);
    glEnd();

    // Left lamp
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0); // Light Yellow
    glVertex2f(x - 0.06, y + 0.53);
    glVertex2f(x - 0.03, y + 0.53);
    glVertex2f(x - 0.03, y + 0.58);
    glVertex2f(x - 0.06, y + 0.58);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2); //  Dark gray
    glVertex2f(x - 0.06, y + 0.58);
    glVertex2f(x - 0.03, y + 0.58);
    glVertex2f(x - 0.03, y + 0.61);
    glVertex2f(x - 0.06, y + 0.61);
    glEnd();

    // Right lamp
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0); // Light Yellow
    glVertex2f(x + 0.03, y + 0.53);
    glVertex2f(x + 0.06, y + 0.53);
    glVertex2f(x + 0.06, y + 0.58);
    glVertex2f(x + 0.03, y + 0.58);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2); //  dark gray
    glVertex2f(x + 0.03, y + 0.58);
    glVertex2f(x + 0.06, y + 0.58);
    glVertex2f(x + 0.06, y + 0.61);
    glVertex2f(x + 0.03, y + 0.61);
    glEnd();
}
//Human
void human1()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(-0.01f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(1.4f, 0.540f, 1.10f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}


void human2()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.02f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.7f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.698f, 0.745f, 0.710f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}
void human3()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.07f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.3, 0.3, 0.6);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.698f, 0.745f, 0.710f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}

void human4()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.09f,-0.32f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.7f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}

void human5()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.17f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}

void human6()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.2f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(0.54, 0.77, 0.8);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.3, 0.3, 0.6);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}

void human7()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.33f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}
void human8()
{

    /**..........................................Human...............................................**/
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.39f,-0.37f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }
    glEnd();

/// Hair bun for the head
    x=0.284f;
    y=0.278f;
    radius =0.0087f;

    triangleAmount = 100;
    twicePi = 2.0f * PI;

    glColor3f(0.0f, 0.0f, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.0f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///.........................Shari.....................
    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.27, 0.5);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glVertex2f(0.31f, 0.26f);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.5, 0.27, 0.5);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();
    glPopMatrix();
}

void human9()
{
    ///..........Head..........
    glPushMatrix();
    glTranslatef(0.2f,-0.3f,0.0f);
    glScalef(0.9f,0.9f,0.0f);
    GLfloat x=0.3f;
    GLfloat y=0.28f;
    GLfloat radius =0.01f;
    int triangleAmount = 100;
    GLfloat twicePi = 1.0f * PI;
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );
    }
    glEnd();

    x=0.3f;
    y=0.28f;
    radius =0.01f;

    triangleAmount = 100;
    twicePi = -1.0f * PI;

    glColor3f(0.702f, 0.447f, 0.240f);

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for(i = 0; i <= triangleAmount; i++)
    {
        glVertex2f( x + (radius * cos(i *  twicePi / triangleAmount)),
                    y + (radius * sin(i * twicePi / triangleAmount)) );

    }

    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.296f, 0.278f);
    glEnd();

    glPointSize(1);
    glBegin(GL_POINTS);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.306f, 0.278f);
    glEnd();

    glPointSize(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f,0.0f, 0.0f);
    glVertex2f(0.298f, 0.275f);
    glVertex2f(0.304f, 0.275f);
    glEnd();

    ///.........................Body.....................
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 0.0);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glLineWidth(0.1);
    glBegin(GL_LINES);
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(0.3f, 0.273f);
    glVertex2f(0.3f, 0.25f);

    glEnd();

    ///....................Hand Right....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.254f);
    glVertex2f(0.318f, 0.25f);
    glVertex2f(0.31f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.31f, 0.273f);
    glVertex2f(0.32f, 0.252f);
    glVertex2f(0.318f, 0.254f);
    glVertex2f(0.31f, 0.265f);
    glEnd();



    ///....................Hand left....................
    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.254f);
    glVertex2f(0.282f, 0.25f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.235f, 0.540f, 0.510f);
    glVertex2f(0.29f, 0.273f);
    glVertex2f(0.28f, 0.252f);
    glVertex2f(0.282f, 0.254f);
    glVertex2f(0.29f, 0.265f);
    glEnd();

    ///................................Legs............................

    glBegin(GL_QUADS);
    glColor3f(0.702f, 0.447f, 0.240f);
    glVertex2f(0.29f, 0.25f);
    glVertex2f(0.29f, 0.23f);
    glVertex2f(0.296f, 0.23f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.23f);
    glVertex2f(0.31f, 0.23f);
    glVertex2f(0.31f, 0.25f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.0, 0.0, 0.0);
    glVertex2f(0.29f, 0.248f);
    glVertex2f(0.29f, 0.232f);
    glVertex2f(0.296f, 0.232f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.3f, 0.25f);
    glVertex2f(0.304f, 0.232f);
    glVertex2f(0.31f, 0.232f);
    glVertex2f(0.31f, 0.25f);
    glEnd();
    glPopMatrix();
}

void drawWheel(float x, float y, float radius) {
    glColor3f(0.0, 0.0, 0.0); // Black
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 30; i++) {
        float angle = 2.0 * M_PI * i / 30;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}


void drawPlane(float x, float y, float angle) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    // Plane body
    glBegin(GL_POLYGON); // Use GL_POLYGON for a more defined shape
    glColor3f(2.7, 2.7,0.9); // White
    glVertex2f(-0.34, 0.0);
    glVertex2f(-0.3, 0.07);
    glVertex2f(-0.18, 0.12);
    glVertex2f(0.18, 0.12);
    glVertex2f(0.3, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.25, -0.07);
    glVertex2f(-0.25, -0.07);
    glEnd();

    // Red stripe
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0); // Red
    glVertex2f(-0.31, -0.025);
    glVertex2f(0.28, -0.025);
    glVertex2f(0.28, -0.035);
    glVertex2f(-0.3, -0.035);
    glEnd();

    // Left Wing
    glBegin(GL_TRIANGLES);
    glColor3f(2.7, 2.7,0.9); // White
    glVertex2f(-0.03, -0.03);
    glVertex2f(0.07, -0.03);
    glVertex2f(0.2, -0.3);
    glEnd();

    // Right Wing
    /*glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0); // White
    glVertex2f(0.33, 0.13);
    glVertex2f(0.32, 0.1);
    glVertex2f(0.4, 0.07);
    glEnd();
*/
    // Windows
    glColor3f(0.54, 0.77, 0.8); // White

    glBegin(GL_QUADS);
    glVertex2f(-0.2, 0.07);
    glVertex2f(-0.15, 0.07);
    glVertex2f(-0.15, 0.03);
    glVertex2f(-0.2, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.1, 0.07);
    glVertex2f(-0.05, 0.07);
    glVertex2f(-0.05, 0.03);
    glVertex2f(-0.1, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.0, 0.07);
    glVertex2f(0.05, 0.07);
    glVertex2f(0.05, 0.03);
    glVertex2f(0.0, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.1, 0.07);
    glVertex2f(0.15, 0.07);
    glVertex2f(0.15, 0.03);
    glVertex2f(0.1, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.2, 0.07);
    glVertex2f(0.25, 0.07);
    glVertex2f(0.25, 0.03);
    glVertex2f(0.2, 0.03);
    glEnd();

    // Tail
    glBegin(GL_TRIANGLES);
    glColor3f(2.7, 2.7,0.9); // Yellowish White
    glVertex2f(0.25, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.35, 0.2);
    glEnd();

    // Wheels
    drawWheel(-0.2, -0.08, 0.018);
    drawWheel(0.0, -0.08, 0.018);
    //drawWheel(0.08, -0.18, 0.018);

    glPopMatrix();
}

float planeAngle = 0.0f;

bool initialMovementDone = false;

   /*void update(int value) {
    cloudOffset += 0.002f; // Move clouds slowly to the right
    if (cloudOffset > 1.2f) { // Reset when off-screen
        cloudOffset = -1.2f;
    }
   if (!initialMovementDone) {
        if (planeX > 0.1f) {
            planeX -= speed; // Move left initially
        } else {
            initialMovementDone = true;
            isAscending = true; // Start ascending
        }
    }

    if (isAscending && !hasFlown) {
        float angle = 45.0f;
        float angle_rad = angle * M_PI / 180.0f;

        planeX -= speed * cos(angle_rad); // Move left
        planeY += speed * sin(angle_rad); // Move up
        planeAngle += 1.0f; // Optional rotation

        if (planeY > 1.5f) { // Stop movement after first flight
            hasFlown = true;
            isAscending = false;
        }
    }

    glutPostRedisplay();

    glutTimerFunc(16, update, 0);
}*/


// Display function
void renderWindow1() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the scene
    drawSky();
    drawGround();
    drawTerminal();
    drawBuildingWithWindows(0.85, 0.0, 0.3, 0.6);
    drawControlTower();
    drawRunway();

    // Draw traffic light
    drawTrafficLight();

    // Draw trees
    //drawTree(0.85, -0.18);
    drawTree(-0.75, -0.55);
    drawTree(0.6, -0.02);
    drawTree(-0.7, -0.02);
 //Tree(0.0, -0.5, 0.4);  // Tree 1
    PalmTree(0.825, -0.18, 0.5);
    //Tree(0.825, -0.18, 0.5);
//    tree1();
    treebig();
    tree_leaf1();

// Draw the sun
    drawSun(0.8, 0.8, 0.1);

    // Draw clouds
    drawCloud(-0.5, 0.8);
    drawCloud(0.0, 0.9);
    drawCloud(0.5, 0.8);

    // Add a streetlight beside the horizontal runway
    drawRoadLight(-0.85, -0.18);

    // Draw birds in the sky
      for (int i = 0; i < 8; i++) {
        birdX[i] += birdMoveX[i]; // Move right
        if (birdX[i] > 1.5f) { // Reset when off-screen
            birdX[i] = -1.2f;
        }

        draw_bird(birdX[i], birdY[i], birdColorType[i]);
    }

    human1();
    human2();
    human3();
    human4();
    human5();
    human6();
    human7();
    human8();
    human9();

    // Draw the plane
    //drawPlane(0.55, -0.29, 0.0);
    //drawPlane(planeX, planeY, 45.0f);
    //drawPlane(planeX, planeY, angle);
    glPushMatrix();
    glTranslatef(planeX + 0.4, planeY - 0.02, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);  // Rotate around the new pivot
    //glTranslatef(-0.15, 0.05, 0.0); // Move back to original position

    drawPlane(0.0, 0.0, 0.0);
    glPopMatrix();

    glutSwapBuffers();
    glFlush();
}


/*int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Airport Scene");
    glClearColor(1.0, 1.0, 1.0, 1.0); // Background color
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glutDisplayFunc(display);
    glutTimerFunc(10, update, 0);
    glutMainLoop();
    return 0;
}*/




//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//Disha's Code

//int i;
float cloudOffsetz = -1.0f;
float planeXz = 1.0f;
float planeYz = -0.28f;
float speedz = 0.006;
float anglez = 0.0;
bool isAscendingz = false;
bool hasFlownz = false;
float planeAnglez = 0.0f;
bool initialMovementDonez = false;

// Function to draw a filled rectangle
void drawRectanglez(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

// Function to draw a filled triangle
void drawTrianglez(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

// Function to draw a filled circle
void drawCirclez(float cx, float cy, float r, float rColor, float gColor, float bColor) {
    glColor3f(rColor, gColor, bColor);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i++) {
        float anglez= i * 3.14159 / 180;
        float x = cx + r * cos(anglez);
        float y = cy + r * sin(anglez);
        glVertex2f(x, y);
    }
    glEnd();
}

void drawWheelz(float x, float y, float radius) {
    glColor3f(0.0, 0.0, 0.0); // Black
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 30; i++) {
        float anglez = 2.0 * M_PI * i / 30;
        glVertex2f(x + radius * cos(anglez), y + radius * sin(anglez));
    }
    glEnd();
}


void drawPlanez(float x, float y, float anglez) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);

    // Plane body
    glBegin(GL_POLYGON); // Use GL_POLYGON for a more defined shape
    glColor3f(2.7, 2.7,0.9); // White
    glVertex2f(-0.34, 0.0);
    glVertex2f(-0.3, 0.07);
    glVertex2f(-0.18, 0.12);
    glVertex2f(0.18, 0.12);
    glVertex2f(0.3, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.25, -0.07);
    glVertex2f(-0.25, -0.07);
    glEnd();

    // Red stripe
    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0); // Red
    glVertex2f(-0.31, -0.025);
    glVertex2f(0.28, -0.025);
    glVertex2f(0.28, -0.035);
    glVertex2f(-0.3, -0.035);
    glEnd();

    // Left Wing
    glBegin(GL_TRIANGLES);
    glColor3f(2.7, 2.7,0.9); // White
    glVertex2f(-0.03, -0.03);
    glVertex2f(0.07, -0.03);
    glVertex2f(0.2, -0.3);
    glEnd();

    // Right Wing
    /*glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0); // White
    glVertex2f(0.33, 0.13);
    glVertex2f(0.32, 0.1);
    glVertex2f(0.4, 0.07);
    glEnd();
*/
    // Windows
    glColor3f(0.54, 0.77, 0.8); // White

    glBegin(GL_QUADS);
    glVertex2f(-0.2, 0.07);
    glVertex2f(-0.15, 0.07);
    glVertex2f(-0.15, 0.03);
    glVertex2f(-0.2, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.1, 0.07);
    glVertex2f(-0.05, 0.07);
    glVertex2f(-0.05, 0.03);
    glVertex2f(-0.1, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.0, 0.07);
    glVertex2f(0.05, 0.07);
    glVertex2f(0.05, 0.03);
    glVertex2f(0.0, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.1, 0.07);
    glVertex2f(0.15, 0.07);
    glVertex2f(0.15, 0.03);
    glVertex2f(0.1, 0.03);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(0.2, 0.07);
    glVertex2f(0.25, 0.07);
    glVertex2f(0.25, 0.03);
    glVertex2f(0.2, 0.03);
    glEnd();

    // Tail
    glBegin(GL_TRIANGLES);
    glColor3f(2.7, 2.7,0.9); // Yellowish White
    glVertex2f(0.25, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.35, 0.2);
    glEnd();

    // Wheels
    drawWheelz(-0.2, -0.08, 0.018);
    drawWheelz(0.0, -0.08, 0.018);
    //drawWheel(0.08, -0.18, 0.018);

    glPopMatrix();
}

// Function to display text on the screen
void renderBitmapStringz(float x, float y, float z, void *font, char *string) {
    char *c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Drawing Of Birds
void drawCirclez(float x, float y, float radius);

// Birds
void circle_birdz(float x1, float y1, float r) {
    int i;

    GLfloat x = x1;
    GLfloat y = y1;
    GLfloat radius = r;
    int triangleAmount = 100; // # of lines used to draw circle

    // Get the aspect ratio of the window
    int width2 = glutGet(GLUT_WINDOW_WIDTH);
    int height2 = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat aspect2 = (GLfloat)height2 / (GLfloat)width2; // Aspect ratio (height/width)

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;

    glColor3ub(80, 80, 80);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {
        glVertex2f(x + (radius * cos(i * twicePi / triangleAmount) * aspect2),
                     y + (radius * sin(i * twicePi / triangleAmount)));
    }
    glEnd();
}


void circlez(float x1, float y1, float r, char c) {
    int i;
    // Get the aspect ratio of the window
    int width2 = glutGet(GLUT_WINDOW_WIDTH);
    int height2 = glutGet(GLUT_WINDOW_HEIGHT);
    GLfloat aspect2 = (GLfloat)height2 / (GLfloat)width2; // Aspect ratio (height/width)

    GLfloat x = x1;
    GLfloat y = y1;
    GLfloat radius = r;
    int triangleAmount = 100; // # of lines used to draw circle

    // GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * PI;
    // glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // center of circle
    for (i = 0; i <= triangleAmount; i++) {

        if (c == 's') {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                         y + (radius * sin(i * twicePi / triangleAmount)));

        } else {
            glVertex2f(x + (radius * cos(i * twicePi / triangleAmount)),
                         y + (radius * sin(i * twicePi / triangleAmount)));

        }
    }
    glEnd();
}





// Function to draw a bird (V-shape)
void draw_birdz(float xAdjust, float yAdjust, int birdType) {

    int wingColorR = 255;
    int wingColorG = 140;
    int wingColorB = 0;
    int bodyColorR = 255;
    int bodyColorG = 255;
    int bodyColorB = 0;
    int beakColorR = 255;
    int beakColorG = 0;
    int beakColorB = 0;


    if (birdType == 1) {  // Green birds
        wingColorR = 0;
        wingColorG = 215;
        wingColorB = 0;
        bodyColorR = 0;
        bodyColorG = 100; // Darker green for body
        bodyColorB = 0;
    } else if (birdType == 2) { // Black birds
        wingColorR = 0;
        wingColorG = 0;
        wingColorB = 0;
        bodyColorR = 50; // Slightly lighter black for body
        bodyColorG = 50;
        bodyColorB = 50;
        beakColorR = 255;
        beakColorG = 255;
        beakColorB = 0;
    }

    glColor3ub(wingColorR, wingColorG, wingColorB);
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f + xAdjust, 0.508f + yAdjust);
    glVertex2f(-0.5f + xAdjust, 0.492f + yAdjust);
    glVertex2f(-0.48f + xAdjust, 0.50f + yAdjust);
    glEnd();

    glColor3ub(bodyColorR, bodyColorG, bodyColorB);
    glBegin(GL_QUADS);
    glVertex2f(-0.51f + xAdjust, 0.49f + yAdjust);
    glVertex2f(-0.53f + xAdjust, 0.55f + yAdjust);
    glVertex2f(-0.53f + xAdjust, 0.52f + yAdjust);
    glVertex2f(-0.52f + xAdjust, 0.47f + yAdjust);
    glEnd();

    glColor3ub(0, 0, 0);
    circle_bird(-0.5 + xAdjust, 0.5 + yAdjust, 0.01);

    glColor3ub(beakColorR, beakColorG, beakColorB); // Beak color
    circle(-0.52 + xAdjust, 0.49 + yAdjust, 0.015, 'l');

    glColor3ub(bodyColorR, bodyColorG, bodyColorB);
    glBegin(GL_POLYGON);
    glVertex2f(-0.52f + xAdjust, 0.49f + yAdjust);
    glVertex2f(-0.55f + xAdjust, 0.47f + yAdjust);
    glVertex2f(-0.535f + xAdjust, 0.465f + yAdjust);
    glVertex2f(-0.55f + xAdjust, 0.43f + yAdjust);
    glVertex2f(-0.515f + xAdjust, 0.485f + yAdjust);
    glEnd();
}

float birdXz[8] = {-0.06f, -0.06f, -0.12f, -0.12f, -0.07f, -0.07f, -0.12f, -0.12f};
float birdYz[8] = {0.05f, 0.09f, 0.03f, 0.07f, 0.25f, 0.29f, 0.24f, 0.21f};
int birdColorTypez[8] = {0, 1, 2, 0, 1, 2, 0, 1};
float birdMoveXz[8] = {0.008f, 0.008f, 0.005f, 0.008f, 0.005f, 0.005f, 0.008f, 0.005f}; // Movement speed for each bird
bool birdMovingRightz[8] = {true, true, true, true, true, true, true, true};       // Direction of movement

bool isUserControllingz = false;

void updatez(int value) {
    cloudOffsetz += 0.002f;
    if (cloudOffsetz > 1.2f) {
        cloudOffsetz = -1.2f;
    }

    if (!initialMovementDone) {
        if (planeX > 0.1f) {
            planeX -= speed;
        } else {
            initialMovementDone = true;
        }
    }

    if (isUserControllingz) {
        float anglez = 45.0f;
        float angle_rad = anglez * M_PI / 180.0f;

        planeXz -= speed * cos(angle_rad);
        planeYz += speed * sin(angle_rad);
        planeAnglez += 1.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, updatez, 0);
}
/*void keyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            isUserControllingz = true;
            break;
    }
}*/
// Function to draw the airport scene
void renderWindow3() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw Sky Background
    drawRectanglez(-1.0, 1.0, 1.0, -0.2, 0.757, 0.929, 0.969 );



    // Draw Hills
     drawTrianglez(0.8, -0.2, 1.0, -0.2, 0.9, 0.11, 0.125, 0.439, 0.0472);
     drawRectanglez(0.75, -0.2, 1.0, 0.02, 0.757, 0.929, 0.969);
     drawTrianglez(0.8, -0.2, 1.0, -0.2, 0.9, 0.11, 0.035, 0.329, 0.067);
     drawTrianglez(0.85, -0.2, 1.0, -0.2, 1.0, 0.15, 0.125, 0.439, 0.0472);
     drawTrianglez(0.36, -0.2, 0.56, -0.2, 0.47, -0.07, 0.035, 0.329, 0.067);
     drawTrianglez(0.6, -0.2, 0.8, -0.2, 0.7, 0.01, 0.125, 0.439, 0.0472);
     drawTrianglez(0.45, -0.2, 0.7, -0.2, 0.55, -0.04, 0.125, 0.439, 0.0472);
     drawTrianglez(-0.1, -0.2, 0.1, -0.2, 0.0, -0.04, 0.035, 0.329, 0.067);
     drawTrianglez(-0.05, -0.2, 0.12, -0.2, 0.04, -0.07, 0.125, 0.439, 0.0472);
     drawTrianglez(0.2, -0.2, 0.46, -0.2, 0.34, -0.07, 0.125, 0.439, 0.0472);
     drawTrianglez(0.1, -0.2, 0.26, -0.2, 0.17, -0.07, 0.035, 0.329, 0.067);
     drawTrianglez(0.02, -0.2, 0.24, -0.2, 0.1, -0.05, 0.125, 0.439, 0.0472);
     drawTrianglez(0.36, -0.2, 0.56, -0.2, 0.47, -0.07, 0.035, 0.329, 0.067);


     //light green
     drawTrianglez(0.36, -0.2, 0.40, -0.2, 0.47, -0.07, 0.373, 0.702, 0.251);
     drawTrianglez(0.21, -0.2, 0.24, -0.2, 0.1, -0.05, 0.373, 0.702, 0.251);
     drawTrianglez(0.2, -0.2, 0.26, -0.2, 0.34, -0.07, 0.373, 0.702, 0.25);
     drawTrianglez(0.10, -0.2, 0.12, -0.2, 0.04, -0.07, 0.373, 0.702, 0.25);
     drawTrianglez(0.099, -0.2, 0.1, -0.2, 0.0, -0.04,  0.373, 0.702, 0.25);
     drawTrianglez(0.67, -0.2, 0.7, -0.2, 0.55, -0.04,0.373, 0.702, 0.25);
     drawTrianglez(0.78, -0.2, 0.8, -0.2, 0.7, 0.01, 0.373, 0.702, 0.25);
     drawTrianglez(0.85, -0.2, 0.87, -0.2, 1.0, 0.15, 0.373, 0.702, 0.25);
     drawTrianglez(0.8, -0.2, 0.82, -0.2, 0.9, 0.11,  0.373, 0.702, 0.25);


     //draw plane
     //draw triangle
    // drawTriangle(0.75, 0.4, 0.82, 0.3, 0.7, 0.2, 1.0, 1.0, 1.0);







    // Draw  Left Clouds
    drawCirclez(-0.825, 0.8, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(-0.7, 0.8, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(-0.577, 0.8, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(-0.775, 0.88, 0.07, 1.0, 1.0, 1.0);
    drawCirclez(-0.625, 0.9, 0.07, 1.0, 1.0, 1.0);
    drawCirclez(-0.650, 0.9, 0.07, 1.0, 1.0, 1.0);




    //Draw Right Clouds
    drawCirclez(0.03, 0.75, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(0.15, 0.75, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(0.25, 0.75, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(0.2, 0.82, 0.075, 1.0, 1.0, 1.0);
    drawCirclez(0.13, 0.8, 0.064, 1.0, 1.0, 1.0);
    drawCirclez(0.08, 0.82, 0.075, 1.0, 1.0, 1.0);



    // Draw Sun (Moved to the right)
    drawCirclez(0.7, 0.83, 0.15, 1.0, 0.85, 0.1) ;// Yellow Sun in the sky

    // Draw Airport Terminal shifted to the left

   drawRectanglez(-0.9, -0.3, -0.85, 0.3, 0.906, 0.918, 0.922);
   drawRectanglez(-0.93, 0.2, -0.82, 0.3, 0.906, 0.918, 0.922);
   drawTrianglez(-1.0, 0.2, -0.93, 0.3, -0.9, 0.2,  0.757, 0.929, 0.969);
   drawTrianglez(-0.85, 0.2, -0.82, 0.3, -0.7, 0.2, 0.757, 0.929, 0.969);
   drawRectanglez(-0.93, 0.28, -0.82, 0.3,  0.2, 0.6, 0.8);
   drawRectanglez(-0.93, 0.3, -0.82, 0.31,   0.906, 0.918, 0.922);
   drawRectanglez(-0.93, 0.31, -0.82, 0.32,  0.2, 0.6, 0.8);
   drawRectanglez(-0.93, 0.32, -0.82, 0.33,   0.906, 0.918, 0.922);

   drawCirclez(-0.875, 0.345, 0.015, 0.2, 0.6, 0.8);
   drawCirclez(-0.875, 0.17, 0.005, 0.2, 0.6, 0.8);
   drawCirclez(-0.875, 0.13, 0.005, 0.2, 0.6, 0.8);
   drawCirclez(-0.875, 0.09, 0.005, 0.2, 0.6, 0.8);
   drawCirclez(-0.875, 0.05, 0.005, 0.2, 0.6, 0.8);
   drawCirclez(-0.875, 0.01, 0.005, 0.2, 0.6, 0.8);
   drawCirclez(-0.875, 0.344, 0.005, 1.0, 0.0, 0.0);


   drawRectanglez(-1.0, -0.01, -0.15, -0.25, 0.3, 0.4, 0.4);
   drawRectanglez(-1.0, -0.04, -0.15, -0.25,0.761, 0.914, 0.969); // Main building




    // Glass Windows
   drawRectanglez(-1.0, -0.3, -0.3, -0.05, 0.2, 0.6, 0.8);
   drawRectanglez(-1.0, -0.15, -0.3, -0.14, 1.0, 1.0, 1.0);
   drawRectanglez(-1.0, -0.05, -0.3, -0.04, 1.0, 1.0, 1.0);
   drawRectanglez(-0.95, -0.3, -0.955, -0.05, 1.0, 1.0, 1.0);
   drawRectanglez(-0.85, -0.3, -0.855, -0.05, 1.0, 1.0, 1.0);
   drawRectanglez(-0.75, -0.3, -0.755, -0.05, 1.0, 1.0, 1.0);
   drawRectanglez(-0.65, -0.3, -0.655, -0.05, 1.0, 1.0, 1.0);
   drawRectanglez(-0.55, -0.3, -0.555, -0.05, 1.0, 1.0, 1.0);
   drawRectanglez(-0.45, -0.3, -0.455, -0.05, 1.0, 1.0, 1.0);
   drawRectanglez(-0.35, -0.3, -0.355, -0.05, 1.0, 1.0, 1.0);// window


    drawCirclez(-0.2, -0.25, 0.25, 0.3, 0.4, 0.4);
    drawCirclez(-0.2, -0.25, 0.24, 1.0, 1.0, 1.0);
    drawCirclez(-0.2, -0.25, 0.23, 0.761, 0.914, 0.969);




    // Add Glass Doors
    drawRectanglez(-0.3, -0.07, -0.1, -0.3,  0.53, 0.81, 0.98); // Center door
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2f(-0.20, -0.07);
    glVertex2f(-0.20, -0.25);

    glVertex2f(-0.30, -0.07);
    glVertex2f(-0.30, -0.25);

    glVertex2f(-0.10, -0.07);
    glVertex2f(-0.10, -0.25);

    glVertex2f(-0.30, -0.07);
    glVertex2f(-0.10, -0.07);// Door separator

    glVertex2f(-0.185, -0.17);
    glVertex2f(-0.185, -0.14);

    glVertex2f(-0.215, -0.17);
    glVertex2f(-0.215, -0.14);


    glEnd();



    // Draw birds in the sky
      for (int i = 0; i < 5; i++) {
        birdXz[i] += birdMoveXz[i]; // Move right
        if (birdXz[i] > 1.5f) { // Reset when off-screen
            birdXz[i] = -1.2f;
        }

        draw_birdz(birdXz[i], birdYz[i], birdColorTypez[i]);
    }

    //bird_left1();
    //bird_left2();
    //bird_left3();
    //bird_left4();



    // Display the airport name
    glColor3f(0.0, 0.0, 0.0); // Black text
    renderBitmapStringz(-0.8f, 0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, "PARO INTERNATIONAL AIRPORT");






    // Draw Runway
    drawRectanglez(-1.0, -0.65, 1.0, -0.27, 0.2, 0.2, 0.2);
    drawRectanglez(-1.0, -1.0, -0.25, -0.27, 0.2, 0.2, 0.2);
    drawRectanglez(-1.0, -0.25, 1.0, -0.27, 1.0, 1.0, 1.0);
    drawRectanglez(-0.25, -0.65, 1.0, -0.63, 1.0, 1.0, 1.0);
    drawTrianglez(-0.68, -1.0, -0.25, -1.0, -0.25, -0.63, 1.0, 1.0, 1.0);
    drawTrianglez(-0.65, -1.0, -0.15, -1.0, -0.25, -0.65, 0.65, 0.45, 0.25 );

 glPushMatrix();
    glTranslatef(planeX + 0.4, planeY - 0.02, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);  // Rotate around the new pivot
    //glTranslatef(-0.15, 0.05, 0.0); // Move back to original position

    drawPlanez(0.0, 0.0, 0.0);
    glPopMatrix();
//drawPlane(0.6, 0.29, 0.6);


    // Draw Runway Centerline
    glColor3f(1, 1, 1);
    glLineWidth(4);
    glBegin(GL_LINES);
    for (float i = -1.0; i < 1.0; i += 0.3) {
        glVertex2f(i, -0.4);
        glVertex2f(i + 0.15, -0.4);
    }
    glEnd();
    glLineWidth(1);




 glutSwapBuffers();
    glFlush();



}











//-----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
float planesX = 2.0f;
float planesX2 = -2.0f;
float cloudX1 = -0.7, cloudX2 = -0.2, cloudX3 = 0.4;
float cloudsOffset = 0.0;
//float skycolor=0.7f; //for day mode
//float skyColor = 0.1;
float skyRed = 0.1f, skyGreen = 0.0f, skyBlue = 0.2f;
float sunMoonRed=0.9f,sunMoonGreen=0.9f, sunMoonBlue=0.9f;
float moonRed = 0.9f;
float moonGreen = 0.9f;
float moonBlue = 0.9f;
float starRed = 1.0f;
float starGreen = 1.0f;
float starBlue = 1.0f;

bool thunder = false;
bool rainActive=false;
bool windowsLit = false;


struct Raindrop {
    float x, y;
};
std::vector<Raindrop> rain;
const int numRaindrops = 200;

void initRain() {
    for (int i = 0; i < numRaindrops; ++i) {
        rain.push_back({ static_cast<float>(rand() % 200) / 100.0f - 1.0f, static_cast<float>(rand() % 200) / 100.0f });
    }
}

void drawRain() {
    if(!rainActive)return;
    glColor3f(0.5, 0.5, 1.0);
    glBegin(GL_LINES);
    for (const auto& drop : rain) {
        glVertex2f(drop.x, drop.y);
        glVertex2f(drop.x, drop.y - 0.05);
    }
    glEnd();
}

void updateRain() {
    if(!rainActive) return;
    for (auto& drop : rain) {
        drop.y -= 0.02;
        if (drop.y < -1.0) {
            drop.y = 1.0;
            drop.x = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
        }
    }
}

void drawGrounds() {
    glColor3f(0.0, 0.5, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, -0.5);
    glVertex2f(-1.0, -0.5);
    glEnd();
}



/*void drawMoon() {
    glColor3f(0.9, 0.9, 0.9);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.1415926 / 180;
        glVertex2f(0.7 + 0.1 * cos(theta), 0.7 + 0.1 * sin(theta));
    }
    glEnd();
}*/

void drawMoon() {
    glColor3f(moonRed, moonGreen, moonBlue);  // Use the moon color
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.1415926 / 180;
        glVertex2f(0.7 + 0.1 * cos(theta), 0.7 + 0.1 * sin(theta));
    }
    glEnd();
}

/*void drawStars() {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < 50; i++) {
        float x = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
        float y = static_cast<float>(rand() % 200) / 100.0f;
        glVertex2f(x, y);
    }
    glEnd();
}
void drawStars() {
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(3.0);
    glBegin(GL_POINTS);
    for (int i = 0; i < 50; i++) {
        float x = static_cast<float>(rand() % 200) / 100.0f - 1.0f;
        float y = static_cast<float>(rand() % 200) / 100.0f;
        glVertex2f(x, y);
    }
    glEnd();
}*/
void drawStars() {
    glColor3f(starRed, starGreen, starBlue);
    glPointSize(6.0);
    glBegin(GL_POINTS);
    glVertex2f(-0.8, 0.8);
    glVertex2f(-0.5, 0.9);
    glVertex2f(0.0, 0.85);
    glVertex2f(0.5, 0.9);
    glVertex2f(0.8, 0.8);
    glEnd();
}


void drawHouse(float x, float y) {
    glColor3f(0.7, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 0.2, y);
    glVertex2f(x + 0.2, y + 0.2);
    glVertex2f(x, y + 0.2);
    glEnd();

    glColor3f(0.5, 0.3, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(x - 0.02, y + 0.2);
    glVertex2f(x + 0.22, y + 0.2);
    glVertex2f(x + 0.1, y + 0.3);
    glEnd();

    // Windows
    glColor3f(windowsLit ? 1.0 : 0.8, windowsLit ? 1.0 : 0.8, windowsLit ? 0.0 : 1.0);
    glBegin(GL_QUADS);
    glVertex2f(x + 0.05, y + 0.05);
    glVertex2f(x + 0.08, y + 0.05);
    glVertex2f(x + 0.08, y + 0.08);
    glVertex2f(x + 0.05, y + 0.08);
    glEnd();

    glBegin(GL_QUADS);
    glVertex2f(x + 0.12, y + 0.05);
    glVertex2f(x + 0.15, y + 0.05);
    glVertex2f(x + 0.15, y + 0.08);
    glVertex2f(x + 0.12, y + 0.08);
    glEnd();
}

void drawBuilding(float x, float y, float width, float height) {
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    for (float i = y + 0.05; i < y + height - 0.05; i += 0.1) {
        for (float j = x + 0.05; j < x + width - 0.05; j += 0.1) {
            glBegin(GL_QUADS);
            glVertex2f(j, i);
            glVertex2f(j + 0.05, i);
            glVertex2f(j + 0.05, i + 0.05);
            glVertex2f(j, i + 0.05);
            glEnd();
        }
    }
}

void drawPond() {
    glColor3f(0.0, 0.5, 1.0);
    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.3 * cos(angle);
        float dy = 0.15 * sin(angle);
        glVertex2f(-0.5 + dx, -0.7 + dy);
    }
    glEnd();
}



/*void drawSun() {
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float theta = i * 3.1415926 / 180;
        glVertex2f(0.7 + 0.1 * cos(theta), 0.7 + 0.1 * sin(theta));
    }
    glEnd();
}*/

void drawClouds(float x, float y) {
    x += cloudOffset;

    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.1 * cos(angle);
        float dy = 0.05 * sin(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.04 * sin(angle);
        glVertex2f(x + 0.12 + dx, y + dy);
    }
    glEnd();

    glBegin(GL_POLYGON);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        float dx = 0.08 * cos(angle);
        float dy = 0.04 * sin(angle);
        glVertex2f(x - 0.12 + dx, y + dy);
    }
    glEnd();
}

void drawPlane(float x, float y, float angle, bool reversed) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    if (reversed) {
        glScalef(-1.0, 1.0, 1.0);
    }

    glBegin(GL_POLYGON);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.34, 0.0);
    glVertex2f(-0.3, 0.07);
    glVertex2f(-0.18, 0.12);
    glVertex2f(0.18, 0.12);
    glVertex2f(0.3, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.25, -0.07);
    glVertex2f(-0.25, -0.07);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0, 0.0, 0.0);
    glVertex2f(-0.31, -0.025);
    glVertex2f(0.28, -0.025);
    glVertex2f(0.28, -0.035);
    glVertex2f(-0.3, -0.035);
    glEnd();

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-0.03, -0.03);
    glVertex2f(0.07, -0.03);
    glVertex2f(0.2, -0.3);
    glEnd();

    glColor3f(0.5, 0.8, 1.0);
    for (float i = -0.2; i <= 0.2; i += 0.1) {
        glBegin(GL_QUADS);
        glVertex2f(i, 0.07);
        glVertex2f(i + 0.05, 0.07);
        glVertex2f(i + 0.05, 0.03);
        glVertex2f(i, 0.03);
        glEnd();
    }

    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(0.25, 0.07);
    glVertex2f(0.3, 0.0);
    glVertex2f(0.35, 0.2);
    glEnd();

    glPopMatrix();
}

void drawThunder() {
    if (thunder) {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        glVertex2f(-0.2, 1.0);
        glVertex2f(-0.1, 0.5);
        glVertex2f(-0.1, 0.5);
        glVertex2f(0.0, 0.7);
        glVertex2f(0.0, 0.7);
        glVertex2f(0.1, 0.3);
        glEnd();
    }
}

void drawFootballPlayground() {
    glColor3f(0.1, 0.6, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(0.0, -0.85);
    glVertex2f(0.5, -0.85);
    glVertex2f(0.5, -0.55);
    glVertex2f(0.0, -0.55);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(0.0, -0.85);
    glVertex2f(0.5, -0.85);
    glVertex2f(0.5, -0.55);
    glVertex2f(0.0, -0.55);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(0.25, -0.85);
    glVertex2f(0.25, -0.55);
    glEnd();

    glBegin(GL_LINE_LOOP);
    for (float angle = 0; angle < 2 * M_PI; angle += 0.1) {
        glVertex2f(0.25 + 0.05 * cos(angle), -0.7 + 0.05 * sin(angle));
    }
    glEnd();
}
bool isUserControlling = false;

/*void keyboard(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            isUserControlling = true;
            break;
    }
}*/

bool isSoundOn = false;
bool isSecondSoundOn = false;


void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        thunder = true;
        rainActive= true ;
        glutPostRedisplay();
        glutTimerFunc(100, [](int) { thunder = false; glutPostRedisplay(); }, 0);
    }
    else if (key == 's' || key == 'S') {
        thunder = false;
        rainActive= false ;
        glutPostRedisplay();
        glutTimerFunc(100, [](int) { thunder = false; glutPostRedisplay(); }, 0);

        //pdateRain();
    }
    /*else if (key == 'c' || key == 'C') {
        //Change sky color to a different value (cycle through colors for example)
        skyRed = (skyRed == 0.1f) ? 0.5f : 0.1f;
        skyGreen = (skyGreen == 0.0f) ? 0.7f : 0.0f;
        skyBlue = (skyBlue == 0.2f) ? 1.0f : 0.2f;

        sunMoonRed = (sunMoonRed == 0.9f) ? 1.0f : 0.9f;
        sunMoonGreen = (sunMoonGreen == 0.9f) ? 1.0f : 0.9f;
        sunMoonBlue = (sunMoonBlue == 0.9f) ? 0.0f : 0.9f;


        glutPostRedisplay();
    }*/

    else if (key == 'd' || key == 'D') {
        // Change the moon color when 'B' is pressed
        moonRed = 1.0f;   // Set to a different color (for example, light blue)
        moonGreen = 1.0f;
        moonBlue = 0.0f;
        windowsLit = false;
        skyRed =0.5f;
        skyGreen =0.7f;
        skyBlue =1.0f;

        starRed=0.5f;
        starGreen=0.7f;
        starBlue=1.0f;

        glutPostRedisplay();  // Update display
    }
    else if (key == 'n' || key == 'N') {
        // Change the moon color to green when 'G' is pressed
        moonRed = 0.9f;
        moonGreen = 0.9f;
        moonBlue = 0.9f;
        windowsLit = true;
        skyRed =0.1f;
        skyGreen =0.0f;
        skyBlue =0.2f;

        starRed=1.0f;
        starGreen=1.0f;
        starBlue=1.0f;

        glutPostRedisplay();  // Update display
    }
    else if (key == '1') {
        currentDisplay = 1;  // Set to display function 1
        glutPostRedisplay(); // Request a redraw
        glutPostRedisplay();
    }
    else if (key == '2') {
        currentDisplay = 2;  // Set to display function 1
        glutPostRedisplay(); // Request a redraw
        glutPostRedisplay();
    }
    else if (key == '3') {
        currentDisplay = 3;  // Set to display function 1
        glutPostRedisplay(); // Request a redraw
        glutPostRedisplay();
    }
    else if (key=='m' || key=='M'){
        if (!isSoundOn) {
                PlaySound(TEXT("sound2.wav"), NULL, SND_ASYNC | SND_LOOP);
                isSoundOn = true;
            } else {  // Press 'M' again to stop sound
                PlaySound(NULL, NULL, SND_ASYNC);
                isSoundOn = false;
            }
    }
    else if(key=='b'||key=='B'){
        if (!isSecondSoundOn) {
                PlaySound(TEXT("sound.wav"), NULL, SND_ASYNC);
                isSecondSoundOn = true;
            }
            else {
                PlaySound(NULL, NULL, SND_ASYNC); // Stop sound
                isSecondSoundOn = false;
            }
    }
else if(key=='t'||key=='T'){
        if (!isSecondSoundOn) {
                PlaySound(TEXT("rain.wav"), NULL, SND_ASYNC);
                isSecondSoundOn = true;
            }
            else {
                PlaySound(NULL, NULL, SND_ASYNC); // Stop sound
                isSecondSoundOn = false;
            }
    }
        //updateRain();

}


void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME); // Get time in milliseconds

        if (currentTime - lastClickTime < DOUBLE_CLICK_TIME) {

            thunder = true;
        rainActive= true ;
        glutPostRedisplay();
        glutTimerFunc(100, [](int) { thunder = false; glutPostRedisplay(); }, 0);

        }

        lastClickTime = currentTime; // Update last click time
    }
}

void keyboard2(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            isUserControlling = true;
            break;
    }
}

void keyUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            isUserControlling = false;
            break;

        case GLUT_KEY_LEFT:
            isUserControlling = false;
            break;

        default:
            glutIdleFunc(NULL);
            break;
    }
}

void renderWindow2() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(-0.9, 0.9);
    const char* message = "Press 'D' to start rain/thunder, 'S' to stop rain";
    for (const char* c = message; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glColor3f(skyRed, skyGreen, skyBlue);
    glBegin(GL_QUADS);
    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);
    glEnd();
    drawRain();

    //drawSun();
    drawStars();
    drawThunder();
    drawMoon();
    drawGrounds();
    drawFootballPlayground();
    drawPond();
    drawHouse(-0.8, -0.5);
    drawHouse(0.5, -0.5);
    drawBuilding(-0.3, -0.5, 0.25, 0.6);
    drawBuilding(0.25, -0.5, 0.2, 0.5);

    drawClouds(cloudX1, 0.5);
    drawClouds(cloudX2, 0.6);
    drawClouds(cloudX3, 0.7);
    drawPlane(planesX, 0.3, 0, false);
    drawPlane(planesX2, 0.15, 0, true);

    glutSwapBuffers();
}

void update(int value) {
    planesX -= 0.01;
    if (planesX < -2.0) {
        planesX = 2.0;
    }

    planesX2 += 0.01;
    if (planesX2 > 2.0) {
        planesX2 = -2.0;
    }

    cloudX1 += 0.005;
    cloudX2 += 0.005;
    cloudX3 += 0.005;

    if (cloudX1 > 1.2) cloudX1 = -1.2;
    if (cloudX2 > 1.2) cloudX2 = -1.2;
    if (cloudX3 > 1.2) cloudX3 = -1.2;

    //if (skyColor > 0.2) {
        //skyColor -= 0.001;
    //}

        cloudOffset += 0.002f; // Move clouds slowly to the right
    if (cloudOffset > 1.2f) { // Reset when off-screen
        cloudOffset = -1.2f;
    }
   if (!initialMovementDone) {
        if (planeX > 0.1f) {
            planeX -= speed; // Move left initially
        } else {
            initialMovementDone = true;
            isAscending = true; // Start ascending
        }
    }

    /*if (isAscending && !hasFlown) {
        float angle = 45.0f;
        float angle_rad = angle * M_PI / 180.0f;

        planeX -= speed * cos(angle_rad); // Move left
        planeY += speed * sin(angle_rad); // Move up
        planeAngle += 1.0f; // Optional rotation

        if (planeY > 1.5f) { // Stop movement after first flight
            hasFlown = true;
            isAscending = false;
        }


    }*/
     if (isUserControlling) {
        float angle = 45.0f;
        float angle_rad = angle * M_PI / 180.0f;

        planeX -= speed * cos(angle_rad);
        planeY += speed * sin(angle_rad);
        planeAngle += 1.0f;
    }

    updateRain();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void init() {
    glClearColor(skyRed, skyGreen, skyBlue, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    initRain();
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    //glutCreateWindow("Cloudy Sky with Airplanes");
    //init();
    //glutDisplayFunc(display);
    glutCreateWindow("Window 1");
    init();
    // Set the rendering function for the first window
    glutDisplayFunc([]() {
        if (currentDisplay == 1) {
            renderWindow1();  // Call display function 1
        } else if(currentDisplay==2)
        {
            renderWindow2();  // Call display function 2
        }
        else{
            renderWindow3();
        }
    });

    // Register keyboard callback function
    glutKeyboardFunc(keyboard);
    glutSpecialUpFunc(keyboard2);
    //glutSpecialUpFunc(keyUp);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
