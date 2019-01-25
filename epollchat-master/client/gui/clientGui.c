#include "../master.h"
#define CHAT_PAGE 0
#define STAT_PAGE 1


void cleanStr (char *str);
int setSend		(const char * ipAddr  );
int sendPacket(int sockd, char * str);

int sendSock = 0;

enum 
{
	COL_CHATBOX,
	N_COL
};



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: destroyGui
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void destroyGui()
--
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Callback to terminate the gui. Called when user closes the gui.
------------------------------------------------------------------------------------------------------------------*/
static void destroyGui()
{	
	gtk_main_quit();
}




/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: createWindow
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void createWindow(GtkWidget **win)
--
--							GtkWidget **win: ptr to the window
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates the GTK Window, centers it, sets the title, default size and border width.
-- Binds the destroyGui() callback to the exit command.
------------------------------------------------------------------------------------------------------------------*/
static void createWindow(GtkWidget **win)
{
	(*win) = gtk_window_new	(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position	(GTK_WINDOW(*win), GTK_WIN_POS_CENTER);
  gtk_window_set_title		(GTK_WINDOW(*win), "Hyper Mega Chat Program! CLIENT");
  
  gtk_window_set_default_size		 (GTK_WINDOW(  *win ), 1000, 600);
	gtk_container_set_border_width (GTK_CONTAINER(*win),        10);

	g_signal_connect(*win, "destroy", G_CALLBACK(destroyGui), NULL);
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeTabs
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void makeTabs(GtkWidget **tabs, GtkWidget **page, const size_t size)
--
--										GtkWidget **tabs: widget for holding tabs.
--										GtkWidget **page: array of pages which we want to create tabs for
--										const size_t size: number of tabs to create
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Makes tabs.
------------------------------------------------------------------------------------------------------------------*/
static void makeTabs(GtkWidget **tabs, GtkWidget **page, const size_t size)
{
	size_t i = 0;
	
	(*tabs) = gtk_notebook_new ();

	for( ; i < size; i++) {
		gtk_notebook_append_page (GTK_NOTEBOOK(*tabs), page[i], 0);
	}
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: callback_sendPacket
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void callback_sendPacket( gpointer data )
--
--								gpointer data: struct containing ipc variables and pointers to user input widgets (cast to void*)
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Callback for sending packets. Called when a user submits a chat.
--
-- Allocs a buffer with the user's input
-- Passes the buffer along with the sendSocket to sendPacket()
-- Clears the user input textbox
-- Frees the buffer
------------------------------------------------------------------------------------------------------------------*/
static void callback_sendPacket( gpointer data )
{
	struct ipc *stipc = (struct ipc*)data;
	
	GtkTextBuffer * buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW(stipc -> userinput));
	GtkTextIter start;
	GtkTextIter end;

	gchar * strToSend;
	
	gtk_text_buffer_get_bounds(buf, &start, &end);
	
	strToSend = gtk_text_buffer_get_text(buf, &start, &end, FALSE);
	

	sendPacket( sendSock, strToSend );
	gtk_text_buffer_delete( buf, &start, &end );
	g_free( strToSend );
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeColumn
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void makeColumn(GtkWidget *chatbox, const char *TITLE, int INDEX)
--
--								GtkWidget *chatbox	: pointer to the chat display widget
--								const char *Title		: column title
--								int INDEX						: column enum index
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates a listbox column with a title.
------------------------------------------------------------------------------------------------------------------*/
static void makeColumn(GtkWidget *chatbox, const char *TITLE, int INDEX)
{
 	GtkCellRenderer 	*cell 	= gtk_cell_renderer_text_new(); 	
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(TITLE, cell, "text", INDEX, NULL);
  
  gtk_tree_view_append_column(GTK_TREE_VIEW(chatbox), column); 
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeChatbox
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void makeChatbox(GtkWidget *horz_box, struct ipc *stipc)
--
--								GtkWidget *horz_box: panel for storing widgets 
--								struct ipc *stipc: struct containing ipc variables and pointers to user input widgets (cast to void*)	
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
--
-- Creates chatbox elements, including the display box, the user input field and submit button.
-- Connects the submit button click event to callback_sendPacket, which gets passed a ptr to the user 
-- input field containing the message.
------------------------------------------------------------------------------------------------------------------*/
static void makeChatbox(GtkWidget *horz_box, struct ipc *stipc)
{
	GtkWidget * scrollbarTop, * scrollbarBot;
	GtkWidget * btnsubmit;
	GtkWidget * chatbox;
	GtkWidget * left_box;
	GtkWidget * send_box;
	
	GtkWidget * userinput;
	GtkListStore * store;

	GtkWidget * img = gtk_image_new_from_file("img/submit.png");

	left_box	= gtk_box_new (GTK_ORIENTATION_VERTICAL  , 0);
	send_box	= gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	
	scrollbarTop	= gtk_scrolled_window_new (NULL, NULL);
	scrollbarBot	= gtk_scrolled_window_new (NULL, NULL);
	
	( btnsubmit ) = gtk_button_new ();
	( userinput) = gtk_text_view_new ();
	
	(  store  	) = gtk_list_store_new 					( N_COL, G_TYPE_STRING  	);
	(  chatbox  ) = gtk_tree_view_new_with_model( GTK_TREE_MODEL( store ));
	

	gtk_button_set_image(GTK_BUTTON(btnsubmit), img);
	
	
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW( userinput ), GTK_WRAP_WORD);
	
	
	makeColumn(chatbox, "\tlet us chat!\t", COL_CHATBOX);
	

	gtk_container_add  (GTK_CONTAINER(scrollbarTop), (  chatbox ));
	gtk_container_add  (GTK_CONTAINER(scrollbarBot), ( userinput));
	 
	gtk_box_pack_start (GTK_BOX(send_box), (scrollbarBot),  TRUE, TRUE, 0 );
	gtk_box_pack_start (GTK_BOX(send_box), ( btnsubmit)  , FALSE, TRUE, 0 );	

	gtk_box_pack_start (GTK_BOX(left_box), scrollbarTop,  TRUE, TRUE, 1 );
	gtk_box_pack_start (GTK_BOX(left_box), send_box    , FALSE, TRUE, 1 );
	gtk_box_pack_start (GTK_BOX(horz_box), left_box    ,  TRUE, TRUE, 1 );
	
	
	stipc -> store 		 = store;
	stipc -> userinput = userinput;
	
	
	g_signal_connect_swapped(btnsubmit, "clicked", G_CALLBACK(callback_sendPacket), stipc);	
}




/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: callback_setSend
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void callback_setSend( gpointer data )
--
--								gpointer data: struct containing ipc variables and pointers to user input widgets (cast to void*)
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Retrieves the ip address set by the user
-- Calls setSend ( passes the ip address ), which returns a newly connected socket for sending data.
-- Increments a sem in the ipc struct  
------------------------------------------------------------------------------------------------------------------*/
static void callback_setSend( gpointer data )
{
	struct ipc *stipc = (struct ipc *)data;
	const char * ipAddr = gtk_entry_get_text (GTK_ENTRY(stipc->connTxt));

	sendSock = setSend(ipAddr);
	
	
	sem_post(stipc -> sem);
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeStatsbox
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void makeStatsbox(GtkWidget *horz_box, struct ipc *stipc)
--
--								GtkWidget *horz_box: frame for holding widgets
--								struct ipc *stipc: struct containing ipc variables and pointers to user input widgets (cast to void*)
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates the interface for specifying the ip address.
--
-- Creates an input box and a submit button.
-- Adds pointers to the input box to stipc struct
-- Connects the submit button click event to callback_setSend
------------------------------------------------------------------------------------------------------------------*/
static void makeStatsbox(GtkWidget *horz_box, struct ipc *stipc)
{
	GtkWidget * ctrlBox;
	GtkWidget * connFrame;
	GtkWidget * connTxt;
	GtkWidget * connBtn;
	
			
	
	ctrlBox  = gtk_box_new  (GTK_ORIENTATION_VERTICAL  , 0);
	connFrame = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	connBtn  = gtk_button_new_with_label ( "connect" );
	connTxt  = gtk_entry_new ();
	

	
	gtk_entry_set_placeholder_text(GTK_ENTRY(connTxt), "server ip address");

	gtk_box_pack_start( GTK_BOX(connFrame), connTxt , FALSE, FALSE, 1 );
	gtk_box_pack_start( GTK_BOX(connFrame), connBtn , FALSE, FALSE, 1 );
	
	gtk_box_pack_start( GTK_BOX(ctrlBox ) , connFrame, FALSE, FALSE, 1 );
	gtk_box_pack_start( GTK_BOX(horz_box) , ctrlBox  , FALSE, FALSE, 1 );
		
		
	stipc -> connTxt = connTxt;
	
	g_signal_connect_swapped(connBtn, "clicked", G_CALLBACK(callback_setSend), stipc);
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeGui
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: void makeGui( struct ipc *stipc )
--
--								struct ipc *stipc: struct containing ipc variables and pointers to user input widgets (cast to void*)
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates the actual gui.
--
-- Calls createWindow
-- Calls makeTabs
-- Calls makeChatbox
-- Calls makeStatsbox
-- Add all widgets to the window
-- Shows the window & its widgets
------------------------------------------------------------------------------------------------------------------*/
void makeGui(struct ipc *stipc)
{

	GtkWidget * window;
	GtkWidget * tabs;

	
	GtkWidget * pages[1];

	
	pages[CHAT_PAGE] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	pages[STAT_PAGE] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		
	
	createWindow( &window );
	
	makeTabs		( &tabs, pages, 1 );
	
	makeChatbox	( pages[CHAT_PAGE], stipc );
	makeStatsbox( pages[CHAT_PAGE], stipc );
	

	
	gtk_container_add  ( GTK_CONTAINER(window), tabs );
	
	gtk_widget_show_all( window );

}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: appendChatMsg
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: gboolean appendChatMsg( GIOChannel * channel, GIOCondition condition, gpointer data )
--
--								GIOChannel * channel: contains the socket fd
--								GIOCondition condition: condition specifying readiness of read
--								gpointer data: struct containing ipc variables and pointers to user input widgets (cast to void*)
--
--
-- RETURNS:	gboolean.
--															
--
-- NOTES:
--
-- Callback for writing received chat packets to display box.
-- 
-- Retrieves unix socket from channel
-- Calls recv on socket and writes the results to the chat display box
-------------------------------------------------------------------------------------------*/
gboolean appendChatMsg( GIOChannel * channel, GIOCondition condition, gpointer data )
{	
	int sockfd = g_io_channel_unix_get_fd(channel);

	struct ipc   * stipc = (struct ipc *)data;
	GtkListStore * store = stipc -> store;
	
	GtkTreeIter iter;
	
	
	char buf[MAXBUFLEN]; 
	cleanStr(   buf   );
	if(condition == 0) return 0;


	while(recv(sockfd, (char*)&buf, sizeof(buf), 0) != -1) {

				gtk_list_store_insert(store, &iter, 									-1);
				gtk_list_store_set	 (store, &iter, COL_CHATBOX, buf, -1);
	}
	return 1;
}
