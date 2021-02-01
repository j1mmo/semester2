#include <crtdbg.h>

#include "Object3D.h"
#include "Utils.h"
#include "RenderingContext.h"



HWND hwnd;
int scenerotations[] = { 0,0,0 };
POINT lastpoint;
RenderingContext rcontext;
Object3D* sphere;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void onSize(DWORD type, UINT cx, UINT cy);
void onKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
void onMouseMove(UINT nFlags, int x, int y);
void onLButtonDown(UINT nFlags, int x, int y);
void onLButtonUp(UINT nFlags, int x, int y);
void onTimer(UINT wParam);
void cleanUp();
void onCreate();
void onDraw();
void createObject();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // This mini section is really useful to find memory leaks
#ifdef _DEBUG   // only include this section of code in the DEBUG build
    //  _CrtSetBreakAlloc(65);  // really useful line of code to help find memory leaks
    _onexit(_CrtDumpMemoryLeaks); // check for memory leaks when the program exits
#endif
    
  // To create a window, we must first define various attributes about it
    WNDCLASSEX classname;
    classname.cbSize = sizeof(WNDCLASSEX);
    classname.style = CS_HREDRAW | CS_VREDRAW;
    classname.lpfnWndProc = (WNDPROC)WndProc;   // This is the name of the event-based callback method
    classname.cbClsExtra = 0;
    classname.cbWndExtra = 0;
    classname.hInstance = hInstance;
    classname.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    classname.hCursor = LoadCursor(NULL, IDC_ARROW);
    classname.hbrBackground = NULL;//::GetSysColorBrush(COLOR_3DFACE);
    classname.lpszMenuName = NULL;
    classname.lpszClassName = L"GettingStartedWithOpenGL";
    classname.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    RegisterClassEx(&classname);

    // Default size will be 800x600 and we'll centre it on the screen - this include the caption and window borders so might not be the canvas size (which will be smaller)
    int width = 1024;
    int height = 768;
    int offx = (::GetSystemMetrics(SM_CXSCREEN) - width) / 2;
    int offy = (::GetSystemMetrics(SM_CYSCREEN) - height) / 2;

    // Create the window using the definition provided above
    CreateWindowEx(NULL, L"GettingStartedWithOpenGL", L"Getting Started with OpenGL", WS_OVERLAPPEDWINDOW | WS_VISIBLE, offx, offy, width, height, NULL, NULL, hInstance, NULL);

    // Set the event-based message system up
    MSG msg;
    PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
    while (msg.message != WM_QUIT)  // keep looping until we get the quit message
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
 //       if (GetMessage(&msg, NULL, 0, 0)) // cause this thread to wait until there is a message to process
        {
            // These two lines of code take the MSG structure, mess with it, and correctly dispatch it to the WndProc defined during the window creation
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            onDraw();
        }
    }
    ::SetTimer(hwnd, 1, 15, NULL);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        hwnd = hWnd;
        onCreate();
        return 0;
        break;
    case WM_SIZE:
        onSize((DWORD)wParam, (UINT)(lParam & 0xFFFF), (UINT)(lParam >> 16));
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT paint;
        BeginPaint(hwnd, &paint);
        onDraw();
        EndPaint(hwnd, &paint);
    }
    break;
    case WM_TIMER:
        onTimer(wParam);
        onDraw();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        cleanUp();
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        onLButtonDown((UINT) wParam, (int) (lParam&0xFFFF), (int) ((lParam>>16)&0xFFFF));
        break;
    case WM_LBUTTONUP:
        onLButtonUp((UINT) wParam, (int) (lParam&0xFFFF), (int) ((lParam>>16)&0xFFFF));
        break;
    case WM_MOUSEMOVE:
        onMouseMove((UINT) wParam, (int) (lParam&0xFFFF), (int) ((lParam>>16)&0xFFFF));
        break;
    case WM_KEYDOWN:
        onKeyDown(wParam, lParam&0xFFFF, lParam>>16);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

void createObject()
{
    float eye[] = { .0f, .0f , 2.0f };
    float centre[] = { .0f, .0f , .0f };
    float up[] = { .0f, 1.0f , .0f };

    Matrix::setLookAt(rcontext.viewmatrix, eye, centre, up);
    sphere = Object3D::loadObject(L"Sphere.obj");
    sphere->createVBOs();
}

void onCreate()
{
    initGL(hwnd);
    GLenum err = glewInit();
    if (err != GLEW_OK)
        displayMessage((char*)glewGetErrorString(err));

    rcontext.glprogram = loadShaders(L"vertShader.vert",L"fragShader.frag");
    glUseProgram(rcontext.glprogram);
    rcontext.poshandle = glGetAttribLocation(rcontext.glprogram, "a_position");
    rcontext.normalhandle = glGetAttribLocation(rcontext.glprogram, "a_normal");
    rcontext.mvphandle = glGetUniformLocation(rcontext.glprogram, "u_mvpmatrix");
    glEnableVertexAttribArray(rcontext.poshandle);
    glEnableVertexAttribArray(rcontext.normalhandle);

    createObject();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

   // ::SetTimer(hwnd, 101, 30, NULL);
}

void onTimer(UINT nIDEvent)
{
    if(nIDEvent == 101)
    {
        onDraw();
    }
}
static DWORD start_time=::GetTickCount();

void onDraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    rcontext.initModelMatrix(true);
    rcontext.rotateY(scenerotations[0]);
    rcontext.rotateX(scenerotations[1]);
    rcontext.rotateZ(scenerotations[2]);
    for (auto y = -2.0f; y <= 3.0f; y += 1.0)
    {
        for (auto x = -2.0f; x <= 2.0f; x += 1.0)
        {
            rcontext.pushModelMatrix();
            rcontext.scale(0.25, 0.25, 0.25);
            rcontext.translate(x, y, 0);
            rcontext.updateMVPs();
            glUniformMatrix4fv(rcontext.mvphandle, 1, false, rcontext.mvpmatrix);
            sphere->drawObject(rcontext);
            rcontext.popModelMatrix();
        }
    }
    

    glFinish();
    SwapBuffers(wglGetCurrentDC());
}

void cleanUp()
{
    delete sphere;
    glDeleteProgram(rcontext.glprogram);

}

void onSize(DWORD type, UINT cx, UINT cy)
{
    if (cx > 0 && cy > 0)
    {
        glViewport(0, 0, cx, cy);
        const float FOVY = (60.0f * (float)M_PI / 180.f);
        const float NEAR_CLIP = 0.01f;
        const float FAR_CLIP = 100.0f;

        float fAspect = (float)cx / cy;
        float top = NEAR_CLIP * tanf(FOVY/2);
        float bottom = -top;
        float left = fAspect * bottom;
        float right = fAspect * top;

        
        Matrix::setFrustum(rcontext.projectionmatrix, left, right, bottom, top, NEAR_CLIP, FAR_CLIP);
        
    }
}

// a key has been pressed while this window has focus
void onKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

// when the left mouse button is pressed down
void onLButtonDown(UINT nFlags, int x, int y)
{
    /*RECT screen;
    GetClientRect(hwnd, &screen);
    y = screen.bottom - y;*/
    
    lastpoint.x = x;
    lastpoint.y = y;
}

// when the left mouse button is released
void onLButtonUp(UINT nFlags, int x, int y)
{

}

// when the mouse moves across the window.  use nFlags to determine if button is down or not plus other stuff
void onMouseMove(UINT nFlags, int x, int y)
{
    if (nFlags & MK_LBUTTON)
    {
        int dx = x - lastpoint.x;
        int dy = y - lastpoint.y;

        if (nFlags & MK_CONTROL)
            scenerotations[2] -= dx;
        else
        {
            scenerotations[0] += dx;
            scenerotations[1] += dy;
        }

        lastpoint.x = x;
        lastpoint.y = y;

        onDraw();
    }
}
