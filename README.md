# Wavefront Tiny Renderer

Inspired by: [ssloy/tinyrenderer](https://github.com/ssloy/tinyrenderer)

But instead of using a TGA image to store the rendering, I'm using FreeGLUT instead.

# Some examples

![](https://raw.githubusercontent.com/Elemeants/wavefront_renderer/main/imgs/HeadTexture.png)
![](https://raw.githubusercontent.com/Elemeants/wavefront_renderer/main/imgs/DiabloTexture.png)
![](https://raw.githubusercontent.com/Elemeants/wavefront_renderer/main/imgs/DiabloTesting.png)

# Author

- [Elemeants](https://github.com/Elemeants)

## Todo's

- Avoid using `GL_TRIANGLE` since we cannot handle pixel details, so refactor how do we plot each
  polygon.
- Abstract into more modular architecture for shading (flat/gouraud/phong/etc) and rendering (wireframe/textured/no-color).
