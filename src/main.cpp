#include <GL/glut.h>

#include "geom.hpp"
#include "lights.hpp"
#include "models.hpp"

#ifndef __RENDERER_VERSION__
#define __RENDERER_VERSION__ "unknown"
#endif

typedef enum {
  RENDER_WIREFRAME,
  RENDER_GRAY_SCALE,
  RENDER_COLORED_LIGHT,
  RENDER_END,
} eRenderMethod;

static Object3D model;
static eRenderMethod renderMethod = RENDER_COLORED_LIGHT;
static bool rotate = false;

static void glut_post_redisplay_p(void) {
  static double t0 = -1.;
  double t, dt;
  t = glutGet(GLUT_ELAPSED_TIME) / 1000.;
  if (t0 < 0.) t0 = t;
  dt = t - t0;

  if (dt < 1. / 60.) return;

  t0 = t;

  glutPostRedisplay();
}

static void handleKeyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'w':
      glTranslatef(0, 0, 0.05);
      break;
    case 's':
      glTranslatef(0, 0, -0.05);
      break;
    case 'a':
      glTranslatef(-0.05, 0, 0);
      break;
    case 'd':
      glTranslatef(0.05, 0, 0);
      break;
    case 'q':
      glTranslatef(0, 0.05, 0);
      break;
    case 'e':
      glTranslatef(0, -0.05, 0);
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
void renderWithGreyScale();
void renderWithColoredLight();
void renderWithTexture();

int main(int argc, char** argv) {
  std::cout << "Version: " << __RENDERER_VERSION__ << std::endl;
  model = WavefrontObjLoader::loadObjWavefrontObj("../wavefront_objs/head/model.obj");
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(700, 700);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

  if (glutCreateWindow("Testing") == GL_FALSE) {
    exit(1);
  }

  glShadeModel(GL_FLAT);
  glFrontFace(GL_CW);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glRotatef(180.0, 0.0, 1.0, 0.0);
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
    case RENDER_GRAY_SCALE:
      renderWithGreyScale();
      break;
    case RENDER_COLORED_LIGHT:
      renderWithColoredLight();
      break;
    default:
      break;
  }
  glFlush();
  glutSwapBuffers();
}

void renderWireframe() {
  for (const ObjectFace3D& face : model.faces) {
    glBegin(GL_LINES);
    glVertex3f(face.p0.x(), face.p0.y(), face.p0.z());
    glVertex3f(face.p1.x(), face.p1.y(), face.p1.z());
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(face.p1.x(), face.p1.y(), face.p1.z());
    glVertex3f(face.p2.x(), face.p2.y(), face.p2.z());
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(face.p2.x(), face.p2.y(), face.p2.z());
    glVertex3f(face.p0.x(), face.p0.y(), face.p0.z());
    glEnd();
  }
}

void renderWithGreyScale() {
  const dVector3D light(0, 0.5, 1);

  for (const ObjectFace3D& face : model.faces) {
    const dVector3D& p0 = face.p0;
    const dVector3D& p1 = face.p1;
    const dVector3D& p2 = face.p2;

    const dVector3D surfaceNormal = face.getSurfaceNormal();
    float lightIntensity = (surfaceNormal % light);

    if (lightIntensity < 0) {
      lightIntensity = 0;
    }

    glBegin(GL_TRIANGLES);
    glColor3f(lightIntensity, lightIntensity, lightIntensity);
    glVertex3f(p0.x(), p0.y(), p0.z());
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glEnd();
  }
}

void renderWithColoredLight() {
  std::vector<Light3D> lights = {Light3D(0, 0, 140, {0, 1, -1}),
                                 Light3D(140, 0, 0, {0, 1, 1}),
                                 Light3D(0, 140, 0, {0, 0, 0})};

  for (const ObjectFace3D& face : model.faces) {
    const dVector3D& p0 = face.p0;
    const dVector3D& p1 = face.p1;
    const dVector3D& p2 = face.p2;
    const dVector3D surfaceNormal =  face.getSurfaceNormal();

    std::vector<std::pair<float, Light3D>> lightsIntensity;
    lightsIntensity.resize(lights.size());

    std::transform(lights.begin(), lights.end(), lightsIntensity.begin(),
                   [&](const Light3D& light) -> std::pair<float, Light3D> {
                     const float i = surfaceNormal % light.position;
                     return std::make_pair(i, light);
                   });

    // Remove negative light intensity
    lightsIntensity.erase(
        std::remove_if(lightsIntensity.begin(), lightsIntensity.end(),
                       [](const std::pair<float, Light3D>& light) {
                         return light.first < 0;
                       }),
        lightsIntensity.end());

    // Normalize light intensity
    float magnitude = 0;
    float maxLightIntensity = 0;
    for (size_t i = 0; i < lightsIntensity.size(); ++i) {
      magnitude += lightsIntensity[i].first;
      if (lightsIntensity[i].first > maxLightIntensity) {
        maxLightIntensity = lightsIntensity[i].first;
      }
    }

    // After normalization get the color on `float`
    for (size_t i = 0; i < lightsIntensity.size(); ++i) {
      lightsIntensity[i].first =
          (lightsIntensity[i].first / (magnitude * maxLightIntensity * 255.0f));
      lightsIntensity[i].second.color.red *= lightsIntensity[i].first;
      lightsIntensity[i].second.color.green *= lightsIntensity[i].first;
      lightsIntensity[i].second.color.blue *= lightsIntensity[i].first;
    }

    // Mix the colors
    ColorRGB color = {0, 0, 0};
    for (size_t i = 0; i < lightsIntensity.size(); ++i) {
      color.red += lightsIntensity[i].second.color.red;
      color.green += lightsIntensity[i].second.color.green;
      color.blue += lightsIntensity[i].second.color.blue;
    }

    glBegin(GL_TRIANGLES);
    glColor3f(color.red, color.green, color.blue);
    glVertex3f(p0.x(), p0.y(), p0.z());
    glVertex3f(p1.x(), p1.y(), p1.z());
    glVertex3f(p2.x(), p2.y(), p2.z());
    glEnd();
  }
}
