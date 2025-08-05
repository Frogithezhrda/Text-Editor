#include "ui.h"
#include "fileMaster.h"


AppState* state = NULL;

void showMessage(const char* message)
{
    //i needed to show a message so i convert it to UTF-8
    int size = MultiByteToWideChar(CP_UTF8, 0, message, -1, NULL, 0);
    wchar_t* wmessage = (wchar_t*)malloc(size * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, message, -1, wmessage, size);

    MessageBox(NULL, wmessage, L"Info!", MB_OK | MB_ICONINFORMATION);
    free(wmessage);
}

void activateUI(GtkApplication* app, gpointer user_data)
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
    state = (AppState*)calloc(1, sizeof(AppState));
    //intializing the app
    state->appWindow = gtk_application_window_new(app);

    gtk_window_set_title(GTK_WINDOW(state->appWindow), "Saban Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(state->appWindow), WIDTH, HEIGHT);
    gtk_window_set_position(GTK_WINDOW(state->appWindow), GTK_WIN_POS_CENTER);
    //creating an app box
    appBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(state->appWindow), appBox);
    //creating a menu bar
    menuBar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(appBox), menuBar, FALSE, FALSE, 0);
    //creating options
    state->textView = gtk_source_view_new();
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(state->textView), TRUE);
    GtkSourceBuffer* buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView)));
    GtkSourceStyleSchemeManager* mgr = gtk_source_style_scheme_manager_get_default();
    gtk_source_style_scheme_manager_append_search_path(mgr, "resources/styles/");
    GtkSourceStyleScheme* scheme = gtk_source_style_scheme_manager_get_scheme(mgr, "saban-dark");

    if (scheme)
        gtk_source_buffer_set_style_scheme(buf, scheme);
    else
        g_warning("Style scheme 'saban-dark' not found");

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
    GtkWidget* widgetArr[WIDGET_COUNT] = { state->appWindow, menuBar, fileMenu, optionsMenu, fileMenuFile, fileMenuSave, fileMenuSaveAs, fileMenuLoad, fileMenuQuit, sep, fileMenuOptions, state->textView };
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
    gtk_container_add(GTK_CONTAINER(scrolledWindow), state->textView);

    gtk_box_pack_start(GTK_BOX(appBox), scrolledWindow, TRUE, TRUE, 0);
    // showing the widgets
    gtk_widget_show_all(state->appWindow);
    // calling for each button its function
    g_signal_connect(state->appWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(fileMenuSave, "activate", G_CALLBACK(saveFile), NULL);
    g_signal_connect(fileMenuSaveAs, "activate", G_CALLBACK(saveAsFile), NULL);
    g_signal_connect(fileMenuLoad, "activate", G_CALLBACK(loadFileOption), NULL);
    g_signal_connect(fileMenuQuit, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(fileMenuOptions, "activate", G_CALLBACK(openOptionsDialog), NULL);
    //when pressing ctrl + s will call the save function
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(state->appWindow), accelGroup);
    gtk_accel_group_connect(accelGroup, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(saveFile), NULL, NULL));
}



void loadFileOption()
{
    GtkWidget* dialog = NULL;
    GtkWidget* parentWindow = gtk_widget_get_toplevel(GTK_WIDGET(state->textView));
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
        if (isFileNameExist())
        {
            g_free(state->filename);
        }
        state->filename = gtk_file_chooser_get_filename(fileChooser);
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

void reloadCss(GtkWidget* widget)
{
    GtkStyleContext* styleContext = NULL;
    GtkCssProvider* cssProvider = gtk_css_provider_new();
    gchar* cssData =
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
        "}\n"
        "    textview text {\n"
        "        background: #696564;\n"
        "        color: white;\n"
        "        padding: 0px;\n"
        "        margin: 0px;\n"
        "    }\n"
        "gtksourceview.view > gutter {\n"
        "    background-color: #313131;\n"
        "    padding-left: 3px;\n"
        "}\n"
        "gtksourceview.view .line-numbers {\n"
        "    color: white;\n"
        "    font-family: \"Courier New\", monospace;\n"
        "    font-size: 12px;\n"
        "}\n"
        "gtksourceview.view {\n"
        "    padding-left: 5px;\n"
        "    margin-left: 0px;\n"
        "    border-width: 0px;\n"
        "    background: #313131;\n"
        "}\n";

    gtk_css_provider_load_from_data(cssProvider, cssData, -1, NULL);
    styleContext = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(styleContext, GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(cssProvider);
}


gboolean updateTextViewOnMainThread(TextPTR* textPtr)
{
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView));
    gtk_window_set_title(GTK_WINDOW(state->appWindow), textPtr->textName);
    gtk_text_buffer_set_text(buffer, textPtr->text, -1);
    free(textPtr->textName);
    free(textPtr->text);
    free(textPtr);
    return FALSE; // remove from idle queue
}
