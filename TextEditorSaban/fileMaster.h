#pragma once
#include "ui.h"

typedef struct File
{
    FILE* file;
    unsigned long length;
} File;

typedef UINT8 Cbool;

void saveFile();
void saveAsFile();
File loadFile();
Cbool isFileNameExist();
DWORD WINAPI loadFileToText(LPVOID lpParam);
void trimQuotes(gchar* str);
