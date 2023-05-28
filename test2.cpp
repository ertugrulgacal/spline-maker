#include <GLUT/glut.h>
#include <vector>

// Window dimensions
const int windowWidth = 1280;
const int windowHeight = 700;

// Square position and size
struct Square
{
    int x;
    int y;
};

std::vector<Square> squares;  // Store the clicked squares

const int squareSize = 5;

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

    // Draw B-spline curves using the stored points
    glColor3f(0.0, 1.0, 0.0);  // Set color to green for the B-spline curves
    if (squares.size() >= 4)
    {
        for (size_t i = 0; i < squares.size() - 3; i++)
        {
            const Square& p0 = squares[i];
            const Square& p1 = squares[i + 1];
            const Square& p2 = squares[i + 2];
            const Square& p3 = squares[i + 3];
            drawBSplineSegment(p0, p1, p2, p3);
        }
    }

    glFlush();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Add the clicked coordinates to the vector
        squares.push_back(Square());
        squares.back().x = x;
        squares.back().y = windowHeight - y;

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
