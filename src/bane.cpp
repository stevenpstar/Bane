#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <bane/bane.hpp>
#include <bane/components/camera.hpp>
#include <bane/utility/shader.hpp>
#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <stbimage/stb_image.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

SDL_Window *window{nullptr};
SDL_Surface *surface{nullptr};
SDL_Renderer *renderer{nullptr};
SDL_GLContext context{nullptr};
SDL_Cursor *cursor{nullptr};

// frame buffer testing
unsigned int screenVAO, screenVBO, fbo, rbo, fboTex;
unsigned int shadowVAO, shadowVBO;
unsigned int shadowDepthMapFBO;
unsigned int depthMap;

int screenWidth, screenHeight;

// text rendering testing
SDL_Surface textSurface;
bool CursorOn = false;

void Print(std::string message) { std::cout << message << std::endl; }

void framebuffer_size_callback(__attribute__((unused)) SDL_Window *window, int width, int height) {
  // glViewport(0, 0, width, height);
}

void SetCursorCallback(SDL_Window *window, void (*f)(SDL_Window *window, double xpos, double ypos)) {
  // glfwSetCursorPosCallback(window, f);
}

void SetMouseInputCallback(SDL_Window *window, void (*f)(SDL_Window *window, int button, int action, int mods)) {
  // glfwSetMouseButtonCallback(window, f);
}

void SetResizeCallback(SDL_Window *window, void (*f)(SDL_Window *, int, int)) {
  // glfwSetFramebufferSizeCallback(window, f);
}

void ResizeViewport(int width, int height) {
  std::cout << "We are resizing viewport!\n";
  screenWidth = width;
  screenHeight = height;
  SDL_SetWindowSize(window, screenWidth, screenHeight);
  glViewport(0, 0, screenWidth, screenHeight);
  UpdateFrameBufferSize(screenWidth, screenHeight);
  std::cout << "Resizing!\n";
}

void CaptureMouse(SDL_Window *window) {
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

SDL_Window *CreateWindow() {
  std::cout << "Creating window!\n";
  screenWidth = 1920;
  screenHeight = 1080;
  bool success = true;
  // trying to init SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
    std::cout << "SDL not init!" << std::endl;
    SDL_Log("SDL could not be initialised! SDL Error: %s\n", SDL_GetError());
    success = false;
  }

  if (TTF_Init() == false) {
    std::cout << "TTF not init!" << std::endl;
    SDL_Log("SDL TTF could not be initialised! SDL Error: %s\n", SDL_GetError());
    success = false;
  } else {
    SDL_Log("SDL TTF initialised!\n");
  }

  SDL_GL_LoadLibrary(nullptr);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  if (success) {
    std::cout << "Loading window!\n";
    if (window = SDL_CreateWindow("Bane Engine", screenWidth, screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
        window == nullptr) {

      std::cout << "Window not created!" << std::endl;
      SDL_Log("Window could not be created, SDL Error: %s\n", SDL_GetError());
      success = false;
    }
  }

  if (success) {
    std::cout << "Creating context" << std::endl;
    context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
      std::cout << "OpenGL context not created!\n" << std::endl;
      SDL_Log("Context null: %s", SDL_GetError());
      DestroyBane();
    } else {
      std::cout << "GL Context created here." << std::endl;
    }
  }

  SDL_GL_MakeCurrent(window, context);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cout << " Failed to load glad" << std::endl;
    throw std::runtime_error("Failed to initialise Glad!");
  }
  SDL_SetWindowResizable(window, true);
  SDL_SetRenderLogicalPresentation(renderer, screenWidth, screenHeight, SDL_LOGICAL_PRESENTATION_STRETCH);
  glViewport(0, 0, screenWidth, screenHeight);
  glEnable(GL_DEPTH_TEST);

  // generating texture once here, instead of every time we want to
  // render text? Will it work? We will see.
  // also initialise the renderer here????
  renderer = SDL_CreateRenderer(window, "opengl");
  if (renderer == nullptr) {
    SDL_Log("Renderer did not create properly: %s", SDL_GetError());
  }

  glEnable(GL_BLEND);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_STENCIL_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  return window;
}

float GetTime() {
  // return glfwGetTime();
  return 0.f;
}

void Poll() {
  // glfwPollEvents();
}

void ClearColour() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
  glStencilMask(0x00);
}

void SwapBuffer(SDL_Window *w) {
  SDL_GL_SwapWindow(w);
  // glfwSwapBuffers(window);
}

glm::mat4 RenderShadow(SDL_Window *window, int width, int height, glm::vec3 lightPos, Shader *shader, Camera *camera) {
  // setup orth projection
  float near_plane = -10.f, far_plane = 20.f;
  glm::mat4 lightProjection = glm::ortho(-10.f, 10.f, -10.f, 10.f, near_plane, far_plane);
  glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
  glm::mat4 lightSpaceMatrix = lightProjection * lightView;

  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);
  shader->use();
  shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
  glViewport(0, 0, width, height);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFBO);
  glClear(GL_DEPTH_BUFFER_BIT);

  return lightSpaceMatrix;
  // Render scene needs to be called in application/game
}

unsigned int getShadowTexture() { return depthMap; }

void SetupShadowBuffer(SDL_Window *window, int width, int height) {
  glGenFramebuffers(1, &shadowDepthMapFBO);

  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColour[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour);

  glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SetupFrameBuffer(SDL_Window *window, int width, int height) {
  // frame buffer testing
  std::cout << "Setup frame buffer" << std::endl;
  glGenFramebuffers(1, &fbo);
  std::cout << "Do we even get here?" << std::endl;
  glGenRenderbuffers(1, &rbo);
  glGenTextures(1, &fboTex);

  float quadVertices[] = {// positions   // texCoords
                          -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,

                          -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f, 1.0f};

  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glBindVertexArray(screenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

  std::cout << "binding to frame buffer\n";
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // bind fboTex
  glBindTexture(GL_TEXTURE_2D, fboTex);

  // this should take in width/height of window in this function probably
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);

  // render buffer object, maybe need to do this for the texture to be correctly loaded (depth etc)
  std::cout << "binding texture /buffer object\n";
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
  auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Fbuffer not complete: " << fboStatus << "\n";
  }
}

void UpdateFrameBufferSize(int width, int height) {
  glBindTexture(GL_TEXTURE_2D, fboTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void SwitchBuffer() {
  glViewport(0, 0, screenWidth, screenHeight);
  glCullFace(GL_BACK);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glEnable(GL_DEPTH_TEST);
}

void RenderToScreenTexture(Shader *screenShader) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.f, 1.f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(screenVAO);
  glDisable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboTex);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void DestroyBane() {
  SDL_DestroyRenderer(renderer);
  renderer = nullptr;
  SDL_DestroySurface(surface);
  surface = nullptr;
  SDL_DestroyWindow(window);
  window = nullptr;
  glDeleteFramebuffers(1, &fbo);
  glDeleteTextures(1, &fboTex);
  TTF_Quit();
  SDL_Quit();
}

bool ShouldClose(SDL_Window *window) {
  return false;
  // return glfwWindowShouldClose(window);
}

// Test function
void RenderColouredTriangle(Shader *shader, std::array<float, 18> &triangle) {
  if (triangle.size() == 0) {
    return;
  }
  glBufferData(GL_ARRAY_BUFFER, triangle.size() * sizeof(float), triangle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  shader->use();
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

// Testing here for now, move to own class/file eventually
unsigned int CreateTextureFromString(std::string text, TTF_Font *font, int &width, int &height) {
  unsigned int textTexture;
  SDL_Color colour = {255, 255, 255, 255};
  // 0 for null terminated text, I think c_str() should be fine here?
  SDL_Surface *textSurface = TTF_RenderText_Blended(font, text.c_str(), 0, colour);
  if (textSurface == nullptr) {
    std::cout << "Text surface did not render / create properly!\n";

    SDL_Log("SDL TEXT TTF: SDL Error: %s\n", SDL_GetError());
    // dunno if this is good if it did not create properly
    SDL_DestroySurface(textSurface);
  } else {

    // create a texture from the surface, to then be converted into an opengl texture
    if (!renderer) {
      SDL_Log("No Renderer found");
      return 0;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (tex == nullptr) {
      SDL_Log("Texture did not load correctly: %s", SDL_GetError());
      return 0;
    }

    width = tex->w;
    height = tex->h;

    // attempting to convert
    glDeleteTextures(1, &textTexture);
    glGenTextures(1, &textTexture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->w, tex->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textSurface->pixels);

    SDL_DestroyTexture(tex);
    SDL_DestroySurface(textSurface);
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  return textTexture;
}

void GetScreenDimensions(SDL_Window *window, int &width, int &height) { SDL_GetWindowSize(window, &width, &height); }

void SetCursorOnOff(bool on) {
  CursorOn = on;
  SDL_SetWindowRelativeMouseMode(window, !CursorOn);
  if (!CursorOn) {
    SDL_SetWindowMouseGrab(window, true);
    SDL_SetWindowRelativeMouseMode(window, true);
  } else {
    SDL_SetWindowMouseGrab(window, false);
    SDL_SetWindowRelativeMouseMode(window, false);
  }
}

SDL_GLContext *ReturnSDLContext() { return &context; }

void SetCursorImage(const char *path) {
  SDL_Surface *surface = IMG_Load(path);
  cursor = SDL_CreateColorCursor(surface, 8, 8);
  SDL_SetCursor(cursor);
}
