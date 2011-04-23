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

#include <string.h>
#include <assert.h>

#include "rendering/scenegraph-private.h"
#include "drawable.h"
#include "common/lwcoord.h"
#include "parsers/model.h"
#include "res-manager.h"
#include "rendering/texture.h"
#include "log.h"
#include "shader-manager.h"

void
sgInitDrawable(SGdrawable *obj)
{
  obj->tex_id[0] = -1;
  obj->tex_id[1] = -1;
  obj->tex_id[2] = -1;
  obj->tex_id[3] = -1;

  obj->shader = 0;

  obj->tex_uni_id[0] = -1;
  obj->tex_uni_id[1] = -1;
  obj->tex_uni_id[2] = -1;
  obj->tex_uni_id[3] = -1;
}

void
sgDrawableLoadShader(SGdrawable *obj, const char *shader)
{
  obj->shader = sgLoadProgram(shader, shader, shader, shader);
  obj->modelview_id = glGetUniformLocation(obj->shader,
                                          "ModelViewMatrix");
  obj->projection_id = glGetUniformLocation(obj->shader,
                                           "ProjectionMatrix");
  obj->tex_uni_id[0] = glGetUniformLocation(obj->shader,
                                     "Tex0");
  obj->tex_uni_id[1] = glGetUniformLocation(obj->shader,
                                        "Tex1");
  obj->tex_uni_id[2] = glGetUniformLocation(obj->shader,
                                        "Tex2");
  obj->tex_uni_id[3] = glGetUniformLocation(obj->shader,
                                        "Tex3");
}

void
sgSetObjectPosLWAndOffset(SGdrawable *obj, const OOlwcoord *lw, float3 offset)
{
  assert(obj != NULL);
  assert(lw != NULL);
  // Get camera position and translate the lw coord with respect to the camera
  SGscene *sc = obj->scene;
  SGscenegraph *sg = sc->sg;
  SGcam *cam = sg->currentCam;

  if (cam->kind == SGCam_Free) {
    float3 relPos = ooLwcDist(lw, &((SGfreecam*)cam)->lwc);
    obj->p = relPos + offset;
  } else if (cam->kind == SGCam_Fixed) {
    SGfixedcam *fix = (SGfixedcam*)cam;
    float3 relPos = ooLwcDist(lw, &fix->body->p) - (mf3_v_mul(fix->body->R, fix->r));
    obj->p = relPos + offset;
  } else if (cam->kind == SGCam_Orbit) {
    SGorbitcam *orb = (SGorbitcam*)cam;
    float3 relPos = ooLwcDist(lw, &orb->body->p);
    obj->p = relPos + offset;
  }
}

void
sgSetObjectPosLW(SGdrawable *obj, const OOlwcoord *lw)
{
  // Get camera position and translate the lw coord with respect to the camera
  SGscene *sc = obj->scene;
  SGscenegraph *sg = sc->sg;
  SGcam *cam = sg->currentCam;

  float3 relPos;
  if (cam->kind == SGCam_Free) {
    relPos = ooLwcDist(lw, &((SGfreecam*)cam)->lwc);
  } else if (cam->kind == SGCam_Fixed) {
    SGfixedcam *fix = (SGfixedcam*)cam;
    relPos = ooLwcDist(lw, &fix->body->p) - (mf3_v_mul(fix->body->R, fix->r));
  } else if (cam->kind == SGCam_Orbit) {
    SGorbitcam *orb = (SGorbitcam*)cam;
    relPos = ooLwcDist(lw, &orb->body->p);
  } else {
    assert(0 && "invalid camera type");
  }
  obj->p = relPos;
}
void
sgSetObjectPos3f(SGdrawable *obj, float x, float y, float z)
{
  assert(obj != NULL);
  obj->p = vf3_set(x, y, z);
}

void
sgSetObjectQuat4f(SGdrawable *obj, float x, float y, float z, float w)
{
  assert(obj != NULL);
  obj->q = vf4_set(x, y, z, w);

  float3x3 m, mt;
  q_mf3_convert(m, obj->q);
  mf3_transpose2(mt, m); // Transpose matrix for OpenGL

  bzero(obj->R, sizeof(GLfloat[16]));

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      obj->R[i*4+j] = mt[i][j];
    }
  }
  obj->R[15] = 1.0f;
}

void
sgSetObjectQuatv(SGdrawable *obj, quaternion_t q)
{
  assert(obj != NULL);
  obj->q = q;

  float3x3 m, mt;
  q_mf3_convert(m, q);
  mf3_transpose2(mt, m); // Transpose matrix for OpenGL

  bzero(obj->R, sizeof(GLfloat[16]));

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      obj->R[i*4+j] = mt[i][j];
    }
  }
  obj->R[15] = 1.0f;
}

void
sgSetObjectScale(SGdrawable *obj, float s)
{
  assert(0);
  assert(obj != NULL);
  //  obj->s = s;
}

void
sgSetObjectSpeed(SGdrawable *obj, float dx, float dy, float dz)
{
  assert(obj != NULL);
  obj->dp = vf3_set(dx, dy, dz);
}

void
sgSetObjectAngularSpeed(SGdrawable *obj, float drx, float dry, float drz)
{
  assert(obj != NULL);
  obj->dr = vf3_set(drx, dry, drz);
}

SGdrawable*
sgNewDrawable(SGdrawable *drawable, const char *name, SGdrawfunc df)
{
  assert(df != NULL);
  assert(drawable != NULL);

  drawable->name = strdup(name);
  drawable->p = vf3_set(0.0f, 0.0f, 0.0f);
  drawable->q = q_rot(1.0f, 0.0f, 0.0f, 0.0f);
  drawable->dr = vf3_set(0.0f, 0.0f, 0.0f);
  drawable->dp = vf3_set(0.0f, 0.0f, 0.0f);

  bzero(drawable->R, sizeof(GLfloat[16]));
  drawable->R[0] = 1.0f;
  drawable->R[5] = 1.0f;
  drawable->R[10] = 1.0f;
  drawable->R[15] = 1.0f;
  drawable->draw = df;

  drawable->scene = NULL;
  drawable->enabled = true;

  LIST_FIRST(drawable->children) = NULL;
  LIST_LAST(drawable->children) = NULL;
  drawable->siblings.next = NULL;
  drawable->siblings.prev = NULL;
  drawable->parent = NULL;

  return drawable;
}

void
sgDrawSphere(SGsphere *sp)
{
  SG_CHECK_ERROR;

  glEnable(GL_TEXTURE_2D);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  sgBindMaterial(&sp->mat);
//  glBindTexture(GL_TEXTURE_2D, sp->super->texId);

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glColor3f(1.0f, 1.0f, 1.0f);

  SG_CHECK_ERROR;

  // Primitive LOD, note that distances may be messed up for smaller objects, as
  // this does not take the camera position into account.
  double angularDiameter = 2.0 * atan(0.5*sp->radius/vf3_abs(sp->super.p));
  //double angularDiameter = sp->radius/vf3_abs(sp->super.p);// Quicker estimate
  if (angularDiameter > DEG_TO_RAD(5.0)) {
    gluSphere(sp->quadratic, sp->radius, 128, 128);
  } else if (angularDiameter > DEG_TO_RAD(1.0)) {
    gluSphere(sp->quadratic, sp->radius, 32, 32);
  } else {
    gluSphere(sp->quadratic, sp->radius, 16, 16);
  }

  SG_CHECK_ERROR;

  // Draw point on the sphere in solid colour and size
  glDisable (GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D); // Lines are not textured...

  SG_CHECK_ERROR;

  glBegin(GL_LINES);
  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, -2.0*sp->radius);
  glVertex3f(0.0, 0.0, -sp->radius);
  glEnd();

  SG_CHECK_ERROR;


  glBegin(GL_LINES);
  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, +2.0*sp->radius);
  glVertex3f(0.0, 0.0, sp->radius);
  glEnd();
  SG_CHECK_ERROR;

  glPointSize(5.0);
  glBegin(GL_POINTS);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glEnd();
  SG_CHECK_ERROR;
}

SGdrawable*
sgNewSphere(const char *name, float radius, const char *tex)
{
  SGsphere *sp = malloc(sizeof(SGsphere));
  sgInitDrawable(&sp->super);
  sgInitMaterial(&sp->mat);

  sp->radius = radius;
  sp->super.tex_id[0] = ooTexLoad(tex, tex);

  sp->quadratic = gluNewQuadric();
  gluQuadricOrientation(sp->quadratic, GLU_OUTSIDE);
  gluQuadricNormals(sp->quadratic, GLU_SMOOTH);
  gluQuadricTexture(sp->quadratic, GL_TRUE);
  gluQuadricDrawStyle(sp->quadratic, GLU_FILL);

  return sgNewDrawable((SGdrawable*)sp, name, (SGdrawfunc) sgDrawSphere);
}


SGdrawable*
sgNewSphere2(const char *name, const char *tex, float radius, float flattening, int stacks, int slices)
{
  SGsphere *sp = malloc(sizeof(SGsphere));
  sgInitDrawable(&sp->super);
  sgInitMaterial(&sp->mat);

  sp->radius = radius;
  sp->super.tex_id[0] = ooTexLoad(tex, tex);

  stacks = stacks & ~1; // Ensure we have an even number of stacks

  // For the triangle strips and the poles
  size_t verticesPerStack = 4+slices*2;
  size_t verticesPerPole = slices + 2;
  size_t vertexCount = (stacks-2) * verticesPerStack + 2 * verticesPerPole;

  sp->northPoleIdx = 0;
  sp->northPoleCount = verticesPerPole;
  sp->southPoleIdx = sp->northPoleCount;
  sp->southPoleCount = verticesPerPole;
  sp->sliceCount = slices;
  sp->stackCount = stacks;
  sp->stripIdx = sp->southPoleIdx + sp->southPoleCount;
  sp->vertices = calloc(vertexCount*3, sizeof(GLfloat));
  sp->normals = calloc(vertexCount*3, sizeof(GLfloat));
  sp->texCoords = calloc(vertexCount*2, sizeof(GLfloat));

  double radsPerSlice = 2.0*M_PI / slices;
  double radsPerStack = M_PI / stacks;

  //x = r * sin lat cos lon
  //y = r * sin lat sin lon
  //z = r * cos lat

  // Create the poles (triangle fans)
  sp->vertices[0] = 0.0;
  sp->vertices[1] = 0.0;
  sp->vertices[2] = radius;
  sp->normals[0] = 0.0;
  sp->normals[1] = 0.0;
  sp->normals[2] = 1.0;

  for (int i = 1 ; i < verticesPerPole ; ++i) {
    double x = radius * sin((double)(stacks/2-1)*radsPerStack) * cos((double)i * radsPerSlice);
    double y = radius * sin((double)(stacks/2-1)*radsPerStack) * sin((double)i * radsPerSlice);
    double z = radius * cos((double)(stacks/2-1)*radsPerStack);
    sp->vertices[i*3+0] = x;
    sp->vertices[i*3+1] = y;
    sp->vertices[i*3+2] = z;
    sp->normals[i*3+0] = x/radius;
    sp->normals[i*3+1] = y/radius;
    sp->normals[i*3+2] = z/radius;
  }

  // Make second pole, opposite of first one... we may need to reverse the vertices?
  for (int i = 0 ; i < verticesPerPole ; ++i) {
    sp->vertices[sp->stripIdx*3+0] = -sp->vertices[i+0];
    sp->vertices[sp->stripIdx*3+1] = -sp->vertices[i+1];
    sp->vertices[sp->stripIdx*3+2] = -sp->vertices[i+2];
    sp->normals[sp->stripIdx*3+0] = -sp->normals[i+0];
    sp->normals[sp->stripIdx*3+1] = -sp->normals[i+1];
    sp->normals[sp->stripIdx*3+2] = -sp->normals[i+2];
  }

  // Now create all the triangle strips needed
  for (int i = 0 ; i < stacks/2 ; ++i) {
//    double x0 = radius * sin((double)i*radsPerStack) * cos((double)j * radsPerSlice);
//    double y0 = radius * sin((double)i*radsPerStack) * sin((double)j * radsPerSlice);
//    double z0 = radius * cos((double)i*radsPerStack);

//    double x1 = radius * sin((double)i*radsPerStack) * cos((double)j * radsPerSlice);
//    double y1 = radius * sin((double)i*radsPerStack) * sin((double)j * radsPerSlice);
//    double z1 = radius * cos((double)i*radsPerStack);

    for (int j = 0 ; i < slices ; ++j) {
      double x0 = radius * sin((double)i*radsPerStack) * cos((double)j * radsPerSlice);
      double y0 = radius * sin((double)i*radsPerStack) * sin((double)j * radsPerSlice);
      double z0 = radius * cos((double)i*radsPerStack);

      double x1 = radius * sin((double)i*radsPerStack) * cos((double)j * radsPerSlice);
      double y1 = radius * sin((double)i*radsPerStack) * sin((double)j * radsPerSlice);
      double z1 = radius * cos((double)i*radsPerStack);
    }
  }

  return sgNewDrawable((SGdrawable*)sp, name, (SGdrawfunc) sgDrawSphere);
}

void
sgDrawSphere2(SGsphere *sp)
{
  //glEnable(GL_TEXTURE_2D); // Lines are not textured...
  //glBindTexture(GL_TEXTURE_2D, sp->texId);
  glEnable(GL_LIGHTING); // Lines are not lit, just colored
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, &sp->vertices[sp->northPoleIdx]);
  glNormalPointer(GL_FLOAT, 0, &sp->normals[sp->northPoleIdx]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, sp->sliceCount+1);

  glVertexPointer(3, GL_FLOAT, 0, &sp->vertices[sp->southPoleIdx]);
  glNormalPointer(GL_FLOAT, 0, &sp->normals[sp->southPoleIdx]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, sp->sliceCount+1);

  for (int i = 0 ; i < sp->stackCount - 2 ; ++ i) {
    glVertexPointer(3, GL_FLOAT, 0, &sp->vertices[sp->stripIdx + i * sp->sliceCount * 2]);
    glNormalPointer(GL_FLOAT, 0, &sp->normals[sp->southPoleIdx]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sp->sliceCount * 2);
  }

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

}

SGmaterial*
sgSphereGetMaterial(SGsphere *sphere)
{
  return &sphere->mat;
}



void
sgDrawEllipsis(SGellipsis *el)
{
  SG_CHECK_ERROR;

  // Rest may be needed later...
  //glDisable (GL_BLEND);
  //glDisable (GL_DITHER);
  //glDisable (GL_FOG);
  //glDisable (GL_LIGHTING);
  //glDisable (GL_TEXTURE_1D);
  //glDisable (GL_TEXTURE_3D);
  //glShadeModel (GL_FLAT);

  glDisable(GL_TEXTURE_2D); // Lines are not textured...
  glDisable(GL_LIGHTING); // Lines are not lit, just colored
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glPushMatrix();
  glLineWidth(1.0);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, el->verts);
  glColor3f(el->colour[0], el->colour[1], el->colour[2]);
  glDrawArrays(GL_LINE_LOOP, 0, el->vertCount);
  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();

  // Place dot at periapsis
  glPointSize(10.0);
  glBegin(GL_POINTS);
  glColor3f(1.0-el->colour[0], 1.0-el->colour[1], 1.0-el->colour[2]);
  glVertex3f(el->verts[0], el->verts[1], el->verts[2]);
  glEnd();

  SG_CHECK_ERROR;
}

SGdrawable*
sgNewEllipsis(const char *name,
              double semiMajor, double semiMinor,
              double longAsc, double inc, double argPeri,
              float r, float g, float b,
              size_t vertCount)
{
  SG_CHECK_ERROR;

  SGellipsis *el = malloc(sizeof(SGellipsis) + vertCount * 3 * sizeof(float));
  sgInitDrawable(&el->super);
  el->semiMajor = semiMajor;
  el->semiMinor = semiMinor;
  el->ecc = sqrt((semiMajor * semiMajor - semiMinor * semiMinor)
                 / (semiMajor * semiMajor));
  el->colour[0] = r;
  el->colour[1] = g;
  el->colour[2] = b;
  el->vertCount = vertCount;
  ooLogInfo("new ellipses '%s' %f %f %f", name, semiMajor, semiMinor, el->ecc);

  // Build an ellipse with the requested number of vertices
  double w = 2.0 * M_PI / (double)vertCount;
  for (int i = 0 ; i < vertCount ; ++ i) {
    double x = -semiMinor * sin(w * (double)i); // Note, when y is semi major axis, then x is pointing downwards
    double y = semiMajor * cos(w * (double)i) - el->semiMajor*el->ecc;
    double z = 0.0;

    float3 v = vf3_set(x, y, z);
    quaternion_t qasc = q_rot(0.0, 0.0, 1.0, DEG_TO_RAD(longAsc));
    quaternion_t qinc = q_rot(0.0, 1.0, 0.0, DEG_TO_RAD(inc));
    quaternion_t qaps = q_rot(0.0, 0.0, 1.0, DEG_TO_RAD(argPeri));

    quaternion_t q = q_mul(qasc, qinc);
    q = q_mul(q, qaps);

    matrix_t m;
    q_m_convert(&m, q);

    // rotate our point
    v = m_v3_mulf(&m, v);

    el->verts[3*i] = vf3_x(v);
    el->verts[3*i+1] = vf3_y(v);
    el->verts[3*i+2] = vf3_z(v);
  }

  return sgNewDrawable((SGdrawable*)el, name, (SGdrawfunc)sgDrawEllipsis);
}

void
sgDrawCylinder(SGcylinder *cyl)
{
  SG_CHECK_ERROR;

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glColor3bv(cyl->col);
  gluCylinder(cyl->quadratic,
              cyl->bottonRadius, cyl->topRadius, cyl->height,
              4, 2);
  SG_CHECK_ERROR;
}

SGdrawable*
sgNewCylinder(const char *name,
              float top, float bottom, float height)
{
  SGcylinder *cyl = malloc(sizeof(SGcylinder));
  sgInitDrawable(&cyl->super);

  cyl->height = height;
  cyl->bottonRadius = bottom;
  cyl->topRadius = top;

  cyl->col[0] = 255;
  cyl->col[1] = 255;
  cyl->col[2] = 255;

  return sgNewDrawable((SGdrawable*)cyl, name, (SGdrawfunc)sgDrawCylinder);
}


static void
setMaterial(material_t *mat)
{
  SG_CHECK_ERROR;

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat->ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat->diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat->specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat->emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat->shininess);

  SG_CHECK_ERROR;
}

void
drawModel(model_object_t *model)
{
  SG_CHECK_ERROR;

  glColor3f(1.0, 1.0, 1.0);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
  glShadeModel(GL_SMOOTH);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
  setMaterial(model->model->materials[model->materialId]);

  glPushMatrix();

  glTranslatef(model->trans[0], model->trans[1], model->trans[2]);
  glMultMatrixf((GLfloat*)model->rot);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glVertexPointer(3, GL_FLOAT, 0, model->vertices.elems);
  glTexCoordPointer(2, GL_FLOAT, 0, model->texCoords.elems);
  glNormalPointer(GL_FLOAT, 0, model->normals.elems);

  glDrawArrays(GL_TRIANGLES, 0, model->vertexCount);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

#if 0
  // DEBUG: Draw normals, do not delete
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  int vertices = model->vertexCount;
  for (int i = 0 ; i < vertices ; ++ i) {
    glVertex3fv(&model->vertices.elems[i*3]);
    glVertex3f(model->vertices.elems[i*3 + 0] + model->normals.elems[i*3 + 0],
               model->vertices.elems[i*3 + 1] + model->normals.elems[i*3 + 1],
               model->vertices.elems[i*3 + 2] + model->normals.elems[i*3 + 2]);
  }
  glEnd();

  glPointSize(2.0);
  glBegin(GL_POINTS);
  glColor3f(1.0, 1.0, 0.0);
  for (int i = 0 ; i < vertices ; ++ i) {
    glVertex3fv(&model->vertices.elems[i*3]);
  }
  glEnd();

  glEnable(GL_BLEND);
  glEnable(GL_LIGHTING);
#endif

  for (int i = 0 ; i < model->children.length ; ++ i) {
    drawModel(model->children.elems[i]);
  }

  glPopMatrix();
  SG_CHECK_ERROR;
}

void
sgDrawModel(SGmodel *model)
{
  SG_CHECK_ERROR;

  glEnable(GL_LIGHTING);
  drawModel(model->modelData->objs.elems[0]);

  SG_CHECK_ERROR;
}

SGdrawable*
sgLoadModel(const char *file)
{
  char *path = ooResGetPath(file);

  SGmodel *model = malloc(sizeof(SGmodel));
  sgInitDrawable(&model->super);

  model->modelData = model_load(path);
  free(path);
  if (model->modelData == NULL) {
    ooLogError("loading model '%s', returned NULL model data", file);
    free(model);
    return NULL;
  }

  return sgNewDrawable((SGdrawable*)model, "unnamed",
                       (SGdrawfunc)sgDrawModel);
}

void
sgDrawableAddChild(SGdrawable * restrict parent, SGdrawable * restrict child,
                   float3 t, quaternion_t q)
{
  LIST_APPEND(parent->children, child, siblings);

  child->p = t;
  child->q = q;
  child->parent = parent;

  float3x3 m, mt;
  q_mf3_convert(m, q);
  mf3_transpose2(mt, m); // Transpose matrix for OpenGL

  bzero(child->R, sizeof(GLfloat[16]));

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      child->R[i*4+j] = mt[i][j];
    }
  }
  child->R[15] = 1.0f;
}
void
sgDrawableLiberate(SGdrawable *drawable)
{
  assert(drawable->parent != NULL); // Makes no sense to liberate roots
}

void
sgPaintDrawable(SGdrawable *drawable)
{
  SG_CHECK_ERROR;
  glUseProgram(drawable->shader);
  glPushMatrix();
  glTranslatef(drawable->p.x, drawable->p.y, drawable->p.z);
  glMultMatrixf(drawable->R);

  SG_CHECK_ERROR;

  if (drawable->shader) {
    GLfloat modelview[16], projection[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    glGetFloatv(GL_PROJECTION_MATRIX, projection);

    glUniformMatrix4fv(drawable->modelview_id, 1, GL_FALSE, modelview);
    glUniformMatrix4fv(drawable->projection_id, 1, GL_FALSE, projection);

    for (int i = 0 ; i < 4 ; i ++) {
      if (drawable->tex_id[i] >= 0) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, drawable->tex_id[i]);
        glUniform1i(drawable->tex_uni_id[i], i);
      }
    }

    GLint shaderIsValid;
    glValidateProgram(drawable->shader);
    glGetProgramiv(drawable->shader, GL_VALIDATE_STATUS, &shaderIsValid);
    assert(shaderIsValid);

    SG_CHECK_ERROR;
  }

  //  glPushAttrib(GL_ENABLE_BIT);
  drawable->draw(drawable);
//  glPopAttrib();
  SG_CHECK_ERROR;

  LIST_FOREACH(SGdrawable, child, drawable->children, siblings) {
    sgPaintDrawable(child);
  }

  glPopMatrix();


  glUseProgram(0);

  SG_CHECK_ERROR;
}

void
sgDrawVector(SGvector *vec)
{
  SG_CHECK_ERROR;

  glDisable(GL_TEXTURE_2D); // Lines are not textured...
  glDisable(GL_LIGHTING); // Lines are not lit, just colored
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  glLineWidth(1.0);

  glColor3f(vec->col[0], vec->col[1], vec->col[2]);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(vec->v.x, vec->v.y, vec->v.z);
  glEnd();

  SG_CHECK_ERROR;
}

SGdrawable*
sgNewVector(const char *name, float3 vec, float r, float g, float b)
{
  SGvector *v = malloc(sizeof(SGvector));
  v->v = vec;
  v->col[0] = r;
  v->col[1] = g;
  v->col[2] = b;

  return sgNewDrawable((SGdrawable*)v, name, (SGdrawfunc)sgDrawVector);
}

void
sgDrawLabel(SGlabel *label)
{

}

SGdrawable*
sgNewLabel(const char *name, const char *str)
{
  SGlabel *lab = malloc(sizeof(SGlabel));
  sgInitDrawable(&lab->super);

  strncpy(lab->buff, str, SG_LABEL_LEN);
  lab->buff[SG_LABEL_LEN-1] = '\0';
  return sgNewDrawable((SGdrawable*)lab, name, (SGdrawfunc)sgDrawLabel);
}

void
sgPrintLabel(SGlabel *label, const char *str)
{
  strncpy(label->buff, str, SG_LABEL_LEN);
  label->buff[SG_LABEL_LEN-1] = '\0';
}


SGdrawable*
sgNewVectorWithLabel(const char *name, float3 v, float r, float g, float b)
{
//  SGdrawable *vec = sgNewVector(name, v, r, g, b);

//  return sgDrawableAddChild(vec, label, v, q_rot(1.0, 0.0, 0.0, 0.0));
  return NULL;
}


void
sgDrawEllipsoid(SGellipsoid *el)
{
  SG_CHECK_ERROR;

  // Rest may be needed later...
  //glDisable (GL_BLEND);
  //glDisable (GL_DITHER);
  //glDisable (GL_FOG);
  //glShadeModel (GL_FLAT);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING); // Fully lit, i.e. just colored
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glPushMatrix();
  glColor4fv(el->colour);

  glEnableClientState(GL_VERTEX_ARRAY);

  //glPrimitiveRestartIndex(0xffffffff)
  //glDrawElements(	GL_TRIANGLE_STRIP, el->vertexCount, GL_UINT, el->indices);

  // TODO: glMultiDrawArrays
  for (unsigned i = 0 ; i < el->stacks ; ++i) {
    glVertexPointer(3, GL_FLOAT, 0, &el->verts[el->vertCountPerStack*i]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, el->vertCountPerStack);
  }

  glVertexPointer(3, GL_FLOAT, 0, &el->verts[el->vertCountPerStack*el->stacks]);
  glDrawArrays(GL_TRIANGLE_FAN, 0, el->slices + 1);

  glDisableClientState(GL_VERTEX_ARRAY);
  glPopMatrix();

  SG_CHECK_ERROR;
}


SGdrawable*
sgNewEllipsoid(const char *name, float a, float b, float c,
               float red, float green, float blue, float alpha,
               unsigned slices, unsigned stacks)
{
  unsigned vertCount = slices * stacks * 2 + 2 + slices + 1;
  SGellipsoid *el = malloc(sizeof(SGellipsoid) + vertCount * 3 * sizeof(float));
  sgInitDrawable(&el->super);

  // glGenVertexArrays
  // glBindVertexArray
  // glBufferData(GL_ARRAY_BUFFER, vertcount*sizeof(float)*3, vertices, GL_STATIC_DRAW)
  el->a = a;
  el->b = b;
  el->c = c;

  el->colour[0] = red;
  el->colour[1] = green;
  el->colour[2] = blue;
  el->colour[3] = alpha;
  el->stacks = stacks;
  el->slices = slices;
  el->vertCountPerStack = (vertCount - (slices + 1)) / stacks;
  el->triCountPerStack = el->vertCountPerStack - 2;

  // Build ellipsoid vertices
  for (unsigned i = 0 ; i < el->stacks ; ++i) {
    float x, y, z;

    // Initiate the first two vertices in the first triangle, after this the loop
    // creates one vertex per triangle.
    x = a * cos(0*DEG_TO_RAD(90.0)/slices) * cos(i*DEG_TO_RAD(360.0)/el->stacks);
    y = b * cos(0*DEG_TO_RAD(90.0)/slices) * sin(i*DEG_TO_RAD(360.0)/el->stacks);
    z = c * sin(0*DEG_TO_RAD(90.0)/slices);
    el->verts[3*(i*el->vertCountPerStack + 2) + 0] = x;
    el->verts[3*(i*el->vertCountPerStack + 2) + 1] = y;
    el->verts[3*(i*el->vertCountPerStack + 2) + 2] = z;

    x = a * cos(0*DEG_TO_RAD(90.0)/slices) * cos(i*DEG_TO_RAD(360.0)/el->stacks);
    y = b * cos(0*DEG_TO_RAD(90.0)/slices) * sin(i*DEG_TO_RAD(360.0)/el->stacks);
    z = c * sin(0*DEG_TO_RAD(90.0)/slices);

    el->verts[3*(i*el->vertCountPerStack + 2) + 3] = x;
    el->verts[3*(i*el->vertCountPerStack + 2) + 4] = y;
    el->verts[3*(i*el->vertCountPerStack + 2) + 5] = z;

    for (unsigned j = 0 ; j < el->triCountPerStack ; ++j) {
      x = a * cos(j*DEG_TO_RAD(90.0)/slices) * cos(i*DEG_TO_RAD(360.0)/el->stacks);
      y = b * cos(j*DEG_TO_RAD(90.0)/slices) * sin(i*DEG_TO_RAD(360.0)/el->stacks);
      z = c * sin(j*DEG_TO_RAD(90.0)/slices);
      el->verts[3*(i*el->vertCountPerStack + j + 2) + 0] = x;
      el->verts[3*(i*el->vertCountPerStack + j + 2) + 1] = y;
      el->verts[3*(i*el->vertCountPerStack + j + 2) + 2] = z;
    }
  }

  return sgNewDrawable((SGdrawable*)el, name, (SGdrawfunc)sgDrawEllipsoid);
}

