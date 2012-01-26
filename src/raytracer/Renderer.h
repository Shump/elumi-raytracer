/*
 * Renderer.h
 *
 *  Created on: Jan 20, 2012
 *      Author: david
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include "Settings.h"
#include "scene/Scene.h"
#include "utilities/Triangle.h"

#include <stdint.h>

namespace raytracer {

class Renderer {
public:
  Renderer();
  virtual ~Renderer();
  
  void loadTriangles(Triangle* triangles, int length, bool overwrite=false);
  void loadCamera(Camera& camera);
  void loadLights(ILight* lights, int length, bool overwrite=false);
  void loadSettings(Settings& settings);

  uint8_t* getFloatArray();
  void render();  // asynchronic
  
  /*
   * @return The number of pixels left to render
   */
  int renderComplete();
  
private:
  Settings* m_settings;
};

}

#endif /* RENDERER_H_ */
