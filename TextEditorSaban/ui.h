#pragma once
#include <gtk/gtk.h>
#include <Windows.h>
#include <gtksourceview/gtksource.h>

#pragma warning(disable:4996)
// main sizes
#define WIDGET_COUNT 17
#define WIDTH 600
#define HEIGHT 400
// resize max
#define MAX_ZOOM 60
#define MIN_ZOOM 6

#define TITLE_TEXT "Saban Text Editor - "

typedef struct AppState
{
    GtkTextView* textView;
    gchar* filename;
    GtkWidget* appWindow;
    int currentFontSize;
} AppState;

typedef struct TextPTR
{
    gchar* text;
    gchar* textName;
} TextPTR;

extern AppState* state;

void activateUI(GtkApplication* app, gpointer userData);
void openOptionsDialog();
void reloadCss(GtkWidget* widget);
gboolean updateTextViewOnMainThread(TextPTR* textPtr);
void showMessage(const char* message);
void loadFileOption();
void aboutOption();
void setFontSize(int fontSize);
void zoomIn();
void zoomOut();