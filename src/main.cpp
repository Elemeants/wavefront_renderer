#include <GL/glut.h>

#include "geom.hpp"
#include "lights.hpp"
#include "models.hpp"
#include "scene.hpp"
#include "wavefront_loader.hpp"

#ifndef __RENDERER_VERSION__
#define __RENDERER_VERSION__ "unknown"
#endif

typedef enum {
  RENDER_WIREFRAME,
  RENDER_GRAY_SCALE,
  RENDER_TEXTURED,
  RENDER_END,
} eRenderMethod;

typedef enum {
  LIGHTNING_MODE_OFF,
  LIGHTNING_MODE_FLAT,
  LIGHTNING_MODE_SMOOTH,
  LIGHTNING_END,
} eLightingMode;

static Scene globalScene;
static eRenderMethod renderMethod = RENDER_TEXTURED;
static bool rotate = false;
static eLightingMode lightningModel = LIGHTNING_MODE_SMOOTH;

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
      std::cout << "RenderMethod: " << renderMethod << std::endl;
      break;
    case 'l':
      lightningModel = (eLightingMode)((int)lightningModel + 1);
      if (lightningModel >= LIGHTNING_END) {
        lightningModel = LIGHTNING_MODE_OFF;
      }
      std::cout << "LightningModel: " << lightningModel << std::endl;
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
      "../wavefront_objs/head/model.obj",
      "../wavefront_objs/head/texture.tga")};
  globalScene.lights = {Light3D(255, 255, 255, ~dVector3D(1, 1, 1))};
}

int main(int argc, char** argv) {
  std::cout << "Version: " << __RENDERER_VERSION__ << std::endl;
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(800, 800);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

  if (glutCreateWindow("Testing") == GL_FALSE) {
    exit(1);
  }

  setupScene();
  loadTextures();

  glShadeModel(GL_SMOOTH);
  glFrontFace(GL_CCW);
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_MODELVIEW);
  glRotated(180.0, 0.0, 1.0, 0.0);

  glutKeyboardFunc(handleKeyboard);
  glutIdleFunc(glut_post_redisplay_p);
  glutDisplayFunc(mainRenderLoop);
  glutMainLoop();

  return 0;
}

void loadTextures() {
  for (Object3D& object : globalScene.models) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, object.texture.width,
                 object.texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 *object.texture.data);

    object.texture.textureRef = texture;
    object.texture.data.reset();
  }
}

void mainRenderLoop() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (rotate) {
    glRotatef(1.0, 0.0, 1.0, 0.0);
  }

  switch (lightningModel) {
    case LIGHTNING_MODE_FLAT:
      globalScene.applyLightingToModels();
      break;
    case LIGHTNING_MODE_SMOOTH:
      globalScene.applyLightningToModelsSmooth();
    default:
      break;
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

      glColor3d(1, 1, 0);
      glBegin(GL_LINES);
      dVector3D f = (face.p0 + (face.p0n * 0.1));
      glVertex3f(face.p0.x(), face.p0.y(), face.p0.z());
      glVertex3f(f.x(), f.y(), f.z());
      glEnd();
      glBegin(GL_LINES);
      f = (face.p1 + (face.p1n * 0.1));
      glVertex3f(face.p1.x(), face.p1.y(), face.p1.z());
      glVertex3f(f.x(), f.y(), f.z());
      glEnd();
      glBegin(GL_LINES);
      f = (face.p2 + (face.p2n * 0.1));
      glVertex3f(face.p2.x(), face.p2.y(), face.p2.z());
      glVertex3f(f.x(), f.y(), f.z());
      glEnd();
      glColor3d(1, 1, 1);

      glColor3d(0, 1, 1);
      glBegin(GL_LINES);
      dVector3D lightPos = globalScene.lights[0].position;
      f = (face.p0 * 0.9f) + (lightPos * 0.1f);
      glVertex3f(face.p0.x(), face.p0.y(), face.p0.z());
      glVertex3f(f.x(), f.y(), f.z());
      glEnd();
      glBegin(GL_LINES);
      f = (face.p1 * 0.9f) + (lightPos * 0.1f);
      glVertex3f(face.p1.x(), face.p1.y(), face.p1.z());
      glVertex3f(f.x(), f.y(), f.z());
      glEnd();
      glBegin(GL_LINES);
      f = (face.p2 * 0.9f) + (lightPos * 0.1f);
      glVertex3f(face.p2.x(), face.p2.y(), face.p2.z());
      glVertex3f(f.x(), f.y(), f.z());
      glEnd();
      glColor3d(1, 1, 1);
    }
  }
}

void renderWithGreyScale() {
  for (Object3D& model : globalScene.models) {
    for (ObjectFace3D& face : model.faces) {
      const dVector3D& p0 = face.p0;
      const dVector3D& p1 = face.p1;
      const dVector3D& p2 = face.p2;
      double lightning0 =
          (face.color0.red + face.color0.green + face.color0.blue) / 3;
      double lightning1 =
          (face.color1.red + face.color1.green + face.color1.blue) / 3;
      double lightning2 =
          (face.color2.red + face.color2.green + face.color2.blue) / 3;
      if (lightningModel == LIGHTNING_MODE_OFF) {
        lightning0 = 1;
        lightning1 = 1;
        lightning2 = 1;
      }

      if (lightningModel != LIGHTNING_MODE_SMOOTH) {
        glBegin(GL_TRIANGLES);
        glColor3f(lightning0, lightning0, lightning0);
        glVertex3f(p0.x(), p0.y(), p0.z());
        glVertex3f(p1.x(), p1.y(), p1.z());
        glVertex3f(p2.x(), p2.y(), p2.z());
        glEnd();
      } else {
        glBegin(GL_TRIANGLES);
        glColor3f(lightning0, lightning0, lightning0);
        glVertex3f(p0.x(), p0.y(), p0.z());
        glColor3f(lightning1, lightning1, lightning1);
        glVertex3f(p1.x(), p1.y(), p1.z());
        glColor3f(lightning2, lightning2, lightning2);
        glVertex3f(p2.x(), p2.y(), p2.z());
        glEnd();
      }
    }
  }
}

void renderWithTexture() {
  for (Object3D& model : globalScene.models) {
    for (ObjectFace3D& face : model.faces) {
      const dVector3D& p0 = face.p0;
      const dVector3D& p1 = face.p1;
      const dVector3D& p2 = face.p2;
      ColorRGB& color0 = face.color0;
      ColorRGB& color1 = face.color1;
      ColorRGB& color2 = face.color2;
      if (lightningModel == LIGHTNING_MODE_OFF) {
        color0 = ColorRGB(1, 1, 1);
        color1 = ColorRGB(1, 1, 1);
        color2 = ColorRGB(1, 1, 1);
      }

      if (lightningModel != LIGHTNING_MODE_SMOOTH) {
        glBindTexture(GL_TEXTURE_2D, model.texture.textureRef);
        glBegin(GL_TRIANGLES);
        glColor3d(color0.red, color0.green, color0.blue);
        glTexCoord2d(face.t0.x(), face.t0.y());
        glVertex3d(p0.x(), p0.y(), p0.z());
        glTexCoord2d(face.t1.x(), face.t1.y());
        glVertex3d(p1.x(), p1.y(), p1.z());
        glTexCoord2d(face.t2.x(), face.t2.y());
        glVertex3d(p2.x(), p2.y(), p2.z());
        glEnd();
      } else {
        glBindTexture(GL_TEXTURE_2D, model.texture.textureRef);
        glBegin(GL_TRIANGLES);
        glColor3d(color0.red, color0.green, color0.blue);
        glTexCoord2d(face.t0.x(), face.t0.y());
        glVertex3d(p0.x(), p0.y(), p0.z());
        glColor3d(color1.red, color1.green, color1.blue);
        glTexCoord2d(face.t1.x(), face.t1.y());
        glVertex3d(p1.x(), p1.y(), p1.z());
        glColor3d(color2.red, color2.green, color2.blue);
        glTexCoord2d(face.t2.x(), face.t2.y());
        glVertex3d(p2.x(), p2.y(), p2.z());
        glEnd();
      }
    }
  }
}
