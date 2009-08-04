/*
  Copyright 2009 Mattias Holm <mattias.holm(at)openorbit.org>

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

#include <assert.h>
#include <stdlib.h>
#include <ode/ode.h>
#include "log.h" 
#include "sim.h"
#include "sim/spacecraft.h"
#include "res-manager.h"
#include "parsers/hrml.h"
#include <vmath/vmath.h>

extern SIMstate gSIM_state;


OOspacecraft* ooScGetCurrent(void)
{
  return gSIM_state.currentSc;
}

OOstage*
ooScStageNew(dWorldID world, float m)
{
  OOstage *stage = malloc(sizeof(OOstage));

  stage->id = dBodyCreate(world);

  dMass mass;
  dMassSetZero(&mass);
  dBodySetMass(stage->id, &mass);
  dBodyDisable(stage->id);

  return stage;
}

OOspacecraft*
ooScNew(dWorldID world, size_t stageCount)
{
  OOspacecraft *sc = malloc(sizeof(OOspacecraft));
  //ooObjVecInit(&sc->stages);
  //sc->mainEngine = NULL;
  sc->body = dBodyCreate(world);
  
  //for (size_t i = 0 ; i < stageCount ; i ++) {
  //  ooObjVecPush(&sc->stages, ooScStageNew(world, 100.0));
  //}
  
  return sc;
}


void
dMassSetConeTotal(dMass *m, dReal total_mass,
                  dReal radius, dReal height)
{
  float i11 = 1.0/10.0 * total_mass * height * height + 
    3.0/20.0 * total_mass * radius * radius;
  float i22 = i11;
  float i33 = 3.0/10.0 * total_mass * radius * radius;
  float cogx, cogy, cogz;
  
  cogx = 0.0;
  cogy = 0.25 * height; // 1/4 from base, see wikipedia entry on Cone_(geometry)
  cogz = 0.0;


  dMassSetParameters(m, total_mass,
                     cogx, cogy, cogz, // TODO: fix, COG 
                     i11, i22, i33,
                     0.0, 0.0, 0.0);
}

void
ooScAddStage(OOspacecraft *sc, OOstage *stage)
{
//  dMassAdjust(dMass *, dReal newmass);
//  dMassRotate(dMass *, const dMatrix3 R);
//  dMassTranslate(dMass *, dReal x, dReal y, dReal z);
//  dMassAdd(dMass *a, const dMass *b);
//  dMassSetBoxTotal(dMass *, dReal total_mass, dReal lx, dReal ly, dReal lz);
//  dMassSetCylinderTotal(dMass *, dReal total_mass, int direction, dReal radius, dReal length);
//  dMassSetCapsuleTotal(dMass *, dReal total_mass, int direction, dReal radius, dReal length);
//  dMassSetSphereTotal(dMass *, dReal total_mass, dReal radius);
//  dMassSetParameters(dMass *, dReal themass,
//                     dReal cgx, dReal cgy, dReal cgz,
//                     dReal I11, dReal I22, dReal I33,
//                     dReal I12, dReal I13, dReal I23);
//  dMassSetZero(dMass *);
//  
}

void
ooScDetatchStage(OOspacecraft *sc)
{
//  OOstage *stage = ooObjVecPop(&sc->stages);
  // TODO: Insert in free object vector
//  dBodyEnable(stage->id);
}
 
void
ooScStep(OOspacecraft *sc)
{
  assert(sc != NULL);
  for (size_t i = 0 ; i < sc->stages.length ; ++ i) {
    OOstage *stage = sc->stages.elems[i];
    if (stage->state == OO_Stage_Enabled) {
      ooScStageStep(sc, stage);
    }
  }
}

void // for scripts and events
ooScForce(OOspacecraft *sc, float rx, float ry, float rz)
{
//    dBodyAddRelForceAtRelPos(sc->body, rx, ry, rz, sc->);
}

void
ooScStageStep(OOspacecraft *sc, OOstage *stage) {
  assert(sc != NULL);
  assert(stage != NULL);

  for (size_t i = 0 ; i < stage->engines.length; ++ i) {
    OOengine *engine = stage->engines.elems[i];
    if (engine->state == OO_Engine_Burning ||
        engine->state == OO_Engine_Fault_Open)
    {
      dBodyAddRelForceAtRelPos(sc->body,
                               engine->dir.x * engine->forceMag,
                               engine->dir.y * engine->forceMag,
                               engine->dir.z * engine->forceMag,
                               engine->p.x, engine->p.y, engine->p.z);
    }
  }
}

typedef int (*qsort_compar_t)(const void *, const void *);
static int compar_stages(const OOstage **s0, const OOstage **s1) {
  return (*s0)->detachOrder - (*s1)->detachOrder;
}

OOspacecraft*
ooScLoad(const char *fileName)
{
  char *path = ooResGetPath(fileName);
  HRMLdocument *spaceCraftDoc = hrmlParse(path);
  free(path);

  if (spaceCraftDoc == NULL) {
    // Parser is responsible for pestering the users with errors for now.
    return NULL;
  }

  OOspacecraft *sc = NULL;
  HRMLobject *root = hrmlGetRoot(spaceCraftDoc);
  HRMLvalue scName = hrmlGetAttrForName(root, "name");

  for (HRMLobject *node = root; node != NULL; node = node->next) {
    if (!strcmp(node->name, "spacecraft")) {
      
    }
  }

  hrmlFreeDocument(spaceCraftDoc);

  ooLogInfo("loaded spacecraft %s", scName.u.str);

  // Ensure that stage vector is sorted by detachOrder
  qsort(&sc->stages.elems[0], sc->stages.length, sizeof(void*),
        (qsort_compar_t)compar_stages);

  return sc;
}
