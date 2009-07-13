/* 
    The contents of this file are subject to the Mozilla Public License
    Version 1.1 (the "License"); you may not use this file except in compliance
    with the License. You may obtain a copy of the License at
    http://www.mozilla.org/MPL/

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the
    License.

    The Original Code is the Open Orbit space flight simulator.

    The Initial Developer of the Original Code is Mattias Holm. Portions
    created by the Initial Developer are Copyright (C) 2006 the
    Initial Developer. All Rights Reserved.

    Contributor(s):
        Mattias Holm <mattias.holm(at)contra.nu>.

    Alternatively, the contents of this file may be used under the terms of
    either the GNU General Public License Version 2 or later (the "GPL"), or
    the GNU Lesser General Public License Version 2.1 or later (the "LGPL"), in
    which case the provisions of GPL or the LGPL License are applicable instead
    of those above. If you wish to allow use of your version of this file only
    under the terms of the GPL or the LGPL and not to allow others to use your
    version of this file under the MPL, indicate your decision by deleting the
    provisions above and replace  them with the notice and other provisions
    required by the GPL or the LGPL.  If you do not delete the provisions
    above, a recipient may use your version of this file under either the MPL,
    the GPL or the LGPL."
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
