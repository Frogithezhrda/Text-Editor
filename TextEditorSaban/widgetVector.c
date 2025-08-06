#include "widgetVector.h"



void initVector(WidgetVector* vector, const int size)
{
	vector->widgetVector = (GtkWidget**)calloc(size, sizeof(GtkWidget*));
	vector->vectorSize = size;
	vector->widgetCount = 0;
}

void clearVector(WidgetVector* vector)
{
	free(vector->widgetVector);
	vector->widgetVector = NULL;
	vector->vectorSize = 0;
	vector->widgetCount = 0;
}

gboolean addToVector(WidgetVector* vector, GtkWidget* widget)
{
	if (vector->widgetCount < vector->vectorSize)
	{
		vector->widgetVector[vector->widgetCount] = widget;
		vector->widgetCount++;
		return TRUE;
	}
	return FALSE;
}

gboolean popWidget(WidgetVector* vector)
{
	if (vector->widgetCount > 0)
	{
		vector->widgetCount--;
		vector->widgetVector[vector->widgetCount] = NULL;
		return TRUE;
	}
	return FALSE;
}
