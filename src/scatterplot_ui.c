/* scatterplot_ui.c */
/*
 * ggobi
 * Copyright (C) AT&T, Duncan Temple Lang, Dianne Cook 1999-2005
 *
 * ggobi is free software; you may use, redistribute, and/or modify it
 * under the terms of the Common Public License, which is distributed
 * with the source code and displayed on the ggobi web site, 
 * www.ggobi.org.  For more information, contact the authors:
 *
 *   Deborah F. Swayne   dfs@research.att.com
 *   Di Cook             dicook@iastate.edu
 *   Duncan Temple Lang  duncan@wald.ucdavis.edu
 *   Andreas Buja        andreas.buja@wharton.upenn.edu
*/

#include <gtk/gtk.h>
#include "vars.h"
#include "externs.h"

/*--------------------------------------------------------------------*/
/*                   Resetting the main menubar                       */
/*--------------------------------------------------------------------*/

static const gchar *mode_ui_str =
"<ui>"
"	<menubar>"
"		<menu action='PMode'>"
"			<menuitem action='1D Plot'/>"
"			<menuitem action='XY Plot'/>"
"			<menuitem action='1D Tour'/>"
"			<menuitem action='Rotation'/>"
"			<menuitem action='2D Tour'/>"
"			<menuitem action='2x1D Tour'/>"
"		</menu>"
"		<menu action='IMode'>"
"			<menuitem action='DefaultIMode'/>"
"			<separator/>"
"			<menuitem action='Scale'/>"
"			<menuitem action='Brush'/>"
"			<menuitem action='Identify'/>"
"			<menuitem action='Edit edges'/>"
"			<menuitem action='Move points'/>"
"		</menu>"
"	</menubar>"
"</ui>";

const gchar *
scatterplot_mode_ui_get(displayd *display)
{
	return(mode_ui_str);
}
#if 0
GtkWidget *
scatterplot_pmode_menu_make (GtkAccelGroup *accel_group, GtkSignalFunc func,
			    ggobid *gg, gboolean useIds)
{
  /* Projections */
  GtkWidget *pmode_menu, *item;
  gboolean radiop = sessionOptions->useRadioMenuItems;

  pmode_menu = gtk_menu_new ();

  item = CreateMenuItemWithCheck (pmode_menu, "1D Plot",
    "^d", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER  (P1PLOT) : gg, gg,
    gg->pmodeRadioGroup, radiop);
  if (radiop && gg->pmode == P1PLOT)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (pmode_menu, "XYPlot",
    "^x", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER  (XYPLOT) : gg, gg,
    gg->pmodeRadioGroup, radiop);
  if (radiop && gg->pmode == XYPLOT)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (pmode_menu, "1D Tour",
    "^t", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER  (TOUR1D) : gg, gg,
    gg->pmodeRadioGroup, radiop);
  if (radiop && gg->pmode == TOUR1D)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (pmode_menu, "Rotation",
    "^r", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER  (TOUR2D3) : gg, gg,
    gg->pmodeRadioGroup, radiop);
  if (radiop && gg->pmode == TOUR2D3)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (pmode_menu, "2D Tour",
    "^g", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER  (TOUR2D) : gg, gg,
    gg->pmodeRadioGroup, radiop);
  if (radiop && gg->pmode == TOUR2D)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (pmode_menu, "2x1D Tour",
    "^c", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER  (COTOUR) : gg, gg,
    gg->pmodeRadioGroup, radiop);
  if (radiop && gg->pmode == COTOUR)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  gtk_widget_show (pmode_menu);
  return (pmode_menu);
}

GtkWidget *
scatterplot_imode_menu_make (GtkAccelGroup *accel_group, GtkSignalFunc func,
			    ggobid *gg, gboolean useIds)
{
  /* Interaction modes for scatterplot.  The first one is "Default"
     which is generally going to mean we want to see the control panel
     corresponding to the projection. */

  GtkWidget *imode_menu, *item;
  gboolean radiop = sessionOptions->useRadioMenuItems;

  imode_menu = gtk_menu_new ();

  item = CreateMenuItemWithCheck (imode_menu,
    (gg->pmode != -1) ? (gchar *)GGOBI(getPModeName)(gg->pmode) : "(Default)",
    "^h", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER (DEFAULT_IMODE) : gg, gg, 
    gg->imodeRadioGroup, radiop);
  if (radiop && gg->imode == DEFAULT_IMODE)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  /* Add a separator? */
  CreateMenuItem (imode_menu, "",
    "", "", NULL, NULL, NULL, NULL, gg);

  item = CreateMenuItemWithCheck (imode_menu, "Scale",
    "^s", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER (SCALE) : gg, gg, 
    gg->imodeRadioGroup, radiop);
  if (radiop && gg->imode == SCALE)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (imode_menu, "Brush",
    "^b", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER (BRUSH) : gg, gg, 
    gg->imodeRadioGroup, radiop);
  if (radiop && gg->imode == BRUSH)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (imode_menu, "Identify",
    "^i", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER (IDENT) : gg, gg, 
    gg->imodeRadioGroup, radiop);
  if (radiop && gg->imode == IDENT)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (imode_menu, "Edit edges",
    "^e", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER (EDGEED) : gg, gg, 
    gg->imodeRadioGroup, radiop);
  if (radiop && gg->imode == EDGEED)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  item = CreateMenuItemWithCheck (imode_menu, "Move points",
    "^m", "", NULL, accel_group, func,
    useIds ? GINT_TO_POINTER (MOVEPTS) : gg, gg, 
    gg->imodeRadioGroup, radiop);
  if (radiop && gg->imode == MOVEPTS)
     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), true);

  gtk_widget_show (imode_menu);
  return (imode_menu);
}
#endif

/*--------------------------------------------------------------------*/
/*                   Setting the display menubar                      */
/*--------------------------------------------------------------------*/

static const gchar *edge_ui = 
"<ui>"
"	<menubar>"
"		<menu action='Edges'>"
"			<menu action='Edgesets'/>"
"		</menu>"
"	</menubar>"
"</ui>";

static const gchar *edge_option_ui =
"<ui>"
"	<menubar>"
"		<menu action='Edges'>"
"			<separator/>"
"			<menuitem action='ShowUndirectedEdges'/>"
"			<menuitem action='ShowDirectedEdges'/>"
"			<menuitem action='ShowArrowheadsOnly'/>"
"			<menuitem action='HideEdges'/>"
"		</menu>"
"	</menubar>"
"</ui>";

/*
 * This handles the initialization of the edge menu item and menu,
 * and it should also be called whenever the number of edge sets
 * might have changed:  when an edge set is added or removed, and
 * when a datad is added or removed.
*/
void
scatterplot_display_edge_menu_update (displayd *display,
                                      GtkAccelGroup *accel_group, ggobid *gg)
{
  datad *d = display->d;  /*-- this dataset --*/
  gint nd = g_slist_length (gg->d);
  datad *e;
  gint k, ne = 0;

  /*-- If this datad has ids, find the number of other datad's with
       edges --*/

  if (d->rowIds) {
    endpointsd *endpoints;
    for (k=0; k<nd; k++) { 
      e = (datad*) g_slist_nth_data (gg->d, k);
      if (e->edge.n > 0) {
        endpoints = resolveEdgePoints(e, d);
        if (endpoints != NULL) {
          ne++;
        }
      }
    }
  }

  /*-- remove any existing submenu --*/
  if (display->edge_merge != -1) {
    gtk_ui_manager_remove_ui(display->menu_manager, display->edge_merge);
    if (display->edge_option_merge != -1)
      gtk_ui_manager_remove_ui(display->menu_manager, display->edge_option_merge);
    if (ne < 1) {
      /*-- destroy menu item if there are no edge sets --*/
      display->edge_merge = display->edge_option_merge = -1;
    }
  }

  /*-- then build the new menu if appropriate --*/
  if (ne) {
    GtkAction *action = NULL;
    GSList *group = NULL;
    const gchar *tooltip = "Attach this edge dataset";
    GtkActionGroup *actions = gtk_action_group_new("Edge Datasets");
	
    if (display->edgeset_action_group) {
	gtk_ui_manager_remove_action_group(display->menu_manager, 
	  display->edgeset_action_group);
	g_object_unref(G_OBJECT(display->edgeset_action_group));
    }
    gtk_ui_manager_insert_action_group(display->menu_manager, actions, -1);
    display->edgeset_action_group = actions;
    
	/*-- build the menu --*/
    display->edge_merge = gtk_ui_manager_add_ui_from_string(display->menu_manager, 
	  edge_ui, -1, NULL);

    if (display->e) {
      gtk_ui_manager_ensure_update(display->menu_manager);
      display->edge_option_merge = gtk_ui_manager_add_ui_from_string(
	  display->menu_manager, edge_option_ui, -1, NULL);
    }
    /*
       When there's only one edge set, indicate that on the menu
       with a single menu item naming the edge set.  Let it behave
       like the other menu items, too, turning on undirected
       edges.  Selecting an edge set is required.
    */

    for (k=0; k<nd; k++) { 
      e = (datad *) g_slist_nth_data (gg->d, k);
      if (e->edge.n > 0) {
	gchar *lbl, *path, *name;
        if (resolveEdgePoints(e, d) != NULL) {
	  if (ne == 1) {
	    lbl = g_strdup_printf ("Attach edge set (%s)", e->name);
	    path = "/menubar/Edges";
	    name = g_strdup ("edges");
	  } else {
	    lbl = datasetName (e, gg);
	    path = "/menubar/Edges/Edgesets";
	    name = g_strdup_printf ("edgeset_%p", e);
	  }

  	  if (ne == 1  || !display->e) {
	    action = gtk_action_new(name, lbl, tooltip, NULL);
          } else {
	    action = GTK_ACTION(gtk_radio_action_new(name, lbl, tooltip, 
	      NULL, GPOINTER_TO_INT(e)));
  	    gtk_radio_action_set_group(GTK_RADIO_ACTION(action), group);
	    group = gtk_radio_action_get_group(GTK_RADIO_ACTION(action));
	    if (e == display->e)
	      gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), true);
          }

	  g_signal_connect(G_OBJECT(action), "activate", 
	    G_CALLBACK(edgeset_add_cb), e);
	  gtk_action_group_add_action(actions, action);
	  g_object_unref(action);
	  gtk_ui_manager_add_ui(display->menu_manager, display->edge_merge, 
  	    path, name, name, GTK_UI_MANAGER_MENUITEM, true);
	  g_object_set_data(G_OBJECT(action), "display", display);
          g_free (lbl);
	  g_free (name);
        }
      }
    }
  }
}

