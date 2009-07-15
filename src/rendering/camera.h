/*
  Copyright 2006,2009 Mattias Holm <mattias.holm(at)openorbit.org>

  This file is part of Open Orbit.

  Open Orbit is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Open Orbit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with Open Orbit.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __CAMERA_H__
#define __CAMERA_H__
#ifdef __cplusplus
extern "C" {
#endif 

#include <stdbool.h>
#include <vmath/vmath.h>
#include <ode/ode.h>

  typedef enum OOcamtype OOcamtype;
  typedef struct OOfreecam OOfreecam;
  typedef struct OOfixedcam OOfixedcam;
  typedef struct OOorbitcam OOorbitcam;
  typedef struct OOcam OOcam;
  
#include "scenegraph.h"

  enum OOcamtype {
      OOCam_Free,
      OOCam_Fixed,
      OOCam_Orbit
  };

  struct OOfreecam {
      vector_t p;
      vector_t dp;
      quaternion_t q;
      quaternion_t dq;
  };

  struct OOfixedcam {
      dBodyID body;

      vector_t r;
      quaternion_t q;   
  };

  struct OOorbitcam {
      dBodyID body;

      vector_t r;
  };

  struct OOcam {
      OOcamtype kind;
      OOscene *scene;
      OOobject *camData;
  };

  void ooSgCamInit(void);
  void ooSgCamStep(OOcam *cam);

  OOcam* ooSgNewFreeCam(OOscenegraph *sg, OOscene *sc,
                        float x, float y, float z, 
                        float rx, float ry, float rz);

  OOcam* ooSgNewFixedCam(OOscenegraph *sg, OOscene *sc, dBodyID body,
                         float dx, float dy, float dz, 
                         float rx, float ry, float rz);

  OOcam* ooSgNewOrbitCam(OOscenegraph *sg, OOscene *sc, dBodyID body,
                         float dx, float dy, float dz);

  void ooSgCamMove(OOcam *cam);
  void ooSgCamRotate(OOcam *cam);

#ifdef __cplusplus
}
#endif 

#endif
