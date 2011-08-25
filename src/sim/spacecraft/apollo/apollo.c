/*
 Copyright 2011 Mattias Holm <mattias.holm(at)openorbit.org>

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

#include "sim.h"
#include "sim/simevent.h"
#include "sim/spacecraft.h"
#include "sim/actuator.h"
#include "common/palloc.h"
#include "log.h"

enum Stages {
  SATURN_1C = 0,
  SATURN_II,
  SATURN_IVB,
  APOLLO_SERVICE,
  APOLLO_COMMAND
};



enum Saturn_1C_II_IVB_Engines {
  ENG_ROCKETDYNE_0 = 0, // Central engine
  ENG_ROCKETDYNE_1,
  ENG_ROCKETDYNE_2,
  ENG_ROCKETDYNE_3,
  ENG_ROCKETDYNE_4
};

enum Apollo_Service_Engines {
  ENG_SERV_PROP = 0,
};
enum Apollo_Command_Engines {
  THR_POSI = 0,
  THR_RETRO_0,
  THR_RETRO_1,
  THR_RETRO_2,
  THR_ROLL_0,
  THR_ROLL_1,
  THR_PITCH_0,
  THR_PITCH_1,
  THR_YAW_0,
  THR_YAW_1,

};

static void
AxisUpdate(OOspacecraft *sc)
{
  // TODO: Replace IO-system queries with sim variable lookups.
  //       This is important in order to allow for multiplexed axis commands,
  //       in turn enabling autopilots and network control.

  OOaxises axises;
  ooGetAxises(&axises);

  switch (sc->detatchSequence) {
    case SATURN_1C: {
      OOstage *saturn_1c = sc->stages.elems[SATURN_1C];
      ARRAY_FOR_EACH(i, saturn_1c->engines) {
        simEngineSetThrottle(ARRAY_ELEM(saturn_1c->engines, i), axises.orbital);
      }
      break;
    }
    case SATURN_II: {
      OOstage *saturn_ii = sc->stages.elems[SATURN_II];
      ARRAY_FOR_EACH(i, saturn_ii->engines) {
        simEngineSetThrottle(ARRAY_ELEM(saturn_ii->engines, i), axises.orbital);
      }
      break;
    }
    case SATURN_IVB: {
      OOstage *saturn_ivb = sc->stages.elems[SATURN_IVB];
      ARRAY_FOR_EACH(i, saturn_ivb->engines) {
        simEngineSetThrottle(ARRAY_ELEM(saturn_ivb->engines, i), axises.orbital);
      }
      break;
    }
    case APOLLO_SERVICE: {
      OOstage *apollo_service = sc->stages.elems[APOLLO_SERVICE];
      SIMengine *eng = ARRAY_ELEM(apollo_service->engines, ENG_SERV_PROP);

      break;
    }
    case APOLLO_COMMAND: {
      OOstage *apollo_cmd = sc->stages.elems[APOLLO_COMMAND];
      break;
    }
    default:
      assert(0 && "invalid case");
  }
}

static void
DetatchComplete(void *data)
{
  ooLogInfo("detatch complete");

  OOspacecraft *sc = (OOspacecraft*)data;
  sc->detatchPossible = false; // Do not reenable, must be false after last stage detatched
  sc->detatchComplete = true;

  //OOstage *stage = sc->stages.elems[MERC_CAPSULE];
  //
  //simEngineDisable(ARRAY_ELEM(stage->engines, THR_POSI));
  //simStageArmEngines(stage);
  //simEngineDisarm(ARRAY_ELEM(stage->engines, THR_POSI));
}

static void
DetatchStage(OOspacecraft *sc)
{
  ooLogInfo("detatch commanded");
  sc->detatchPossible = false;

  switch (sc->detatchSequence) {
  case SATURN_1C: {
    ooLogInfo("detatching saturn 1c stage");
    OOstage *sat_1c = sc->stages.elems[SATURN_1C];
    OOstage *sat_ii = sc->stages.elems[SATURN_II];

    simStageDisableEngines(sat_1c);
    simStageLockEngines(sat_1c);

    simDetatchStage(sc, sat_1c);

    simEngineArm(ARRAY_ELEM(sat_ii->engines, THR_POSI));
    simEngineFire(ARRAY_ELEM(sat_ii->engines, THR_POSI));

    sc->detatchComplete = false;
    simEnqueueDelta_s(1.0, DetatchComplete, sc);
    break;
  }
  case SATURN_II:
    ooLogInfo("detatching saturn ii stage");
    break;
  case SATURN_IVB:
    ooLogInfo("detatching saturn ivb stage");
    break;
  case APOLLO_SERVICE:
    ooLogInfo("detatching apollo service module");
    break;
  case APOLLO_COMMAND:
    break;
  default:
    assert(0 && "invalid case");
  }
}


static void
MainEngineToggle(OOspacecraft *sc)
{
  switch (sc->detatchSequence) {
    case SATURN_1C: {
      OOstage *sat_1c = sc->stages.elems[SATURN_1C];
      break;
    }
    case SATURN_II: {
      OOstage *sat_ii = sc->stages.elems[SATURN_II];
      break;
    }
    case SATURN_IVB: {
      OOstage *sat_ivb = sc->stages.elems[SATURN_IVB];
      break;
    }
    case APOLLO_SERVICE: {
      OOstage *apollo_serv = sc->stages.elems[APOLLO_SERVICE];
      break;
    }
    case APOLLO_COMMAND: {
      OOstage *apollo_cmd = sc->stages.elems[APOLLO_COMMAND];
      break;
    }
    default:
      assert(0 && "invalid case");
  }
}

static OOspacecraft*
ApolloNew(void)
{
  OOspacecraft *sc = smalloc(sizeof(OOspacecraft));
  simScInit(sc, "Saturn V");
  sc->detatchStage = DetatchStage;
  sc->toggleMainEngine = MainEngineToggle;
  sc->axisUpdate = AxisUpdate;
  // inertia tensors are entered in the base form, assuming that the total
  // mass = 1.0
  // for the redstone mercury rocket we assume a solid cylinder for the form
  // 1/2 mrr = 0.5 * 1.0 * 0.89 * 0.89 = 0.39605
  // 1/12 m(3rr + hh) = 27.14764852

  OOstage *sat_1c = simNewStage(sc, "Saturn 1C",
                                 "spacecrafts/saturn/saturn_1c.ac");
  OOstage *sat_ii = simNewStage(sc, "Saturn II",
                                 "spacecrafts/saturn/saturn_ii.ac");
  OOstage *sat_ivb = simNewStage(sc, "Saturn IVB",
                                  "spacecrafts/saturn/saturn_ivb.ac");
  OOstage *apollo_serv = simNewStage(sc, "Service Module",
                                      "spacecrafts/saturn/apollo_serv.ac");
  OOstage *apollo_cmd = simNewStage(sc, "Command Module",
                                     "spacecrafts/saturn/apollo_cmd.ac");


  plMassSet(&sat_1c->obj->m, 1.0f, // Default to 1.0 kg
            0.0f, 0.0f, 0.0f,
            27.14764852, 0.39605, 27.14764852,
            0.0, 0.0, 0.0);
  plMassMod(&sat_1c->obj->m, 24000.0 + 2200.0);
  plMassTranslate(&sat_1c->obj->m, 0.0, 8.9916, 0.0);
  plMassSetMin(&sat_1c->obj->m, 4400.0);
  plSetDragCoef(sat_1c->obj, 0.5);
  plSetArea(sat_1c->obj, 2.0*M_PI);

  scStageSetOffset3f(sat_1c, 0.0, 0.0, 0.0);

  simNewEngine("Rocketdyne F-1:0", sat_1c, SIM_Thruster, SIM_Armed, 1.0f,
               (float3){0.0,0.0,0.0}, (float3){0.0,370.0e3,0.0});
  simNewEngine("Rocketdyne F-1:1", sat_1c, SIM_Thruster, SIM_Armed, 1.0f,
               (float3){0.0,0.0,0.0}, (float3){0.0,370.0e3,0.0});
  simNewEngine("Rocketdyne F-1:2", sat_1c, SIM_Thruster, SIM_Armed, 1.0f,
               (float3){0.0,0.0,0.0}, (float3){0.0,370.0e3,0.0});
  simNewEngine("Rocketdyne F-1:3", sat_1c, SIM_Thruster, SIM_Armed, 1.0f,
               (float3){0.0,0.0,0.0}, (float3){0.0,370.0e3,0.0});
  simNewEngine("Rocketdyne F-1:4", sat_1c, SIM_Thruster, SIM_Armed, 1.0f,
               (float3){0.0,0.0,0.0}, (float3){0.0,370.0e3,0.0});

  return sc;
}

INIT_STATIC_SC_PLUGIN
{
  simNewSpacecraftClass("apollo", ApolloNew);
}
