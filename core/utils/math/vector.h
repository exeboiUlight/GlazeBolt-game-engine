#pragma once

#include <cmath>
#include <ostream>

struct Matrix3x3 {
    float m[3][3];
    
    Matrix3x3() {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }
    
    static Matrix3x3 identity() {
        return Matrix3x3();
    }
    
    static Matrix3x3 translation(float x, float y) {
        Matrix3x3 result;
        result.m[0][2] = x;
        result.m[1][2] = y;
        return result;
    }
    
    static Matrix3x3 scale(float x, float y) {
        Matrix3x3 result;
        result.m[0][0] = x;
        result.m[1][1] = y;
        return result;
    }
    
    static Matrix3x3 rotation(float angleDeg) {
        float angleRad = angleDeg * 3.14159265358979323846f / 180.0f;
        float c = cos(angleRad);
        float s = sin(angleRad);
        
        Matrix3x3 result;
        result.m[0][0] = c;
        result.m[0][1] = -s;
        result.m[1][0] = s;
        result.m[1][1] = c;
        return result;
    }
    
    Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result.m[i][j] = 0;
                for (int k = 0; k < 3; k++) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    void toFloatArray(float* array) const {
        array[0] = m[0][0]; array[1] = m[0][1]; array[2] = m[0][2];
        array[3] = m[1][0]; array[4] = m[1][1]; array[5] = m[1][2];
        array[6] = m[2][0]; array[7] = m[2][1]; array[8] = m[2][2];
    }
};

class Vector2 {
public:
    float x, y;

    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}

    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator-() const { return Vector2(-x, -y); }

    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
    Vector2 operator/(float scalar) const { return Vector2(x / scalar, y / scalar); }

    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
    Vector2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2& operator/=(float scalar) { x /= scalar; return *this; }

    bool operator==(const Vector2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
    Vector2 normalized() const { float len = length(); return len > 0 ? *this / len : Vector2(); }
    void normalize() { *this = normalized(); }

    float dot(const Vector2& other) const { return x * other.x + y * other.y; }
    float cross(const Vector2& other) const { return x * other.y - y * other.x; }
};

class Vector3 {
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    Vector3 operator/(float scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

    Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vector3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    bool operator==(const Vector3& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(const Vector3& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y + z * z); }
    float lengthSquared() const { return x * x + y * y + z * z; }
    Vector3 normalized() const { float len = length(); return len > 0 ? *this / len : Vector3(); }
    void normalize() { *this = normalized(); }

    float dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

class Vector4 {
public:
    float x, y, z, w;

    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4(const Vector3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}

    Vector4 operator+(const Vector4& other) const { return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); }
    Vector4 operator-(const Vector4& other) const { return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); }
    Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

    Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
    Vector4 operator/(float scalar) const { return Vector4(x / scalar, y / scalar, z / scalar, w / scalar); }

    Vector4& operator+=(const Vector4& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
    Vector4& operator-=(const Vector4& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
    Vector4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    Vector4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

    bool operator==(const Vector4& other) const { return x == other.x && y == other.y && z == other.z && w == other.w; }
    bool operator!=(const Vector4& other) const { return !(*this == other); }

    float length() const { return std::sqrt(x * x + y * y + z * z + w * w); }
    float lengthSquared() const { return x * x + y * y + z * z + w * w; }
    Vector4 normalized() const { float len = length(); return len > 0 ? *this / len : Vector4(); }
    void normalize() { *this = normalized(); }

    float dot(const Vector4& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }

    Vector3 toVector3() const { return Vector3(x, y, z); }
};

inline Vector2 operator*(float scalar, const Vector2& vec) { return vec * scalar; }
inline Vector3 operator*(float scalar, const Vector3& vec) { return vec * scalar; }
inline Vector4 operator*(float scalar, const Vector4& vec) { return vec * scalar; }

inline std::ostream& operator<<(std::ostream& os, const Vector2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}
inline std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}
inline std::ostream& operator<<(std::ostream& os, const Vector4& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}