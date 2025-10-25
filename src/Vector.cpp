#include <stdexcept>

#include "Vector.h"

namespace une
{
	////////// Math //////////
	double Degrees(double radians)
	{
		return radians * (180 / PI);
	}
	double Radians(double degrees)
	{
		return degrees * (PI / 180);
	}

	////////// Vector 2 int //////////

	//Constructors
	Vector2Int::Vector2Int()
	{
		x = 0;
		y = 0;
	}
	Vector2Int::Vector2Int(int64_t all)
	{
		x = all;
		y = all;
	}
	Vector2Int::Vector2Int(int64_t x, int64_t y)
	{
		this->x = x;
		this->y = y;
	}
	Vector2Int::Vector2Int(const Vector2& v)
	{
		x = std::lround(v.x);
		y = std::lround(v.y);
	}
	Vector2Int::Vector2Int(const Vector3Int& v)
	{
		x = v.x;
		y = v.y;
	}
	Vector2Int::Vector2Int(const Vector3& v)
	{
		x = std::lround(v.x);
		y = std::lround(v.y);
	}

	//Indexing
	int64_t& Vector2Int::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw std::out_of_range("Index in Vector2Int out of range");
		}
	}
	const int64_t& Vector2Int::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw std::out_of_range("Index in Vector2Int out of range");
		}
	}

	//Comparison
	bool Vector2Int::operator==(const Vector2Int& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool Vector2Int::operator!=(const Vector2Int& rhs) const
	{
		return !(x == rhs.x && y == rhs.y);
	}

	//Add
	Vector2Int Vector2Int::operator+(const int64_t& add) const
	{
		return Vector2Int(x + add, y + add);
	}
	Vector2Int Vector2Int::operator+(const Vector2Int& add) const
	{
		return Vector2Int(x + add.x, y + add.y);
	}
	Vector2Int& Vector2Int::operator+=(const Vector2Int& add)
	{
		x += add.x;
		y += add.y;
		return *this;
	}

	//Subtract
	Vector2Int Vector2Int::operator-(const int64_t& sub) const
	{
		return Vector2Int(x - sub, y - sub);
	}
	Vector2Int Vector2Int::operator-(const Vector2Int& sub) const
	{
		return Vector2Int(x - sub.x, y - sub.y);
	}
	Vector2Int& Vector2Int::operator-=(const Vector2Int& sub)
	{
		x -= sub.x;
		y -= sub.y;
		return *this;
	}

	//Multiply
	Vector2Int Vector2Int::operator*(const int64_t& mult) const
	{
		return Vector2Int(x * mult, y * mult);
	}
	Vector2Int Vector2Int::operator*(const Vector2Int& mult) const
	{
		return Vector2Int(x * mult.x, y * mult.y);
	}
	Vector2Int& Vector2Int::operator*=(const int64_t& mult)
	{
		x *= mult;
		y *= mult;
		return *this;
	}
	Vector2Int& Vector2Int::operator*=(const Vector2Int& mult)
	{
		x *= mult.x;
		y *= mult.y;
		return *this;
	}

	//Divide
	Vector2Int Vector2Int::operator/(const int64_t& div) const
	{
		return Vector2Int(x / div, y / div);
	}
	Vector2Int Vector2Int::operator/(const Vector2Int& div) const
	{
		return Vector2Int(x / div.x, y / div.y);
	}
	Vector2Int& Vector2Int::operator/=(const int64_t& div)
	{
		x /= div;
		y /= div;
		return *this;
	}

	std::string Vector2Int::ToString() const
	{
		char buff[255];
		std::snprintf(buff, sizeof(buff) - 1, "(%lld, %lld)", x, y);
		return std::string(buff);
	}

	////////// Vector 2 //////////

	//Constructors
	Vector2::Vector2()
	{
		x = 0;
		y = 0;
	}
	Vector2::Vector2(double all)
	{
		x = all;
		y = all;
	}
	Vector2::Vector2(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	Vector2::Vector2(const Vector2Int& v)
	{
		x = (double)v.x;
		y = (double)v.y;
	}
	Vector2::Vector2(const Vector3& vec3)
	{
		x = vec3.x;
		y = vec3.y;
	}

	//Indexing
	double& Vector2::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw std::out_of_range("Index in Vector2 out of range");
		}
	}
	const double& Vector2::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		default:
			throw std::out_of_range("Index in Vector2 out of range");
		}
	}

	//Comparison
	bool Vector2::operator==(const Vector2& rhs) const
	{
		return fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon;
	}
	bool Vector2::operator!=(const Vector2& rhs) const
	{
		return !(fabs(x - rhs.x) < epsilon && fabs(y - rhs.y) < epsilon);
	}

	//Add
	Vector2 Vector2::operator+(const double& add) const
	{
		return Vector2(x + add, y + add);
	}
	Vector2 Vector2::operator+(const Vector2& add) const
	{
		return Vector2(x + add.x, y + add.y);
	}
	Vector2& Vector2::operator+=(const Vector2& add)
	{
		x += add.x;
		y += add.y;
		return *this;
	}

	//Subtract
	Vector2 Vector2::operator-(const double& sub) const
	{
		return Vector2(x - sub, y - sub);
	}
	Vector2 Vector2::operator-(const Vector2& sub) const
	{
		return Vector2(x - sub.x, y - sub.y);
	}
	Vector2& Vector2::operator-=(const Vector2& sub)
	{
		x -= sub.x;
		y -= sub.y;
		return *this;
	}

	//Multiply
	Vector2 Vector2::operator*(const double& mult) const
	{
		return Vector2(x * mult, y * mult);
	}
	Vector2 Vector2::operator*(const Vector2& mult) const
	{
		return Vector2(x * mult.x, y * mult.y);
	}
	Vector2& Vector2::operator*=(const double& mult)
	{
		x *= mult;
		y *= mult;
		return *this;
	}
	Vector2& Vector2::operator*=(const Vector2& mult)
	{
		x *= mult.x;
		y *= mult.y;
		return *this;
	}

	//Divide
	Vector2 Vector2::operator/(const double& div) const
	{
		return Vector2(x / div, y / div);
	}
	Vector2 Vector2::operator/(const Vector2& div) const
	{
		return Vector2(x / div.x, y / div.y);
	}
	Vector2& Vector2::operator/=(const double& div)
	{
		x /= div;
		y /= div;
		return *this;
	}

	double Vector2::Length() const
	{
		return sqrt(x * x + y * y);
	}
	Vector2 Vector2::Normalize() const
	{
		const double length = Length();
		return Vector2(x / length, y / length);
	}
	double Vector2::Dot(const Vector2& b) const
	{
		return (x * b.x) + (y * b.y);
	}
	Vector2 Vector2::LeftNormal() const
	{
		return Vector2(-y, x).Normalize();
	}
	Vector2 Vector2::RightNormal() const
	{
		return Vector2(y, -x).Normalize();
	}

	std::string Vector2::ToString() const
	{
		char buff[255];
		std::snprintf(buff, sizeof(buff) - 1, "(%f, %f)", x, y);
		return std::string(buff);
	}

	////////// Vector 3 Int //////////

	//Constructors
	Vector3Int::Vector3Int()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3Int::Vector3Int(int64_t all)
	{
		x = all;
		y = all;
		z = all;
	}
	Vector3Int::Vector3Int(int64_t x, int64_t y, int64_t z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3Int::Vector3Int(const Vector2Int& vec2, int64_t z)
	{
		x = vec2.x;
		y = vec2.y;
		this->z = z;
	}
	Vector3Int::Vector3Int(const Vector3& vec3)
	{
		x = std::lround(vec3.x);
		y = std::lround(vec3.y);
		z = std::lround(vec3.z);
	}

	//Indexing
	int64_t& Vector3Int::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::out_of_range("Index in Vector3Int out of range");
		}
	}
	const int64_t& Vector3Int::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::out_of_range("Index in Vector3Int out of range");
		}
	}

	//Comparison
	bool Vector3Int::operator==(const Vector3Int& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool Vector3Int::operator!=(const Vector3Int& rhs) const
	{
		return !(x == rhs.x && y == rhs.y && z == rhs.z);
	}

	//Addition
	Vector3Int Vector3Int::operator+(const int64_t& add) const
	{
		return Vector3Int(x + add, y + add, z + add);
	}
	Vector3Int Vector3Int::operator+(const Vector3Int& add) const
	{
		return Vector3Int(x + add.x, y + add.y, z + add.z);
	}
	Vector3Int& Vector3Int::operator+=(const Vector3Int& add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
		return *this;
	}

	//Subtraction
	Vector3Int Vector3Int::operator-(const int64_t& sub) const
	{
		return Vector3Int(x - sub, y - sub, z - sub);
	}
	Vector3Int Vector3Int::operator-(const Vector3Int& sub) const
	{
		return Vector3Int(x - sub.x, y - sub.y, z - sub.z);
	}
	Vector3Int& Vector3Int::operator-=(const Vector3Int& sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
		return *this;
	}

	//Multiplication
	Vector3Int Vector3Int::operator*(const int64_t& mult) const
	{
		return Vector3Int(x * mult, y * mult, z * mult);
	}
	Vector3Int Vector3Int::operator*(const Vector3Int& mult) const
	{
		return Vector3Int(x * mult.x, y * mult.y, z * mult.z);
	}
	Vector3Int& Vector3Int::operator*=(const int64_t& mult)
	{
		x *= mult;
		y *= mult;
		z *= mult;
		return *this;
	}

	//Division
	Vector3Int Vector3Int::operator/(const int64_t& div) const
	{
		return Vector3Int(x / div, y / div, z / div);
	}
	Vector3Int Vector3Int::operator/(const Vector3Int& div) const
	{
		return Vector3Int(x / div.x, y / div.y, z / div.z);
	}
	Vector3Int& Vector3Int::operator/=(const int64_t& div)
	{
		x /= div;
		y /= div;
		z /= div;
		return *this;
	}

	std::string Vector3Int::ToString() const
	{
		char buff[255];
		std::snprintf(buff, sizeof(buff) - 1, "(%lld, %lld, %lld)", x, y, z);
		return std::string(buff);
	}

	////////// Vector 3 //////////

	//Constructors
	Vector3::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	Vector3::Vector3(double all)
	{
		x = all;
		y = all;
		z = all;
	}
	Vector3::Vector3(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3::Vector3(const Vector2& vec2, double z)
	{
		x = vec2.x;
		y = vec2.y;
		this->z = z;
	}
	Vector3::Vector3(const Vector2Int& vec2, double z)
	{
		x = (double)vec2.x;
		y = (double)vec2.y;
		this->z = z;
	}
	Vector3::Vector3(const Vector3Int& v)
	{
		x = (double)v.x;
		y = (double)v.y;
		z = (double)v.z;
	}
	Vector3::Vector3(const Vector4& vec4)
	{
		x = vec4.x;
		y = vec4.y;
		z = vec4.z;
	}

	//Indexing
	double& Vector3::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::out_of_range("Index in Vector3 out of range");
		}
	}
	const double& Vector3::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			throw std::out_of_range("Index in Vector3 out of range");
		}
	}

	//Comparison
	bool Vector3::operator==(const Vector3& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}
	bool Vector3::operator!=(const Vector3& rhs) const
	{
		return !(x == rhs.x && y == rhs.y && z == rhs.z);
	}

	//Addition
	Vector3 Vector3::operator+(const double& add) const
	{
		return Vector3(x + add, y + add, z + add);
	}
	Vector3 Vector3::operator+(const Vector3& add) const
	{
		return Vector3(x + add.x, y + add.y, z + add.z);
	}
	Vector3& Vector3::operator+=(const double& add)
	{
		x += add;
		y += add;
		z += add;
		return *this;
	}
	Vector3& Vector3::operator+=(const Vector3& add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
		return *this;
	}

	//Subtraction
	Vector3 Vector3::operator-(const double& sub) const
	{
		return Vector3(x - sub, y - sub, z - sub);
	}
	Vector3 Vector3::operator-(const Vector3& sub) const
	{
		return Vector3(x - sub.x, y - sub.y, z - sub.z);
	}
	Vector3& Vector3::operator-=(const double& sub)
	{
		x -= sub;
		y -= sub;
		z -= sub;
		return *this;
	}
	Vector3& Vector3::operator-=(const Vector3& sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
		return *this;
	}

	//Multiplication
	Vector3 Vector3::operator*(const double& mult) const
	{
		return Vector3(x * mult, y * mult, z * mult);
	}
	Vector3 Vector3::operator*(const Vector3& mult) const
	{
		return Vector3(x * mult.x, y * mult.y, z * mult.z);
	}
	Vector3& Vector3::operator*=(const double& mult)
	{
		x *= mult;
		y *= mult;
		z *= mult;
		return *this;
	}
	Vector3& Vector3::operator*=(const Vector3& mult)
	{
		x *= mult.x;
		y *= mult.y;
		z *= mult.z;
		return *this;
	}

	//Division
	Vector3 Vector3::operator/(const double& div) const
	{
		return Vector3(x / div, y / div, z / div);
	}
	Vector3 Vector3::operator/(const Vector3& div) const
	{
		return Vector3(x / div.x, y / div.y, z / div.z);
	}
	Vector3& Vector3::operator/=(const double& div)
	{
		x /= div;
		y /= div;
		z /= div;
		return *this;
	}
	Vector3& Vector3::operator/=(const Vector3& div)
	{
		x /= div.x;
		y /= div.y;
		z /= div.z;
		return *this;
	}

	double Vector3::Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vector3 Vector3::Normalize() const
	{
		double length = sqrt(x * x + y * y + z * z);
		return Vector3(x / length, y / length, z / length);
	}
	double Vector3::Dot(Vector3 b) const
	{
		return (x * b.x) + (y * b.y) + (z * b.z);
	}
	Vector3 Vector3::Cross(Vector3 b) const
	{
		return Vector3((y * b.z) - (z * b.y), (z * b.x) - (x * b.z), (x * b.y) - (y * b.x));
	}
	//Distance from this point to another
	double Vector3::Distance(Vector3 b) const
	{
		return sqrt((b.x - x) * (b.x - x) + (b.y - y) * (b.y - y) + (b.z - z) * (b.z - z));
	}

	//Returns a glm vec3
	glm::vec3 Vector3::ToGlm() const
	{
		return glm::vec3(x, y, z);
	}

	std::string Vector3::ToString() const
	{
		char buff[255];
		std::snprintf(buff, sizeof(buff) - 1, "(%f, %f, %f)", x, y, z);
		return std::string(buff);
	}

	//////////// Vector 4 //////////////

	//Constructors
	Vector4::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}
	Vector4::Vector4(double all)
	{
		x = all;
		y = all;
		z = all;
		w = all;
	}
	Vector4::Vector4(double x, double y, double z, double w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	Vector4::Vector4(const Vector3& vec3, double w)
	{
		x = vec3.x;
		y = vec3.y;
		z = vec3.z;
		this->w = w;
	}

	//Comparison
	bool Vector4::operator==(const Vector4& rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
	}
	bool Vector4::operator!=(const Vector4& rhs) const
	{
		return !(x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
	}

	//Indexing
	double& Vector4::operator[](int i)
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw std::out_of_range("Index in Vector4 out of range");
		}
	}
	const double& Vector4::operator[](int i) const
	{
		switch (i)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			throw std::out_of_range("Index in Vector4 out of range");
		}
	}

	//Addition
	Vector4 Vector4::operator+(const double& add) const
	{
		return Vector4(x + add, y + add, z + add, w + add);
	}
	Vector4 Vector4::operator+(const Vector4& add) const
	{
		return Vector4(x + add.x, y + add.y, z + add.z, w + add.w);
	}
	Vector4& Vector4::operator+=(const Vector4& add)
	{
		x += add.x;
		y += add.y;
		z += add.z;
		w += add.w;
		return *this;
	}

	//Subtraction
	Vector4 Vector4::operator-(const double& sub) const
	{
		return Vector4(x - sub, y - sub, z - sub, w - sub);
	}
	Vector4 Vector4::operator-(const Vector4& sub) const
	{
		return Vector4(x - sub.x, y - sub.y, z - sub.z, w - sub.w);
	}
	Vector4& Vector4::operator-=(const Vector4& sub)
	{
		x -= sub.x;
		y -= sub.y;
		z -= sub.z;
		w -= sub.w;
		return *this;
	}

	//Multiplication
	Vector4 Vector4::operator*(const double& mult) const
	{
		return Vector4(x * mult, y * mult, z * mult, w * mult);
	}
	Vector4 Vector4::operator*(const Vector4& mult) const
	{
		return Vector4(x * mult.x, y * mult.y, z * mult.z, w * mult.w);
	}
	Vector4& Vector4::operator*=(const double& mult)
	{
		x *= mult;
		y *= mult;
		z *= mult;
		w *= mult;
		return *this;
	}

	//Division
	Vector4 Vector4::operator/(const double& div) const
	{
		return Vector4(x / div, y / div, z / div, w / div);
	}
	Vector4 Vector4::operator/(const Vector4& div) const
	{
		return Vector4(x / div.x, y / div.y, z / div.z, w / div.w);
	}
	Vector4& Vector4::operator/=(const double& div)
	{
		x /= div;
		y /= div;
		z /= div;
		w /= div;
		return *this;
	}

	std::string Vector4::ToString() const
	{
		char buff[255];
		std::snprintf(buff, sizeof(buff) - 1, "(%f, %f, %f, %f)", x, y, z, w);
		return std::string(buff);
	}
}