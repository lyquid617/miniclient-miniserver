#ifndef __USERINTERFACE__
#define __USERINTERFACE__

#include <gtk/gtk.h>

struct EntryStruct
{
    GtkEntry *Entry1;
    GtkEntry *Entry2;
    GtkWidget *window;
};

GtkWidget * newWindow(char * title,int width,int height);
GtkWidget* newButtonBox(GtkWidget *window);


void onBtn1Clicked(GtkWidget *widget,gpointer data);
void onBtn2Clicked(GtkWidget *widget,gpointer data);
void onBtn3Clicked(GtkWidget *widget,gpointer data);
void onBtn4Clicked(GtkWidget *widget,gpointer data);
void onBtn5Clicked(GtkWidget *widget,gpointer data);
void onBtn6Clicked(GtkWidget *widget,gpointer data);
void onBtn7Clicked(GtkWidget *widget,gpointer data);

void onConnectBtnClicked(GtkWidget *widget,gpointer *data);
void onSendBtnClicked(GtkWidget *widget,gpointer *data);

void print_info(char * info);

void *waitServer(void * para);

#endif