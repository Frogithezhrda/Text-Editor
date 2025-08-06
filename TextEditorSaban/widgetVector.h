#pragma once
#include <gtk/gtk.h>

typedef struct WidgetVector {

	GtkWidget** widgetVector;
	int vectorSize;
	int widgetCount;
} WidgetVector;

void initVector(WidgetVector* vector, const int size);
void clearVector(WidgetVector* vector);
gboolean addToVector(WidgetVector* vector, GtkWidget* widget);
gboolean popWidget(WidgetVector* vector);
