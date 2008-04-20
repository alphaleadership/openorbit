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

 
#include <tgmath.h>
#include <string.h>

#include <vmath/vmath-matvec.h>
#include <vmath/vmath-constants.h>

/* standard non vectorised routines */
void
m_v_mul(vec_arr_t res, mat_arr_t a, const vec_arr_t v) {
    for (int i = 0 ; i < 4 ; i ++) {
        res[i] = a[i][0] * v[0] + a[i][1] * v[1] + a[i][2] * v[2] + a[i][3] * v[3];
    }
}

void
m_mul(mat_arr_t res, mat_arr_t a, mat_arr_t b) {
    for (int i = 0 ; i < 4 ; i ++) {
        for (int j = 0 ; j < 4 ; j ++) {
            res[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j] + a[i][2]*b[2][j] + a[i][3]*b[3][j];
        }
    }
}


void
m_add(mat_arr_t res, mat_arr_t a, mat_arr_t b)
{
    for (int i = 0 ; i < 4 ; i ++) {
        for (int j = 0 ; j < 4 ; j ++) {
            res[i][j] = a[i][j] + b[i][j];
        }
    }
}

void
v_add(vec_arr_t res, vec_arr_t a, const vec_arr_t b)
{
    for (int i = 0 ; i < 4 ; i ++) {
        res[i] = a[i] + b[i];
    }
}

void
v_sub(vec_arr_t res, vec_arr_t a, const vec_arr_t b)
{
    for (int i = 0 ; i < 4 ; i ++) {
        res[i] = a[i] - b[i];
    }
}


void
v_cross(vec_arr_t res, const vec_arr_t a, const vec_arr_t b)
{
    res[0] = a[1]*b[2]-a[2]*b[1];
    res[1] = a[2]*b[0]-a[0]*b[2];
    res[2] = a[0]*b[1]-a[1]*b[0];
}

scalar_t
v_dot(const vec_arr_t a, const vec_arr_t b)
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

void
v_s_mul(vec_arr_t res, vec_arr_t a, scalar_t s)
{
    res[0] = a[0] * s;
    res[1] = a[1] * s;
    res[2] = a[2] * s;
    res[3] = a[3] * s;
}

void
v_s_div(vec_arr_t res, vec_arr_t a, scalar_t s)
{
    scalar_t d = S_CONST(1.0) / s;
    res[0] = a[0] * d;
    res[1] = a[1] * d;
    res[2] = a[2] * d;
    res[3] = a[3] * d;
}

void
m_lu(const matrix_t *a, matrix_t *l, matrix_t *u)
{
    // compute L, U in A=LU, where L, U are triangular
//    m_cpy(u, a);
//    m_zero(l);
//    for (int i = 0 ; i < 4 ; i ++) {
//        vector_t v;
//        v_s_div(&v, u.a[i], u.a[i][i]);
//        v_s_mul(&v, &v, u.a[i+1][j]);
        
//    }
    return;
}

void
v_normalise(vec_arr_t v)
{
    scalar_t norm = v_abs(v);
    v_s_mul(v, v, S_ONE/norm);
}

scalar_t
m_det(const matrix_t *m)
{
    // compute the four subdeterminants (Saurrus)
    // note, this version has been written to work, not to be efficient in any
    // way
    scalar_t sub_det[4];
    
    sub_det[0] =  MAT_ELEM(*m, 1, 1) * MAT_ELEM(*m, 2, 2) * MAT_ELEM(*m, 3, 3);
    sub_det[0] += MAT_ELEM(*m, 1, 2) * MAT_ELEM(*m, 2, 3) * MAT_ELEM(*m, 3, 1); 
    sub_det[0] += MAT_ELEM(*m, 1, 3) * MAT_ELEM(*m, 2, 1) * MAT_ELEM(*m, 3, 2); 
    sub_det[0] -= MAT_ELEM(*m, 3, 1) * MAT_ELEM(*m, 2, 2) * MAT_ELEM(*m, 1, 3); 
    sub_det[0] -= MAT_ELEM(*m, 3, 2) * MAT_ELEM(*m, 2, 3) * MAT_ELEM(*m, 1, 1); 
    sub_det[0] -= MAT_ELEM(*m, 3, 3) * MAT_ELEM(*m, 2, 1) * MAT_ELEM(*m, 1, 2); 

    sub_det[1] =  MAT_ELEM(*m, 1, 0) * MAT_ELEM(*m, 2, 2) * MAT_ELEM(*m, 3, 3); 
    sub_det[1] += MAT_ELEM(*m, 1, 2) * MAT_ELEM(*m, 2, 3) * MAT_ELEM(*m, 3, 0); 
    sub_det[1] += MAT_ELEM(*m, 1, 3) * MAT_ELEM(*m, 2, 0) * MAT_ELEM(*m, 3, 2); 
    sub_det[1] -= MAT_ELEM(*m, 3, 0) * MAT_ELEM(*m, 2, 2) * MAT_ELEM(*m, 1, 3); 
    sub_det[1] -= MAT_ELEM(*m, 3, 2) * MAT_ELEM(*m, 2, 3) * MAT_ELEM(*m, 1, 0); 
    sub_det[1] -= MAT_ELEM(*m, 3, 3) * MAT_ELEM(*m, 2, 0) * MAT_ELEM(*m, 1, 2); 
    
    sub_det[2] =  MAT_ELEM(*m, 1, 0) * MAT_ELEM(*m, 2, 1) * MAT_ELEM(*m, 3, 3); 
    sub_det[2] += MAT_ELEM(*m, 1, 1) * MAT_ELEM(*m, 2, 3) * MAT_ELEM(*m, 3, 0); 
    sub_det[2] += MAT_ELEM(*m, 1, 3) * MAT_ELEM(*m, 2, 0) * MAT_ELEM(*m, 3, 1); 
    sub_det[2] -= MAT_ELEM(*m, 3, 0) * MAT_ELEM(*m, 2, 1) * MAT_ELEM(*m, 1, 3); 
    sub_det[2] -= MAT_ELEM(*m, 3, 1) * MAT_ELEM(*m, 2, 3) * MAT_ELEM(*m, 1, 0); 
    sub_det[2] -= MAT_ELEM(*m, 3, 3) * MAT_ELEM(*m, 2, 0) * MAT_ELEM(*m, 1, 1); 
    
    sub_det[3] =  MAT_ELEM(*m, 1, 0) * MAT_ELEM(*m, 2, 1) * MAT_ELEM(*m, 3, 2); 
    sub_det[3] += MAT_ELEM(*m, 1, 1) * MAT_ELEM(*m, 2, 2) * MAT_ELEM(*m, 3, 0); 
    sub_det[3] += MAT_ELEM(*m, 1, 2) * MAT_ELEM(*m, 2, 0) * MAT_ELEM(*m, 3, 1); 
    sub_det[3] -= MAT_ELEM(*m, 3, 0) * MAT_ELEM(*m, 2, 1) * MAT_ELEM(*m, 1, 2); 
    sub_det[3] -= MAT_ELEM(*m, 3, 1) * MAT_ELEM(*m, 2, 2) * MAT_ELEM(*m, 1, 0); 
    sub_det[3] -= MAT_ELEM(*m, 3, 2) * MAT_ELEM(*m, 2, 0) * MAT_ELEM(*m, 1, 1); 
    
    scalar_t det = MAT_ELEM(*m, 0, 0) * sub_det[0]
                 - MAT_ELEM(*m, 0, 1) * sub_det[1]
                 + MAT_ELEM(*m, 0, 2) * sub_det[2]
                 - MAT_ELEM(*m, 0, 3) * sub_det[3];
    
    return det;
}

scalar_t
m_subdet3(const matrix_t *m, int k, int l)
{
    scalar_t acc = S_CONST(0.0);
    matrix_t m_prim;
    // copy the relevant matrix elements
    for (int i0 = 0, i1 = 0 ; i0 < 4 ; i0 ++) {
        if (k == i0) continue; // skip row for sub det
        for(int j0 = 0, j1 = 0 ; j0 < 4 ; j0 ++) {
            if (l == j0) continue; // skip col for subdet
            MAT_ELEM(m_prim, i1, j1) = MAT_ELEM(*m, i0, j0);
            j1 ++;
        }
        i1 ++;
    }

    
    // Apply Sarrus
    for (int i = 0 ; i < 3 ; i ++) {
        acc += MAT_ELEM(m_prim, 0, (0+i) % 3) * MAT_ELEM(m_prim, 1, (1+i) % 3)
             * MAT_ELEM(m_prim, 2, (2+i) % 3);
        
        acc -= MAT_ELEM(m_prim, 2, (0+i) % 3) * MAT_ELEM(m_prim, 1, (1+i) % 3)
             * MAT_ELEM(m_prim, 0, (2+i) % 3);
    }

    return acc;
}

matrix_t
m_adj(const matrix_t *m)
{
    matrix_t M_adj;
    scalar_t sign = S_CONST(1.0);
    for (int i = 0 ; i < 4 ; i ++) {
        for(int j = 0; j < 4 ; j ++) {
            MAT_ELEM(M_adj, j, i) = sign * m_subdet3(m, i, j);
            sign *= S_CONST(-1.0); 
        }
    }
    
    for (int i = 0; i < 4 ; i ++) {
        printf("adj: %vf\n", M_adj.v[i]);
    }
    
    return M_adj;
}

matrix_t
m_inv(const matrix_t *M)
{
    // Very brute force, there are more efficient ways to do this
    scalar_t det = m_det(M);
    matrix_t M_adj = m_adj(M);
    matrix_t M_inv;
    
    if (det != S_CONST(0.0)) {
        scalar_t sign = S_CONST(1.0);
        for (int i = 0 ; i < 4 ; i ++) {
            for (int j = 0 ; j < 4 ; j ++) {
                MAT_ELEM(M_inv, i, j) = MAT_ELEM(M_adj, i, j) / det;
            }
        }
    } else {
        // If the determinant is zero, then the matrix is not invertible
        // thus, return NANs in all positions
        for (int i = 0; i < 4; i ++) {
            for (int j = 0; j < 4; j ++) {
                MAT_ELEM(M_inv, i, j) = NAN;
            }
        }
    }
    
    for (int i = 0; i < 4 ; i ++) {
        printf("inverse: %vf\n", M_inv.v[i]);
    }
    return M_inv;
}


scalar_t
v_abs(const vec_arr_t v)
{
    return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2] + v[3]*v[3]);
}

void
m_transpose(mat_arr_t mt, mat_arr_t m)
{
    for ( unsigned int i = 0; i < 4; i += 1 ) {
        for ( unsigned int j = 0; j < 4; j += 1 ) {
            mt[j][i] = m[i][j];
        }
    }
}

void
m_axis_rot_x(mat_arr_t m, const angle_t a)
{
    scalar_t sin_a = sin(a);
    scalar_t cos_a = cos(a);
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = S_ONE;
    m[1][1] = cos_a; m[1][1] = sin_a; 
    m[2][1] = -sin_a; m[2][1] = cos_a;
    m[3][3] = S_ONE;
}

void
m_axis_rot_y(mat_arr_t m, const angle_t a)
{
    scalar_t sin_a = sin(a);
    scalar_t cos_a = cos(a);
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = cos_a; m[0][2] = -sin_a;
    m[1][1] = S_ONE;
    m[2][0] = sin_a; m[2][2] = cos_a;
    m[3][3] = S_ONE;
}

void
m_axis_rot_z(mat_arr_t m, const angle_t a)
{
    scalar_t sin_a = sin(a);
    scalar_t cos_a = cos(a);
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = cos_a; m[0][1] = sin_a;
    m[1][0] = -sin_a; m[1][1] = cos_a;
    m[2][2] = S_ONE;
    m[3][3] = S_ONE;
}


void
m_vec_rot_x(mat_arr_t m, const angle_t a)
{
    scalar_t sin_a = sin(a);
    scalar_t cos_a = cos(a);
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = S_ONE;
    m[1][1] = cos_a; m[1][1] = -sin_a; 
    m[2][1] = sin_a; m[2][1] = cos_a;
    m[3][3] = S_ONE;
}

void
m_vec_rot_y(mat_arr_t m, const angle_t a)
{
    scalar_t sin_a = sin(a);
    scalar_t cos_a = cos(a);
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = cos_a; m[0][2] = sin_a;
    m[1][1] = S_ONE;
    m[2][0] = -sin_a; m[2][2] = cos_a;
    m[3][3] = S_ONE;
}

void
m_vec_rot_z(mat_arr_t m, const angle_t a)
{
    scalar_t sin_a = sin(a);
    scalar_t cos_a = cos(a);
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = cos_a; m[0][1] = -sin_a;
    m[1][0] = sin_a; m[1][1] = cos_a;
    m[2][2] = S_ONE;
    m[3][3] = S_ONE;
}

void
m_unit(mat_arr_t m)
{
    memset(m, 0, sizeof(mat_arr_t));
    m[0][0] = S_ONE;
    m[1][1] = S_ONE;
    m[2][2] = S_ONE;
    m[3][3] = S_ONE;
}

void
m_zero(mat_arr_t m)
{
    memset(m, 0, sizeof(mat_arr_t));
}


void
v_cpy(vec_arr_t dst, const vec_arr_t src)
{
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    dst[3] = src[3];
}

void
m_cpy(mat_arr_t dst, mat_arr_t src)
{
    for (int i = 0 ; i < 4 ; i ++) {
        dst[i][0] = src[i][0];
        dst[i][1] = src[i][1];
        dst[i][2] = src[i][2];
        dst[i][3] = src[i][3];
    }
}


void
v_set(vec_arr_t v, scalar_t v0, scalar_t v1, scalar_t v2, scalar_t v3)
{
    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;    
}

bool
v_eq(const vec_arr_t a, const vec_arr_t b, scalar_t tol)
{
    for (int i = 0 ; i < 4 ; i ++) {
        if (!((a[i] <= b[i]+tol) && (a[i] >= b[i]-tol))) {
            return false;
        }
    }
    
    return true;
}

bool
m_eq(mat_arr_t a, mat_arr_t b, scalar_t tol)
{
    for (int i = 0 ; i < 4 ; i ++) {
        for (int j = 0 ; j < 4 ; j ++) {
            if (!((a[i][j] <= b[i][j]+tol) && (a[i][j] >= b[i][j]-tol))) {
                return false;
            }
        }
    }
    
    return true;    
}


void
m_translate(matrix_t *m, scalar_t x, scalar_t y, scalar_t z, scalar_t w)
{
    memset(m, 0, sizeof(mat_arr_t));    
    
    m->a[0][0] = x;
    m->a[1][1] = y;
    m->a[2][2] = z;
    m->a[3][3] = w;
}