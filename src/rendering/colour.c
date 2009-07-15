/*
  Copyright 2006 Mattias Holm <mattias.holm(at)openorbit.org>

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

#include "colour.h"
#include <stdlib.h>
#include <stdint.h>

uint8_t gCOLOUR_temperature_tbl[][3] = {
	{255, 56, 0}, // 1000K
	{255, 71, 0},
	{255, 83, 0},
	{255, 93, 0},
	{255, 101, 0},
	{255, 109, 0},
	{255, 115, 0},
	{255, 121, 0},
	{255, 126, 0},
	{255, 131, 0},
	{255, 137, 18},
	{255, 142, 33},
	{255, 147, 44},
	{255, 152, 54},
	{255, 157, 63},
	{255, 161, 72},
	{255, 165, 79},
	{255, 169, 87},
	{255, 173, 94},
	{255, 177, 101},
	{255, 180, 107},
	{255, 184, 114},
	{255, 187, 120},
	{255, 190, 126},
	{255, 193, 132},
	{255, 196, 137},
	{255, 199, 143},
	{255, 201, 148},
	{255, 204, 153},
	{255, 206, 159},
	{255, 209, 163},
	{255, 211, 168},
	{255, 213, 173},
	{255, 215, 177},
	{255, 217, 182},
	{255, 219, 186},
	{255, 221, 190},
	{255, 223, 194},
	{255, 225, 198},
	{255, 227, 202},
	{255, 228, 206},
	{255, 230, 210},
	{255, 232, 213},
	{255, 233, 217},
	{255, 235, 220},
	{255, 236, 224},
	{255, 238, 227},
	{255, 239, 230},
	{255, 240, 233},
	{255, 242, 236},
	{255, 243, 239},
	{255, 244, 242},
	{255, 245, 245},
	{255, 246, 248},
	{255, 248, 251},
	{255, 249, 253},
	{254, 249, 255},
	{252, 247, 255},
	{249, 246, 255},
	{247, 245, 255},
	{245, 243, 255},
	{243, 242, 255},
	{240, 241, 255},
	{239, 240, 255},
	{237, 239, 255},
	{235, 238, 255},
	{233, 237, 255},
	{231, 236, 255},
	{230, 235, 255},
	{228, 234, 255},
	{227, 233, 255},
	{225, 232, 255},
	{224, 231, 255},
	{222, 230, 255},
	{221, 230, 255},
	{220, 229, 255},
	{218, 228, 255},
	{217, 227, 255},
	{216, 227, 255},
	{215, 226, 255},
	{214, 225, 255},
	{212, 225, 255},
	{211, 224, 255},
	{210, 223, 255},
	{209, 223, 255},
	{208, 222, 255},
	{207, 221, 255},
	{207, 221, 255},
	{206, 220, 255},
	{205, 220, 255},
	{204, 219, 255},
	{203, 219, 255},
	{202, 218, 255},
	{201, 218, 255},
	{201, 217, 255},
	{200, 217, 255},
	{199, 216, 255},
	{199, 216, 255},
	{198, 216, 255},
	{197, 215, 255},
	{196, 215, 255},
	{196, 214, 255},
	{195, 214, 255},
	{195, 214, 255},
	{194, 213, 255},
	{193, 213, 255},
	{193, 212, 255},
	{192, 212, 255},
	{192, 212, 255},
	{191, 211, 255},
	{191, 211, 255},
	{190, 211, 255},
	{190, 210, 255},
	{189, 210, 255},
	{189, 210, 255},
	{188, 210, 255},
	{188, 209, 255},
	{187, 209, 255},
	{187, 209, 255},
	{186, 208, 255},
	{186, 208, 255},
	{185, 208, 255},
	{185, 208, 255},
	{185, 207, 255},
	{184, 207, 255},
	{184, 207, 255},
	{183, 207, 255},
	{183, 206, 255},
	{183, 206, 255},
	{182, 206, 255},
	{182, 206, 255},
	{182, 205, 255},
	{181, 205, 255},
	{181, 205, 255},
	{181, 205, 255},
	{180, 205, 255},
	{180, 204, 255},
	{180, 204, 255},
	{179, 204, 255},
	{179, 204, 255},
	{179, 204, 255},
	{178, 203, 255},
	{178, 203, 255},
	{178, 203, 255},
	{178, 203, 255},
	{177, 203, 255},
	{177, 202, 255},
	{177, 202, 255},
	{177, 202, 255},
	{176, 202, 255},
	{176, 202, 255},
	{176, 202, 255},
	{175, 201, 255},
	{175, 201, 255},
	{175, 201, 255},
	{175, 201, 255},
	{175, 201, 255},
	{174, 201, 255},
	{174, 201, 255},
	{174, 200, 255},
	{174, 200, 255},
	{173, 200, 255},
	{173, 200, 255},
	{173, 200, 255},
	{173, 200, 255},
	{173, 200, 255},
	{172, 199, 255},
	{172, 199, 255},
	{172, 199, 255},
	{172, 199, 255},
	{172, 199, 255},
	{171, 199, 255},
	{171, 199, 255},
	{171, 199, 255},
	{171, 198, 255},
	{171, 198, 255},
	{170, 198, 255},
	{170, 198, 255},
	{170, 198, 255},
	{170, 198, 255},
	{170, 198, 255},
	{170, 198, 255},
	{169, 198, 255},
	{169, 197, 255},
	{169, 197, 255},
	{169, 197, 255},
	{169, 197, 255},
	{169, 197, 255},
	{169, 197, 255},
	{168, 197, 255},
	{168, 197, 255},
	{168, 197, 255},
	{168, 197, 255},
	{168, 196, 255},
	{168, 196, 255},
	{168, 196, 255},
	{167, 196, 255},
	{167, 196, 255},
	{167, 196, 255},
	{167, 196, 255},
	{167, 196, 255},
	{167, 196, 255},
	{167, 196, 255},
	{166, 196, 255},
	{166, 195, 255},
	{166, 195, 255},
	{166, 195, 255},
	{166, 195, 255},
	{166, 195, 255},
	{166, 195, 255},
	{166, 195, 255},
	{165, 195, 255},
	{165, 195, 255},
	{165, 195, 255},
	{165, 195, 255},
	{165, 195, 255},
	{165, 195, 255},
	{165, 194, 255},
	{165, 194, 255},
	{165, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{164, 194, 255},
	{163, 194, 255},
	{163, 194, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{163, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 193, 255},
	{162, 192, 255},
	{162, 192, 255},
	{162, 192, 255},
	{162, 192, 255},
	{162, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{161, 192, 255},
	{160, 192, 255},
	{160, 192, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{160, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 191, 255},
	{159, 190, 255},
	{159, 190, 255},
	{159, 190, 255},
	{159, 190, 255},
	{159, 190, 255},
	{159, 190, 255},
	{159, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{158, 190, 255},
	{157, 190, 255},
	{157, 190, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{157, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 189, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{156, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255},
	{155, 188, 255} // 40000K
};

uint16_t
temp2ci(int temp)
{
    int index = temp / 100 - 10;
	
    return index;
}


double
bv_to_temp(double bv)
{    
    double temp = 1000.0+(5000.0/(bv+0.5));
    
    return temp;
}

uint8_t
*get_temp_colour(int temp)
{
	if (temp < 1000 || temp > 40000) {
		return NULL;
	}
	
	int index = (temp-1000) / 100;
	
	return gCOLOUR_temperature_tbl[index];
}

