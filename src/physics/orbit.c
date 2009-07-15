/*
  Copyright 2008 Mattias Holm <mattias.holm(at)openorbit.org>

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

#include "orbit.h"
#include <ode/ode.h>
#include <assert.h>


#include <vmath/vmath.h>
#include "sim.h"
#include "sim/simtime.h"
#include "geo/geo.h"
#include "log.h"
#include "parsers/hrml.h"
#include "res-manager.h"
#include "rendering/scenegraph.h"

/*
    NOTE: G is defined as 6.67428 e-11 (m^3)/kg/(s^2), let's call that G_m. In AU,
      this would then be G_au = G_m / (au^3)

      This means that G_au = 1.99316734 e-44 au^3/kg/s^2
      or: G_au = 1.99316734 e-44 au^3/kg/s^2

      1 au = 149 597 870 000 m

*/

double
comp_orbital_period(double semimajor, double g, double m1, double m2)
{
  return 2.0 * M_PI * sqrt(pow(semimajor, 3.0)/(g*(m1 + m2)));
}

double
comp_orbital_period_for_planet(double semimajor)
{
  return sqrt(pow(semimajor, 3.0));
}

OOorbsys*
ooOrbitNewRootSys(const char *name, OOscene *scene, float m, float rotPeriod)
{
  OOorbsys *sys = malloc(sizeof(OOorbsys));
  sys->world = dWorldCreate();
  sys->name = strdup(name);
  ooObjVecInit(&sys->sats);
  ooObjVecInit(&sys->objs);

  sys->parent = NULL;

  sys->scale.dist = 1.0f;
  sys->scale.distInv = 1.0f;
  sys->scale.mass = 1.0f;
  sys->scale.massInv = 1.0f;

  sys->phys.k.G = 6.67428e-11;
  sys->phys.param.m = m;
  sys->phys.param.orbitalPeriod = 0.0;
  sys->phys.param.rotationPeriod = rotPeriod;

  sys->scene = scene;
  sys->level = 0;
  sys->orbit = NULL;

  return sys;
}


OOorbsys*
ooOrbitNewSys(const char *name, OOscene *scene,
              float m, float orbitPeriod, float rotPeriod,
              float semiMaj, float semiMin)
{
  OOorbsys *sys = ooOrbitNewRootSys(name, scene, m, rotPeriod);
  sys->orbit = ooGeoEllipseAreaSeg(300, semiMaj, semiMin);
  sys->phys.param.orbitalPeriod = orbitPeriod;
  return sys;
}

OOorbobj*
ooOrbitNewObj(OOorbsys *sys, const char *name,
              OOdrawable *drawable,
              float m,
              float x, float y, float z,
              float vx, float vy, float vz,
              float qx, float qy, float qz, float qw,
              float rqx, float rqy, float rqz, float rqw)
{
  assert(sys != NULL);
  assert(m >= 0.0);
  OOorbobj *obj = malloc(sizeof(OOorbobj));
  obj->name = strdup(name);
  obj->m = m;
  obj->id = dBodyCreate(sys->world);
  dBodySetGravityMode(obj->id, 0); // Ignore standard ode gravity effects

  dBodySetData(obj->id, drawable); // 
  dBodySetMovedCallback(obj->id, ooSgUpdateObject);

  dQuaternion quat = {qw, qx, qy, qz};
  dBodySetQuaternion(obj->id, quat);
  dBodySetAngularVel(obj->id, 0.0, 0.0, 1.0);

  obj->sys = sys;

  ooObjVecPush(&sys->objs, obj);

  return obj;
}

v4f_t ooOrbitDist(OOorbobj * restrict a, OOorbobj * restrict b)
{
  assert(a != NULL);
  assert(b != NULL);

  const dReal *pa = dBodyGetPosition(a->id);
  const dReal *pb = dBodyGetPosition(b->id);

  if (a->sys == b->sys) {
    v4f_t dist = ode2v3(pa) - ode2v3(pb);
    return dist;
  } else {
    assert(0 && "distances between diffrent systems not yet supported");
    return v4f_make(0.0, 0.0, 0.0, 0.0);
  }
}


void
ooOrbitAddChildSys(OOorbsys * restrict parent, OOorbsys * restrict child)
{
  assert(parent != NULL);
  assert(child != NULL);

  ooLogInfo("adding child system %s to %s", child->name, parent->name);
  ooObjVecPush(&parent->sats, child);
  
  child->parent = parent;
  child->level = parent->level + 1;
}

void
ooOrbitSetScale(OOorbsys *sys, float ms, float ds)
{
  assert(sys != NULL);

  sys->scale.mass = ms;
  sys->scale.massInv = 1.0f/ms;
  sys->scale.dist = ds;
  sys->scale.distInv = 1.0f/ds;
}

void
ooOrbitSetScene(OOorbsys *sys, OOscene *scene)
{
  assert(sys != NULL);
  assert(scene != NULL);

  sys->scene = scene;
}


void
ooOrbitClear(OOorbsys *sys)
{
  for (size_t i ; i < sys->objs.length ; i ++) {
    dBodySetForce(((OOorbobj*)sys->objs.elems[i])->id, 0.0f, 0.0f, 0.0f);
    dBodySetTorque(((OOorbobj*)sys->objs.elems[i])->id, 0.0f, 0.0f, 0.0f);
  }

  for (size_t i; i < sys->sats.length ; i ++) {
    ooOrbitClear(sys->sats.elems[i]);
  }
}



void
ooOrbitStep(OOorbsys *sys, float stepSize)
{
  bool needsCompacting = false;
  // First compute local gravity for each object
  for (size_t i ; i < sys->objs.length ; i ++) {
    // Since objects can migrate to other systems...
    if (sys->objs.elems[i] != NULL) {
      OOorbobj *obj = sys->objs.elems[i];
      //sys->phys.param.m
      const dReal *objPos_ = dBodyGetPosition(obj->id);
      vector_t objPos = v_set(objPos_[0], objPos_[1], objPos_[2], 0.0f);
      vector_t dist = objPos; // Since system origin is 0.0
      scalar_t r12 = v_abs(dist);
      r12 = r12 * r12;
      vector_t f12 = v_s_mul(v_normalise(v_neg(dist)), //negate, force should point to center object
                            -sys->phys.k.G * sys->phys.param.m * obj->m / r12);
      dBodyAddForce(obj->id, f12.x, f12.y, f12.z);
    } else {
      needsCompacting = true;
    }
  }

  // Do ODE step
  dWorldStep(sys->world, stepSize);

  // Update current position
  if (sys->orbit) {
    sys->phys.param.pos = ooGeoEllipseSegPoint(sys->orbit,
                                               (ooTimeGetJD(ooSimTimeState()) / 
                                                 sys->phys.param.orbitalPeriod)*
                                                 (float)sys->orbit->vec.length);

    ooLogTrace("%f: %s: %f: %vf",
               ooTimeGetJD(ooSimTimeState()),
               sys->name,
               sys->phys.param.orbitalPeriod,
               sys->phys.param.pos);
  } else {
    sys->phys.param.pos = v4f_make(0.0, 0.0, 0.0, 0.0);
  }
  // Recurse and do the same for each subsystem
  for (size_t i = 0; i < sys->sats.length ; i ++) {
    ooOrbitStep(sys->sats.elems[i], stepSize);
  }

  if (sys->scene) {
    vector_t v = {.v = sys->phys.param.pos};
    ooSgSetScenePos(sys->scene, v.x, v.y, v.z);
    //ooSgSetSceneQuat(sys->scene, float x, float y, float z, float w);
    //ooSgSetSceneScale(sys->scene, float scale);
  }

  if (needsCompacting) {
    ooObjVecCompress(&sys->objs);
  }
}

void
ooOrbitSetConstant(OOorbsys *sys, const char *key, float k)
{
    assert(sys != NULL && "sys is null");
    assert(key != NULL && "key is null");

    if (!strcmp(key, "G")) {
        sys->phys.k.G = k;
    }
}


void
ooOrbitLoadComet(HRMLobject *obj)
{
  assert(obj);
  assert(obj->val.typ == HRMLNode);

  for (HRMLobject *child = obj->children; child != NULL; child = child->next) {

  }
}

void
ooOrbitLoadMoon(HRMLobject *obj)
{
  assert(obj);
  assert(obj->val.typ == HRMLNode);

  for (HRMLobject *child = obj->children; child != NULL; child = child->next) {

  }
}


OOorbsys*
ooOrbitLoadPlanet(HRMLobject *obj, OOscene *parentScene)
{
  assert(obj);
  assert(obj->val.typ == HRMLNode);

  HRMLvalue planetName = hrmlGetAttrForName(obj, "name");

  double mass, radius, siderealPeriod;
  double semiMajor, ecc, inc, longAscNode, longPerihel, meanLong, rightAsc,
         declin;
  const char *tex = NULL;
  HRMLobject *sats = NULL;
  
  for (HRMLobject *child = obj->children; child != NULL ; child = child->next) {
    if (!strcmp(child->name, "physical")) {
      for (HRMLobject *phys = child->children; phys != NULL; phys = phys->next) {
        if (!strcmp(phys->name, "mass")) {
          mass = hrmlGetReal(phys);
        } else if (!strcmp(phys->name, "radius")) {
          radius = hrmlGetReal(phys);
        } else if (!strcmp(phys->name, "sidereal-rotational-period")) {
          siderealPeriod = hrmlGetReal(phys);
        }
      }
    } else if (!strcmp(child->name, "orbit")) {
      for (HRMLobject *orbit = child->children; orbit != NULL; orbit = orbit->next) {
        if (!strcmp(orbit->name, "semimajor-axis")) {
          semiMajor = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "eccentricity")) {
          ecc = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "inclination")) {
          inc = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "longitude-ascending-node")) {
          longAscNode = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "longitude-perihelion")) {
          longPerihel = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "mean-longitude")) {
          meanLong = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "right-ascension")) {
          rightAsc = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "declination")) {
          declin = hrmlGetReal(orbit);
        } else if (!strcmp(orbit->name, "reference-date")) {

        }
      }
    } else if (!strcmp(child->name, "atmosphere")) {

    } else if (!strcmp(child->name, "rendering")) {
      for (HRMLobject *rend = child->children; rend != NULL; rend = rend->next) {
        if (!strcmp(rend->name, "model")) {

        } else if (!strcmp(rend->name, "texture")) {
          tex = hrmlGetStr(rend);
        }
      }
    } else if (!strcmp(child->name, "sattelites")) {
      sats = child;
    }
  }
  
  OOscene *sc = ooSgNewScene(parentScene, planetName.u.str/*(planetName)*/);
  // Create scene object for planet
  
  
  //ooSgSetObjectAngularSpeed(drawable, )
  // Period will be in years assuming that semiMajor is in au
  double period = comp_orbital_period_for_planet(semiMajor);
  
  OOorbsys *sys = ooOrbitNewSys(planetName.u.str, sc,
                                mass, period, 1.0,//float period,
                                semiMajor, ooGeoComputeSemiMinor(semiMajor, ecc));

  OOdrawable *drawable = ooSgNewSphere(planetName.u.str, radius, tex);
  ooSgSceneAddObj(sc, drawable); // TODO: scale to radius
  quaternion_t q = q_rot(1.0, 0.0, 0.0, DEG_TO_RAD(90.0));
  quaternion_t qr = q_rot(0.0/*x*/,1.0/*y*/,0.0/*z*/,DEG_TO_RAD(1.0)); // TODO: real rot

  OOorbobj *orbObj = ooOrbitNewObj(sys, planetName.u.str, drawable,
                                   mass,
                                   0.0, 0.0, 0.0,
                                   0.0, 0.0, 0.0,
                                   q.x, q.y, q.z, q.w,
                                   qr.x, qr.y, qr.z, qr.w);

  sys->scale.dist = 149598000000.0;
  sys->scale.distInv = 1.0/149598000000.0;
  return sys;
}

void
ooOrbitLoadSatellites(HRMLobject *obj, OOorbsys *sys, OOscene *parentScene)
{
  assert(obj);
  assert(obj->val.typ == HRMLNode);

  for (HRMLobject *child = obj->children; child != NULL; child = child->next) {
    if (!strcmp(child->name, "planet")) {
      OOorbsys *psys = ooOrbitLoadPlanet(child, parentScene);
      ooOrbitAddChildSys(sys, psys);
    } else if (!strcmp(child->name, "moon")) {
      ooOrbitLoadMoon(child);
    } else if (!strcmp(child->name, "comet")) {

    }
  }
}


OOorbsys*
ooOrbitLoadStar(HRMLobject *obj)
{
  assert(obj);
  assert(obj->val.typ == HRMLNode);
  HRMLvalue starName = hrmlGetAttrForName(obj, "name");
  double mass = 0.0;
  double radius, siderealPeriod;

  OOscene *sc = ooSgNewScene(NULL, starName.u.str);

  OOorbsys *sys = ooOrbitNewRootSys(starName.u.str, sc,
                                    mass, 0.0 //float period
                                    );
  HRMLobject *sats = NULL;
  for (HRMLobject *child = obj->children; child != NULL ; child = child->next) {
    if (!strcmp(child->name, "satellites")) {
      sats = child;
    } else if (!strcmp(child->name, "physical")) {
      for (HRMLobject *phys = child->children; phys != NULL; phys = phys->next) {
        if (!strcmp(phys->name, "mass")) {
          mass = hrmlGetReal(phys);
        } else if (!strcmp(phys->name, "radius")) {
          radius = hrmlGetReal(phys);
        } else if (!strcmp(phys->name, "sidereal-rotational-period")) {
          siderealPeriod = hrmlGetReal(phys);
        }
      }
    } else if (!strcmp(child->name, "rendering")) {
      for (HRMLobject *rend = child->children; rend != NULL; rend = rend->next) {
        if (!strcmp(rend->name, "texture")) {
        
        }
      }
    }
  }
  

  sys->phys.param.m = mass;
  sys->scale.dist = 1.0;
  sys->scale.distInv = 1.0;
  
  assert(sats != NULL);
  ooOrbitLoadSatellites(sats, sys, sc);
  
  return sys;
}

OOorbsys*
ooOrbitLoad(OOscenegraph *sg, const char *fileName)
{
  char *file = ooResGetPath(fileName);
  HRMLdocument *solarSys = hrmlParse(file);
  free(file);
  //HRMLschema *schema = hrmlLoadSchema(ooResGetFile("solarsystem.hrmlschema"));
  //hrmlValidate(solarSys, schema);
  if (solarSys == NULL) {
    // Parser is responsible for pestering the users with errors for now.
    return NULL;
  }

  OOorbsys *sys = NULL;
  // Go through the document and handle each entry in the document

  for (HRMLobject *node = hrmlGetRoot(solarSys); node != NULL; node = node->next) {
    if (!strcmp(node->name, "openorbit")) {
      for (HRMLobject *star = node->children; star != NULL; star = star->next) {
        sys = ooOrbitLoadStar(star); //BUG: If more than one star is specified
        ooSgSetRoot(sg, sys->scene);
      }
    }
  }

  hrmlFreeDocument(solarSys);

  ooLogInfo("loaded solar system");
  return sys;
}

OOorbsys*
ooOrbitGetSys(const OOorbsys *root,  const char *name)
{
  char str[strlen(name)+1];
  strcpy(str, name); // TODO: We do not trust the user, should probably
                     // check alloca result

  OOorbsys *sys = (OOorbsys*)root;
  char *strp = str;
  char *strTok = strsep(&strp, "/");
  int idx = 0;
  OOobjvector *vec = NULL;
  while (sys) {
    if (!strcmp(sys->name, strTok)) {
      if (strp == NULL) {
        // At the end of the sys path
        return sys;
      }

      // If this is not the lowest level, go one level down
      strTok = strsep(&strp, "/");

      vec = &sys->sats;
      idx = 0;
      if (vec->length <= 0) return NULL;
      sys = vec->elems[idx];
    } else {
      if (vec == NULL) return NULL;
      idx ++;
      if (vec->length <= idx) return NULL;
      sys = vec->elems[idx];
    }
  }
  return NULL;
}
