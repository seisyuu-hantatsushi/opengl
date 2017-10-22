#ifndef _RIGID_HPP_
#define _RIGID_HPP_

#include <geoMath.hpp>

typedef GeoMath::Vector<float> GMVector;
typedef GeoMath::Matrix<float> GMMatrix;
typedef GeoMath::Quaternion<float> GMQuaternion;

class CRigid {

public:
	enum Kind {SPHERE, ELLIPSOID, CUBE, BOX, CYLINDER, COLUMN, TORUS, CONE, PYRAMID, KOMA, VEHICLE, PET};
	enum Color {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, WHITE, GRAY};

private:
	
	Kind m_kind;
	Color m_color;
	GMVector m_vSize;
	GMVector m_vPos;
	int m_pos[3]; //整数化位置
	GMVector m_vPosCurrent; //衝突前の位置
	GMVector m_vEuler; //オイラー角(姿勢の初期設定,deg)
	GMVector m_vAxis;  //回転軸
	float   m_angle;  //回転角度(rad)

	//運動シュミレーション用
	float   m_mass;               //質量(kg)
	GMVector m_vForce;             //外力(newton)
	GMVector m_vForce0, m_vForce1; //外力初期値
	GMVector m_Torque;             //力のモーメント (トルク, Newton/m)
	GMVector m_Accel;              //加速度(m/s^2)
	GMVector m_Velocity;           //速度(m/s)
	GMVector m_Forward;            //進行方向
	GMVector m_vOmega;             //角速度(rad/s)
	GMVector m_vAlpha;             //角加速度(rad/s^2)
	GMVector m_vGravityToPoint;    //重心から衝突点までのベクトル
	GMMatrix m_Inertia;            //慣性モーメントのテンソル
	GMMatrix m_InertiaInverse;
	GMQuaternion m_q;

public:
	CRigid():m_kind(SPHERE),
			 m_color(WHITE),
			 m_vSize(1.0,1.0,1.0){
		
		//m_vSize = GMVector(1.0,1.0,1.0);
		
	};
	~CRigid(){
	}
};


#endif /* _RIGID_HPP_ */

