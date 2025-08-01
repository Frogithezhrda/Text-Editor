#pragma once
#include <gtk/gtk.h>
#include <Windows.h>

#pragma warning(disable:4996)

#define WIDGET_COUNT 12
#define WIDTH 600
#define HEIGHT 400

#define TITLE_TEXT "Saban Text Editor - "

typedef struct AppState
{
    GtkTextView* textView;
    gchar* filename;
    GtkWidget* appWindow;
} AppState;

typedef struct TextPTR
{
    gchar* text;
    gchar* textName;
} TextPTR;

extern AppState* state;

void activateUI(GtkApplication* app, gpointer user_data);
void openOptionsDialog();
void reloadCss(GtkWidget* widget);
gboolean updateTextViewOnMainThread(TextPTR* textPtr);
void showMessage(const char* message);
void loadFileOption();
