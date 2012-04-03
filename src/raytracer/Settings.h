/*
 * Settings.h
 *
 *  Created on: Jan 20, 2012
 *      Author: david
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <glm/glm.hpp>
#include <stdint.h>

using namespace glm;

namespace raytracer {

struct Settings {

  Settings() {
    // Screen
    width = 400;
    height = 300;

    opengl_version = 2;
    background_color = vec4(0,0,0,0);
    use_first_bounce = false;
    wireframe = 0;
    test = 0 ;
    debug_mode=0;

    // Data acc
    tree = 2;
    draw_data_struct = true;

    // Tracer
    tracer = 2;

    // Recursion
    max_recursion_depth = 5;
    recursion_attenuation_threshold = 0.015625;

    // Threading
    threads = 0;  // auto

    // Tonemapping
    key = 0.18;
    white = 1e20;

    //Photon mapper
    photons = 32*1024;
    gather_radius = 0.5f;
    photonmap_size = 1024;
  }

  // Screen
  int width;
  int height;

  int opengl_version;
  bool use_first_bounce;
  int wireframe;
  vec4 background_color;
  float test;
  int debug_mode;

  // Data acc
  int tree;
  bool draw_data_struct;

  // Tracer
  int tracer;

  // Recursion
  unsigned int max_recursion_depth;
  float recursion_attenuation_threshold;

  // Threading
  int threads;

  // Tonemapping
  float key;
  float white;

  //Photon mapper
  size_t photons; //32*1024;
  float gather_radius;
  size_t photonmap_size;
};

}

#endif /* SETTINGS_H_ */
