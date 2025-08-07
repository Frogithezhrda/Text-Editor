#include "fileMaster.h"

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


void saveFile()
{
    FILE* savedFile = NULL;
    GtkTextBuffer* buffer = NULL;
    GtkTextIter end;
    GtkTextIter start;
    gchar* text = NULL;
    //checking there is a file to open
    if (state->appWindow == NULL || !isFileNameExist())
    {
        saveAsFile();
        return;
    }

    //if there is we get the buffer and the text
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView));
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    //open the file and writ to it closing it and freeing the memory of the text
    savedFile = fopen(state->filename, "w");
    fprintf(savedFile, "%s", text);
    fflush(savedFile);
    fclose(savedFile);
    g_free(text);
}

void saveAsFile()
{
    GtkWidget* dialog;
    GtkWidget* parentWindow = gtk_widget_get_toplevel(GTK_WIDGET(state->appWindow));
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
        //removing memory leak
        if (state->filename)
            g_free(state->filename);

        state->filename = gtk_file_chooser_get_filename(fileChooser);
        printf("File selected: %s\n", state->filename);

        if (!isFileNameExist())
        {
            return;
        }
        //saving the file
        TextPTR* textPtr = (TextPTR*)malloc(sizeof(TextPTR));
        buildTextName(textPtr);
        gtk_window_set_title(GTK_WINDOW(state->appWindow), textPtr->textName);
        free(textPtr);
        saveFile();
    }

    gtk_widget_destroy(dialog);
}


File loadFile()
{
    File file = { 0 };
    if (!isFileNameExist())
    {
        return file;
    }
    file.file = fopen(state->filename, "r");
    if (file.file == NULL)
    {
        showMessage("Couldnt Open!");
    }
    else
    {
        fseek(file.file, 0, SEEK_END);
        file.length = ftell(file.file);
        fseek(file.file, 0, SEEK_SET);
    }
    return file;
}

gboolean isFileNameExist()
{
    if (state == NULL || state->filename == NULL || !strcmp(state->filename, "")) return FALSE;
    return TRUE;
}


DWORD WINAPI loadFileToText(LPVOID lpParam)
{
    File file = loadFile();
    if (!file.file)
        return 0;

    TextPTR* textPtr = createTextPtr(file);
    if (!textPtr)
        return 0;

    buildTextName(textPtr);

    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView));
    if (!buffer)
    {
        showMessage("Failed to get text buffer");
        g_free(textPtr->text);
        g_free(textPtr->textName);
        free(textPtr);
        return 0;
    }

    g_idle_add((GSourceFunc)updateTextViewOnMainThread, textPtr);

    return 0;
}

void buildTextName(TextPTR* ptr)
{
    ptr->textName = (gchar*)malloc(strlen(state->filename) + strlen(TITLE_TEXT) + 1);
    if (!ptr->textName) return;

    strcpy(ptr->textName, TITLE_TEXT);
    strcat(ptr->textName, state->filename);
}

TextPTR* createTextPtr(File file)
{
    GError* error = NULL;
    TextPTR* textPtr = (TextPTR*)malloc(sizeof(TextPTR));
    if (!textPtr) return NULL;

    textPtr->text = (gchar*)malloc(file.length + 1);
    if (!textPtr->text)
    {
        free(textPtr);
        return NULL;
    }

    fread(textPtr->text, 1, file.length, file.file);
    textPtr->text[file.length] = '\0';
    fclose(file.file);

    if (!g_utf8_validate(textPtr->text, file.length, NULL))
    {
        gchar* converted = g_locale_to_utf8(textPtr->text, file.length, NULL, NULL, &error);
        if (!converted)
        {
            showMessage(error->message);
            g_error_free(error);
            free(textPtr->text);
            free(textPtr);
            return NULL;
        }
        free(textPtr->text);
        textPtr->text = converted;
    }

    return textPtr;
}

