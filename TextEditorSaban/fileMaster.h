#pragma once
#include "ui.h"

typedef struct File
{
    FILE* file;
    unsigned long length;
} File;

//saving files and loading
void saveFile();
void saveAsFile();
File loadFile();

//file checks
gboolean isFileNameExist();
void trimQuotes(gchar* str);

//background text handling
DWORD WINAPI loadFileToText(LPVOID lpParam);
void buildTextName(TextPTR* ptr);
TextPTR* createTextPtr(File file);

