/*
 * Renderer.h
 *
 *  Created on: Jan 20, 2012
 *      Author: david
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "Settings.h"
#include "ITracer.h"
#include "scene/Scene.h"
#include "utilities/Triangle.h"

#include <iostream>
#include <boost/thread.hpp>
#include "glm/glm.hpp"


using namespace glm;

namespace raytracer {

class Renderer {
public:
  Renderer(Scene* scene);
  virtual ~Renderer();
  
  Scene* getScene();
  ITracer* getTracer();

  void loadTriangles(vector<Triangle*> triangles, bool overwrite=false);
  void loadCamera(Camera& camera);
  void loadLights(ILight** lights, int length, bool overwrite=false);
  void setSettings(Settings* settings);
  void loadScene(Scene* scene);

  float* getColorBuffer();
  void render();       // synchronic
  void asyncRender();  // asynchronic
  void stopRendering();// synchronic
  
  /*
   * @return The number of pixels left to render
   */
  unsigned int renderComplete();
  
private:
  void init();
  Scene* m_scene;
  //Settings* m_settings;
  int buffer_length;
  ITracer* m_tracer;
  bool abort;

  float* color_buffer;

  boost::thread* renderthread;
};

}

#endif /* RENDERER_H_ */
