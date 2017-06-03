#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <sys/epoll.h>

#include <pthread.h>
#include <wx/wx.h>

class MyApp: public wxApp
{
public:
	virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:
	MyFrame();
private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
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
        : wxFrame(NULL, wxID_ANY, "Hello World")
{
	wxMenu *menuFile = new wxMenu;
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
	SetStatusText( "Welcome to wxWidgets!" );
	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
	Close( true );
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox( "This is a wxWidgets' Hello world sample",
				  "About Hello World", wxOK | wxICON_INFORMATION );
}
void MyFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello world from wxWidgets!");
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

