#ifndef SOFTWARE_RENDERER_VECTORS_H
#define SOFTWARE_RENDERER_VECTORS_H
#include <cmath>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// looks like a standard implementation of vector functions.
template <class t> struct Vector2 {
    union {
        struct {t u, v;};
        struct {t x, y;};
        t raw[2];
    };
    Vector2() : u(0), v(0) {}
    Vector2(t _u, t _v) : u(_u),v(_v) {}
    inline t operator[](int idx) const { return raw[idx]; }
    inline Vector2<t> operator +(const Vector2<t> &V) const { return Vector2<t>(u+V.u, v+V.v); }
    inline Vector2<t> operator -(const Vector2<t> &V) const { return Vector2<t>(u-V.u, v-V.v); }
    inline Vector2<t> operator *(float f)          const { return Vector2<t>(u*f, v*f); }
    template <class > friend std::ostream& operator<<(std::ostream& s, Vector2<t>& v);
};

template <class t> struct Vector3 {
    union {
        struct {t x, y, z;};
        struct { t ivert, iuv, inorm; };
        t raw[3];
    };
    Vector3() : x(0), y(0), z(0) {}
    Vector3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
    inline t operator[](int idx) const { return raw[idx]; }
    inline Vector3<t> cross(const Vector3<t> &other) { return Vector3<t>(y*other.z - z*other.y, z*other.x-x*other.z, x*other.y - y*other.x); }
    inline Vector3<t> operator ^(const Vector3<t> &v) const { return Vector3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    inline Vector3<t> operator +(const Vector3<t> &v) const { return Vector3<t>(x+v.x, y+v.y, z+v.z); }
    inline Vector3<t> operator -(const Vector3<t> &v) const { return Vector3<t>(x-v.x, y-v.y, z-v.z); }
    inline Vector3<t> operator *(float f)          const { return Vector3<t>(x*f, y*f, z*f); }
    inline t       operator *(const Vector3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
    float norm () const { return std::sqrt(x*x+y*y+z*z); }
    Vector3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
    template <class > friend std::ostream& operator<<(std::ostream& s, Vector3<t>& v);
};

typedef Vector2<float> Vector2f;
typedef Vector2<int32_t>   Vector2i;
typedef Vector3<float> Vector3f;
typedef Vector3<int32_t> Vector3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vector2<t>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vector3<t>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

#endif //SOFTWARE_RENDERER_VECTORS_H
