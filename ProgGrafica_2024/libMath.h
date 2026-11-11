#pragma once
#include <cmath>
#include <iostream>

using namespace std;

// ============================================================
//  VECTOR 4D
// ============================================================
typedef struct vec4float {
    union {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };   // alias color
        float v[4];
    };
} vec4float;

inline vec4float make_vector4f(float x, float y, float z, float w) {
    vec4float r{};
    r.x = x; r.y = y; r.z = z; r.w = w;
    return r;
}

inline vec4float normalize(vec4float a) {
    float len = sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
    if (len == 0.0f || len == 1.0f) return a;
    a.x /= len; a.y /= len; a.z /= len;
    return a;
}

inline float length(const vec4float& a) {
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline vec4float operator+(const vec4float& a, const vec4float& b) {
    return make_vector4f(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
}
inline vec4float operator-(const vec4float& a, const vec4float& b) {
    return make_vector4f(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
}
inline vec4float operator*(const vec4float& a, float b) {
    return make_vector4f(a.x*b, a.y*b, a.z*b, a.w*b);
}
inline vec4float operator*(float b, const vec4float& a) { return a * b; }

inline vec4float operator/(const vec4float& a, float b) {
    if (b == 0.0f) {
        return a;
    }

    return make_vector4f(
        a.x / b,
        a.y / b,
        a.z / b,
        a.w / b
    );
}

inline float dot(const vec4float& a, const vec4float& b) {
    return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}
inline vec4float cross(const vec4float& a, const vec4float& b) {
    return make_vector4f(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x,
        0.0f
    );
}

// ============================================================
//  MATRIZ 4x4
// ============================================================
typedef struct matrix4x4f {
    union {
        float mat[16];
        vec4float matV[4];
    };
} matrix4x4f;

inline float get(const matrix4x4f& m, int row, int col) {
    return m.mat[row*4 + col];
}
inline void set(matrix4x4f& m, int row, int col, float value) {
    m.mat[row*4 + col] = value;
}

inline matrix4x4f make_identityf() {
    matrix4x4f m{};
    for (int i = 0; i < 4; ++i) set(m, i, i, 1.0f);
    return m;
}

inline matrix4x4f make_translate(float X, float Y, float Z) {
    matrix4x4f m = make_identityf();
    set(m, 0, 3, X); set(m, 1, 3, Y); set(m, 2, 3, Z);
    return m;
}

inline matrix4x4f make_scale(float X, float Y, float Z) {
    matrix4x4f m = make_identityf();
    set(m, 0, 0, X); set(m, 1, 1, Y); set(m, 2, 2, Z);
    return m;
}

inline matrix4x4f operator+(const matrix4x4f& a, const matrix4x4f& b) {
    matrix4x4f r{};
    for (int i = 0; i < 16; ++i) r.mat[i] = a.mat[i] + b.mat[i];
    return r;
}
inline matrix4x4f operator-(const matrix4x4f& a, const matrix4x4f& b) {
    matrix4x4f r{};
    for (int i = 0; i < 16; ++i) r.mat[i] = a.mat[i] - b.mat[i];
    return r;
}
inline matrix4x4f operator*(const matrix4x4f& a, float s) {
    matrix4x4f r{};
    for (int i = 0; i < 16; ++i) r.mat[i] = a.mat[i] * s;
    return r;
}
inline matrix4x4f operator*(const matrix4x4f& A, const matrix4x4f& B) {
    matrix4x4f C{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k) sum += get(A,r,k)*get(B,k,c);
            set(C, r, c, sum);
        }
    return C;
}
inline vec4float operator*(const matrix4x4f& A, const vec4float& v) {
    return make_vector4f(
        get(A,0,0)*v.x + get(A,0,1)*v.y + get(A,0,2)*v.z + get(A,0,3)*v.w,
        get(A,1,0)*v.x + get(A,1,1)*v.y + get(A,1,2)*v.z + get(A,1,3)*v.w,
        get(A,2,0)*v.x + get(A,2,1)*v.y + get(A,2,2)*v.z + get(A,2,3)*v.w,
        get(A,3,0)*v.x + get(A,3,1)*v.y + get(A,3,2)*v.z + get(A,3,3)*v.w
    );
}

inline matrix4x4f transpose(const matrix4x4f A) {
    matrix4x4f T{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            T.mat[r*4+c] = A.mat[c*4+r];
    return T;
}

inline matrix4x4f make_rotateX(float a) {
    matrix4x4f m = make_identityf();
    float c = cosf(a), s = sinf(a);
    set(m,1,1,c); set(m,1,2,-s);
    set(m,2,1,s); set(m,2,2, c);
    return m;
}
inline matrix4x4f make_rotateY(float a) {
    matrix4x4f m = make_identityf();
    float c = cosf(a), s = sinf(a);
    set(m,0,0, c); set(m,0,2,s);
    set(m,2,0,-s); set(m,2,2,c);
    return m;
}
inline matrix4x4f make_rotateZ(float a) {
    matrix4x4f m = make_identityf();
    float c = cosf(a), s = sinf(a);
    set(m,0,0,c); set(m,0,1,-s);
    set(m,1,0,s); set(m,1,1, c);
    return m;
}
inline matrix4x4f make_rotate(float angleX, float angleY, float angleZ) {
    return (make_rotateZ(angleZ) * make_rotateY(angleY)) * make_rotateX(angleX);
}

inline matrix4x4f inverse(matrix4x4f A) {
    matrix4x4f inv = make_identityf();
    for (int i = 0; i < 4; ++i) {
        float pivot = get(A, i, i);
        if (fabsf(pivot) < 1e-6f) return make_identityf(); // singular
        float invP = 1.0f / pivot;
        for (int c = 0; c < 4; ++c) {
            set(A,   i, c, get(A,   i, c) * invP);
            set(inv, i, c, get(inv, i, c) * invP);
        }
        for (int r = 0; r < 4; ++r) {
            if (r == i) continue;
            float factor = get(A, r, i);
            for (int c = 0; c < 4; ++c) {
                set(A,   r, c, get(A,   r, c) - factor * get(A,   i, c));
                set(inv, r, c, get(inv, r, c) - factor * get(inv, i, c));
            }
        }
    }
    return inv;
}

// ============================================================
//  QUATERNION
// ============================================================
inline vec4float make_quaternion(float x, float y, float z, float angle) {
    float len = sqrtf(x*x + y*y + z*z);
    if (len == 0.0f) return make_vector4f(0,0,0,1);
    x /= len; y /= len; z /= len;
    float half = angle * 0.5f;
    float s = sinf(half), c = cosf(half);
    return make_vector4f(x*s, y*s, z*s, c);
}

inline vec4float normalize_quaternion(vec4float q) {
    float len = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
    if (len == 0.0f) return make_vector4f(0,0,0,1);
    q.x/=len; q.y/=len; q.z/=len; q.w/=len;
    return q;
}

inline matrix4x4f make_rotate_quaternion(vec4float q) {
    q = normalize_quaternion(q);
    float x=q.x, y=q.y, z=q.z, w=q.w;
    matrix4x4f m = make_identityf();
    set(m,0,0, 1-2*y*y-2*z*z); set(m,0,1, 2*x*y-2*z*w); set(m,0,2, 2*x*z+2*y*w);
    set(m,1,0, 2*x*y+2*z*w);   set(m,1,1, 1-2*x*x-2*z*z); set(m,1,2, 2*y*z-2*x*w);
    set(m,2,0, 2*x*z-2*y*w);   set(m,2,1, 2*y*z+2*x*w); set(m,2,2, 1-2*x*x-2*y*y);
    return m;
}
