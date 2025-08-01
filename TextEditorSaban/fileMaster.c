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
        return 0;
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
    GtkWidget* parentWindow = gtk_widget_get_toplevel(GTK_WIDGET(state->filename));
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
        state->filename = gtk_file_chooser_get_filename(fileChooser);
        printf("File selected: %s\n", state->filename);

        if (!isFileNameExist())
        {
            return 0;
        }
        //saving the file
        saveFile();
    }

    gtk_widget_destroy(dialog);
}


File loadFile()
{
    File file;
    if (!isFileNameExist())
    {
        return;
    }
    file.file = fopen(state->filename, "r");
    if (file.file == NULL)
    {
        showMessage("Couldnt Open!");
    }
    fseek(file.file, 0, SEEK_END);
    file.length = ftell(file.file);
    fseek(file.file, 0, SEEK_SET);
    return file;
}

Cbool isFileNameExist()
{
    if (state == NULL || state->filename == NULL || !strcmp(state->filename, "")) return FALSE;
    return TRUE;
}


DWORD WINAPI loadFileToText(LPVOID lpParam)
{
    GtkTextBuffer* buffer = NULL;
    File file = loadFile();
    TextPTR* textPtr = (TextPTR*)malloc(sizeof(TextPTR));

    textPtr->text = (gchar*)malloc(sizeof(char) * (file.length) + 1);
    textPtr->textName = (gchar*)malloc(sizeof(char) * (strlen(state->filename) + strlen(TITLE_TEXT)) + 1);
    if (textPtr->text)
    {
        fread(textPtr->text, 1, file.length, file.file);
        textPtr->text[file.length] = '\0';
        fclose(file.file);

        buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->textView));
        if (buffer == NULL)
        {
            showMessage("Failed to get text buffer");
            return;
        }
        strcpy(textPtr->textName, TITLE_TEXT);
        strcat(textPtr->textName, state->filename);
    }

    g_idle_add((GSourceFunc)updateTextViewOnMainThread, textPtr);
    return 0;
}
