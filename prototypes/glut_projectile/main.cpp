#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include <time.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include "rigid.hpp"

enum ShiftMode {TRANSLATE, ROTATION, LIGHT, VELOCITY};
enum CoordMode {INERTIAL, OBJECT};
enum State {FREE, RESTING, RESTING_ON_FLOOR, FREE_ON_FLOOR};

struct View {
	//カメラ
	GMVector vPos;//位置（視点）
	GMVector vCenter;//注視点
	float dist;  //注視点から視点までの距離(R)
	float theta; //仰角（水平面との偏角）
	float phi;   //方位角
	//視体積
	float fovY;  //視野角
	float nearZ; //前方クリップ面(近平面)
	float farZ;  //後方クリップ面(遠平面)
};

static const View s_view0 = {
	GMVector(),
	GMVector(0.0, 0.0, 5.0),
	50.0f, 0.0f, 0.0f,
	40.0f, 1.0f,  200.0f
};

static const float lightPos0[] = {20.0, 20.0, 100.0, 1.0};//光源位置(初期値)

struct AppCtx {
	View view;
	
	uint64_t prevDisplayTime;
	
	int width;       //画面の大きさ
	int height;

	//光源
	float lightPos[4];
	
	GLuint shaderProg; //シェーダプログラム番号

	//マウス操作
	int xStart;
	int yStart;
	bool bParamShow;
	bool bMouse;
	bool bCameraTrack;
	bool bSlide;
	bool bWireFrame;
	bool bShadow;
	bool bAnimStart;
	bool bAnimSuspend;
	bool bOneStep;
	bool bDispFloor;
	bool bStrobo;
	bool bCoord;
	bool bTrack;
	
	float elapseTimeStrobo;
	float periodTimeStrobo;
	uint32_t countOfStrobo;
	uint32_t maxOfStrobo;
	uint32_t totalOfStrobo;
	float timeRate;

	bool bEnableDrag;//慣性摩擦抵抗の有効/無効

	ShiftMode shiftMode;
	CoordMode coordMode;
	
} g_theApp = {
	.view = s_view0,
	.prevDisplayTime = 0,
	.width = 600,
	.height = 500,
	.lightPos = {lightPos0[0],lightPos0[1],lightPos0[2],lightPos0[3]},
	.shaderProg = 0,
	.xStart = 0,
	.yStart = 0,
	.bParamShow = false,
	.bMouse = false,
	.bCameraTrack = false,
	.bSlide = false,
	.bWireFrame = false,
	.bShadow = true,
	.bAnimStart = false,
	.bAnimSuspend = false, //bAnimSuspend
	.bOneStep = false, //bOneStep
	.bDispFloor = true, //bDispFloor
	.bStrobo = false, //bStrobo
	.bCoord  = false, //bCoord
	.bTrack  = false,
	.elapseTimeStrobo = 0.0f,
	.periodTimeStrobo = 1.0f,
	.countOfStrobo = 0,
	.maxOfStrobo   = 40,
	.totalOfStrobo = 100,
	.timeRate      = 1.0f,
	.bEnableDrag   = false,
	.shiftMode     = TRANSLATE,
	.coordMode     = INERTIAL
};

static void resize(int w, int h);

static uint64_t nanosecCounter(){
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	return (now.tv_sec*1000000000+now.tv_nsec);
}

static void setCamera(struct AppCtx *pAppCtx){
	double theta = (M_PI * pAppCtx->view.theta)/180.0;
	double phi   = (M_PI * pAppCtx->view.phi)/180.0;
	
	pAppCtx->view.vPos[0] = 
		pAppCtx->view.vCenter[0] + pAppCtx->view.dist * cos(theta) * cos(phi); 
	pAppCtx->view.vPos[1] =
		pAppCtx->view.vCenter[1] + pAppCtx->view.dist * cos(theta) * sin(phi); 
	pAppCtx->view.vPos[2] =
		pAppCtx->view.vCenter[2] + pAppCtx->view.dist * cos(theta);

	resize(pAppCtx->width, pAppCtx->height);
	return;
}

void setLight(struct AppCtx *pAppCtx)
{
	float lightAmbient0[]  = {0.5f, 0.5f, 0.5f, 1.0f}; //環境光
	float lightDiffuse0[]  = {0.7f, 0.7f, 0.7f, 1.0f}; //拡散光
	float lightSpecular0[] = {0.5f, 0.5f, 0.5f, 1.0f};//鏡面光
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular0);
	glLightfv(GL_LIGHT0, GL_POSITION, pAppCtx->lightPos);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

static void init(struct AppCtx *pAppCtx){

	//背景色
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	setCamera(pAppCtx);
	setLight(pAppCtx);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	//カメラと視体積の初期値の変更
	pAppCtx->view = s_view0;
	setCamera(pAppCtx);

	//時間関係

	return;
}

static void initObject(struct AppCtx *pAppCtx){
	pAppCtx->bAnimStart       = false;
	pAppCtx->bStrobo          = false;
	pAppCtx->countOfStrobo    = 0;
	pAppCtx->elapseTimeStrobo = 0.0f;
	pAppCtx->periodTimeStrobo = 0.0f;
	pAppCtx->maxOfStrobo      = 100;

	//剛体の初期設定
	
	
	return;
}


static void initGLSL(struct AppCtx *pAppCtx, const char *pVertexFile, const char *pFragmentFile){
	//glewの初期化
	GLenum err = glewInit();
	
	return;
}

static void resize(int w, int h){
	//ビューポート変換
	glViewport(0, 0, w, h);
	//プロジェクション行列の指定
	glMatrixMode(GL_PROJECTION);
	//プロジェクション行列の初期化
	glLoadIdentity();
	//透視投影行列の設定(投影変換)
	gluPerspective(g_theApp.view.fovY,
				   static_cast<double>(w)/static_cast<double>(h),
				   g_theApp.view.nearZ,
				   g_theApp.view.farZ);
	glMatrixMode(GL_MODELVIEW);
	//モデルビュー行列の初期化
	glLoadIdentity();
	//表示
	g_theApp.width  = w;
	g_theApp.height = h;
	return;
};

static void display(void){
	struct timespec now;
	struct AppCtx *pAppCtx = &g_theApp;
	
	 //時間計測	
	pAppCtx->prevDisplayTime = nanosecCounter();

	//Color Buffer/Depth Bufferのクリア
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	//カメラ仰角90度でカメラの上方向ベクトルを切り替え
	if((M_PI * pAppCtx->view.theta/180.0) >= 0.0){
		gluLookAt(pAppCtx->view.vPos[0], pAppCtx->view.vPos[1], pAppCtx->view.vPos[2],
				  pAppCtx->view.vCenter[0], pAppCtx->view.vCenter[1], pAppCtx->view.vCenter[2],
				  0.0, 0.0, 1.0);
	}
	else {
		gluLookAt(pAppCtx->view.vPos[0], pAppCtx->view.vPos[1], pAppCtx->view.vPos[2],
				  pAppCtx->view.vCenter[0], pAppCtx->view.vCenter[1], pAppCtx->view.vCenter[2],
				  0.0, 0.0, -1.0);
	}

	glLightfv(GL_LIGHT0, GL_POSITION, pAppCtx->lightPos);

	if(g_theApp.bWireFrame){
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_POINT);
	}
	else{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	glUseProgram(pAppCtx->shaderProg);
	
}

static void keyboard(unsigned char key, int x, int y)
{
	struct AppCtx *pAppCtx = &g_theApp;
	
	switch((unsigned char)key){
	case 27://Esc
		exit(0);
		break;
	case 'R'://camera reset
		pAppCtx->view = s_view0;
		setCamera(&g_theApp);
		for(int i = 0; i < 3; i++){
			pAppCtx->lightPos[i] = lightPos0[i];
		}
		break;
	case 's':
		pAppCtx->bAnimStart = !pAppCtx->bAnimStart; 
		if(!pAppCtx->bAnimStart) initObject(pAppCtx);
		//if(g_theApp.bAnimStart) elapseTime2 = 0.0;
		break;
	case 'q':
		pAppCtx->bAnimSuspend = !pAppCtx->bAnimSuspend;
		if(!pAppCtx->bAnimSuspend) pAppCtx->bOneStep = false;
		break;
	case '1'://one step motion
		pAppCtx->bOneStep = true;
		pAppCtx->bAnimSuspend = false;
		break;
	case 'd':
		pAppCtx->bEnableDrag = !g_theApp.bEnableDrag;
		break; 
	case 't':
		pAppCtx->shiftMode = TRANSLATE;
		break;
	case 'r':
		pAppCtx->shiftMode = ROTATION;
		break;
	case 'l':
		pAppCtx->shiftMode = LIGHT;
		break;
	case 'i':
		if(pAppCtx->coordMode == INERTIAL) pAppCtx->coordMode = OBJECT;
		else pAppCtx->coordMode = INERTIAL;
		break;
	case 'w':
		if(pAppCtx->bWireFrame) pAppCtx->bWireFrame = false;
		else pAppCtx->bWireFrame = true;
		break;
	case 'a':
		pAppCtx->bShadow = !pAppCtx->bShadow;
		break;
	case 'p':
		pAppCtx->bParamShow = !pAppCtx->bParamShow;
		break;
	case 'c':
		pAppCtx->bCoord = !pAppCtx->bCoord;
		break;
	case 'F':
		pAppCtx->bDispFloor = !pAppCtx->bDispFloor;
		break;
	case 'T':
		pAppCtx->bTrack = !pAppCtx->bTrack;
		break;
	case 'S':
		pAppCtx->bStrobo = !pAppCtx->bStrobo;
		break;		
	default:
		break;
  }
}

static void special(int key, int x, int y){
	struct AppCtx *pAppCtx = &g_theApp;

	if(pAppCtx->shiftMode == TRANSLATE){
		//選択剛体の平行移動
		float d = 0.1f;
		switch(key) {
		case GLUT_KEY_RIGHT:
			break;
		}		
	}
	
	return;
}


static void idle(void){
	glutPostRedisplay();
	return;
}

int main(int argc, char **argv){

	glutInit(&argc, argv);

	//表示モード
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	
	glutInitWindowSize(g_theApp.width, g_theApp.height);
	//左上の位置
	glutInitWindowPosition(500, 100);
	//ウインドウの作成
	glutCreateWindow("GLUT_Projectile");
	//ウインドウのサイズ変更
	glutReshapeFunc(resize);
	//表示
	glutDisplayFunc(display);
	//キーボードの利用
	glutKeyboardFunc(keyboard);

	glutIdleFunc(idle);
	
	init(&g_theApp);

	initGLSL(&g_theApp, "shading.vert", "shading.frag");

	glutMainLoop();

	
	return 0;
}
