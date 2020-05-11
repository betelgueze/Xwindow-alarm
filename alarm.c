#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <libintl.h>
#include <gdk/gdk.h>
#ifndef GDK_KEY_n
#define GDK_KEY_n GDK_n
#define GDK_KEY_Delete GDK_Delete
#define GKD_KEY_minus GDK_minus
#define GDK_KEY_plus GDK_plus
#define GDK_KEY_d GDK_d
#define GDK_KEY_r GDK_r
#define GDK_KEY_a GDK_a
#define GDK_KEY_q GDK_q
#define GDK_KEY_e GDK_e
#define GDK_KEY_d GDK_d
#define GDK_KEY_f GDK_f
#define GDK_KEY_Hiragana GDK_Hiragana
#define GDK_KEY_Katakana GDK_Katakana
#define GDK_KEY_Hiragana_Katakana GDK_Hiragana_Katakana
#endif

/*
 * Common C library include files
 */
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <math.h>
#include <string.h>

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 480

#define LEFT_BOX_WIDTH_RELATIVE 33

#define LEFT_BOTTOM_BOX_HEIGHT_RELATIVE 15
#define RIGHT_BOTTOM_BOX_HEIGHT_RELATIVE 25

#define DYNAMIC_BUFFER_SIZE 1024

#define BULGARIAN_CONSTANT 5

#define ALLOC_STEP 50

#define MAX_ALARMS 4

#define min(a,b) ((b<a)?b:a)

#define _(x) gettext (x)

typedef enum { false, true } bool;

int previous_width,previous_height;

#define PIX(a,i) a[i][0]
#define SIZ(a,i) a[i][1]

typedef struct
{
    bool enabled;
    int ss;
    int mm;
    int hh;
    bool repeat;
} Alarm;

typedef struct
{
    Alarm ** alarms;
    GtkWidget ** alarmButtons;
    int ac;                 //alarms count
    int actual_alarms_count;
    int max_alarms;
    Alarm * currentAlarm;
    GtkWidget * currentAlarmButton;
    int currentAlarmPosition;
    
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hboxchild;
    
    GtkWidget *vboxLeft;    //contains 2 hboxes
    
        GtkWidget *vboxLTop;        //contains alarm list
        GtkWidget *hgridLBottom;     //contorls (add,delete,delete all, deactivate all,enable/disable)
    
    GtkWidget *vboxRight;    //contains 2 hboxes
    
        GtkWidget *hboxRTop;        //selected alarm settings
        GtkWidget *hboxRBottom;     //time indicator
        
    GtkWidget * draw;       //needed to catch expose events
    
    GtkWidget *menubar;
    PangoContext *pango_ctxt;
    PangoFontDescription *pango_fd;
    
    guint statusbar_context_id;
    
    GtkAccelGroup *accel_group;
    //menu items
    GtkWidget *file;
    GtkWidget *view;
    GtkWidget *help;    
    GtkWidget *filemenu;
    GtkWidget *viewmenu;
    GtkWidget *helpmenu;
    GtkWidget *b_new, *b_disable, *b_disable_all, *b_enable, *b_enable_all, *b_delete_all, *b_del, *b_quit;
    GtkWidget *b_disable_resize,*b_reset_size;
    GtkWidget *b_about;
    //buttons
    GtkWidget *bb_add,*bb_delete,*bb_disable_all,*bb_enable_all,*bb_switch_trigger;
    //button labels
    GtkWidget *bb_add_label,*bb_delete_label,*bb_disable_all_label,*bb_enable_all_label,*bb_switch_trigger_label;
    //labels
    GtkWidget *l_time;
    
#if GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION > 15
    GtkWidget *gl_area;
#endif
    
    int absolute_font_size; //state keepers
    bool add_disabled;

} AlarmClock;

AlarmClock * clocks;

//untraslated characters
const gchar * b1text = "+";
const gchar * b2text = "-";
const gchar * b3text = "\xf0\x9f\xa1\x87";
const gchar * b4text = "\xf0\x9f\xa1\x85";
const gchar * b5text = "\xf0\x9f\x9e\x89";


/* window callback prototypes */
void destroy_signal(GtkWidget *widget, gpointer data);
gboolean delete_event(GtkWidget *widget, GdkEvent *event, AlarmClock *clocks);

/* file menu callbacks prototypes */
void new_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void delete_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void disable_all_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void delete_all_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void disable_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void enable_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void enable_all_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void reset_size_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void disable_resize_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);
void quit_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);

/* APPLICATION CONTROLS callbacks prototypes*/
void add_button(GtkWidget *menuitem, AlarmClock *clocks);
void delete_button(GtkWidget *menuitem, AlarmClock *clocks);
void disable_all_button(GtkWidget *menuitem, AlarmClock *clocks);
void enable_all_button(GtkWidget *menuitem, AlarmClock *clocks);
void switch_button(GtkWidget *menuitem, AlarmClock *clocks);
void alarm_selected(GtkWidget *menuitem, AlarmClock *clocks);

/* help menu callback prototypes */
void about_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks);

/* event handlers*/
static gboolean window_expose_event (GtkWidget *widget, GdkEventExpose *event);

/* misc. function prototypes */
void error_message(AlarmClock *clocks, const gchar *message);
#if GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION > 15
static gboolean render (GtkGLArea *area, GdkGLContext *context);
#endif
gboolean update_time(gpointer data);
char * get_text_formated(const char * content);
void format_button(GtkWidget * w, const char * content);
GtkWidget * get_button_first_label_child(GtkWidget * w);
void reset_left_labels_to_height();
void init_sizing(AlarmClock *clocks,int i);
void start_timer (GtkWidget * w, gpointer data);
void init_app(AlarmClock *clocks);
void switch_all(AlarmClock *clocks);
void switch_alarm(AlarmClock *clocks);
void delete_all(AlarmClock *clocks);
void add_alarm(AlarmClock *clocks);
void delete_alarm(AlarmClock *clocks);
void switch_repeat(AlarmClock *clocks);

void reset_default_status(AlarmClock *clocks)
{
    clocks->currentAlarm = NULL;
    clocks->currentAlarmButton = NULL;
    clocks->alarms = NULL;
    clocks->ac = 0;
    clocks->max_alarms = 0;
    clocks->currentAlarmPosition = 0;
    clocks->absolute_font_size = 38;
    clocks->add_disabled = false;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	bindtextdomain("alarm", ".");
	textdomain("alarm");

	/* allocate the memory needed by AlarmClock struct */
	clocks = g_slice_new(AlarmClock);

	/* initialize GTK+ libraries */
	gtk_init(&argc, &argv);

	/* create widgets */
	init_app(clocks);

	/* show the window */   
	gtk_widget_show_all(clocks->window);

	/* enter GTK+ main loop */
	gtk_main();

	/* free memory */
	g_slice_free(AlarmClock, clocks);
	return 0;
}

void redraw()
{
    gtk_widget_show_all(clocks->window);
    init_sizing(clocks,0);   
}


/* display error dialog */
void
error_message(AlarmClock *clocks, const gchar *message)
{
	GtkWidget *dialog;
	
	/* create an error message dialog and display modally */
	dialog = gtk_message_dialog_new(GTK_WINDOW(clocks->window), 
			GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"%s", message);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Error!"));
	gtk_dialog_run(GTK_DIALOG(dialog));      
	gtk_widget_destroy(dialog);
}

/* create widgets */
void 
init_app(AlarmClock *clocks)
{
#if !GTK_CHECK_VERSION(3,16,0)
	PangoFontDescription *font_desc;
#endif
	/* create main window */
	clocks->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(clocks->window), _("Gtk+ Alarm Clock"));
    //gtk_window_set GDK_EXPOSURE_MASK
	//gtk_window_set_default_size(GTK_WINDOW(clocks->window), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
	g_signal_connect(G_OBJECT(clocks->window), "destroy", G_CALLBACK(destroy_signal), NULL);
	g_signal_connect(G_OBJECT(clocks->window), "delete_event", G_CALLBACK(delete_event), clocks);

	/* create main containers */
#if GTK_MAJOR_VERSION >= 3
    clocks->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    clocks->hboxchild = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    clocks->vboxLeft = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    clocks->vboxRight = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    clocks->vboxLTop = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    clocks->hboxRTop = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    clocks->hboxRBottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);    
#else
    clocks->vbox = gtk_vbox_new(FALSE, 0);
    clocks->hboxchild = gtk_hbox_new(FALSE, 0);
    clocks->vboxLeft = gtk_vbox_new(FALSE, 0);
    clocks->vboxRight = gtk_vbox_new(FALSE, 0);
    clocks->vboxLTop = gtk_vbox_new(FALSE, 0);
    clocks->hboxRTop = gtk_hbox_new(FALSE, 0);
    clocks->hboxRBottom = gtk_hbox_new(FALSE, 0);    
#endif
    /* kreslici plocha */
    clocks->draw = gtk_drawing_area_new();
    gtk_widget_set_size_request(clocks->draw, 0, 0);
    
#if GTK_MAJOR_VERSION >= 3  
    g_signal_connect(G_OBJECT(clocks->draw), "draw", G_CALLBACK(window_expose_event), NULL);
#else
    g_signal_connect(G_OBJECT(clocks->draw), "expose-event",	G_CALLBACK(window_expose_event), NULL);
#endif    
    
    clocks->hgridLBottom = gtk_grid_new ();
    gtk_grid_set_column_homogeneous(GTK_GRID(clocks->hgridLBottom), TRUE);
    
    previous_width = DEFAULT_WINDOW_WIDTH;
    previous_height = DEFAULT_WINDOW_HEIGHT;
    
    gtk_widget_set_size_request (GTK_WIDGET(clocks->hboxchild),DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);
    
    gtk_container_add(GTK_CONTAINER(clocks->window), clocks->vbox);
    
    gtk_box_pack_start(GTK_BOX(clocks->hboxchild), clocks->draw, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(clocks->hboxchild), clocks->vboxLeft,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(clocks->hboxchild), clocks->vboxRight,FALSE,FALSE,0);
    
    gtk_container_add(GTK_CONTAINER(clocks->vboxLeft), clocks->vboxLTop);
    gtk_container_add(GTK_CONTAINER(clocks->vboxLeft), clocks->hgridLBottom);
    
    gtk_container_add(GTK_CONTAINER(clocks->vboxRight), clocks->hboxRTop);
    gtk_container_add(GTK_CONTAINER(clocks->vboxRight), clocks->hboxRBottom);

    /* create main parts - menu, scrolled window and status bar */
    clocks->menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(clocks->vbox), clocks->menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(clocks->vbox), clocks->hboxchild,TRUE,TRUE, 0);

    /* start accelerator group */
    clocks->accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(clocks->window), clocks->accel_group);

    /* File pulldown menu */
    clocks->file = gtk_menu_item_new_with_mnemonic(_("_File"));
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->menubar), clocks->file);
    clocks->filemenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(clocks->file), clocks->filemenu);

    /* New button in File menu */
    clocks->b_new = gtk_menu_item_new_with_mnemonic(_("New"));
    gtk_widget_add_accelerator(clocks->b_new, "activate", clocks->accel_group,
        GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_new);
    g_signal_connect(G_OBJECT(clocks->b_new), "activate", G_CALLBACK(new_menu_item), clocks);

    /* Delete button in File menu */
    clocks->b_del = gtk_menu_item_new_with_mnemonic(_("Delete"));
    gtk_widget_add_accelerator(clocks->b_del, "activate", clocks->accel_group,
        GDK_KEY_Delete, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_del);
    g_signal_connect(G_OBJECT(clocks->b_del), "activate", G_CALLBACK(delete_menu_item), clocks);
    
    /* Delete all button in File menu */
    clocks->b_delete_all = gtk_menu_item_new_with_mnemonic(_("_Delete All"));
    gtk_widget_add_accelerator(clocks->b_delete_all, "activate", clocks->accel_group,
        GDK_KEY_Hiragana_Katakana, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_delete_all);
    g_signal_connect(G_OBJECT(clocks->b_delete_all), "activate", G_CALLBACK(delete_all_menu_item), clocks);
    
    /* Disable all button in File menu */
    clocks->b_disable = gtk_menu_item_new_with_mnemonic(_("_Disable"));
    gtk_widget_add_accelerator(clocks->b_disable, "activate", clocks->accel_group,
        GDK_KEY_minus, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_disable);
    g_signal_connect(G_OBJECT(clocks->b_disable), "activate", G_CALLBACK(disable_menu_item), clocks);
    
    /* Disable all button in File menu */
    clocks->b_disable_all = gtk_menu_item_new_with_mnemonic(_("Disable All"));
    gtk_widget_add_accelerator(clocks->b_disable_all, "activate", clocks->accel_group,
        GDK_KEY_Katakana, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_disable_all);
    g_signal_connect(G_OBJECT(clocks->b_disable_all), "activate", G_CALLBACK(disable_all_menu_item), clocks);
    
    /* Enable button in File menu */
    clocks->b_enable = gtk_menu_item_new_with_mnemonic(_("_Enable"));
    gtk_widget_add_accelerator(clocks->b_enable, "activate", clocks->accel_group,
        GDK_KEY_plus, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_enable);
    g_signal_connect(G_OBJECT(clocks->b_enable), "activate", G_CALLBACK(enable_menu_item), clocks);
    
    /* Enable all button in File menu */
    clocks->b_enable_all = gtk_menu_item_new_with_mnemonic(_("Enable All"));
    gtk_widget_add_accelerator(clocks->b_enable_all, "activate", clocks->accel_group,
        GDK_KEY_Hiragana, 0, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_enable_all);
    g_signal_connect(G_OBJECT(clocks->b_enable_all), "activate", G_CALLBACK(enable_all_menu_item), clocks);
    
    /* Quit button in File menu */
    clocks->b_quit = gtk_menu_item_new_with_mnemonic(_("_Quit"));
    gtk_widget_add_accelerator(clocks->b_quit, "activate", clocks->accel_group,
        GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->filemenu), clocks->b_quit);
    g_signal_connect(G_OBJECT(clocks->b_quit), "activate", G_CALLBACK(quit_menu_item), clocks);
    
    /* View pulldown menu */
    clocks->view = gtk_menu_item_new_with_mnemonic(_("_View"));
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->menubar), clocks->view);
    clocks->viewmenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(clocks->view), clocks->viewmenu);

    /* Enable Resize button in View menu */
    clocks->b_disable_resize = gtk_menu_item_new_with_mnemonic(_("Enable/Disable Resize"));
    gtk_widget_add_accelerator(clocks->b_disable_resize, "activate", clocks->accel_group,
        GDK_KEY_d, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->viewmenu), clocks->b_disable_resize);
    g_signal_connect(G_OBJECT(clocks->b_disable_resize), "activate", G_CALLBACK(disable_resize_menu_item), clocks);

    /* Reset Minimum Size button in View menu */
    clocks->b_reset_size = gtk_menu_item_new_with_mnemonic(_("_Reset Size to default"));
    gtk_widget_add_accelerator(clocks->b_reset_size, "activate", clocks->accel_group,
        GDK_KEY_f, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->viewmenu), clocks->b_reset_size);
    g_signal_connect(G_OBJECT(clocks->b_reset_size), "activate", G_CALLBACK(reset_size_menu_item), clocks);

    /* Help pulldown menu */
    clocks->help = gtk_menu_item_new_with_mnemonic(_("_Help"));
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->menubar), clocks->help);
    clocks->helpmenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(clocks->help), clocks->helpmenu);

    clocks->b_about = gtk_menu_item_new_with_mnemonic(_("_About"));
    gtk_widget_add_accelerator(clocks->b_about, "activate", clocks->accel_group,
        GDK_KEY_a, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_menu_shell_append(GTK_MENU_SHELL(clocks->helpmenu), clocks->b_about);
    g_signal_connect(G_OBJECT(clocks->b_about), "activate", G_CALLBACK(about_menu_item), clocks);
    
    //prepare clocks->hgridLBottom buttons
    clocks->bb_add = gtk_button_new_with_label("");
    clocks->bb_add_label = get_button_first_label_child(clocks->bb_add);
    format_button(clocks->bb_add_label,b1text);
    gtk_widget_set_tooltip_text(GTK_WIDGET(clocks->bb_add), _("New") );
    
    clocks->bb_delete = gtk_button_new_with_label("");
    clocks->bb_delete_label = get_button_first_label_child(clocks->bb_delete);
    format_button(clocks->bb_delete_label,b2text);   
    gtk_widget_set_tooltip_text(GTK_WIDGET(clocks->bb_delete), _("Delete") );
    
    clocks->bb_enable_all = gtk_button_new_with_label ("");
    clocks->bb_enable_all_label = get_button_first_label_child(clocks->bb_enable_all);
    format_button(clocks->bb_enable_all_label,b4text);
    gtk_widget_set_tooltip_text(GTK_WIDGET(clocks->bb_enable_all), _("Enable All") );
    
    clocks->bb_disable_all = gtk_button_new_with_label ("");
    clocks->bb_disable_all_label = get_button_first_label_child(clocks->bb_disable_all);
    format_button(clocks->bb_disable_all_label,b3text);
    gtk_widget_set_tooltip_text(GTK_WIDGET(clocks->bb_disable_all), _("Disable All") );
    
    clocks->bb_switch_trigger = gtk_button_new_with_label ("");
    clocks->bb_switch_trigger_label = get_button_first_label_child(clocks->bb_switch_trigger);
    format_button(clocks->bb_switch_trigger_label,b5text);
    gtk_widget_set_tooltip_text(GTK_WIDGET(clocks->bb_switch_trigger), _("Switch") );

    // Connect the "clicked" signal of the button to our callback
    g_signal_connect (G_OBJECT(clocks->bb_add), "clicked", G_CALLBACK (add_button), NULL);
    g_signal_connect (G_OBJECT(clocks->bb_delete), "clicked", G_CALLBACK (delete_button), NULL);
    g_signal_connect (G_OBJECT(clocks->bb_enable_all), "clicked", G_CALLBACK (enable_all_button), NULL);
    g_signal_connect (G_OBJECT(clocks->bb_disable_all), "clicked", G_CALLBACK (disable_all_button), NULL);    
    g_signal_connect (G_OBJECT(clocks->bb_switch_trigger), "clicked", G_CALLBACK(switch_button), NULL);
    
    //fill up clocks->hgridLBottom
    gtk_grid_attach (GTK_GRID (clocks->hgridLBottom), clocks->bb_add,0,0,1,1);
    gtk_grid_attach (GTK_GRID (clocks->hgridLBottom), clocks->bb_delete,1,0,1,1);
    gtk_grid_attach (GTK_GRID (clocks->hgridLBottom), clocks->bb_enable_all,2,0,1,1);
    gtk_grid_attach (GTK_GRID (clocks->hgridLBottom), clocks->bb_disable_all,3,0,1,1);    
    gtk_grid_attach (GTK_GRID (clocks->hgridLBottom), clocks->bb_switch_trigger,4,0,1,1);
    
    //fill up clocks->hboxRBottom
    // RESEVRED FOR CLOCKS
    
    
    //fill up clocks->hboxRTop
#if GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION > 15
    clocks->gl_area = gtk_gl_area_new();
    gtk_box_pack_start(GTK_BOX(clocks->hboxRTop),clocks->gl_area, TRUE, TRUE, 1);
    g_signal_connect (clocks->gl_area, "render", G_CALLBACK (render), NULL);
#endif
    
    //create selectable label for current time
    clocks->l_time = gtk_label_new(NULL);
    gtk_label_set_selectable(GTK_LABEL(clocks->l_time),TRUE);
    
    //start timer 
    start_timer(NULL,clocks->l_time);
    g_timeout_add_seconds(1, update_time, clocks->l_time);
    gtk_box_pack_start(GTK_BOX(clocks->hboxRBottom), clocks->l_time, TRUE, TRUE, 0);
    
    init_sizing(clocks,0);
    
    gtk_window_set_resizable(GTK_WINDOW(clocks->window), TRUE);

    /* init the rest */
	reset_default_status(clocks);
}

#if GTK_MAJOR_VERSION == 3 && GTK_MINOR_VERSION > 15
static gboolean render (GtkGLArea *area, GdkGLContext *context)
{
  /*// inside this function it's safe to use GL; the given
  // #GdkGLContext has been made current to the drawable
  // surface used by the #GtkGLArea and the viewport has
  // already been set to be the size of the allocation

  // we can start by clearing the buffer
  glClearColor (0, 0, 0, 0);
  glClear (GL_COLOR_BUFFER_BIT);

  // draw your object
  draw_an_object ();

  // we completed our drawing; the draw commands will be
  // flushed at the end of the signal emission chain, and
  // the buffers will be drawn on the window*/
  return TRUE;
}
#endif

/* window destroyed */
void destroy_signal(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

/* request for window close from WM */
gboolean delete_event(GtkWidget *widget, GdkEvent *event, AlarmClock *clocks)
{
	return FALSE;   	/* send destroy signal */
}

/* quit clocks */
void quit_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks)
{
	gtk_main_quit();
}

/* about application */
void about_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks)
{
	static const gchar * const authors[] = {
		"Martin Risa <xrisam00@stud.fit.vutbr.cz>",
		NULL
	};
	gtk_show_about_dialog (GTK_WINDOW (clocks->window),
		"authors", authors,
		"comments", "Gtk+ Example",
		"copyright", "Copyright \xc2\xa9 2017 Martin Risa",
		"version", "0.99",
		"website", "Kurt Kombajn @smazeny mozog on twitter and others",
		"program-name", "Gtk+ Alarm Clock",
		"logo-icon-name", NULL,
		NULL); 
}

Alarm * get_alarm()
{
    Alarm * ret = malloc(sizeof(Alarm));
    
    ret->enabled = false;
    ret->ss = 0;
    ret->mm = 0;
    ret->hh = 0;
    ret->repeat = false;
    
    return ret;
}

void add_alarm_united()
{
    //create new button
    char label[20];
    sprintf(label,"%d.",++clocks->ac);
    
    GtkWidget * button = gtk_button_new_with_label("");
    GtkWidget * tmp = get_button_first_label_child(button);
    format_button(tmp,label);
    
    gtk_box_pack_start(GTK_BOX(clocks->vboxLTop), button, FALSE, FALSE, 0);
    
    g_signal_connect_after(G_OBJECT(button), "clicked", G_CALLBACK (alarm_selected), NULL);
    
    if(clocks->alarms == NULL)
    {
        clocks->actual_alarms_count = 0;
        clocks->max_alarms = ALLOC_STEP;
        
        clocks->alarms = malloc(sizeof(Alarm *) * ALLOC_STEP);
        clocks->alarmButtons = malloc(sizeof(GtkWidget *) * ALLOC_STEP);
    }
    
    if(clocks->ac > clocks->max_alarms)
    {
        clocks->max_alarms += ALLOC_STEP;
        clocks->alarms = realloc(clocks->alarms,clocks->max_alarms);
        clocks->alarmButtons = realloc(clocks->alarmButtons,clocks->max_alarms);
    }
    
    if(clocks->ac > clocks->actual_alarms_count)
        ++clocks->actual_alarms_count;
    
    if(clocks->ac == MAX_ALARMS)
    {
         gtk_widget_set_sensitive (GTK_WIDGET(clocks->bb_add), FALSE);
         clocks->add_disabled = true;
    }
    
    if(clocks->currentAlarm != NULL)
        gtk_widget_set_sensitive (GTK_WIDGET(clocks->currentAlarmButton), TRUE);
    gtk_widget_set_sensitive (GTK_WIDGET(button), FALSE);
    
    clocks->currentAlarm = clocks->alarms[clocks->ac - 1] = get_alarm();
    clocks->currentAlarmButton = clocks->alarmButtons[clocks->ac - 1] = button;
    clocks->currentAlarmPosition = clocks->ac - 1;   
    
    redraw();
}

void delete_alarm_united()
{
    if(clocks->currentAlarm != NULL)
    {
        int i = clocks->currentAlarmPosition;
        
        gtk_widget_destroy(GTK_WIDGET(clocks->currentAlarmButton));
        
        free(clocks->alarms[i]);
        clocks->alarms[i] = NULL;
        clocks->alarmButtons[i] = NULL;
        clocks->currentAlarm = NULL;
        clocks->currentAlarmButton = NULL;
        
        --clocks->ac;
        
        gtk_widget_set_sensitive (GTK_WIDGET(clocks->bb_add), TRUE);
        
        redraw();
    }    
}

void new_menu_item(GtkMenuItem *menuitem, AlarmClock *c)
{
    add_alarm_united();
}

void delete_menu_item(GtkMenuItem *menuitem, AlarmClock *c)
{   
    delete_alarm_united();
}

void delete_all_menu_item(GtkMenuItem *menuitem, AlarmClock *c)
{
    int i = 0;
    for(; i< clocks->ac; ++i)
    {
        
        free(clocks->alarms[i]);
        gtk_widget_destroy(GTK_WIDGET(clocks->alarmButtons[i]));
    }
    clocks->currentAlarm = NULL;
    clocks->currentAlarmButton = NULL;
    
    free(clocks->alarms);
    free(clocks->alarmButtons);
    
    clocks->alarms = NULL;
    clocks->alarmButtons = NULL;
    
    clocks->ac = 0;
    clocks->max_alarms = 0;
    
    if(i != 0)
        redraw();
}

void disable_menu_item(GtkMenuItem *menuitem, AlarmClock *c)
{
    if(clocks->currentAlarm != NULL)
    {
        clocks->currentAlarm->enabled = false;
        redraw();
    }
}

void disable_all_united()
{
    int i = 0;
    for(; i< clocks->ac; ++i)
        if(clocks->alarms[i] != NULL)
            clocks->alarms[i]->enabled = false;
    
    if(i != 0)
        redraw();
}

void disable_all_menu_item(GtkMenuItem *menuitem, AlarmClock *c)
{
    disable_all_united();
}

void enable_menu_item(GtkMenuItem *menuitem, AlarmClock *c)
{
    if(clocks->currentAlarm != NULL)
    {
        clocks->currentAlarm->enabled = true;
        redraw();
    }
}

void enable_all_united()
{
    int i = 0;
    for(; i< clocks->ac; ++i)
        if(clocks->alarms[i] != NULL)
            clocks->alarms[i]->enabled = true;
    
    if(i != 0)
        redraw();
}

void enable_all_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks)
{
    enable_all_united();
}

void add_button(GtkWidget *menuitem, AlarmClock *clocks)
{
    add_alarm_united();
}

void delete_button(GtkWidget *menuitem, AlarmClock *clocks)
{
    delete_alarm_united();    
}

void disable_all_button(GtkWidget *menuitem, AlarmClock *clocks)
{
    disable_all_united();
}

void enable_all_button(GtkWidget *menuitem, AlarmClock *clocks)
{
    enable_all_united();
}

void switch_button(GtkWidget *menuitem, AlarmClock *c)
{    
    if(clocks->currentAlarm != NULL)
    {
        if(clocks->currentAlarm->enabled == false)
            clocks->currentAlarm->enabled = true;
        else
            clocks->currentAlarm->enabled = false;
        
        redraw();
    }
}

gboolean update_time(gpointer data)
{
    time_t rawtime;
    time(&rawtime);
    struct tm * timeinfo = localtime (&rawtime);
    
    GtkLabel *label = GTK_LABEL(clocks->l_time);
    
    char buf[256];
    //set clock font size and style
    snprintf(buf, 255, "%d : %d : %d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    char *  text = get_text_formated(buf);
    
    gtk_label_set_markup (label, text);
    
    free(text);
    
    return true;
}

void start_timer (GtkWidget * w, gpointer data)
{
    GtkWidget *label = data;
    g_timeout_add_seconds(1, update_time, label);
}

void my_get_allocation(GtkWidget * ww, int * w, int * h)
{
#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 17
    *w = clocks->hboxchild->allocation.width;
    *h = clocks->hboxchild->allocation.height;    
#else
    GtkAllocation* allocation = g_new(GtkAllocation, 1);
    
    gtk_widget_get_allocation(GTK_WIDGET(ww), allocation);
    
    *w = allocation->width;
    *h = allocation->height;
    
    g_free(allocation);
#endif
}

void my_set_allocation(GtkWidget * ww, int w, int h)
{
    gtk_widget_set_size_request(GTK_WIDGET(ww),w,h);    
/*#if GTK_MAJOR_VERSION == 2 && GTK_MINOR_VERSION <= 17
    ww->allocation.width = w;
    ww->allocation.height = h;
#else
    GtkAllocation* allocation = g_new(GtkAllocation, 1);
    
    allocation->width = w;
    allocation->height = h;
    gtk_widget_set_allocation(GTK_WIDGET(ww), allocation);
    
    g_free(allocation);
#endif*/
}

void init_sizing(AlarmClock *clocks, int method)
{  
    //fprintf(stderr,"-----------------------------------------------------\n");
    int width,height;
    if(method == 0)
    {
        //get actuall size of hbox we want ot keep with dynamic size
        my_get_allocation(clocks->hboxchild, &width, &height);
    }
    else
    {
        //get default values
        width = DEFAULT_WINDOW_WIDTH;
        height = DEFAULT_WINDOW_HEIGHT;
    }
    
    //if hbox didn't change
    if(previous_width == width && previous_height == height)
        return;
    
    previous_width = width;
    previous_height = height;
    
    //work with a little smaller sizes
    width -= BULGARIAN_CONSTANT;
    height -= BULGARIAN_CONSTANT;
    
    //check negative and too small values
    if(width < BULGARIAN_CONSTANT)
        return;
        
    if(height < BULGARIAN_CONSTANT)
        return;
    
    int widthleft = width * LEFT_BOX_WIDTH_RELATIVE / 100;
    
    my_set_allocation(GTK_WIDGET(clocks->vboxLeft),widthleft,height);
    my_set_allocation(GTK_WIDGET(clocks->vboxRight),width - widthleft,height);
    
    int heightbottom = height * LEFT_BOTTOM_BOX_HEIGHT_RELATIVE / 100;
    my_set_allocation(GTK_WIDGET(clocks->hgridLBottom),widthleft,heightbottom);
    my_set_allocation(GTK_WIDGET(clocks->vboxLTop),widthleft,height - heightbottom);
    int lbot_font_size = heightbottom/2 - BULGARIAN_CONSTANT;
    
    //fprintf(stderr,"PICKED FONT SIZE FOR BUTTONS: h=%d size=%d\n",heightbottom,lbot_font_size);
    //int ltopw = widthleft;
    //int ltoph = height - heightbottom;
    
    my_set_allocation(GTK_WIDGET(clocks->bb_add), widthleft / 5, heightbottom);
    my_set_allocation(GTK_WIDGET(clocks->bb_delete), widthleft / 5, heightbottom);
    my_set_allocation(GTK_WIDGET(clocks->bb_disable_all), widthleft / 5, heightbottom);
    my_set_allocation(GTK_WIDGET(clocks->bb_enable_all), widthleft / 5, heightbottom);
    my_set_allocation(GTK_WIDGET(clocks->bb_switch_trigger), widthleft - 4*(widthleft / 5), heightbottom);
    
    heightbottom = height * RIGHT_BOTTOM_BOX_HEIGHT_RELATIVE / 100;
    my_set_allocation(GTK_WIDGET(clocks->hboxRBottom),width - widthleft,heightbottom);
    my_set_allocation(GTK_WIDGET(clocks->hboxRTop),width - widthleft,height - heightbottom);
    int rbot_font_size = heightbottom/2 - BULGARIAN_CONSTANT;
    //fprintf(stderr,"PICKED FONT SIZE FOR CLOCKS: h=%d size=%d\n",heightbottom,rbot_font_size);
    
    //set font size of labels that change only during resize
    clocks->absolute_font_size = lbot_font_size;
    reset_left_labels_to_height();
    //set font size for dynamic labels (eg. clocks)
    clocks->absolute_font_size = rbot_font_size;   
    update_time(NULL);
    
    
    //TODO set sizing of alarm list labels
}

char * FORMAT_1_OF_3 = "<span size='";
char * FORMAT_2_OF_3 = "'>";
char * FORMAT_3_OF_3 = "</span>";

char * get_text_formated(const char * content)
{
    char buf[256];
    char * str = malloc(DYNAMIC_BUFFER_SIZE);    
    
    strcpy(str,FORMAT_1_OF_3);    
    snprintf(buf, 255, "%d", clocks->absolute_font_size*1000);
    strcat(str, buf);    
    strcat(str,FORMAT_2_OF_3);    
    strcat(str, content);    
    strcat(str, FORMAT_3_OF_3);
    
    return str;
}

GtkWidget * get_button_first_label_child(GtkWidget * w)
{
    //we expect that button_with_label has allways first child its label
    GList * childrens = gtk_container_get_children(GTK_CONTAINER(w));
    GtkWidget * ret = GTK_WIDGET(childrens->data);
    g_list_free(childrens);
    return ret;
}

void format_button(GtkWidget * w, const char * content)
{
    char * text = get_text_formated(content);        
    gtk_label_set_markup(GTK_LABEL(w), text);
    free(text);
}

static gboolean window_expose_event (GtkWidget *widget, GdkEventExpose *event)
{
    init_sizing(clocks,0);    
    return FALSE;
}

void alarm_selected(GtkWidget *w, AlarmClock *c)
{
    //find desired widget
    GtkWidget ** tmp = clocks->alarmButtons;
    int i=0;
    while((*tmp) != w && i < clocks->actual_alarms_count)
    {
        ++i;
        ++tmp;
    }
    
    //if something found
    if(*tmp == w)
    {
        //change button state to "selected"
        gtk_widget_set_sensitive (GTK_WIDGET(w), FALSE);
        
        //if there was some previos selection, enable it
        if(clocks->currentAlarmButton != NULL)
            gtk_widget_set_sensitive (GTK_WIDGET(clocks->currentAlarmButton), TRUE);
        
        //change selection to current
        clocks->currentAlarmButton = w;
        clocks->currentAlarm = clocks->alarms[i];
        clocks->currentAlarmPosition = i;
        
        redraw();
    }
}

gboolean finish_resizing_after_unmaximize()
{
    gtk_window_resize (GTK_WINDOW(clocks->window),DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);
    gtk_widget_set_size_request(GTK_WIDGET(clocks->vboxLeft),0,0);
    gtk_widget_set_size_request(GTK_WIDGET(clocks->vboxLTop),0,0);
    gtk_widget_set_size_request(GTK_WIDGET(clocks->hgridLBottom),0,0);
    gtk_widget_set_size_request(GTK_WIDGET(clocks->vboxRight),0,0);
    gtk_widget_set_size_request(GTK_WIDGET(clocks->hboxRTop),0,0);
    gtk_widget_set_size_request(GTK_WIDGET(clocks->hboxRBottom),0,0);
    
    gtk_widget_set_size_request(GTK_WIDGET(clocks->hboxchild), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    init_sizing(clocks,1);    
    
    return FALSE;
}

void reset_size_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks)
{    
    //unmaximize window
    gtk_window_unmaximize(GTK_WINDOW(clocks->window));
    //when idle finish resizing... SOMETIMES DOESNT WORK AND DON'T KNOW WHY. ANYWAY SECOND CALL(after some time) ALLWAYS WORKS
    //IMPORTANT: WE HAVE TO WAIT TILL UNMAXIMAZIATION TAKES EFFECT(we are lazy to catch another signal when it actuall does so we wait for the next idle state, that may possibly never happen but who cares about such extreme cases)
    g_idle_add (finish_resizing_after_unmaximize,
            NULL);
}

void disable_resize_menu_item(GtkMenuItem *menuitem, AlarmClock *clocks)
{
    
    if(gtk_window_get_resizable (GTK_WINDOW(clocks->window)) == TRUE)
        gtk_window_set_resizable(GTK_WINDOW(clocks->window), FALSE);
    else
        gtk_window_set_resizable(GTK_WINDOW(clocks->window), TRUE);
}

void reset_left_labels_to_height()
{
    format_button(clocks->bb_add_label,b1text);
    format_button(clocks->bb_delete_label,b2text);   
    format_button(clocks->bb_enable_all_label,b4text);
    format_button(clocks->bb_disable_all_label,b3text);
    format_button(clocks->bb_switch_trigger_label,b5text);
}
