#include "userinterface.h"
#include "miniclient.h"


static GtkTextBuffer *textBuf;
extern pthread_t p;
extern int socketfd;
char count_str[30]={0};

int main( int   argc, char *argv[] )
{
    gtk_init (&argc, &argv);
    //init window
    GtkWidget *window = newWindow("Socket Client",800,400);
    GtkWidget *contentGrid = gtk_grid_new();
    GtkWidget *services = newButtonBox(window);
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL,NULL);
    GtkWidget *textView = gtk_text_view_new();
    textBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textView),false);

    //If len is -1, text must be nul-terminated
    gtk_text_buffer_set_text(textBuf, "\
    This is a mini-Client.\n \
    Please click the connect button to get started.\n",-1);

    //scrolled
    gtk_widget_set_hexpand(GTK_WIDGET(scrolled),TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(scrolled),TRUE);

    //contentGrid
    gtk_grid_set_column_homogeneous(GTK_GRID(contentGrid),FALSE);
    gtk_grid_set_row_homogeneous(GTK_GRID(contentGrid),FALSE);
    GtkWidget *contentGrid0 = gtk_label_new("Services");
    GtkWidget *contentGrid1 = gtk_label_new("Infomation");
    gtk_grid_attach (GTK_GRID(contentGrid),contentGrid0,0,0,1,1);
    gtk_grid_attach (GTK_GRID(contentGrid),contentGrid1,1,0,1,1);
    gtk_grid_attach (GTK_GRID(contentGrid),services,0,1,1,1);
    gtk_grid_attach (GTK_GRID(contentGrid),scrolled,1,1,1,1);
    //
    gtk_grid_set_column_spacing(GTK_GRID(contentGrid),20);

    //a GtkWindow can only contain one widget at a time
    gtk_container_add (GTK_CONTAINER (window), contentGrid);

    gtk_container_add (GTK_CONTAINER (scrolled), textView);
    
    //show widgets
    gtk_widget_show_all (window);
    //wait for events occur
    gtk_main ();
    return 0;
}


GtkWidget * newWindow(char * title,int width,int height){
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),title);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width (GTK_CONTAINER (window), 50);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
    //clicking close will trigger destory
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (gtk_main_quit), NULL);
    return window;
}


GtkWidget* newButtonBox(GtkWidget *window){
    GtkWidget *buttonBox = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    GtkWidget *button;
    int count = 1;
    while(count<=7){
        switch (count)
        {
        case 1:
            button=gtk_button_new_with_label("1.Connect to server");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn1Clicked),NULL);              
            break;
        case 2:                    
            button=gtk_button_new_with_label("2.Disconnect");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn2Clicked),NULL);
            break;
        case 3:            
            button=gtk_button_new_with_label("3.Get time from server");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn3Clicked),NULL);
            break;
        case 4:         
            button=gtk_button_new_with_label("4.Get name from server");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn4Clicked),NULL);
            break;
        case 5:  
            button=gtk_button_new_with_label("5.Get Client-List from server");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn5Clicked),NULL);
            break;
        case 6: 
            button=gtk_button_new_with_label("6.Send message to other clients");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn6Clicked),NULL);
            break;
        case 7:                        
            button=gtk_button_new_with_label("7.Quit");
            g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(onBtn7Clicked),window);
            break;
        default:    break;
        }
        count++;   
        gtk_container_add(GTK_CONTAINER(buttonBox),button);
        gtk_widget_show(button);

    }
    return buttonBox;
}


void onBtn1Clicked(GtkWidget *widget,gpointer data){
    gtk_init (NULL, NULL);
    GtkWidget* window = newWindow("Connect to server",300,200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    GtkWidget *ipLabel = gtk_label_new("Please enter the IP address:");
    GtkWidget *ipEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ipEntry),"IP address");
    
    GtkWidget *portLabel = gtk_label_new("Please enter the server port:");
    GtkWidget *portEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(portEntry),"server port");

    GtkWidget *connectButton = gtk_button_new_with_label("Connect");
    struct EntryStruct entries;
    entries.Entry1 = (GtkEntry*)ipEntry;
    entries.Entry2 = (GtkEntry*)portEntry;
    entries.window = (GtkWidget*)window;
    g_signal_connect(G_OBJECT(connectButton),"clicked",G_CALLBACK(onConnectBtnClicked),(gpointer)&entries);
    
    gtk_container_add(GTK_CONTAINER(window),box); 
    gtk_container_add(GTK_CONTAINER(box),ipLabel); 
    gtk_container_add(GTK_CONTAINER(box),ipEntry); 
    //gtk_container_add(GTK_CONTAINER(box),portLabel); 
    //gtk_container_add(GTK_CONTAINER(box),portEntry); 
    gtk_container_add(GTK_CONTAINER(box),connectButton); 
    gtk_widget_show_all(window);
 	gtk_main();	
}

void onConnectBtnClicked(GtkWidget *widget,gpointer *data){
    struct EntryStruct *entry = (struct EntryStruct*) data;
    GtkEntry *ipEntry = (GtkEntry*) entry->Entry1;
    GtkEntry *portEntry = (GtkEntry*) entry->Entry2;
    GtkWidget *window  = (GtkWidget*) entry->window;
    const char *ip = gtk_entry_get_text(GTK_ENTRY(ipEntry)); 
    const char *port = gtk_entry_get_text(GTK_ENTRY(portEntry)); 

    bool success =  connect2Server(ip,port);
    print_info("(Client) Connecting\n");
    if(success){
        print_info("(Client) Connected Successfully\n");
        //create thread
        pthread_create(&p, NULL, &waitServer, (void *)&socketfd);
    }
    else
        print_info("(Client) Connection failed\n");
    
    gtk_widget_destroy(GTK_WIDGET(window));
}

void onSendBtnClicked(GtkWidget *widget,gpointer *data){
    struct EntryStruct *entry = (struct EntryStruct*) data;
    GtkEntry *clientEntry = (GtkEntry*) entry->Entry1;
    GtkEntry *msgEntry = (GtkEntry*) entry->Entry2;
    GtkWidget *window  = (GtkWidget*) entry->window;
    const char *client = gtk_entry_get_text(GTK_ENTRY(clientEntry)); 
    const char *msg = gtk_entry_get_text(GTK_ENTRY(msgEntry)); 
    printf("send socketfd %s : %s\n",client,msg);
    sendMessageRequestPacket(client,msg);
    print_info("(Client) Sending\n");


    gtk_widget_destroy(GTK_WIDGET(window));
}

void onBtn2Clicked(GtkWidget *widget,gpointer data){

    sendDisRequestPacket();
    print_info("(Client) Disconnected\n");

}
void onBtn3Clicked(GtkWidget *widget,gpointer data){
    
    sendTimeRequestPacket(); 

}
void onBtn4Clicked(GtkWidget *widget,gpointer data){

    sendNameRequestPacket();

}
void onBtn5Clicked(GtkWidget *widget,gpointer data){

    sendListRequestPacket();

}
void onBtn6Clicked(GtkWidget *widget,gpointer data){

    gtk_init (NULL, NULL);
    GtkWidget* window = newWindow("Send message",300,200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    GtkWidget *clientLabel = gtk_label_new("PLease enter the client id you want to send::");
    GtkWidget *clientEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(clientEntry),"Client id");
    
    GtkWidget *msgLabel = gtk_label_new("PLease enter the message you want to send:");
    GtkWidget *msgEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(msgEntry),"Message");

    GtkWidget *sendButton = gtk_button_new_with_label("Send");
    struct EntryStruct entries;
    entries.Entry1 = (GtkEntry*)clientEntry;
    entries.Entry2 = (GtkEntry*)msgEntry;
    entries.window = (GtkWidget*)window;
    g_signal_connect(G_OBJECT(sendButton),"clicked",G_CALLBACK(onSendBtnClicked),(gpointer)&entries);
    
    gtk_container_add(GTK_CONTAINER(window),box); 
    gtk_container_add(GTK_CONTAINER(box),clientLabel); 
    gtk_container_add(GTK_CONTAINER(box),clientEntry); 
    gtk_container_add(GTK_CONTAINER(box),msgLabel); 
    gtk_container_add(GTK_CONTAINER(box),msgEntry); 
    gtk_container_add(GTK_CONTAINER(box),sendButton); 
    gtk_widget_show_all(window);
 	gtk_main();	

}
void onBtn7Clicked(GtkWidget *widget,gpointer data){

    sendDisRequestPacket();
    GtkWidget *window  = (GtkWidget*) data;
    gtk_widget_destroy(GTK_WIDGET(window));

}

void print_info(char * info){

    GtkTextIter start,end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(textBuf),&start,&end);
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(textBuf),&end,info,strlen(info));

}

void *waitServer(void* para){
	packet pkt;
    int ret = 0;
    int countTime = 0;
	while(1) {
		memset(pkt.data, 0, sizeof(pkt.data));       
        ret = recv( *(int*)para, (char *)&pkt, sizeof(pkt), 0);
		if(pkt.type == TERMINATE && pkt.pType == INSTRUCT) {
			print_info("(Client) Server connection terminated!\n");
			pthread_exit(0);
		}
        #ifdef ONEHUNDRED
        if(pkt.pType == RESPONSE && pkt.data[13] == 'D'){
            countTime++;
            printf("count of time response:%d\n",countTime);
        }
        #endif  
        //becasue need to call print_info, so placed in ui
        print_info(pkt.data);
	}
}
