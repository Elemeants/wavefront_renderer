
#include <GL/glut.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif


#define ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS false

typedef struct {
  GLfloat x;
  GLfloat y;
} g2DPoint;

typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
} g3DPoint;

g3DPoint centroid3D(g3DPoint p1, g3DPoint p2, g3DPoint p3) {
  return { (p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3, (p1.z + p2.z + p3.z) / 3};
}

g3DPoint sum(g3DPoint p1, g3DPoint p2) {
  return {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
}

g3DPoint subs(g3DPoint p1, g3DPoint p2) {
  return {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z};
}

g3DPoint cross(g3DPoint p1, g3DPoint p2) {
  return {(p1.y * p2.z) - (p1.z * p2.y), (p1.z * p2.x) - (p1.x * p2.z), (p1.x * p2.y) - (p1.y * p2.x)};
}

g3DPoint normalize(g3DPoint p1) {
  GLfloat norm = sqrt((p1.x * p1.x) + (p1.y * p1.y) + (p1.z * p1.z));
  return {p1.x / norm, p1.y / norm, p1.z / norm};
}

g3DPoint multiply(g3DPoint p1, g3DPoint p2) {
  return {p1.x * p2.x, p1.y * p2.y, p1.z * p2.z};
}

g3DPoint multiply(g3DPoint p1, float scalar) {
  return {p1.x * scalar, p1.y * scalar, p1.z * scalar};
}

float multiplyf(g3DPoint p1, g3DPoint p2) {
  g3DPoint p = multiply(p1, p2);
  return p.x + p.y + p.z;
}

typedef struct {
  g3DPoint p1;
  g3DPoint p2;
} g3DLine;

typedef struct {
  std::vector<g3DPoint> vertices;

  //
  // Faces represents on this program a "triangle" where each index
  // corresponds to a vertice in #vertices array.
  //
  // So a Face could be (1, 2, 3) so we are referencing to verices (i0, i1, i2)
  //
  std::vector<std::vector<int>> faces;
} g3DModel;

typedef enum {
  RENDER_WIREFRAME,
  RENDER_RANDOM_COLOR_FACES,
  RENDER_GRAY_SCALE,
  RENDER_END,
} eRenderMethod;

static g3DModel model;
static eRenderMethod renderMethod = RENDER_GRAY_SCALE;
static bool rotate = false;

static void glut_post_redisplay_p(void) {
  static double t0 = -1.;
  double t, dt;
  t = glutGet(GLUT_ELAPSED_TIME) / 1000.;
  if (t0 < 0.) t0 = t;
  dt = t - t0;

  if (dt < 1. / 30.) return;

  t0 = t;

  glutPostRedisplay();
}

static void handleKeyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'w':
      glTranslatef(0, 0, 0.1);
      break;
    case 's':
      glTranslatef(0, 0, -0.1);
      break;
    case 'a':
      glTranslatef(-0.1, 0, 0);
      break;
    case 'd':
      glTranslatef(0.1, 0, 0);
      break;
    case 'q':
      glTranslatef(0, 0.1, 0);
      break;
    case 'e':
      glTranslatef(0, -0.1, 0);
      break;
    case 'z':
      rotate = !rotate;
      break;
    case 'c':
      renderMethod = (eRenderMethod)((int)renderMethod + 1);
      if (renderMethod >= RENDER_END) {
        renderMethod = RENDER_WIREFRAME;
      }
      break;
  }
}

void mainRenderLoop();
void renderWireframe();
void renderTriangesRandomColor();
void renderWithGreyScale();
void loadObjWavefront();
g3DPoint parseVectorLine(const std::string& line);
std::vector<int> parseFace(const std::string& line);

int main(int argc, char** argv) {
  loadObjWavefront();
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(700, 700);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

  if (glutCreateWindow("Testing") == GL_FALSE) {
    exit(1);
  }

  glMatrixMode(GL_MODELVIEW);

  glutKeyboardFunc(handleKeyboard);
  glutIdleFunc(glut_post_redisplay_p);
  glutDisplayFunc(mainRenderLoop);
  glutMainLoop();
  return 0;
}

void mainRenderLoop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (rotate) {
    glRotatef(1.0, 0.0, 1.0, 0.0);
  }
  switch (renderMethod) {
    case RENDER_WIREFRAME:
      renderWireframe();
      break;
    case RENDER_RANDOM_COLOR_FACES:
      renderTriangesRandomColor();
      break;
    case RENDER_GRAY_SCALE:
      renderWithGreyScale();
      break;
    default:
      break;
  }
  glFlush();
  glutSwapBuffers();
}

void renderWireframe() {
  for (size_t i = 0; i < model.faces.size(); i++) {
    g3DPoint p0 = model.vertices[model.faces[i][0]];
    g3DPoint p1 = model.vertices[model.faces[i][1]];
    g3DPoint p2 = model.vertices[model.faces[i][2]];
    glBegin(GL_LINES);
    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(p2.x, p2.y, p2.z);
    glVertex3f(p0.x, p0.y, p0.z);
    glEnd();
  }
}

void renderTriangesRandomColor() {
  for (size_t i = 0; i < model.faces.size(); i++) {
    g3DPoint p0 = model.vertices[model.faces[i][0]];
    g3DPoint p1 = model.vertices[model.faces[i][1]];
    g3DPoint p2 = model.vertices[model.faces[i][2]];
    glBegin(GL_TRIANGLES);
    glColor3b(rand() % 255, rand() % 255, rand() % 255);
    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
    glEnd();
  }
}

void renderWithGreyScale() {
  g3DPoint light = { 0, 0, 1 };

  for (size_t i = 0; i < model.faces.size(); i++) {
    g3DPoint p0 = model.vertices[model.faces[i][0]];
    g3DPoint p1 = model.vertices[model.faces[i][1]];
    g3DPoint p2 = model.vertices[model.faces[i][2]];

    g3DPoint a = normalize(cross(
      subs(p1, p0),
      subs(p2, p0)
    ));
    float lightIntensity = multiplyf(a, light);

    if (lightIntensity <= 0) { continue; }

    glBegin(GL_TRIANGLES);
    glColor3f(lightIntensity, lightIntensity, lightIntensity);
    glVertex3f(p0.x, p0.y, p0.z);
    glVertex3f(p1.x, p1.y, p1.z);
    glVertex3f(p2.x, p2.y, p2.z);
    glEnd();
  }
}

void loadObjWavefront() {
  std::ifstream in;
  std::cout << "Loading wavefront obj" << std::endl;
  in.open("./wavefront_objs/head/head.obj", std::ifstream::in);
  if (in.fail()) {
    std::cout << "Error loading wavefront object" << std::endl;
    exit(1);
  }

  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    bool is3DPoint = line.compare(0, 2, "v ") == 0;
    bool isFace = line.compare(0, 2, "f ") == 0;

    if (is3DPoint) {
      g3DPoint point = parseVectorLine(line);
      model.vertices.push_back(point);
#if ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS
      std::cout << "v " << point.x << " " << point.y << " " << point.z
                << std::endl;
#endif
    } else if (isFace) {
      std::vector<int> face = parseFace(line);
      model.faces.push_back(face);
#if ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS
      std::cout << "f " << face[0] << " " << face[1] << " " << face[2]
                << std::endl;
#endif
    }
  }

#if ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS
  std::cout << "total vertices: " << model.vertices.size()
            << ", total faces: " << model.faces.size() << std::endl;
#endif
}

g3DPoint parseVectorLine(const std::string& line) {
  g3DPoint point;
  std::stringstream ss;
  ss.str(line.c_str());
  std::string trash;

  ss >> trash >> point.x >> point.y >> point.z;
  return point;
}

std::vector<int> parseFace(const std::string& line) {
  const char* s = line.c_str();
  const size_t len = line.size() - 1;
  std::vector<int> vertices;
  vertices.reserve(3);

  std::stringstream ss;
  ss.str(s);

  std::string f;
  // Remove initial trash characters.
  ss >> f;
  for (size_t i = 0; i < 3; i++) {
    ss >> f;
    const size_t c = f.find('/');
    // Parse the index (starts with 0 so remove 1)
    const std::string p = f.substr(0, c);
    const int vertex_index = std::atoi(p.c_str()) - 1;
    vertices.push_back(vertex_index);
  }
  return vertices;
}
