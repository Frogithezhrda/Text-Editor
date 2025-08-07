#include "ui.h"
#include "fileMaster.h"

AppState* state = NULL;

void activateUI(GtkApplication* app, gpointer userData)
{
    UIWidgets* ui = (UIWidgets*)malloc(sizeof(UIWidgets));
    DWORD  dwThreadId;

    //intializing the app
    setupWindow(app, ui);
    //creating a menu bar
    setupMenu(app, ui);
    //creating options
    setupTextView(app, ui);

    //opening the loading thread if needed to load then it would load
    openLoadThread();

    //loading each widget the css for it
    reloadCss();
    // showing the widgets
    gtk_widget_show_all(state->appWindow);
    // calling for each button its function
    setupSignals(app, ui);
    
    state->ui = ui;
}

//Setup Handling
void setupWindow(GtkApplication* app, UIWidgets* widgets)
{
    state->appWindow = gtk_application_window_new(app);
    state->currentFontSize = DEFAULT_ZOOM;
    gtk_window_set_title(GTK_WINDOW(state->appWindow), "Saban Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(state->appWindow), WIDTH, HEIGHT);
    gtk_window_set_position(GTK_WINDOW(state->appWindow), GTK_WIN_POS_CENTER);
    //creating an app box
    widgets->appBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(state->appWindow), widgets->appBox);
}

void setupMenu(GtkApplication* app, UIWidgets* widgets)
{
    widgets->menuBar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(widgets->appBox), widgets->menuBar, FALSE, FALSE, 0);
    //main menus
    widgets->menus.file = gtk_menu_new();
    widgets->menus.help = gtk_menu_new();
    widgets->menus.edit = gtk_menu_new();
    widgets->menus.languages = gtk_menu_new();
    //menu widget
    widgets->fileMenu = gtk_menu_item_new_with_label("File");
    widgets->helpMenu = gtk_menu_item_new_with_label("Help");
    widgets->optionsMenu = gtk_menu_item_new_with_label("Edit");

    //action items
    widgets->actions.about = gtk_menu_item_new_with_label("About");
    widgets->actions.save = gtk_menu_item_new_with_label("Save");
    widgets->actions.saveAs = gtk_menu_item_new_with_label("Save As");
    widgets->actions.load = gtk_menu_item_new_with_label("Load");
    widgets->actions.quit = gtk_menu_item_new_with_label("Quit");

    //zoomin items
    widgets->zoom.zoomIn = gtk_menu_item_new_with_label("Zoom In");
    widgets->zoom.zoomOut = gtk_menu_item_new_with_label("Zoom Out");
    widgets->zoom.zoomReset = gtk_menu_item_new_with_label("Reset Zoom");
    widgets->languagesMenu = gtk_menu_item_new_with_label("Languages");

    //languages
    widgets->lang.langC = gtk_menu_item_new_with_label("C");
    widgets->lang.langCPP = gtk_menu_item_new_with_label("C++");
    widgets->lang.langASM = gtk_menu_item_new_with_label("Asm 32");

    //seperators
    initVector(&widgets->separators, SEPARATORS_COUNT);
    addToVector(&widgets->separators, gtk_separator_menu_item_new());
    addToVector(&widgets->separators, gtk_separator_menu_item_new());
    //adding the options to the order
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(widgets->fileMenu), widgets->menus.file);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(widgets->optionsMenu), widgets->menus.edit);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(widgets->helpMenu), widgets->menus.help);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(widgets->languagesMenu), widgets->menus.languages);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menuBar), widgets->fileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menuBar), widgets->optionsMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menuBar), widgets->helpMenu);


    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.file), widgets->actions.save);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.file), widgets->actions.saveAs);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.file), widgets->actions.load);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.help), widgets->actions.about);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.help), widgets->separators.widgetVector[0]);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.help), widgets->actions.quit);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.edit), widgets->zoom.zoomIn);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.edit), widgets->zoom.zoomOut);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.edit), widgets->zoom.zoomReset);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.edit), widgets->separators.widgetVector[1]);

    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.edit), widgets->languagesMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.languages), widgets->lang.langC);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.languages), widgets->lang.langCPP);
    gtk_menu_shell_append(GTK_MENU_SHELL(widgets->menus.languages), widgets->lang.langASM);


}

void setupTextView(GtkApplication* app, UIWidgets* widgets)
{
    state->uiText = (UITextView*)malloc(sizeof(UITextView));
    state->textView = gtk_source_view_new();

    //adding number lines handling
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(state->textView), TRUE);
    state->uiText->buf = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView)));
    state->uiText->mgr = gtk_source_style_scheme_manager_get_default();
    gtk_source_style_scheme_manager_append_search_path(state->uiText->mgr, "resources/styles/"); //s earch path
    state->uiText->scheme = gtk_source_style_scheme_manager_get_scheme(state->uiText->mgr, "saban-dark"); //finding a new style using xml
    //checking if the scheme exists
    if (state->uiText->scheme)
        gtk_source_buffer_set_style_scheme(state->uiText->buf, state->uiText->scheme);

    //language handling
    setLanguage("None", "None");
    //handle the scrolling
    widgets->scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(widgets->scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(widgets->scrolledWindow), state->textView);

    gtk_box_pack_start(GTK_BOX(widgets->appBox), widgets->scrolledWindow, TRUE, TRUE, 0);

}

void setupSignals(GtkApplication* app, UIWidgets* widgets)
{
    //button signals
    g_signal_connect(widgets->actions.save, "activate", G_CALLBACK(saveFile), NULL);
    g_signal_connect(widgets->actions.saveAs, "activate", G_CALLBACK(saveAsFile), NULL);
    g_signal_connect(widgets->actions.load, "activate", G_CALLBACK(loadFileOption), NULL);

    g_signal_connect(widgets->actions.quit, "activate", G_CALLBACK(quitOption), app);
    g_signal_connect(widgets->actions.about, "activate", G_CALLBACK(aboutOption), NULL);

    g_signal_connect(widgets->zoom.zoomIn, "activate", G_CALLBACK(zoomIn), NULL);
    g_signal_connect(widgets->zoom.zoomOut, "activate", G_CALLBACK(zoomOut), NULL);
    g_signal_connect(widgets->zoom.zoomReset, "activate", G_CALLBACK(resetZoom), NULL);

    g_signal_connect(widgets->lang.langC, "activate", G_CALLBACK(setLanguage), "c");
    g_signal_connect(widgets->lang.langASM, "activate", G_CALLBACK(setLanguage), "asm");
    g_signal_connect(widgets->lang.langCPP, "activate", G_CALLBACK(setLanguage), "cpp");

    //handling keyboard signals
    widgets->accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(state->appWindow), widgets->accelGroup);
    gtk_accel_group_connect(widgets->accelGroup, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(saveFile), NULL, NULL));
    gtk_accel_group_connect(widgets->accelGroup, GDK_KEY_equal, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(zoomIn), NULL, NULL));
    gtk_accel_group_connect(widgets->accelGroup, GDK_KEY_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(zoomOut), NULL, NULL));
    gtk_accel_group_connect(widgets->accelGroup, GDK_KEY_z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(undo), NULL, NULL));
    gtk_accel_group_connect(widgets->accelGroup, GDK_KEY_z, GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE, g_cclosure_new(G_CALLBACK(redo), NULL, NULL));

}

//Option Handling
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
        openLoadThread();
    }

    gtk_widget_destroy(dialog);
}

void aboutOption()
{
    showMessage("Text Editor By Omer Saban!");
}
//Language Handling
void setLanguage(GtkMenuItem* menuitem, gpointer userData)
{
    const char* langName = (const char*)userData;
    state->uiText->lm = gtk_source_language_manager_get_default();
    state->uiText->lang = gtk_source_language_manager_get_language(state->uiText->lm, langName); //getting the language
    //if the language not found warn
    if (!state->uiText->lang)
    {
        g_warning("Language %s not found", langName);
        gtk_source_buffer_set_language(state->uiText->buf, NULL);
    }
    //setting the language
    gtk_source_buffer_set_language(state->uiText->buf, state->uiText->lang);

}
//Undo/Redo
void undo(GtkWidget* widget, gpointer userData)
{
    gtk_source_buffer_undo(state->uiText->buf);
}

void redo(GtkWidget* widget, gpointer userData)
{
    gtk_source_buffer_redo(state->uiText->buf);
}

//Zoom Handling
void setFontSize()
{
    PangoFontDescription* fontDesc = pango_font_description_from_string("Monospace");
    pango_font_description_set_size(fontDesc, state->currentFontSize * PANGO_SCALE);
    gtk_widget_override_font(GTK_WIDGET(state->textView), fontDesc);
    pango_font_description_free(fontDesc);
}


void zoomIn() 
{
    if (state->currentFontSize < MAX_ZOOM)
    {  // max limit
        state->currentFontSize += ADD_ZOOM;
        setFontSize();
    }
}

void zoomOut() 
{
    if (state->currentFontSize > MIN_ZOOM) 
    {  // min limit
        state->currentFontSize -= ADD_ZOOM;
        setFontSize();
    }
}

void quitOption(GtkMenuItem* menuitem, gpointer userData)
{
    GtkApplication* app = GTK_APPLICATION(userData);
    g_application_quit(G_APPLICATION(app));
}

void resetZoom()
{
    state->currentFontSize = DEFAULT_ZOOM;
    setFontSize();
}

//Css Handling
void reloadCss()
{
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

    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(cssProvider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(cssProvider);
}



//Thread Handling/Message Handling
void openLoadThread()
{
    DWORD  dwThreadId;
    HANDLE loadThread = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        loadFileToText,       // thread function name
        NULL,          // argument to thread function 
        0,                      // use default creation flags 
        &dwThreadId);   // returns the thread identifier 
}

gboolean updateTextViewOnMainThread(TextPTR* textPtr)
{
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView));
    gtk_window_set_title(GTK_WINDOW(state->appWindow), textPtr->textName);
    gtk_text_buffer_set_text(buffer, textPtr->text, -1);
    free(textPtr->textName);
    //free(textPtr->text);
    free(textPtr);
    return FALSE; // remove from idle queue
}

void showMessage(const char* message)
{
    //i needed to show a message so i convert it to UTF-8
    int size = MultiByteToWideChar(CP_UTF8, 0, message, -1, NULL, 0);
    wchar_t* wmessage = (wchar_t*)malloc(size * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, message, -1, wmessage, size);

    MessageBox(NULL, wmessage, L"Text Editor Saban", MB_OK | MB_ICONINFORMATION);
    free(wmessage);
}
