#include "fileMaster.h"


static void activate(GtkApplication* app, gpointer user_data)
{
    activateUI(app, user_data);
}


#if _DEBUG
int main(int argc, char* argv[])
{
    GtkApplication* app = NULL;
    int status = 0;
    g_setenv("GSETTINGS_SCHEMA_DIR", "C:\\msys64\\mingw64\\share\\glib-2.0\\schemas", TRUE);
    g_setenv("GTKSOURCEVIEW_LANGUAGE_RNG", "C:/Users/Omer/Documents/Text-Editor/TextEditorSaban/share/gtksourceview-3.0/language-specs/language.rng", TRUE);
    app = gtk_application_new("my.texteditor", G_APPLICATION_FLAGS_NONE);
    if (!app)
    {
        showMessage("Failed to initialize GTK app!");
        return 0;
    }
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    //getting the fileName from the openwith
    state = (AppState*)calloc(1, sizeof(AppState));
    if (argc == 2)
    {
        state->filename = g_strdup(argv[1]);
        trimQuotes(state->filename);
    }
    status = g_application_run(G_APPLICATION(app), NULL, NULL);
    g_object_unref(app);
    if (state->filename) g_free(state->filename);
    free(state);
    return 0;
}

#else

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GtkApplication* app = NULL;
    int status = 0;
    g_setenv("GSETTINGS_SCHEMA_DIR", "C:\\msys64\\mingw64\\share\\glib-2.0\\schemas", TRUE);
    app = gtk_application_new("my.texteditor", G_APPLICATION_FLAGS_NONE);
    if (!app) {
        showMessage("Failed to initialize GTK app!");
        return 0;
    }
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    //getting the fileName from the openwith
    state = (AppState*)calloc(1, sizeof(AppState));
    if (*lpCmdLine != '\0')
    {
        state->filename = g_strdup(lpCmdLine);
        trimQuotes(state->filename);
    }
    status = g_application_run(G_APPLICATION(app), NULL, NULL);
    g_object_unref(app);
    if (state->filename) g_free(state->filename);
    return 0;
}
#endif
    