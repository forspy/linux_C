#include "../master.h"

#define CHAT_PAGE 0
#define STAT_PAGE 1
#define NUM_PAGES	2

void cleanStr(char *str);

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
static void destroyGui( )
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
  	gtk_window_set_title	(GTK_WINDOW(*win), "Hyper Mega Chat Program! SERVER");
  
  	gtk_window_set_default_size		(GTK_WINDOW(  *win ), 1000, 600);
	gtk_container_set_border_width 	(GTK_CONTAINER(*win),        10);

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
--								const char *Title	: column title
--								int INDEX			: column enum index
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates a listbox column with a title.
------------------------------------------------------------------------------------------------------------------*/
static void makeColumn(GtkWidget *clientbox, const char *TITLE, int INDEX)
{
 	GtkCellRenderer 	*cell 	= gtk_cell_renderer_text_new(); 	
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(TITLE, cell, "text", INDEX, NULL);
  
  gtk_tree_view_append_column(GTK_TREE_VIEW(clientbox), column); 
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeClientbox
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE: static void makeClientbox(GtkWidget *horz_box, GtkListStore **store)
--
--								GtkWidget *horz_box: panel containing other widgets
--								GtkListStore **store: widget for storing listbox text		
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
--
-- Creates the client display box
------------------------------------------------------------------------------------------------------------------*/
static void makeClientbox(GtkWidget *horz_box, GtkListStore **store)
{
	GtkWidget * scrollbarTop;
	GtkWidget * box;
	GtkWidget * clientbox;

	box	= gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
		
	scrollbarTop	= gtk_scrolled_window_new (NULL, NULL);
	
	
	(* store  	) = gtk_list_store_new 			( N_COL, G_TYPE_STRING    );
	(  clientbox) = gtk_tree_view_new_with_model( GTK_TREE_MODEL( *store ));


	makeColumn(clientbox, "\tServer\t", COL_CHATBOX);
	
	
	gtk_container_add (GTK_CONTAINER(scrollbarTop), ( clientbox ));
	
	gtk_box_pack_start(GTK_BOX(box), scrollbarTop , TRUE, TRUE, 1);
	gtk_box_pack_start(GTK_BOX(horz_box),   box   , TRUE, TRUE, 1);
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
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates an empty right panel for no purpose at all! (Placeholder for adding extra functionality in the future)
------------------------------------------------------------------------------------------------------------------*/
static void makeStatsbox(GtkWidget *horz_box)
{
	GtkWidget * right_box;
	
	right_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	
	gtk_box_pack_start ( GTK_BOX(horz_box), right_box, TRUE, TRUE, 1 );	
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
-- Calls makeClientbox
-- Calls makeStatsbox
-- Adds all widgets to the window
-- Shows the window & its widgets
------------------------------------------------------------------------------------------------------------------*/
void makeGui(struct ipc *stipc)
{
	GtkWidget * window;
	GtkWidget * tabs;
	
	
	GtkWidget * pages[NUM_PAGES];

	pages[CHAT_PAGE] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	pages[STAT_PAGE] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
		
	
	createWindow( &window );
	makeTabs	( &tabs, pages, NUM_PAGES );
	
	
	makeClientbox( pages[CHAT_PAGE], &stipc -> store );
	makeStatsbox ( pages[CHAT_PAGE] );
	
	
	gtk_container_add  ( GTK_CONTAINER(window), tabs );
	gtk_widget_show_all( window );

	sem_post( stipc -> sem );
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
