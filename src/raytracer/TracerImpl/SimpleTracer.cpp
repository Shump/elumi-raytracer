/*
 * SimpleTracer.cpp
 *
 *  Created on: Jan 26, 2012
 *      Author: david
 */

#include "SimpleTracer.h"

namespace raytracer {

SimpleTracer::SimpleTracer(Scene* scene) {
  // TODO Auto-generated constructor stub
  SimpleTracer::scene = scene;
}

SimpleTracer::~SimpleTracer() {
  // TODO Auto-generated destructor stub
}

void SimpleTracer::trace(Ray* rays, int length, unsigned char* buffer) {
  //IAccDataStruct *ads = scene.getAccDataStruc();

  for(int i = 0; i < length; i++) {
    //rays[i]
  }
}

void SimpleTracer::shade() {

}

}
