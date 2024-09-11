#include <GL/glut.h>

#include "geom.hpp"
#include "lights.hpp"
#include "models.hpp"
#include "wavefront_loader.hpp"
#include "scene.hpp"

#ifndef __RENDERER_VERSION__
#define __RENDERER_VERSION__ "unknown"
#endif

typedef enum {
  RENDER_WIREFRAME,
  RENDER_GRAY_SCALE,
  RENDER_TEXTURED,
  RENDER_END,
} eRenderMethod;

static Scene globalScene;
static eRenderMethod renderMethod = RENDER_TEXTURED;
static bool rotate = false;
static bool isLightningEnabled = false;

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
    case 'l':
      isLightningEnabled = !isLightningEnabled;
      break;
  }
}

void loadTextures();
void mainRenderLoop();
void renderWireframe();
void renderWithGreyScale();
void renderWithTexture();
void renderWithTexture();

void setupScene() {
  globalScene.models = {WavefrontObjLoader::loadObjWavefrontObj(
      "../wavefront_objs/head/model.obj", "../wavefront_objs/head/texture.tga")};
  globalScene.lights = {Light3D(255, 255, 255, {0, 1, -1}),
                        Light3D(255, 255, 255, {0, 1, 1}),
                        Light3D(255, 255, 255, {0, 0, 0})};
}

int main(int argc, char** argv) {
  std::cout << "Version: " << __RENDERER_VERSION__ << std::endl;
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(700, 700);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

  if (glutCreateWindow("Testing") == GL_FALSE) {
    exit(1);
  }

  setupScene();
  loadTextures();
  
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

void loadTextures() {
  for (Object3D &object : globalScene.models) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, object.texture.width, object.texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, *object.texture.data);

    object.texture.textureRef = texture;
    object.texture.data.reset();
  }
}

void mainRenderLoop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (rotate) {
    glRotatef(1.0, 0.0, 1.0, 0.0);
  }
  
  if (isLightningEnabled) {
    globalScene.applyLightingToModels();
  }

  switch (renderMethod) {
    case RENDER_WIREFRAME:
      glDisable(GL_TEXTURE_2D);
      renderWireframe();
      break;
    case RENDER_GRAY_SCALE:
      renderWithGreyScale();
      break;
    case RENDER_TEXTURED:
      glEnable(GL_TEXTURE_2D);
      renderWithTexture();
      break;
    default:
      break;
  }

  glFlush();
  glutSwapBuffers();
}

void renderWireframe() {
  glColor3f(1, 1, 1);
  for (Object3D& model : globalScene.models) {
    for (ObjectFace3D& face : model.faces) {
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
}

void renderWithGreyScale() {
  for (Object3D& model : globalScene.models) {
    for (ObjectFace3D& face : model.faces) {
      const dVector3D& p0 = face.p0;
      const dVector3D& p1 = face.p1;
      const dVector3D& p2 = face.p2;
      const ColorRGB& color = face.color;
      double lightning = (color.red + color.green + color.blue) / 3;
      if (!isLightningEnabled) {
        lightning = 1;
      }

      glBegin(GL_TRIANGLES);
      glColor3f(lightning, lightning, lightning);
      glVertex3f(p0.x(), p0.y(), p0.z());
      glVertex3f(p1.x(), p1.y(), p1.z());
      glVertex3f(p2.x(), p2.y(), p2.z());
      glEnd();
    }
  }
}

void renderWithTexture() {
  for (Object3D& model : globalScene.models) {
    for (ObjectFace3D& face : model.faces) {
      const dVector3D& p0 = face.p0;
      const dVector3D& p1 = face.p1;
      const dVector3D& p2 = face.p2;
      ColorRGB& color = face.color;
      if (!isLightningEnabled) {
        color = ColorRGB(1, 1, 1);
      }

      glBindTexture(GL_TEXTURE_2D, model.texture.textureRef);
      glBegin(GL_TRIANGLES);
      glColor3f(color.red, color.green, color.blue);
      glTexCoord2d(face.t0.x(), face.t0.y()); glVertex3d(p0.x(), p0.y(), p0.z());
      glTexCoord2d(face.t1.x(), face.t1.y()); glVertex3d(p1.x(), p1.y(), p1.z());
      glTexCoord2d(face.t2.x(), face.t2.y()); glVertex3d(p2.x(), p2.y(), p2.z());
      glEnd();
    }
  }
}
