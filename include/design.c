#include <gtk/gtk.h>

typedef struct _ProgressData {
    GtkWidget *window;
    GtkWidget *pbar[8];
    int timer;
	int id;
} ProgressData;

/* Update the value of the progress bar so that we get
 * some movement */
gint progress_timeout( gpointer data )
{
    gfloat new_val;
    GtkAdjustment *adj;

    /* Calculate the value of the progress bar using the
     * value range set in the adjustment object */

    new_val = gtk_progress_get_value( GTK_PROGRESS(data) ) + 1;

    adj = GTK_PROGRESS (data)->adjustment;
    if (new_val > adj->upper)
      new_val = adj->upper;

    /* Set the new value */
    gtk_progress_set_value (GTK_PROGRESS (data), new_val);

    /* As this is a timeout function, return TRUE so that it
     * continues to get called */
    return(TRUE);
} 

/* Callback that toggles the text display within the progress
 * bar trough */
void toggle_show_text( GtkWidget    *widget,
                       ProgressData *pdata )
{
    gtk_progress_set_show_text (GTK_PROGRESS (pdata->pbar),
                                GTK_TOGGLE_BUTTON (widget)->active);
}

/* Callback that toggles the continuous mode of the progress
 * bar */
void set_continuous_mode( GtkWidget    *widget,
                          ProgressData *pdata )
{
    gtk_progress_bar_set_bar_style (GTK_PROGRESS_BAR (pdata->pbar),
                                    GTK_PROGRESS_CONTINUOUS);
}

/* Clean up allocated memory and remove the timer */
void destroy_progress( GtkWidget     *widget,
                       ProgressData *pdata)
{
    gtk_timeout_remove (pdata->timer);
    pdata->timer = 0;
    pdata->window = NULL;
    g_free(pdata);
    gtk_main_quit();
}



void create_progressBar(ProgressData *pdata, 
                        GtkWidget *vbox, int id){
	GtkWidget *align;
	GtkAdjustment *adj;

	
	align = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 5);
	adj = (GtkAdjustment *) gtk_adjustment_new (0, 1, 150, 0, 0, 0);
	pdata->pbar[id] = gtk_progress_bar_new_with_adjustment (adj);

	/* Here the values of progress Bar */
	gtk_progress_bar_set_text(GTK_PROGRESS (pdata->pbar[id]),
		                      "%v from [%l-%u] (=%p%%)");
							
    gtk_container_add (GTK_CONTAINER (align), pdata->pbar[id]);
	pdata->id = id;
    gtk_widget_show(pdata->pbar[id]);

	pdata->timer = gtk_timeout_add ((id*100), progress_timeout, pdata->pbar[id]);
							
	gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 10);


		gtk_widget_show(align);	
}

typedef struct _progBar {
	int id;
	ProgressData *pBar;
} progBar;

	ProgressData *pdata;
    GtkWidget *separator;
    GtkWidget *table;
    GtkWidget *button;
    GtkWidget *check;
    GtkWidget *vbox;

int main( int   argc,
          char *argv[])
{
	

    gtk_init (&argc, &argv);

    /* Allocate memory for the data that is passwd to the callbacks */
    pdata = g_malloc( sizeof(ProgressData) );
  
    pdata->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_policy (GTK_WINDOW (pdata->window), FALSE, FALSE, TRUE);

    gtk_signal_connect (GTK_OBJECT (pdata->window), "destroy",
                        GTK_SIGNAL_FUNC (destroy_progress),
                        pdata);
    gtk_window_set_title (GTK_WINDOW (pdata->window), "GtkProgressBar");
    gtk_container_set_border_width (GTK_CONTAINER (pdata->window), 0);

    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 10);
    gtk_container_add (GTK_CONTAINER (pdata->window), vbox);
    gtk_widget_show(vbox);

	/* Here create the progress Bars */
	int i;
	for(i=1;i<=8;i++){
		create_progressBar(pdata,vbox,i);
	}
	separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);
    gtk_widget_show(separator);
	
    /* rows, columns, homogeneous */
    table = gtk_table_new (2, 3, FALSE);
    gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, TRUE, 0);
    gtk_widget_show(table);

       
    /* Add a button to exit the program */
    button = gtk_button_new_with_label ("close");
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                               (GtkSignalFunc) gtk_widget_destroy,
                               GTK_OBJECT (pdata->window));
    gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

    /* This makes it so the button is the default. */
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

    /* This grabs this button to be the default button. Simply hitting
     * the "Enter" key will cause this button to activate. */
    gtk_widget_grab_default (button);
    gtk_widget_show(button);

    gtk_widget_show (pdata->window);

    gtk_main ();
    
    return(0);
}