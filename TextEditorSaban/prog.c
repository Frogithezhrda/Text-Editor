#include "fileMaster.h"
#include "ui.h"

void freeAppState(AppState* state);
GtkApplication* initApp();

static void activate(GtkApplication* app, gpointer user_data)
{
    activateUI(app, user_data);
}


#if _DEBUG
int main(int argc, char* argv[])
{
    GtkApplication* app = initApp();
    if (argc == 2)
    {
        state->filename = g_strdup(argv[1]);
        trimQuotes(state->filename);
    }
    g_application_run(G_APPLICATION(app), NULL, NULL);
    g_object_unref(app);
    freeAppState(state);
    return 0;
}

#else

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GtkApplication* app = NULL;

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
    GtkApplication* app = gtk_application_new("my.texteditor", G_APPLICATION_FLAGS_NONE);
    if (!app)
    {
        showMessage("Failed to initialize GTK app!");
        return 0;
    }
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    //allocating the states
    state = (AppState*)calloc(1, sizeof(AppState));
    return app;
}

void freeAppState(AppState* state) 
{
    if (!state) return;

    if (state->filename) g_free(state->filename);

    if (state->ui)
    {
        clearVector(&state->ui->separators);
        free(state->ui);
    }

    if (state->uiText)
        free(state->uiText);

    free(state);
}