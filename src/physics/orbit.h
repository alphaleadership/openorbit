/*
  Copyright 2008, 2009, 2010 Mattias Holm <mattias.holm(at)openorbit.org>

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

/*
    Large orbital objects
*/
#ifndef _ORBIT_H_
#define _ORBIT_H_

typedef struct PLworld PLworld;
typedef struct PLsystem PLsystem;
typedef struct PLastrobody PLastrobody;

#include <ode/ode.h>

#include <vmath/vmath.h>

#include "geo/geo.h"
#include "rendering/scenegraph.h"
#include "common/lwcoord.h"


static inline v4f_t
ode2v4(const dReal *vec)
{
  return v4f_make(vec[0], vec[1], vec[2], vec[3]);
}

static inline v4f_t
ode2v3(const dReal *vec)
{
  return v4f_make(vec[0], vec[1], vec[2], 1.0f);
}

typedef struct PL_keplerian_elements {
  double ecc;
  double a; // Semi-major
  double b; // Auxillary semi-minor
  double inc;
  double longAsc;
  double argPeri;
  double meanAnomalyOfEpoch;
} PL_keplerian_elements ;

struct PLastrobody {
  char *name;
  PLworld *world;
  PLsystem *sys;
  dBodyID id; // Using ODE at the moment, but this is not really necisary
  OOlwcoord p; // Large world coordinates
  quaternion_t q; // Current quaternion
  quaternion_t dq; // Rotational speed quaternion
  double m;
  double GM;
  PL_keplerian_elements *kepler;
  SGdrawable *drawable; //!< Link to scenegraph drawable object representing this
                        //!< object.
  SGlight *lightSource; //!< Light source if the object emits light
};


struct PLsystem {
  PLworld *world;
  PLsystem *parent;

  const char *name;
  double effectiveRadius; //!< Radius of the entire system, i.e how far away
                          //!< objects will still be considered to be under its
                          //!< influence. This is set to two times the radius of
                          //!< the largest orbit in the system (in case the
                          //!< system has sattelites), or to the distance it
                          //!< takes for the gravitational influence to diminish
                          //!< to XXX, which ever is greater.

  // TODO: These should be in some kind of oct-tree type structure
  obj_array_t orbits; // suborbits
  obj_array_t objs; // objects in this system

  PLastrobody *orbitalBody; // The body actually orbiting at this point, note that it is
  double orbitalPeriod;
  SGdrawable *orbitDrawable; // Pointer to the drawable representing the ellipsis
};

struct PLworld {
  dWorldID world;
  const char *name;
  OOscene *scene; // Scene in the sg
  PLsystem *rootSys;
};

PLworld* plNewWorld(const char *name, OOscene *sc,
                    double m, double gm, double radius,
                    double siderealPeriod, double obliquity);

PLsystem* plNewRootSystem(PLworld *world, const char *name, double m, double gm, double obliquity);


PLsystem* plNewOrbit(PLworld *world, const char *name, double m, double gm,
                     double orbitPeriod, double obliquity,
                     double semiMaj, double semiMin,
                     double inc, double ascendingNode, double argOfPeriapsis,
                     double meanAnomaly);
PLsystem* plNewSubOrbit(PLsystem *orb, const char *name, double m, double gm,
                        double orbitPeriod, double obliquity,
                        double semiMaj, double semiMin,
                        double inc, double ascendingNode, double argOfPeriapsis,
                        double meanAnomaly);

PLastrobody* plGetObject(PLworld *world, const char *name);
float3 plGetPos(const PLastrobody *obj);
float3 plGetPosForName(const PLworld *world, const char *name);
void plGetPosForName3f(const PLworld *world, const char *name,
                       float *x, float *y, float *z);


/*!
 Loads an hrml description of a solar system and builds a solar system graph
 it also connects the physics system to the graphics system.
 
 This function does not belong in the physics system, but will be here for
 now beeing.
 */
PLworld* ooOrbitLoad(OOscenegraph *sg, const char *fileName);

void plWorldStep(PLworld *world, double dt);
void plWorldClear(PLworld *world);



#endif /* ! _ORBIT_H_ */
