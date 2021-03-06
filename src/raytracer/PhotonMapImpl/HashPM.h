/*
 * HashPM.h
 *
 *  Created on: Mar 20, 2012
 *      Author: ulvinge
 */

#ifndef HASHPM_H_
#define HASHPM_H_

#include "../IPhotonMap.h"
#include "../utilities/HashPoint.h"
#include <vector>

using namespace std;

namespace raytracer {

class HashPM : public IPhotonMap {
public:
  HashPM(float bucketsize, size_t n_buckets);
  virtual ~HashPM();

  void balance();
  void addPhoton(Photon p);
  //void gatherFromG(vec3 point, float r, Photon* p, size_t g);
  vector<Photon*> gatherFromR(vec3 point, float r) const;
  size_t getTotalPhotons() const;
  size_t getNumberOfBuckets();
  vector<Photon>& getBucket(size_t t);

  Photon* getBucket(vec3 point);


private:
  HashPoint<Photon> hashpoint;
  size_t totalPhotons;
};

}

#endif /* HASHPM_H_ */
