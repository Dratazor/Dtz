#include <cmath>
#include <valarray>
#include <ctime>
#include <vector>
#include <gl/freeglut.h>

static constexpr int seedNr = 15;
static constexpr int screenH = 600;
static constexpr int screenW = 600;

struct point
{
    float x = 0.;
    float y = 0.;

    point()
    {
        x = static_cast<float>(rand() % screenH);
        y = static_cast<float>(rand() % screenW);
    }

    point(float x, float y) : x{x}, y{y}
    {
    }

    float pyDist(point other)
    {
        return sqrtf(powf(other.x - this->x, 2) + powf(other.y - this->y, 2));
    }

    float taxiDist(point other)
    {
        return fabs(other.x - this->x) + fabs(other.y - this->y);
    }
};

struct lineSegment
{
    point origin;
    point target;
    
    float lenght(int pyORcab = 1)
    {
        switch (pyORcab)
        {
        case 1:
            return origin.pyDist(target);
        case 2:
            return origin.taxiDist(target);
        default:
            return origin.pyDist(target);
        }
    }

    point midPoint()
    {
        return {(target.x + origin.x) / 2, (target.y + origin.y) / 2};
    }
};

struct line
{
    float slope;
    float intercept;

    line(point one, point two)
    {
        slope = (two.y - one.y) / (two.x - one.x);
        intercept = one.y - slope * one.x;
    }

    line(lineSegment segment)
    {
        slope = (segment.target.y - segment.origin.y) / (segment.target.x - segment.origin.x);
        intercept = segment.origin.y - slope * segment.origin.x;
    }

    line(float slope, float intercep) : slope(slope), intercept(intercep)
    {
    }

    line perpendicularAtPoint(point where)
    {
        return {(-1.0f / slope), (where.x * (slope * slope + 1))/slope + intercept};
    }
};

point lineIntersection(line one, line two)
{
    return {
        ((two.intercept - one.intercept) / (one.slope - two.slope)),
        one.slope * ((two.intercept - one.intercept) / (one.slope - two.slope)) + one.intercept
    };
}

struct triangle
{
    point one;
    point two;
    point three;

    point findCircleCenter()
    {
        lineSegment faceOne = {one, two};
        lineSegment faceTwo = {two, three};
        line lineOne = {faceOne};
        line lineTwo = {faceTwo};
        line temp = lineOne.perpendicularAtPoint(faceOne.midPoint());
        lineOne = temp;
        temp = lineTwo.perpendicularAtPoint(faceTwo.midPoint());
        
        return lineIntersection(lineOne, temp);
    }

    float circleRadius(point center)
    {
        return one.pyDist(center);
    }
};

point seeds[seedNr];

std::vector<triangle> delaunay;

bool checkTriangle (int i1, int i2, int i3)
{
    point const v1 = seeds[i1];
    point const v2 = seeds[i2];
    point const v3 = seeds[i3];
    bool ok = true;
    
    for(int l = 0; l < seedNr; l++)
        if(l != i1 && l != i2 && l != i3)
        {
            point const pt = seeds[l];
            triangle trying = {v1, v2, v3};
            point center = trying.findCircleCenter();
            float const radius = trying.circleRadius(center);
            if(center.pyDist(pt) < radius)
                ok = false;
        }
    
    if(ok)
        return true;
    return false;
}

void tryTriangles()
{
    for(int i = 0; i < seedNr - 2; i++)
        for(int j = i + 1; j < seedNr - 1; j++)
            for(int k = j + 1; k < seedNr; k++)
                        if(checkTriangle(i, j, k))
                            delaunay.push_back({seeds[i], seeds[j], seeds[k]});
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glPointSize(5.0);
    glShadeModel(GL_FLAT);
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    delaunay.clear();
    
    for(int i = 0; i < seedNr; i++)
    {
        seeds[i].x = static_cast<float>(rand()%screenW);
        seeds[i].y = static_cast<float>(rand()%screenH);
    }
    tryTriangles();
    
    
    /*
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex2f(check.one.x, check.one.y); 
    glVertex2f(check.two.x, check.two.y);
    glVertex2f(check.one.x, check.one.y); 
    glVertex2f(check.three.x, check.three.y);
    glVertex2f(check.two.x, check.two.y);
    glVertex2f(check.three.x, check.three.y);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex2f(checkCenter.x, checkCenter.y);
    glVertex2f(check.one.x, check.one.y);
    glVertex2f(checkCenter.x, checkCenter.y);
    glVertex2f(check.two.x, check.two.y);
    glVertex2f(checkCenter.x, checkCenter.y);
    glVertex2f(check.three.x, check.three.y);
    glEnd();

    glBegin(GL_POINTS);
    glColor3f(1.0, 0.0, 1.0);
    glVertex2f(checkCenter.x, checkCenter.y);
    glEnd();
    */

    float const r = static_cast<float>(rand()%255) / 255;
    float const g = static_cast<float>(rand()%255) / 255;
    float const b = static_cast<float>(rand()%255) / 255;

    for(int i = 0; i < static_cast<int>(delaunay.size()); i++)
    {
        float const v1x = delaunay[i].one.x, v1y = delaunay[i].one.y, v2x = delaunay[i].two.x,
        v2y = delaunay[i].two.y, v3x = delaunay[i].three.x, v3y = delaunay[i].three.y;
        
        glBegin(GL_LINES);
        glColor3f(r, g, b);
        glVertex2f(v1x, v1y);
        glVertex2f(v2x, v2y);
        glVertex2f(v2x, v2y);
        glVertex2f(v3x, v3y);
        glVertex2f(v3x, v3y);
        glVertex2f(v1x, v1y);
        glEnd();
    }

    for(int i = 0; i < seedNr; i++)
    {
        glBegin(GL_POINTS);
        glColor3f(r, g, b);
        glVertex2f(seeds[i].x, seeds[i].y);
        glEnd();
    }

    glBegin(GL_LINE_STRIP);
    glColor3f(1., 1., 1.);
    glVertex2f(0., 0.);
    glVertex2f(screenW, 0.);
    glVertex2f(screenW, screenH);
    glVertex2f(0., screenH);
    glVertex2f(0., 0.);
    glEnd();
    
    glFlush();
}

void reshape(int w = screenW, int h = screenH)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, (GLdouble)w, -1.0, (GLdouble)h);
}

int main(int argc, char** argv)
{
    srand(static_cast<unsigned int>(time(nullptr)));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(screenW + 1, screenH + 1);
    glutInitWindowPosition(10, 10);
    glutCreateWindow("Delaunay triangulation");
    init();
    glutDisplayFunc(draw);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
