#include "GLSetup.h"
#include <stdio.h>
#include <io.h>
#include "Utils.h"

BOOL setupPixelFormat(HDC hdc);
int loadProgram(int type, const wchar_t* filename);
bool linkProgram(int program, int shader, int fragment);

HDC initGL(HWND parent)
{
  HDC hdcNew=::GetDC(parent);

  if (!setupPixelFormat(hdcNew))
    return 0;

  HGLRC hrc=wglCreateContext(hdcNew);
  wglMakeCurrent(hdcNew, hrc);

  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);

  return hdcNew;
}

BOOL setupPixelFormat(HDC hdc)
{
  int pixelformat;
  PIXELFORMATDESCRIPTOR pfd = 
	{
    sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
    1,                              // version number
    PFD_DRAW_TO_WINDOW |            // support window
    PFD_SUPPORT_OPENGL |            // support OpenGL
    PFD_DOUBLEBUFFER,               // double buffered
    PFD_TYPE_RGBA,                  // RGBA type
    32,                             // 32-bit color depth
    0, 0, 0, 0, 0, 0,               // color bits ignored
    0,                              // no alpha buffer
    0,                              // shift bit ignored
    0,                              // no accumulation buffer
    0, 0, 0, 0,                     // accum bits ignored
    32,                             // 32-bit z-buffer
    32,                             // 32-bit stencil buffer  - !no stencil buffer
    0,                              // no auxiliary buffer
    PFD_MAIN_PLANE,                 // main layer
    0,                              // reserved
    0, 0, 0                         // layer masks ignored
  };

  if (!(pixelformat=ChoosePixelFormat(hdc, &pfd)))
  {
    MessageBox(NULL, L"ChoosePixelFormat failed", L"GLSetup", MB_OK);
    return FALSE;
  }

  if (!SetPixelFormat(hdc, pixelformat, &pfd))
  {
    MessageBox(NULL, L"SetPixelFormat failed", L"GLSetup", MB_OK);
    return FALSE;
  }
  return TRUE;
}

int loadShaders(const wchar_t* vertshader, const wchar_t* fragshader)
{
  int vhandle=loadProgram(GL_VERTEX_SHADER, vertshader); 
  int fhandle=loadProgram(GL_FRAGMENT_SHADER, fragshader);

  if (vhandle!=0 && fhandle!=0)
  {
    int phandle=glCreateProgram();
    if (linkProgram(phandle, vhandle, fhandle))
      return phandle;
    else
    {
      glDeleteProgram(phandle);
      glDeleteShader(fhandle);
      glDeleteShader(vhandle);
    }
  }
  return 0;
}

int loadProgram(int type, const wchar_t* filename)
{
  int shader=glCreateShader(type);
    
  if (shader!=0)
  {
    FILE* file=NULL;
    _wfopen_s(&file, filename, L"rb");
    if (file)
    {
      int len=_filelength(_fileno(file));
      char* data=(char*) malloc(len);
      int read=0;
      int pos=0;
      do
      {
        read=fread(data+pos, 1, len-pos, file);
        if (read>0)
          pos+=read;
      }
      while (read>0 && pos!=len);
      fclose(file);

      glShaderSource(shader, 1, &data, &len);
      glCompileShader(shader);
      free(data);
      
      int status;
      glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

      if(status==GL_FALSE) 
      {
        // get the shader info log
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        char* error=(char*) malloc(len+1);
        glGetShaderInfoLog(shader, len, &status, error);
        error[len]='\0';
        displayMessage(error);
        free(error);

        glDeleteShader(shader);
        shader=0;
      }
    }
    else
    {
      glDeleteShader(shader);
      shader=0;
    }
  }
  return shader;
}

bool linkProgram(int program, int shader, int fragment)
{
  glAttachShader(program, shader);
  glAttachShader(program, fragment);

  glLinkProgram(program);
      
  int status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if(status==GL_FALSE)
  {
    int length;

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    char* error=(char*) malloc(length+1);
    glGetProgramInfoLog(program, length, &status, error);
    error[length]='\0';
    displayMessage(error);
    free(error);
    return false;
  }
  return true;
}
