/*
 * OmniLight.h
 *
 *  Created on: Feb 1, 2012
 *      Author: david
 */

#ifndef OMNILIGHT_H_
#define OMNILIGHT_H_

#include <glm/glm.hpp>
#include "../ILight.h"

using namespace glm;

namespace raytracer {

class BaseLight : public ILight {
public:
  BaseLight();
  BaseLight(vec3 position);
  virtual ~BaseLight();

  virtual vec3 getPosition();
  virtual float getIntensity();
  virtual float getFalloff(float distance);
  virtual vec3 getColor() const;
  virtual FalloffType getFalloffType() const;

  virtual void setPosition(vec3 position);
  virtual void setDistanceFalloff(FalloffType falloff_type);
  virtual void setIntensity(float intesity);
  virtual void setColor(vec3 color);

  virtual void getRays(Ray* rays, size_t n);

  virtual void draw();
  virtual float calcLight(IAccDataStruct* datastruct, vec3 point, vec3 offset = vec3(0.0f,0.0f,0.0f));

  void initCaches(size_t nr_of_threads);

protected:
  vec3 position;
  float intensity;
  vec3 color;
  FalloffType falloff_type;

private:
  float distanceToBlocker(IAccDataStruct* datastruct, vec3 point, vec3 offset = vec3(0.0f,0.0f,0.0f));
  bool isBlocked(IAccDataStruct* datastruct, vec3 point, vec3 offset = vec3(0.0f,0.0f,0.0f)) {
    return distanceToBlocker(datastruct, point, offset) > 0.0001f;
  }

  size_t nr_of_caches;
  Triangle** cache;

};

}

#endif /* OMNILIGHT_H_ */
