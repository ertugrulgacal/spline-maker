#include <GLUT/glut.h>
#include <vector>
#include <iostream>

using namespace std;

// Window dimensions
const int windowWidth = 1280;
const int windowHeight = 700;

// Square position and size
struct Square
{
    int x;
    int y;
};

struct Button
{
    int x;
    int y;
};

std::vector<Square> squares;  // Store the clicked squares

const int squareSize = 5;
const int toolbarWidth = 100;

// Toolbar button dimensions and positions
const int buttonGaps = 20;
const int buttonWidth = 80;
const int buttonHeight = 30;

struct Button buttonAdd = {10, 50};
struct Button buttonRemove = {buttonAdd.x, buttonAdd.y + buttonHeight + buttonGaps};
struct Button buttonClear = {buttonRemove.x, buttonRemove.y + buttonHeight + buttonGaps};


// Number of line segments for B-spline curves
const int numSegments = 100;

// Function to draw a B-spline segment given 4 control points
void drawBSplineSegment(const Square& p0, const Square& p1, const Square& p2, const Square& p3)
{
    const int segments = 100;  // Number of line segments to approximate the curve

    glColor3f(0.0, 1.0, 0.0);  // Set color to green for the B-spline curve

    glBegin(GL_LINE_STRIP);

    for (int i = 0; i <= segments; i++)
    {
        float t = static_cast<float>(i) / segments;

        // Calculate the blending functions
        float b0 = (1 - t) * (1 - t) * (1 - t) / 6.0f;
        float b1 = (3 * t * t * t - 6 * t * t + 4) / 6.0f;
        float b2 = (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0f;
        float b3 = t * t * t / 6.0f;

        // Calculate the x and y coordinates of the B-spline point
        float x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
        float y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;

        glVertex2f(x, y);
    }

    glEnd();
}

void drawButton(int x, int y, string label)
{
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + buttonWidth, y);
    glVertex2i(x + buttonWidth, y + buttonHeight);
    glVertex2i(x, y + buttonHeight);
    glEnd();


    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2i(x + 10, y + 10);
    for (int i = 0; i < label.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, label[i]);
    }
}

void drawToolbar()
{
    // Draw the toolbar background
    glColor3f(0.8, 0.8, 0.8);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(toolbarWidth, 0);
    glVertex2i(toolbarWidth, windowHeight);
    glVertex2i(0, windowHeight);
    glEnd();

    drawButton(buttonAdd.x, buttonAdd.y, "Add");
    drawButton(buttonRemove.x, buttonRemove.y, "Remove");
    drawButton(buttonClear.x, buttonClear.y, "Clear");
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw all the squares
    glColor3f(1.0, 0.0, 0.0);  // Set color to red
    for (const Square& square : squares)
    {
        glBegin(GL_QUADS);
        glVertex2i(square.x - squareSize/2, square.y - squareSize/2);  // Top-left vertex
        glVertex2i(square.x + squareSize/2, square.y - squareSize/2);  // Top-right vertex
        glVertex2i(square.x + squareSize/2, square.y + squareSize/2);  // Bottom-right vertex
        glVertex2i(square.x - squareSize/2, square.y + squareSize/2);  // Bottom-left vertex
        glEnd();
    }

    // Draw B-spline curves
    glColor3f(0.0, 0.0, 1.0);  // Set color to blue
    if (squares.size() >= 4)
    {
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < squares.size() - 3; i++)
        {
            for (int t = 0; t <= numSegments; t++)
            {
                float u = t / static_cast<float>(numSegments);
                float u2 = u * u;
                float u3 = u2 * u;
                float b0 = (-u3 + 3 * u2 - 3 * u + 1) / 6.0f;
                float b1 = (3 * u3 - 6 * u2 + 4) / 6.0f;
                float b2 = (-3 * u3 + 3 * u2 + 3 * u + 1) / 6.0f;
                float b3 = u3 / 6.0f;

                int x = static_cast<int>(
                    b0 * squares[i].x +
                    b1 * squares[i + 1].x +
                    b2 * squares[i + 2].x +
                    b3 * squares[i + 3].x
                );
                int y = static_cast<int>(
                    b0 * squares[i].y +
                    b1 * squares[i + 1].y +
                    b2 * squares[i + 2].y +
                    b3 * squares[i + 3].y
                );

                glVertex2i(x, y);
            }
        }
        glEnd();
    }

    // Draw the toolbar
    drawToolbar();

    glFlush();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {   
        // Add the clicked coordinates to the vector
        int true_y = windowHeight - y;
        squares.push_back(Square());
        if (x >= toolbarWidth)
        {
            squares.back().x = x;
            squares.back().y = true_y;
        }
        else if (x <= buttonAdd.x + buttonWidth && x >= buttonAdd.x)
        {
            if (true_y <= buttonAdd.y + buttonHeight && true_y >= buttonAdd.y)
            {
                cout << "add ";
            }
            else if (true_y <= buttonRemove.y + buttonHeight && true_y >= buttonRemove.y)
            {
                cout << "remove ";
            }
            else if (true_y <= buttonClear.y + buttonHeight && true_y >= buttonClear.y)
            {
                cout << "clear ";
            }
        }
        
        glutPostRedisplay();  // Trigger a redraw
    }
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Click to Draw Red Squares and B-splines");

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
