
#include <gtk/gtk.h>
#include <strings.h>

#include "vars.h"
#include "externs.h"


static void
id_remove_labels_cb (GtkWidget *w, ggobid *gg)
{
  g_slist_free (gg->identify.sticky_ids);
  gg->identify.sticky_ids = (GSList *) NULL;
  displays_plot (NULL, QUICK, gg);
}
static void
id_all_sticky_cb (GtkWidget *w, ggobid *gg)
{
  gint i, m;

  /*-- clear the list before adding to avoid redundant entries --*/
  g_slist_free (gg->identify.sticky_ids);
  gg->identify.sticky_ids = (GSList *) NULL;

  for (m=0; m<gg->nrows_in_plot; m++) {
    i = gg->rows_in_plot[m];
    gg->identify.sticky_ids = g_slist_append (gg->identify.sticky_ids,
                                              GINT_TO_POINTER (i));
  }
  displays_plot (NULL, QUICK, gg);
}
static void
identify_link_cb (GtkCheckMenuItem *w, gpointer cbd)
{
  gchar *lbl = (gchar *) cbd;
  g_printerr("state: %d, cbd: %s\n", w->active, lbl);
}

/*----------------------------------------------------------------------*/
/*                     Handling mouse events                            */
/*----------------------------------------------------------------------*/

static void
displays_add_point_labels (splotd *splot, gint k, ggobid *gg) {
  GList *dlist, *slist;
  displayd *display;
  splotd *sp;
  GtkWidget *w;

  for (dlist = gg->displays; dlist; dlist = dlist->next) {
    display = (displayd *) dlist->data;
    for (slist = display->splots; slist; slist = slist->next) {
      sp = (splotd *) slist->data;
      if (sp != splot) {
        w = sp->da;
        splot_pixmap0_to_pixmap1 (sp, false, gg);
        splot_add_point_label (sp, k, true, gg);  /*-- nearest = true --*/
        splot_pixmap1_to_window (sp, gg);
      }
    }
  }
}


static gint
motion_notify_cb (GtkWidget *w, GdkEventButton *event, splotd *sp)
{
  gint k;
  ggobid *gg = GGobiFromSPlot(sp);

/*
 * w = sp->da
 * sp = gtk_object_get_data (GTK_OBJECT (w), "splotd"));
*/

  gdk_gc_set_foreground (gg->plot_GC, &gg->accent_color);
/*-- use mousepos_get_motion for this --*/
  gdk_window_get_pointer (w->window,
    &gg->mousepos.x, &gg->mousepos.y, NULL);
  k = find_nearest_point (&gg->mousepos, sp, gg);
  gg->app.nearest_point = k;

  if (k != gg->app.nearest_point_prev) {

    splot_pixmap0_to_pixmap1 (sp, false, gg);

    if (k != -1)  /*-- nearest --*/
      splot_add_point_label (sp, k, true, gg);

    splot_pixmap1_to_window (sp, gg);
    
    if (k != -1)
      displays_add_point_labels (sp, k, gg);

    if (gg->identify_handler.handler) {
      (gg->identify_handler.handler)(gg->identify_handler.user_data,
        k, sp, w, gg);
    }

    gg->app.nearest_point_prev = k;
  }

  return true;  /* no need to propagate the event */
}

static gint
button_press_cb (GtkWidget *w, GdkEventButton *event, splotd *sp)
{
/*
 * If nearest_point is a member of gg->identify.sticky_ids, remove it; if
 * it isn't, add it.
*/
  ggobid *gg = GGobiFromSPlot (sp);
  gint id;
  gboolean id_in_list = false;
  gpointer ptr;

  if (gg->app.nearest_point != -1) {

    if (g_slist_length (gg->identify.sticky_ids) > 0) {
      GSList *l;
      for (l = gg->identify.sticky_ids; l; l = l->next) {
        id = GPOINTER_TO_INT (l->data);
        if (id == gg->app.nearest_point) {
          id_in_list = true;
          ptr = l->data;
          break;
        }
      }

      if (id_in_list)
        gg->identify.sticky_ids = g_slist_remove (gg->identify.sticky_ids, ptr);
    }

    if (!id_in_list) {
      ptr = GINT_TO_POINTER (gg->app.nearest_point);
      gg->identify.sticky_ids = g_slist_append (gg->identify.sticky_ids, ptr);
    }
  }

  return true;
}

static gint
button_release_cb (GtkWidget *w, GdkEventButton *event, splotd *sp)
{
  return true;
}

void
identify_event_handlers_toggle (splotd *sp, gboolean state) {
  
  if (state == on) {
    sp->press_id = gtk_signal_connect (GTK_OBJECT (sp->da),
                                       "button_press_event",
                                       (GtkSignalFunc) button_press_cb,
                                       (gpointer) sp);
    sp->release_id = gtk_signal_connect (GTK_OBJECT (sp->da),
                                         "button_release_event",
                                         (GtkSignalFunc) button_release_cb,
                                         (gpointer) sp);
    sp->motion_id = gtk_signal_connect (GTK_OBJECT (sp->da),
                                        "motion_notify_event",
                                        (GtkSignalFunc) motion_notify_cb,
                                        (gpointer) sp);
  } else {
    gtk_signal_disconnect (GTK_OBJECT (sp->da), sp->press_id);
    gtk_signal_disconnect (GTK_OBJECT (sp->da), sp->release_id);
    gtk_signal_disconnect (GTK_OBJECT (sp->da), sp->motion_id);
  }

}


/*----------------------------------------------------------------------*/
/*                   Resetting the main menubar                         */
/*----------------------------------------------------------------------*/


void
identify_menus_make (ggobid *gg) {
  GtkWidget *item;

/*
 * Link menu
*/
  gg->identify.link_menu = gtk_menu_new ();

  item = gtk_check_menu_item_new_with_label("Link identification");
  gtk_signal_connect (GTK_OBJECT (item), "toggled",
                      GTK_SIGNAL_FUNC (identify_link_cb),
                      (gpointer) NULL);
  gtk_menu_append (GTK_MENU (gg->identify.link_menu), item);
  gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (item), true);
  gtk_widget_show(item);
}

void
cpanel_identify_make(ggobid *gg) {
  GtkWidget *btn;
  
  gg->control_panel[IDENT] = gtk_vbox_new(false, VBOX_SPACING);
  gtk_container_set_border_width(GTK_CONTAINER(gg->control_panel[IDENT]), 5);

/*
 * button for removing all labels
*/
  btn = gtk_button_new_with_label ("Remove labels");
  gtk_tooltips_set_tip (GTK_TOOLTIPS (gg->tips),
                        btn, "Remove all labels", NULL);
  gtk_signal_connect (GTK_OBJECT (btn), "clicked",
                      GTK_SIGNAL_FUNC (id_remove_labels_cb),
                      gg);
  gtk_box_pack_start (GTK_BOX (gg->control_panel[IDENT]),
                      btn, false, false, 1);

/*
 * button for making all labels sticky
*/
  btn = gtk_button_new_with_label ("Make all sticky");
  gtk_tooltips_set_tip (GTK_TOOLTIPS(gg->tips),
                        btn, "Make all labels sticky, or persistent", NULL);
  gtk_signal_connect (GTK_OBJECT (btn), "clicked",
                      GTK_SIGNAL_FUNC (id_all_sticky_cb),
                      (gpointer) gg);
  gtk_box_pack_start (GTK_BOX (gg->control_panel[IDENT]),
                      btn, false, false, 1);

  gtk_widget_show_all (gg->control_panel[IDENT]);
}

