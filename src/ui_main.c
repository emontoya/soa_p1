#include "iinterface.h"
#include "controller.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

static GtkWidget *window;
static GtkWidget *button;

static int tcurrent = -1;

struct wthread{
        GtkWidget *tlabel;
        GtkWidget *pbar;
        GtkWidget *check;
        GtkWidget *label;
        GtkWidget *hbox;
};

static struct wthread **threads;

void static create_wthreads(int threads_count, GtkWidget *vbox){
        threads = malloc(threads_count * sizeof(GtkWidget *));
        
        GtkWidget *hbox;

        GtkWidget *pbar;
        GtkWidget *check;
        GtkWidget *label;
        GtkWidget *tlabel;

        char str[15];


        int i;
        for (i = 0; i < threads_count; i++){
                hbox = gtk_hbox_new(TRUE, 1);
                gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
                gtk_widget_show(hbox);

                snprintf(str, 16, "%d", i);
                check =gtk_check_button_new_with_label(str);
                gtk_widget_set_sensitive ((GtkWidget*) check, FALSE);
                gtk_box_pack_start (GTK_BOX (hbox), check, TRUE, TRUE, 1);
                gtk_widget_show(check);

                //label = gtk_label_new(NULL);
                //gtk_label_set_markup(GTK_LABEL(label), "<b>Tickets = </b>");
                //gtk_label_set_width_chars(GTK_LABEL(label), 5);
                //gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 1);
                //gtk_widget_show(label);

                tlabel = gtk_label_new(NULL);
                gtk_label_set_justify(GTK_LABEL(tlabel), GTK_JUSTIFY_LEFT);
                gtk_label_set_text(GTK_LABEL(tlabel), "0");
                gtk_box_pack_start (GTK_BOX (hbox), tlabel, TRUE, TRUE, 1);
                gtk_widget_show(tlabel);

                pbar = gtk_progress_bar_new();
                gtk_box_pack_start (GTK_BOX (hbox), pbar, TRUE, TRUE, 1);
                gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pbar), "%0");
                gtk_widget_show(pbar);

                label = gtk_label_new(NULL);
                gtk_label_set_markup(GTK_LABEL(label), "<b>2 * arccos(0) = </b>");
                gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 1);
                gtk_widget_show(label);

                label = gtk_label_new(NULL);
                gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
                gtk_label_set_text(GTK_LABEL(label), "0");
                gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 1);
                gtk_widget_show(label);

                threads[i] = malloc(sizeof(struct wthread));
                threads[i]->hbox = hbox;
                threads[i]->check = check;
                threads[i]->pbar = pbar;
                threads[i]->label = label;
                threads[i]->tlabel = tlabel;
        }

}
void application_started (gpointer user_data, GtkWidget *widget){
        gtk_widget_hide(button);
        gtk_widget_queue_draw (button);
        while (gtk_events_pending ())
                gtk_main_iteration ();

        printf("Application started\n");
        
        // Start the rocessing
        controller_start();

        // Release the processing env
        controller_end();
}

void create_ui_main(int threads_count){
        GtkWidget *vbox;
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

        gtk_window_set_title(GTK_WINDOW(window), "SOA - Programming task #1: Ediber Montoya Moreira; Leandro Cordero");
        gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
        gtk_container_set_border_width(GTK_CONTAINER(window), 5);

        vbox = gtk_vbox_new(TRUE, 1);
        gtk_container_add(GTK_CONTAINER(window), vbox);
        gtk_container_set_border_width (GTK_CONTAINER (vbox), 1);
        gtk_widget_show(vbox);

        button = gtk_button_new_with_label("Start");
        g_signal_connect(G_OBJECT(button), "clicked", 
                G_CALLBACK(application_started), G_OBJECT(window));

        gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);
        gtk_widget_show(button);

        create_wthreads(threads_count, vbox);

        g_signal_connect_swapped(G_OBJECT(window), "destroy",
                G_CALLBACK(gtk_main_quit), NULL);

        gtk_widget_show(window);
}


void setup_interface(){
        gtk_init(NULL, NULL);
}

void launch_interface(){
        gtk_main();
}


void running_thread_changed(int thread_id){
        GtkWidget *ccheck;
        GtkWidget *ncheck;

        printf("Now running thread %d\n", thread_id);
        if (tcurrent != -1){
                ccheck = threads[tcurrent]->check;
                //gtk_widget_set_sensitive ((GtkWidget*) ccheck, TRUE);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ccheck), FALSE);
                //gtk_widget_set_sensitive ((GtkWidget*) ccheck, FALSE);

                gtk_widget_queue_draw (ccheck);
        }

        ncheck = threads[thread_id]->check;
        //gtk_widget_set_sensitive ((GtkWidget*) ncheck, TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ncheck), TRUE);
        //gtk_widget_set_sensitive ((GtkWidget*) ncheck, FALSE);
        gtk_widget_queue_draw (ncheck);

        tcurrent = thread_id;

        while (gtk_events_pending ())
                gtk_main_iteration ();
}

void thread_value_changed(int thread_id, long double nvalue, unsigned long long total, unsigned long long actual){
        char str[100];
        char str1[100];

        GtkWidget *pbar = threads[thread_id]->pbar;
        GtkWidget *label = threads[thread_id]->label;

        double perc = (double)actual / (double)total;
        snprintf(str, 99,"%%%f (%llu of %llu)",perc * 100 , actual, total);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pbar), str);
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pbar), perc);

        snprintf(str1, 99,"%.16LG", nvalue);
        gtk_label_set_text(GTK_LABEL(label), str1);


        gtk_widget_queue_draw (pbar);
        gtk_widget_queue_draw (label);

        while (gtk_events_pending ())
                gtk_main_iteration ();
}

void set_tickets_count(int thread_id, long ticketc){
        char str[100];
        GtkWidget *label = threads[thread_id]->tlabel;

        snprintf(str, 99,"%ld", ticketc);
        gtk_label_set_text(GTK_LABEL(label), str);

        gtk_widget_queue_draw (label);

        while (gtk_events_pending ())
                gtk_main_iteration ();
}
