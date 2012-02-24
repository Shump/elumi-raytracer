#include <GL/glew.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/algorithm/string.hpp>

#include "io/ExporterImpl/PNGExporter.h"
#include "io/ImporterImpl/OBJImporter.h"
#include "raytracer/Renderer.h"
#include "raytracer/scene/ILight.h"
#include "raytracer/scene/LightImpl/OmniLight.h"
#include "raytracer/utilities/glutil.h"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <GL/glfw.h>
#include <string>
#include <sstream>
#include <vector>
#include <omp.h>
#include <glm/gtc/type_ptr.hpp> //value_ptr
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367

using namespace std;
using namespace raytracer;

raytracer::Settings settings;
float* buffer;

GLuint shader_program;
raytracer::Camera camera;
vec2 mousePrev;
double prevTime;
int renderMode = 2;
raytracer::Renderer* myRenderer;

void timedCallback();
void mouse(int button, int action);
void mouseMove(int x, int y);
void mouseMove(int x, int y);
void windowSize(int width, int height);
void initGL();

unsigned int win_width, win_height;

int main(int argc, char* argv[]) {
  int running = GL_TRUE;

  // Initial values.
  settings.width = 180;
  settings.height = 180;
  settings.background_color[0] = 0;
  settings.background_color[1] = 50.0f / 255.0f;
  settings.background_color[2] = 50.0f / 255.0f;
  settings.background_color[3] = 1.0f;

  char* inputFileName, *outputFileName;

  settings.width = 400;
  settings.height = 400;

  // Declare the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "produce help message")
      ("no_opengl", "Do not use OpenGL")
      ("input-file,i", po::value<string>(), "Input file")
      ("output-file,o", po::value<string>(), "Output file")
      ("settings-file,s", po::value<string>(), "Settings file")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << "\n";
    return 1;
  }

  if (vm.count("settings-file")) {
    string line;
    ifstream settings_stream(vm["settings-file"].as<string>().c_str());
    if (settings_stream.is_open()) {
      while(settings_stream.good()) {

        getline(settings_stream, line);
        vector<string> strs;
        boost::split(strs, line, boost::is_any_of(":")); // Line may show error in eclipse but it should compile anyhow.
        string option = boost::trim_copy(strs[0]);
        string value = boost::trim_copy(strs[1]);

        cout << "Using setting: " << option << "\t\twith value: " << value << endl;
        stringstream ssvalue(value);
        if(option == "width") {
          ssvalue >> settings.width;
        } else if(option == "height") {
          ssvalue >> settings.height;
        } else if(option == "use_opengl") {
          ssvalue >> settings.use_opengl;
        } else if(option == "background_color") {
          //TODO Handle backgroudn color
        } else if(option == "use_first_bounce") {
          ssvalue >> settings.use_first_bounce;
        } else {
          cout << "Unknown option: " << option << endl;
        }

      }
    }
  }

  if (vm.count("input-file")) {
    inputFileName = const_cast<char*>(vm["input-file"].as<string>().c_str());
  } else {
    cout << "No input file! Use raytracer --help to see command options.";
    return 1;
  }

  if (vm.count("output-file")) {
      outputFileName = const_cast<char*>(vm["output-file"].as<string>().c_str());
  } else {
    cout << "Saving file to default destination (out.png)." << endl;
    outputFileName = "out.png";
  }

  if (vm.count("height")) {
    settings.height = vm["height"].as<int>();
  } else {
  }

  if (vm.count("width")) {
    settings.width = vm["width"].as<int>();
  } else {
  }

  if (vm.count("input-file")) {
  } else {
  }
  if (vm.count("no_opengl")) {
    cout << "Not using OpenGL" << endl;
    settings.use_opengl = false;
  } else {
    cout << "Using OpenGL.\n";
  }

  //Initialize GLFW
  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  if (settings.use_opengl) {
    //Open an OpenGl window
    if (!glfwOpenWindow(settings.width, settings.height, 0, 0, 0, 0, 0, 0,
        GLFW_WINDOW)) {
      glfwTerminate();
      exit(EXIT_FAILURE);
    }
    initGL();
    CHECK_GL_ERROR();

    glfwSetMouseButtonCallback(mouse);
    glfwSetMousePosCallback(mouseMove);
  }

  /* IMPORTER
   ***************** */

  raytracer::IImporter* importer = new raytracer::OBJImporter();
  importer->loadFile(inputFileName);
  std::vector<raytracer::Triangle*> triangles = importer->getTriangleList();
  std::vector<raytracer::Material*> materials = importer->getMaterialList();
  std::vector<raytracer::Texture*> textures = importer->getTextures();
  //CHECK_GL_ERROR();

  /* RENDERER
   ***************** */
  //camera.setPosition(vec3(3.512f, 2.5f, 3.5f));
  camera.setPosition(vec3(-3.0f, 0.0f, 5.0f));
  //camera.setDirection(normalize(vec3(-1.0f, -0.5f, -1.0f)));
  camera.setDirection(normalize(vec3(1.0f, -0.0f, -1.0f)));
  camera.setUpVector(vec3(0.0f, 1.0f, 0.0f));

  ILight* lights = new OmniLight(vec3(5, 3, 5));
  lights->setIntensity(800);
  lights->setColor(vec3(1,1,1));
  lights->setDistanceFalloff(LINEAR);

  ILight* light2 = new OmniLight(vec3(5, 0, 5));
  light2->setIntensity(800);
  light2->setColor(vec3(1,1,1));
  light2->setDistanceFalloff(QUADRATIC);

  myRenderer = new Renderer(&settings);
  myRenderer->loadCamera(camera);
  if (!triangles.empty()) {
    myRenderer->getScene().loadMaterials(materials); //load materials BEFORE triangles!
    myRenderer->getScene().loadTextures(textures); //load materials BEFORE triangles!
    myRenderer->loadTriangles(triangles);
  }

  //myRenderer->loadLights(lights, 1, false);
  myRenderer->loadLights(light2, 1, false);

  buffer = myRenderer->getColorBuffer();
  for (int i = 0; i < settings.width * settings.height-3; i += 3) {
    buffer[i * 4 + 0] = 1;
    buffer[i * 4 + 1] = 0;
    buffer[i * 4 + 2] = 1;
    buffer[i * 4 + 3] = 1;
    buffer[i * 4 + 4] = 0;
    buffer[i * 4 + 5] = 0;
    buffer[i * 4 + 6] = 0;
    buffer[i * 4 + 7] = 1;
    buffer[i * 4 + 8] = 0;
    buffer[i * 4 + 9] = 0;
    buffer[i * 4 + 10] = 0;
    buffer[i * 4 + 11] = 1;
  }

  if (!settings.use_opengl) {
    myRenderer->render();
  } else {
    myRenderer->asyncRender();

    win_width = settings.width;
    win_height = settings.height;
    /* WINDOW
     ***************** */

    glfwEnable(GLFW_AUTO_POLL_EVENTS);
    glfwSetWindowSizeCallback(windowSize); // TODO: In settings

    while (running) {
      //OpenGl rendering goes here...d
      glViewport(0, 0, win_width, win_height);

      glClearColor(settings.background_color.x, settings.background_color.y,
          settings.background_color.z, settings.background_color.a);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_CULL_FACE);


      switch (renderMode) {
      case 1: {
        glUseProgram(shader_program);
        int loc = glGetUniformLocation(shader_program,
            "modelViewProjectionMatrix");
        mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(view));

        myRenderer->getScene().drawVertexArray();
        break;
      }
      case 2:
        glRasterPos2f(-1,-1);
        glPixelZoom((float) win_width / settings.width, (float) win_height / settings.height);
        glDrawPixels(settings.width, settings.height, GL_RGBA,
            GL_FLOAT, buffer);
        break;
      }

      glUseProgram(0);
      CHECK_GL_ERROR();

      //Swap front and back rendering buffers
      glfwSwapBuffers();

      timedCallback();
      glfwSleep(0.01);

      //Check if ESC key was pressed or window was closed
      running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
    }

    //Close window and terminate GLFW
    glfwTerminate();
  }
  /* EXPORTER
   ***************** */
  if (myRenderer->renderComplete() == 0) {
    raytracer::IExporter* exporter = new raytracer::PNGExporter;
    exporter->exportImage(outputFileName, settings.width, settings.height, buffer);
    delete exporter;
  }
  delete importer;
  delete myRenderer;
  std::cout << std::endl << "end of PROGRAM" << std::endl;

  exit(EXIT_SUCCESS);
}

void initGL() {
  glewInit();
  //startupGLDiagnostics();
  CHECK_GL_ERROR();

  // Workaround for AMD, which hopefully will not be neccessary in the near future...
  if (!glBindFragDataLocation) {
    glBindFragDataLocation = glBindFragDataLocationEXT;
  }

  //                      Create Shaders
  //************************************
  // The loadShaderProgram and linkShaderProgam functions are defined in glutil.cpp and
  // do exactly what we did in lab1 but are hidden for convenience
  shader_program = loadShaderProgram("simple.vert", "simple.frag");
  glBindAttribLocation(shader_program, 0, "position");
  glBindAttribLocation(shader_program, 1, "color");
  glBindAttribLocation(shader_program, 2, "normal");
  glBindAttribLocation(shader_program, 3, "texCoordIn");
  glBindAttribLocation(shader_program, 4, "material");

  glBindFragDataLocation(shader_program, 0, "fragmentColor");
  linkShaderProgram(shader_program);

  //************************************
  //        Set uniforms
  //************************************

  glUseProgram(shader_program);
  CHECK_GL_ERROR();

  // Get the location in the shader for uniform tex0
  int texLoc = glGetUniformLocation(shader_program, "colortexture");
  // Set colortexture to 0, to associate it with texture unit 0
  glUniform1i(texLoc, 0);
}

void windowSize(int width, int height) {
  win_width=width;
  win_height=height;
}

void mouseMove(int x, int y) {
  vec2 pos = vec2(x, y);
  if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
    vec2 diff = pos - mousePrev;
    camera.rotate(0.5f * diff);
    mousePrev = pos;
  }
}
void mouse(int button, int action) {
  int x, y;
  glfwGetMousePos(&x, &y);
  vec2 pos = vec2(x, y);
  if (action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
      mousePrev = pos;
    }
  }
  //mouseMove(pos.x,pos.y);
}
void timedCallback() {
  double diffTime = glfwGetTime() - prevTime;
  prevTime += diffTime;

  double speed = diffTime * 10.0;
  if (glfwGetKey('W')) {
    camera.translate(vec3(speed, 0, 0));
  }
  if (glfwGetKey('S')) {
    camera.translate(vec3(-speed, 0, 0));
  }
  if (glfwGetKey('D')) {
    camera.translate(vec3(0, speed, 0));
  }
  if (glfwGetKey('A')) {
    camera.translate(vec3(0, -speed, 0));
  }
  if (glfwGetKey(' ')) {
    camera.translate(vec3(0, 0, speed));
  }
  if (glfwGetKey('Z')) {
    camera.translate(vec3(0, 0, -speed));
  }
  if (glfwGetKey('1')) {
    renderMode = 1;
  }
  if (glfwGetKey('2')) {
    renderMode = 2;
  }
  if (glfwGetKey('3')) {
    renderMode = 3;
  }
  if (glfwGetKey('4')) {
    renderMode = 4;
  }
  if (glfwGetKey('5')) {
    renderMode = 5;
  }
  if (glfwGetKey('6')) {
    renderMode = 6;
  }
  if (glfwGetKey('R')) {
    myRenderer->loadCamera(camera);
    myRenderer->asyncRender();
    renderMode = 2;
  }
  if (glfwGetKey('T')) {
    myRenderer->stopRendering();
  }
  if (glfwGetKey('F')) {
    myRenderer->loadCamera(camera);
    myRenderer->stopRendering();
    settings.use_first_bounce = true;
    myRenderer->asyncRender();
    renderMode = 2;
  }
  if (glfwGetKey('G')) {
    myRenderer->loadCamera(camera);
    myRenderer->stopRendering();
    settings.use_first_bounce = false;
    myRenderer->asyncRender();
    renderMode = 2;
  }

  //cout << myRenderer->renderComplete() << "\n";
}
