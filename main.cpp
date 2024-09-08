
#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct {
  GLfloat x;
  GLfloat y;
  GLfloat z;
} g3DPoint;

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

static g3DModel model;


void mainRenderLoop();
void loadObjWavefront();
g3DPoint parseVectorLine(const std::string& line);
std::vector<int> parseFace(const std::string& line);

int main(int argc, char **argv) {
  loadObjWavefront();
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(300, 300);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);

  if (glutCreateWindow("Testing") == GL_FALSE) {
    exit(1);
  }

  glutDisplayFunc(mainRenderLoop);
  glutMainLoop();
  return 0;
}


void mainRenderLoop() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

  glutSwapBuffers();
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
  while(!in.eof()) {
    std::getline(in, line);
    bool is3DPoint = line.compare(0, 2, "v ") == 0;
    bool isFace = line.compare(0, 2, "f ") == 0;

    if (is3DPoint) {
      g3DPoint point = parseVectorLine(line);
      model.vertices.push_back(point);
      std::cout << "v " << point.x << " " << point.y << " " << point.z << std::endl;
    } else if (isFace) {
      std::vector<int> face = parseFace(line);
      model.faces.push_back(face);
      std::cout << "f " << face[0] << " " << face[1] << " " << face[2] << std::endl;
    }
  }

  std::cout << "total vertices: " << model.vertices.size() << ", total faces: " << model.faces.size() << std::endl;
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
