/*
 Copyright 2010 Mattias Holm <mattias.holm(at)openorbit.org>

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

#ifndef SIM_MFD_MFD_H
#define SIM_MFD_MFD_H

#include "rendering/scenegraph.h"

typedef struct SIMmfd SIMmfd;

typedef void (*SIMmfddraw)(SIMmfd *mfd);

struct SIMmfd {
  // drawing context
  SIMmfddraw draw;
};

// Draw top level MFD and the MFD screen
void simMfdDraw(SIMmfd *mfd);


typedef struct SIMhud SIMhud;

typedef void (*SIMhuddraw)(SIMhud *hud);

struct SIMhud {
  // drawing context
  SIMhuddraw draw;
};


void simHudDraw(SIMhud *hud);


#endif /* !SIM_MFD_MFD_H */