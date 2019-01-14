/*
  (C) TANIGUCHI, Kazushige 2019
 */

#ifndef _MATRIX4x4_H_
#define _MATRIX4x4_H_

typedef float matrix4x4[4][4];

inline void matrix4x4Identify(matrix4x4 dst){
	dst[0][0] = 1.0f; dst[0][1] = 0.0f; dst[0][2] = 0.0f; dst[0][3] = 0.0f;
	dst[1][0] = 0.0f; dst[1][1] = 1.0f; dst[1][2] = 0.0f; dst[1][3] = 0.0f;
	dst[2][0] = 0.0f; dst[2][1] = 0.0f; dst[2][2] = 1.0f; dst[2][3] = 0.0f;
	dst[3][0] = 0.0f; dst[3][1] = 0.0f; dst[3][2] = 0.0f; dst[3][3] = 1.0f;
}

inline void matrix4x4Add(matrix4x4 dst, matrix4x4 left, matrix4x4 right){
	dst[0][0] = left[0][0]+right[0][0];
	dst[0][1] = left[0][1]+right[0][1];
	dst[0][2] = left[0][2]+right[0][2];
	dst[0][3] = left[0][3]+right[0][3];
	dst[1][0] = left[1][0]+right[1][0];
	dst[1][1] = left[1][1]+right[1][1];
	dst[1][2] = left[1][2]+right[1][2];
	dst[1][3] = left[1][3]+right[1][3];
	dst[2][0] = left[2][0]+right[2][0];
	dst[2][1] = left[2][1]+right[2][1];
	dst[2][2] = left[2][2]+right[2][2];
	dst[2][3] = left[2][3]+right[2][3];
	dst[3][0] = left[3][0]+right[3][0];
	dst[3][1] = left[3][1]+right[3][1];
	dst[3][2] = left[3][2]+right[3][2];
	dst[3][3] = left[3][3]+right[3][3];
	return;
}

/* カメラ移動関数 */
inline void lookAtMatrix(){
	return;
}

/* 平行透視変換 */
inline void orthogonalMatrix(matrix4x4 dst, float left, float right, float bottom, float top, float near, float far){
	/*
      透視変換行列
      | 2/(right-left),                    0,                 0,             -(right+left)/(right-left) |
      |              0,       2/(top-bottom),                 0,             -(top+bottom)/(top-bottom) |
      |              0,                    0,     -2/(far-near),                 -(far+near)/(far-near) |
      |              0,                    0,                 0,                                     1  |

      OpenGLの場合は転置させないといけないので
      |             2(right-left),                         0,                        0,                         0 |
      |                         0,            2/(top-bottom),                        0,                         0 |
      |                         0,                         0,            -2/(far-near),                         0 |
      |-(right+left)/(right-left),-(top+bottom)/(top-bottom),   -(far+near)/(far-near),                         1 |
	 */
	dst[0][0] = 2.0f/(right-left);           dst[0][1] = 0.0f;                       dst[0][2] = 0.0f;                       dst[0][3] = 0.0f;
	dst[1][0] = 0.0f;                        dst[1][1] = 2.0f/(top-bottom);          dst[1][2] = 0.0f;                       dst[1][3] = 0.0f;
	dst[2][0] = 0.0f;                        dst[2][1] = 0.0f;                       dst[2][2] = -2.0f/(far-near);           dst[2][3] = -(far+near)/(far-near);
	dst[3][0] = -(right+left)/(right-left);  dst[3][1] = -(top+bottom)/(top-bottom); dst[3][2] = -(far+near)/(far-near);      dst[3][3] = 1.0f;
	return;
}

inline void frustumProjectMatrix(matrix4x4 dst, float left, float right, float bottom, float top, float near, float far){
	/*
      透視変換行列
      | 2*near/(right-left),                    0,    (right+left)/(right-left),                         0 |
      |                   0,  2*near/(top-bottom),    (top+bottom)/(top-bottom),                         0 |
      |                   0,                    0,     -(far+near)/(far-near),    -2*far*near/(far - near) |
      |                   0,                    0,                           -1,                         0 |

      OpenGLの場合は転置させないといけないので
      |       2*near/(right-left),                         0,                        0,                         0 |
      |                         0,       2*near/(top-bottom),                        0,                         0 |
      | (right+left)/(right-left), (top+bottom)/(top-bottom),   -(far+near)/(far-near),                        -1 |
      |                         0,                         0, -2*far*near/(far - near),                         0 |
	 */

	dst[0][0] = 2.0f*near/(right-left);    dst[0][1] = 0.0f;                       dst[0][2] = 0.0f;                       dst[0][3] =  0.0f;
	dst[1][0] = 0.0f;                      dst[1][1] = 2*near/(top-bottom);        dst[1][2] = 0.0f;                       dst[1][3] =  0.0f;
	dst[2][0] = (right+left)/(right-left); dst[2][1] = (top+bottom)/(top-bottom);  dst[2][2] = -(far+near)/(far-near);     dst[2][3] = -1.0f;
	dst[3][0] = 0;                         dst[3][1] = 0;                          dst[3][2] = -2.0f*far*near/(far-near);  dst[3][3] =  0.0f;

	return;
}

/*
   fovy: Unit in Degree
   aspect: width/height
*/
inline void perspectiveProjectMatrix(matrix4x4 dst, float fovy, float aspect, float zNear, float zFar){
	float top = tanf(fovy*M_PI/360.0)*zNear;
	float bottom = -top;
	float right =  top * aspect;
	float left  = -top * aspect;

	frustumProjectMatrix(dst, left, right, bottom, top, zNear, zFar);

	return;
}

#endif /* _MATRIX4x4H_ */


