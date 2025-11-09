#include <stdio.h>
#include <gtk/gtk.h>
#include <Windows.h>

#pragma warning(disable:4996)

#define WIDGET_COUNT 12
#define WIDTH 600
#define HEIGHT 400

#define TITLE_TEXT "Saban Text Editor - "

typedef struct TextPTR
{
    gchar* text;
    gchar* textName;
} TextPTR;


typedef struct File
{
    FILE* file;
    unsigned long length;
} File;

static void activate(GtkApplication* app, gpointer user_data);
void saveFile();
void saveAsFile();
File loadFile();
void loadFileOption();
void openOptionsDialog();
void reloadCss(GtkWidget* widget);
DWORD WINAPI loadFileToText(LPVOID lpParam);
gboolean updateTextViewOnMainThread(TextPTR* textPtr);
void trimQuotes(gchar* str);

GtkTextView* textView = NULL; 
gchar* filename = NULL;
GtkWidget* appWindow = NULL;

void show_message(const char* message)
{
    //i needed to show a message so i convert it to UTF-8
    int size = MultiByteToWideChar(CP_UTF8, 0, message, -1, NULL, 0);
    wchar_t* wmessage = (wchar_t*)malloc(size * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, message, -1, wmessage, size);

    MessageBox(NULL, wmessage, L"Info!", MB_OK | MB_ICONINFORMATION);
    free(wmessage);
}

void trimQuotes(gchar* str) 
{
    // removing the quotes
    int len = strlen(str);
    if (len > 0 && str[0] == '"')
    {
        memmove(str, str + 1, len - 1);
        str[len - 2] = '\0'; 
    }
}
#if _DEBUG
int main()
{
    GtkApplication* app = NULL;
    int status = 0;
    app = gtk_application_new("my.texteditor", G_APPLICATION_FLAGS_NONE);
    if (!app) {
        show_message("Failed to initialize GTK app!");
        return 0;
    }
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    //getting the fileName from the openwith
    //if (*lpCmdLine != '\0')
    //{
    //    filename = g_strdup(lpCmdLine);
    //    trim_quotes(filename);
    //}
    status = g_application_run(G_APPLICATION(app), NULL, NULL);
    g_object_unref(app);
    if (filename) g_free(filename);
    return 0;
}

#else

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GtkApplication* app = NULL;
<<<<<<< Updated upstream
    int status = 0;
    app = gtk_application_new("my.texteditor", G_APPLICATION_FLAGS_NONE);
    if (!app) {
        show_message("Failed to initialize GTK app!");
=======

    //getting the app
    app = initApp();
    //getting the files from command
    if (*lpCmdLine != '\0')
    {
        state->filename = g_strdup(lpCmdLine);
        trimQuotes(state->filename);
    }

    g_application_run(G_APPLICATION(app), NULL, NULL);
    g_object_unref(app);
    freeAppState(state);
    return 0;
}
#endif
    

GtkApplication* initApp()
{
    g_setenv("GSETTINGS_SCHEMA_DIR", "share/glib-2.0/schemas", TRUE);
    GtkApplication* app = gtk_application_new("my.texteditor", G_APPLICATION_FLAGS_NONE);
    if (!app)
    {
        showMessage("Failed to initialize GTK app!");
>>>>>>> Stashed changes
        return 0;
    }
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    //getting the fileName from the openwith
    if (*lpCmdLine != '\0')
    {
        filename = g_strdup(lpCmdLine);
        trimQuotes(filename);
    }
    status = g_application_run(G_APPLICATION(app), NULL, NULL);
    g_object_unref(app);
    if (filename)
    {
        g_free(filename);
    }
    return 0;
}
#endif

static void activate(GtkApplication* app, gpointer user_data) 
{
    GdkPixbuf* appIcon = NULL;
    GtkWidget* appBox = NULL;
    GtkWidget* menuBar = NULL;
    GtkWidget* fileMenu = NULL;
    GtkWidget* optionsMenu = NULL;
    GtkWidget* fileMenuFile = NULL;
    GtkWidget* fileMenuSave = NULL;
    GtkWidget* fileMenuSaveAs = NULL;
    GtkWidget* fileMenuLoad = NULL;
    GtkWidget* fileMenuQuit = NULL;
    GtkWidget* sep = NULL;
    GtkWidget* fileMenuOptions = NULL;
    GtkWidget* scrolledWindow = NULL;
    GtkAccelGroup* accelGroup = NULL;

    //intializing the app
    appWindow = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(appWindow), "Saban Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(appWindow), WIDTH, HEIGHT);
    gtk_window_set_position(GTK_WINDOW(appWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_from_file(GTK_WINDOW(appWindow), "icon.png", NULL);
    //creating an app box
    appBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(appWindow), appBox);
    //creating a menu bar
    menuBar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(appBox), menuBar, FALSE, FALSE, 0);
    //creating options
    textView = gtk_text_view_new();
    loadFile();
    fileMenu = gtk_menu_new();
    fileMenuFile = gtk_menu_item_new_with_label("File");
    fileMenuSave = gtk_menu_item_new_with_label("Save");
    fileMenuSaveAs = gtk_menu_item_new_with_label("Save As");
    fileMenuLoad = gtk_menu_item_new_with_label("Load");
    fileMenuQuit = gtk_menu_item_new_with_label("Quit");
    sep = gtk_separator_menu_item_new();
    optionsMenu = gtk_menu_new();
    fileMenuOptions = gtk_menu_item_new_with_label("Options");
    //adding the options to the order
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenuFile), fileMenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMenuOptions), optionsMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), fileMenuFile);
    gtk_menu_shell_append(GTK_MENU_SHELL(menuBar), fileMenuOptions);
    //widget arr for appending
    GtkWidget* widgetArr[WIDGET_COUNT] = { appWindow, menuBar, fileMenu, optionsMenu, fileMenuFile, fileMenuSave, fileMenuSaveAs, fileMenuLoad, fileMenuQuit, sep, fileMenuOptions, textView };
    //appending only needed staff
    for (int i = 5; i < 10; i++)
    {
        gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), widgetArr[i]);
    }
    //loading each widget the css for it

    for (int i = 0; i < WIDGET_COUNT; i++)
    {
        reloadCss(widgetArr[i]);
    }
    //creating a text view which could be scrolled when needed
    scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), textView);

    gtk_box_pack_start(GTK_BOX(appBox), scrolledWindow, TRUE, TRUE, 0);
    // showing the widgets
    gtk_widget_show_all(appWindow);
    // calling for each button its function
    g_signal_connect(appWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(fileMenuSave, "activate", G_CALLBACK(saveFile), NULL);
    g_signal_connect(fileMenuSaveAs, "activate", G_CALLBACK(saveAsFile), NULL);
    g_signal_connect(fileMenuLoad, "activate", G_CALLBACK(loadFileOption), NULL);
    g_signal_connect(fileMenuQuit, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(fileMenuOptions, "activate", G_CALLBACK(openOptionsDialog), NULL);
    //when pressing ctrl + s will call the save function
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(appWindow), accelGroup);
    gtk_accel_group_connect(accelGroup, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(saveFile), NULL, NULL));
}

void saveFile()
{
    FILE* savedFile = NULL;
    GtkTextBuffer* buffer = NULL;
    GtkTextIter end;
    GtkTextIter start;
    gchar* text = NULL;
    //checking there is a file to open
    if (filename == NULL || !strcmp(filename, ""))
    {
        saveAsFile();
        return 0;
    }

    //if there is we get the buffer and the text
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    //open the file and writ to it closing it and freeing the memory of the text
    savedFile = fopen(filename, "w");
        fprintf(savedFile, "%s", text);
        fflush(savedFile);
        fclose(savedFile);
        g_free(text);
}

void saveAsFile()
{
    GtkWidget* dialog;
    GtkWidget* parentWindow = gtk_widget_get_toplevel(GTK_WIDGET(textView));
    GtkFileChooser* fileChooser;
    //choosing where to open which will give us the file name
    dialog = gtk_file_chooser_dialog_new("Save As",
        NULL,
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save", GTK_RESPONSE_ACCEPT,
        NULL);

    fileChooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_do_overwrite_confirmation(fileChooser, TRUE);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) 
    {
        filename = gtk_file_chooser_get_filename(fileChooser);
        printf("File selected: %s\n", filename);

        if (filename == NULL) 
        {
            return 0;
        }
        //saving the file
        saveFile();
    }

    gtk_widget_destroy(dialog);
}

void loadFileOption() 
{
    GtkWidget* dialog = NULL;
    GtkWidget* parentWindow = gtk_widget_get_toplevel(GTK_WIDGET(textView));
    GtkFileChooser* fileChooser = NULL;
    // loading the filee like the save as
    dialog = gtk_file_chooser_dialog_new("Open",
        GTK_WINDOW(parentWindow),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    fileChooser = GTK_FILE_CHOOSER(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) 
    {
        // getting the filename
        if (filename)
        {
            g_free(filename);
        }
        filename = gtk_file_chooser_get_filename(fileChooser);
        //reading from the file name and getting it into a file
        DWORD  dwThreadId;
        HANDLE loadThread = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            loadFileToText,       // thread function name
            NULL,          // argument to thread function 
            0,                      // use default creation flags 
            &dwThreadId);   // returns the thread identifier 

    }

    gtk_widget_destroy(dialog);
}

void openOptionsDialog() 
{
    fprintf(stdout, "[*] %s\n", "Open options..");
}

File loadFile()
{
    File file;
    if (filename == NULL)
    {
        return;
    }
    file.file = fopen(filename, "r");
    if (file.file == NULL)
    {
        show_message("Couldnt Open!");
    }
    fseek(file.file, 0, SEEK_END);
    file.length = ftell(file.file);
    fseek(file.file, 0, SEEK_SET);
    return file;
}


DWORD WINAPI loadFileToText(LPVOID lpParam)
{
    GtkTextBuffer* buffer = NULL;
    File file = loadFile();
    TextPTR* textPtr = (TextPTR*)malloc(sizeof(TextPTR));
    
    textPtr->text = (gchar*)malloc(sizeof(char) * (file.length) + 1);
    textPtr->textName = (gchar*)malloc(sizeof(char) * (strlen(filename) + strlen(TITLE_TEXT)) + 1);
    if (textPtr->text)
    {
        fread(textPtr->text, 1, file.length, file.file);
        textPtr->text[file.length] = '\0';
        fclose(file.file);

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
        if (buffer == NULL)
        {
            show_message("Failed to get text buffer");
            return;
        }
        strcpy(textPtr->textName, TITLE_TEXT);
        strcat(textPtr->textName, filename);
    }

    g_idle_add((GSourceFunc)updateTextViewOnMainThread, textPtr);
    return 0;
}

gboolean updateTextViewOnMainThread(TextPTR* textPtr) 
{
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
    gtk_window_set_title(GTK_WINDOW(appWindow), textPtr->textName);
    gtk_text_buffer_set_text(buffer, textPtr->text, -1);
    free(textPtr->textName);
    free(textPtr->text);
    free(textPtr);
    return FALSE; // remove from idle queue
}

void reloadCss(GtkWidget* widget)
{
    GtkStyleContext* style_context = NULL;
    GtkCssProvider* css_provider = gtk_css_provider_new();
    gchar* css_data =
        "\n"
        "window {\n"
        "    background-color: #696564; /* Dark gray background for the window */\n"
        "    color: #ffffff; /* White text color for menus */\n"
        "}\n"
        "menubar {\n"
        "    background: #3e3e3e; \n"
        "}\n"
        "menu, menuitem {\n"
        "    background: #3e3e3e; /* Medium gray background for menus */\n"
        "    color: #ffffff; /* White text color for menus */\n"
        "}\n"
        "    menu:active, menuitem:active {\n"
        "        background: #4e4e4e; /* Slightly lighter gray for menu item hover state */\n"
        "    }\n"
        "    menu:hover, menuitem:hover {\n"
        "        background: #4e4e4e; /* Slightly lighter gray for menu item hover state */\n"
        "    }\n"
        "    menu:focus, menuitem:focus {\n"
        "        background: #3e3e3e; /* Background color for focused menu items */\n"
        "        color: #ffffff; /* Text color for focused menu items */\n"
        "    }\n"
        "textview {\n"
        "    background: #313131;\n"
        "    font-family: \"Courier New\", monospace;\n"
        "    font-size: 14px;\n"
        "    border: 1px solid #313131;\n"
        "    padding-left: 15px;\n"
        "}\n"
        "    textview text {\n"
        "        background: #696564;\n"
        "        color: white;\n"
        "        padding: 0px;\n"
        "        margin:0px;\n"
        "    }\n";
    //loading the css info!
    gtk_css_provider_load_from_data(css_provider, css_data, -1, NULL);
    style_context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(style_context, GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);
}
