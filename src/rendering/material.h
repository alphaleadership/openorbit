/*
 Copyright 2010 Mattias Holm <mattias.holm(at)openorbit.org>

 This file is part of Open Orbit.

 Open Orbit is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Open Orbit is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Open Orbit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SG_MATERIAL_H__
#define SG_MATERIAL_H__

#include <vmath/vmath.h>
#include "rendering/types.h"

void sg_init_material(sg_material_t *mat);
void sg_bind_material(sg_material_t *mat);

void sg_set_material_amb4f(sg_material_t *mat, float r, float g, float b, float a);
void sg_set_material_diff4f(sg_material_t *mat, float r, float g, float b, float a);
void sg_set_material_spec4f(sg_material_t *mat, float r, float g, float b, float a);
void sg_set_material_emiss4f(sg_material_t *mat, float r, float g, float b, float a);
void sg_set_material_shininess(sg_material_t *mat, float s);



#endif /* !SG_MATERIAL_H__ */
