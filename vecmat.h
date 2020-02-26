
// Three-dimensional vector
struct Vector3
{
	float x, y, z;//, w; // w used to align size.
	Vector3() {x = y = z = 0;}
	Vector3(float a, float b) {x = a; y = b; z = 0;}
	Vector3(float a, float b, float c) {x = a; y = b; z = c;}

	Vector3 operator+(const Vector3 &a) const {return Vector3(x + a.x, y + a.y, z + a.z);}
	Vector3 operator-(const Vector3 &a) const {return Vector3(x - a.x, y - a.y, z - a.z);}
	Vector3 operator*(const Vector3 &a) const {return Vector3(x * a.x, y * a.y, z * a.z);}
	Vector3 operator/(const Vector3 &a) const {return Vector3(x / a.x, y / a.y, z / a.z);}
	Vector3 operator+(float a) const {return Vector3(x + a, y + a, z + a);}
	Vector3 operator-(float a) const {return Vector3(x - a, y - a, z - a);}
	Vector3 operator*(float a) const {return Vector3(x * a, y * a, z * a);}
	Vector3 operator/(float a) const {return Vector3(x / a, y / a, z / a);}
	Vector3 operator-() const {return Vector3(-x, -y, -z);}

	Vector3 &operator+=(const Vector3 &a) {x += a.x; y += a.y; z += a.z; return *this;}
	Vector3 &operator-=(const Vector3 &a) {x -= a.x; y -= a.y; z -= a.z; return *this;}
	Vector3 &operator*=(const Vector3 &a) {x *= a.x; y *= a.y; z *= a.z; return *this;}
	Vector3 &operator/=(const Vector3 &a) {x /= a.x; y /= a.y; z /= a.z; return *this;}
	Vector3 &operator+=(float a) {x += a; y += a; z += a; return *this;}
	Vector3 &operator-=(float a) {x -= a; y -= a; z -= a; return *this;}
	Vector3 &operator*=(float a) {x *= a; y *= a; z *= a; return *this;}
	Vector3 &operator/=(float a) {x /= a; y /= a; z /= a; return *this;}

	bool operator==(const Vector3 &a) const {return (x==a.x) && (y==a.y) && (z==a.z);}
	bool operator!=(const Vector3 &a) const {return !( (x==a.x) && (y==a.y) && (z==a.z) );}

	//void print() const {printf("(%f, %f, %f)\n", x, y, z);}
	float len2xy() const {return sqrt(x*x + y*y);}
	float sqlen2xy() const {return x*x + y*y;}
	float len2xz() const {return sqrt(x*x + z*z);}
	float sqlen2xz() const {return x*x + z*z;}
	float len3() const {return sqrt(x*x + y*y + z*z);}
	float sqlen3() const {return x*x + y*y + z*z;}
	Vector3 normal() const {float l = len3(); if(l != 0.0f) return Vector3(x/l, y/l, z/l); else return Vector3(0,0,0);}
	Vector3 normal2xz() const {float l = len2xz(); if(l != 0.0f) return Vector3(x/l, 0, z/l); else return Vector3(0,0,0);}
	float dot(const Vector3 &a) const {return a.x * x + a.y * y + a.z * z;}
	float dot2xz(const Vector3 &a) const {return a.x * x + a.z * z;}
	Vector3 cross(const Vector3 &v) const {return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);}
};
