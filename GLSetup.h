#pragma once

#define GLEW_STATIC 

#include <Windows.h>
#include "GL\glew.h"

HDC initGL(HWND parent);
int loadShaders(const wchar_t* vertshader, const wchar_t* fragshader);
