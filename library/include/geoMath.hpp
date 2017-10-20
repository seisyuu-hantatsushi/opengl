#ifndef _GEOMATH_HPP_
#define _GEOMATH_HPP_

#include <stdexcept>
#include <math.h>

namespace GeoMath {
	
	template<typename T> class Vector;

	template<typename T> inline Vector<T> normalize(const Vector<T> &v);

	template<typename T>
	class Vector {

	private:
		//メンバ変数
		T m_v[3]; //x=m_v[0],y=m_v[1],z=m_v[2]
	
	public:	
		//コンストラクタ
		Vector() {
			m_v[0] = m_v[1] = m_v[2] = 0.0;
		};
	
		Vector(T x, T y, T z){
			m_v[0] = x;
			m_v[1] = y;
			m_v[2] = z;
		};
	
		Vector(T v[3]){
			m_v[0] = v[0];
			m_v[1] = v[1];
			m_v[2] = v[2];
		}

	
		T x() const { return m_v[0]; };
		T y() const { return m_v[1]; };
		T z() const { return m_v[2]; };
	
		//演算子
		T operator[](size_t i) const {
			if(i < sizeof(m_v)/sizeof(m_v[0])){
				return m_v[i];
			}
			else{
				throw std::range_error("out of index of vector");
			}
		}

		T& operator[](size_t i) {
			if(i < sizeof(m_v)/sizeof(m_v[0])){
				return m_v[i];
			}
			else{
				throw std::range_error("out of index of vector");
			}
		}
	
		//加算
		const Vector<T> operator+(const Vector<T>& rhs) const {
			return Vector<T>(m_v[0] + rhs[0],
							 m_v[1] + rhs[1],
							 m_v[2] + rhs[2]);
		}
	
		//減算
		const Vector<T> operator-(const Vector<T>& rhs) const {
			return Vector<T>(m_v[0] - rhs[0],
							 m_v[1] - rhs[1],
							 m_v[2] - rhs[2]);
		}

		//乗算 scalar*vector
		friend Vector<T> operator*(const T &s, const Vector<T> &v) {
			return CVector(v[0]*s,v[1]*s,v[2]*s);
		}

		//乗算 vector*scalar
		friend Vector operator*(const Vector<T> &v, const T &s) {
			return CVector(v[0]*s,v[1]*s,v[2]*s);
		}
	
		//除算 vector/scalar
		friend Vector<T> operator/(const Vector<T> &v, const T s){
			return CVector(v[0]/s,
						   v[1]/s,
						   v[2]/s);
		}
	
		Vector<T>& operator+=(const Vector<T> &v){
			m_v[0] += v[0];
			m_v[1] += v[1];
			m_v[2] += v[2];
			return *this;
		}
	
		Vector<T>& operator-=(const Vector<T> &v){
			m_v[0] -= v[0];
			m_v[1] -= v[1];
			m_v[2] -= v[2];
			return *this;
		}
	
		Vector<T>& operator*=(T s){
			m_v[0] *= s;
			m_v[1] *= s;
			m_v[2] *= s;
			return *this;
		}

		Vector<T>& operator/=(T s){
			m_v[0] /= s;
			m_v[1] /= s;
			m_v[2] /= s;
			return *this;
		}   	
	};

	template<typename T>
	inline T dot(const Vector<T> &lhs, const Vector<T> &rhs){
		return lhs[0]*rhs[0]+lhs[1]*rhs[1]+lhs[2]*rhs[2];
	}
	
	template<typename T>
	inline T length(const Vector<T> &v){
		return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	}

	template<typename T>
	inline Vector<T> normalize(const Vector<T> &v){
		const T eps = 0.000001;
		double len = length(v);
		double e[3];
	
		if(len < eps) len = 1.0;
	
		return Vector<T>((fabs(v[0]) < eps)?0.0:v[0]/len,
						 (fabs(v[1]) < eps)?0.0:v[1]/len,
						 (fabs(v[2]) < eps)?0.0:v[2]/len);
	}

	// 3 x 3 Matrix
	template<typename T>
	class Matrix {
	private:
		T m_e[3][3];

	public:
		Matrix() {
			size_t i,j;
			for(i=0; i<3; i++){
				for(j=0; j<3; j++){
					m_e[i][j] = 0.0;
				}
			}
		}

		Matrix(T e[3][3]){
			size_t i,j;
			for(i=0; i<3; i++){
				for(j=0; j<3; j++){
					m_e[i][j] = e[i][j];
				}
			}
		}
	};

	//Quaternion
	template<typename T> class Quaternion {
	private:
		T m_s; //スカラ値
		Vector<T> m_v; //ベクトル値
	   		
	public:
		Quaternion(){
			m_s = 0.0;
			m_v[0] = m_v[1] = m_v[2] = 0.0;
		}
	};
};


#endif /* _GEOMATH_HPP_ */
