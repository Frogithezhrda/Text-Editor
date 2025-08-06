#pragma once
#include <gtk/gtk.h>
#include <Windows.h>
#include <gtksourceview/gtksource.h>
#include "widgetVector.h"

#pragma warning(disable:4996)
// main sizes
#define SEPARATORS_COUNT 4
#define WIDTH 600
#define HEIGHT 400
// resize max
#define MAX_ZOOM 60
#define MIN_ZOOM 8

#define ADD_ZOOM 4
#define DEFAULT_ZOOM 12

#define TITLE_TEXT "Saban Text Editor - "


typedef struct TextPTR
{
    gchar* text;
    gchar* textName;
} TextPTR;

typedef struct MenuItems {
    GtkWidget* file;
    GtkWidget* edit;
    GtkWidget* help;
} MenuItems;

typedef struct MenuActions {
    GtkWidget* save;
    GtkWidget* saveAs;
    GtkWidget* load;
    GtkWidget* quit;
    GtkWidget* about;
} MenuActions;

typedef struct ZoomControls {
    GtkWidget* zoomIn;
    GtkWidget* zoomOut;
    GtkWidget* zoomReset;
} ZoomControls;

typedef struct UIWidgets {
    GtkWidget* appWindow;
    GtkWidget* appBox;
    GtkWidget* menuBar;
    GtkWidget* textView;
    GtkWidget* scrolledWindow;

    GtkWidget* fileMenu;
    GtkWidget* helpMenu;
    GtkWidget* optionsMenu;

    MenuItems menus;
    MenuActions actions;
    ZoomControls zoom;

    WidgetVector* separators;

    GtkAccelGroup* accelGroup;
} UIWidgets;


typedef struct UITextView {
    GtkSourceBuffer* buf;
    GtkSourceStyleSchemeManager* mgr;
    GtkSourceStyleScheme* scheme;
    GtkSourceLanguageManager* lm;
    GtkSourceLanguage* lang;
} UITextView;

typedef struct AppState
{
    GtkTextView* textView;
    gchar* filename;
    GtkWidget* appWindow;
    unsigned int currentFontSize;

    UIWidgets* ui;
    UITextView* uiText;
} AppState;

extern AppState* state;

//Activate Handling
void activateUI(GtkApplication* app, gpointer userData);
void setupWindow(GtkApplication* app, UIWidgets* widgets);
void setupMenu(GtkApplication* app, UIWidgets* widgets);
void setupTextView(GtkApplication* app, UIWidgets* widgets);
void setupSignals(GtkApplication* app, UIWidgets* widgets);

//Css Handling
void reloadCss();

//Thread/Message Handling
void openLoadThread();
gboolean updateTextViewOnMainThread(TextPTR* textPtr);
void showMessage(const char* message);

//Zoom Handling
void resetZoom();
void zoomIn();
void zoomOut();
void setFontSize();

//Option Handling
void loadFileOption();
void aboutOption();
void quitOption(GtkMenuItem* menuitem, gpointer userData);