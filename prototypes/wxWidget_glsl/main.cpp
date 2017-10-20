#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>
#include <pthread.h>

#include <GL/glu.h>
#include <GL/gl.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>

class BasicGLPanel: public wxGLCanvas {
private:
	wxGLContext *m_pContext;

public:
	BasicGLPanel(wxFrame *pParent, int *args);
	~BasicGLPanel();

	void resized(wxSizeEvent &evt);

	int getWidth();
	int getHeight();

	void render(wxPaintEvent& evt);
	void prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);
	void prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y);

	void mouseMoved(wxMouseEvent& event);
	void mouseDown(wxMouseEvent& event);
	void mouseWheelMoved(wxMouseEvent& event);
	void mouseReleased(wxMouseEvent& event);
	void rightClick(wxMouseEvent& event);
	void mouseLeftWindow(wxMouseEvent& event);
	void keyPressed(wxKeyEvent& event);
	void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(BasicGLPanel, wxGLCanvas)
EVT_MOTION(BasicGLPanel::mouseMoved)
EVT_LEFT_DOWN(BasicGLPanel::mouseDown)
EVT_LEFT_UP(BasicGLPanel::mouseReleased)
EVT_RIGHT_DOWN(BasicGLPanel::rightClick)
EVT_LEAVE_WINDOW(BasicGLPanel::mouseLeftWindow)
EVT_SIZE(BasicGLPanel::resized)
EVT_KEY_DOWN(BasicGLPanel::keyPressed)
EVT_KEY_UP(BasicGLPanel::keyReleased)
EVT_MOUSEWHEEL(BasicGLPanel::mouseWheelMoved)
EVT_PAINT(BasicGLPanel::render)
END_EVENT_TABLE()

void BasicGLPanel::mouseMoved(wxMouseEvent& event){};
void BasicGLPanel::mouseDown(wxMouseEvent& event){};
void BasicGLPanel::mouseWheelMoved(wxMouseEvent& event){};
void BasicGLPanel::mouseReleased(wxMouseEvent& event){};
void BasicGLPanel::rightClick(wxMouseEvent& event){};
void BasicGLPanel::mouseLeftWindow(wxMouseEvent& event){};
void BasicGLPanel::keyPressed(wxKeyEvent& event){};
void BasicGLPanel::keyReleased(wxKeyEvent& event){};

GLfloat v[8][3];
GLint faces[6][4] = {
	{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
	{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3}
};

BasicGLPanel::BasicGLPanel(wxFrame* parent, int* args) :
	wxGLCanvas(parent, wxID_ANY, args, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
{
	m_pContext = new wxGLContext(this);
	// prepare a simple cube to demonstrate 3D render
    // source: http://www.opengl.org/resources/code/samples/glut_examples/examples/cube.c
    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	//To avoid flashing on MSW
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

}

BasicGLPanel::~BasicGLPanel(){
	delete m_pContext;
}

void BasicGLPanel::resized(wxSizeEvent& event){
	// wxGLCanvas::OnSize(evt);
	Refresh();
}

void BasicGLPanel::prepare3DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y){

	float ratio_w_h;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glClearDepth(1.0f);      // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);  // The Type of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_COLOR_MATERIAL);

	glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ratio_w_h = static_cast<float>(bottomrigth_x-topleft_x)/static_cast<float>(bottomrigth_y-topleft_y);
	gluPerspective(45.0f, ratio_w_h, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;
}

void BasicGLPanel::prepare2DViewport(int topleft_x, int topleft_y, int bottomrigth_x, int bottomrigth_y){

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(topleft_x, topleft_y, bottomrigth_x-topleft_x, bottomrigth_y-topleft_y);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	gluOrtho2D(topleft_x, bottomrigth_x, bottomrigth_y, topleft_y);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;
}

int BasicGLPanel::getWidth(){
	return GetSize().x;
}

int BasicGLPanel::getHeight(){
	return GetSize().y;
}

void BasicGLPanel::render( wxPaintEvent& event ){
	if(!IsShown()) return;

	wxGLCanvas::SetCurrent(*m_pContext);
	wxPaintDC(this);

	// ------------- draw some 2D ----------------
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	prepare2DViewport(0,0,getWidth()/2, getHeight());
	glLoadIdentity();
#if 1
	// white background
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(getWidth(), 0.0f, 0.0f);
	glVertex3f(getWidth(), getHeight(), 0.0f);
	glVertex3f(0.0f, getHeight(), 0.0f);
	glEnd();

	// red square
	glColor4f(1, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex3f(getWidth()/8, getHeight()/3, 0);
    glVertex3f(getWidth()*3/8, getHeight()/3, 0);
    glVertex3f(getWidth()*3/8, getHeight()*2/3, 0);
    glVertex3f(getWidth()/8, getHeight()*2/3, 0);
    glEnd();

    // ------------- draw some 3D ----------------
    prepare3DViewport(getWidth()/2,0,getWidth(), getHeight());
    glLoadIdentity();

    glColor4f(0,0,1,1);
    glTranslatef(0,0,-5);
    glRotatef(50.0f, 0.0f, 1.0f, 0.0f);

    glColor4f(1, 0, 0, 1);
    for (int i = 0; i < 6; i++)
    {
        glBegin(GL_LINE_STRIP);
        glVertex3fv(&v[faces[i][0]][0]);
        glVertex3fv(&v[faces[i][1]][0]);
        glVertex3fv(&v[faces[i][2]][0]);
        glVertex3fv(&v[faces[i][3]][0]);
        glVertex3fv(&v[faces[i][0]][0]);
        glEnd();
    }
#endif

    glFlush();
    SwapBuffers();

	return;
}

class MyApp: public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame: public wxFrame
{
private:
	BasicGLPanel* m_pGLPanel;
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
public:
	MyFrame();
};

enum
{
	ID_Hello = 1
};

bool MyApp::OnInit()
{
	MyFrame *frame = new MyFrame();
	frame->Show( true );
	return true;
}

MyFrame::MyFrame()
        : wxFrame(NULL, wxID_ANY, "Simple Open GL")
{
	wxMenu *menuFile = new wxMenu;
	int args[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
					 "Help string shown in status bar for this menu item");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, "&File" );
	menuBar->Append( menuHelp, "&Help" );
	SetMenuBar( menuBar );
	CreateStatusBar();
	SetStatusText( "OpenGL display on wxWidget!" );
	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

	m_pGLPanel = new BasicGLPanel(dynamic_cast<wxFrame*>(this), args);
	SetSizer(sizer);
	SetAutoLayout(true);

	sizer->Add(m_pGLPanel, 1, wxEXPAND);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close( true );
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox( "This is a wxWidgets' Simple Open GL sample",
				  "About Simple Open GL", wxOK | wxICON_INFORMATION );
}
void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Simple Open GL from wxWidgets!");
}

struct AppContext {
	bool bTerm;
	int termfd;
};

static void *mainThread(void *pCookie){
	struct AppContext *pApp =
		reinterpret_cast<struct AppContext *>(pCookie);

	printf("start main thread\n");

	wxTheApp->CallOnInit();
	wxTheApp->OnRun();
	wxTheApp->OnExit();

	printf("stop main thread\n");

	{
		uint64_t value = 1;
		write(pApp->termfd, &value, sizeof(value));
	}
	pApp->bTerm = true;

	return NULL;
}

wxIMPLEMENT_APP_NO_MAIN(MyApp);
int main(int argc, char **argv){
	int ret;
	struct AppContext appCtx = {
		.bTerm = false,
		.termfd = -1
	};
	pthread_t thId;
	sigset_t mask;
	struct signalfd_siginfo fdsi;
	struct epoll_event ev;
	int epollfd=-1,sigfd=-1;

	epollfd = epoll_create(10);
	if(epollfd == -1){
		fprintf(stderr, "failed to create epoll. %s(%d)\n", strerror(errno), errno);
		return 0;
	}

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	sigaddset(&mask, SIGTERM);

	ret = sigprocmask(SIG_BLOCK, &mask, NULL);
	if(ret != 0){
		fprintf(stderr, "failed to signal mask. %s(%d)\n", strerror(errno), errno);
		goto error_exit;
	}

	sigfd = signalfd(-1, &mask, 0);
	if(sigfd == -1){
		fprintf(stderr, "failed to create signal fd. %s(%d)\n", strerror(errno), errno);
		goto error_exit;
	}

	appCtx.termfd = eventfd(0, 0);
	if(appCtx.termfd == -1){
		fprintf(stderr, "failed to create event fd. %s(%d)\n", strerror(errno), errno);
		goto error_exit;
	}

	ev.events  = EPOLLIN;
	ev.data.fd = sigfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, sigfd, &ev);

	ev.events  = EPOLLIN;
	ev.data.fd = appCtx.termfd;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, appCtx.termfd, &ev);

	wxEntryStart(argc, argv);

	ret = pthread_create(&thId, NULL, mainThread, &appCtx);
	if(ret != 0){
		goto error_cleanup;
	}

	while(!appCtx.bTerm){
		uint32_t i;
		struct epoll_event evs[8];
		ret = epoll_wait(epollfd, evs, sizeof(evs)/sizeof(evs[0]), -1);
		for(i=0;i<ret;i++){
			if(evs[i].data.fd == sigfd){
				struct signalfd_siginfo fdsi;
				read(sigfd, &fdsi, sizeof(fdsi));
				appCtx.bTerm = true;
			}
			else if(evs[i].data.fd == appCtx.termfd){
				uint64_t value;
				read(appCtx.termfd, &value, sizeof(value));
				appCtx.bTerm = true;
			}
		}
	}

	wxTheApp->ExitMainLoop();
	pthread_join(thId, NULL);
 error_cleanup:

	wxEntryCleanup();
 error_exit:

	if(appCtx.termfd != -1){
		close(appCtx.termfd);
	}

	if(sigfd != -1){
		close(sigfd);
	}

	if(epollfd != -1) {
		close(epollfd);
	}

	return 0;
}

