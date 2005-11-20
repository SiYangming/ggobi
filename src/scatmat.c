/* scatmat.c */
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

#ifdef USE_STRINGS_H
#include <strings.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>
#include "vars.h"
#include "externs.h"

#include "scatmatClass.h"
#include "scatterplotClass.h"

#define WIDTH 200
#define HEIGHT 200
#define MAXNVARS 4   /* only used to set up the initial matrix */

/*
static void
scatmat_rows_print (displayd *display) {
  GList *l;
  g_printerr ("rows: ");
  for (l=display->scatmat_rows; l != NULL; l=l->next)
    g_printerr ("%d ", GPOINTER_TO_INT (l->data));
  g_printerr ("\n");
}
static void
scatmat_cols_print (displayd *display) {
  GList *l;
  g_printerr ("cols: ");
  for (l=display->scatmat_cols; l != NULL; l=l->next)
    g_printerr ("%d ", GPOINTER_TO_INT (l->data));
  g_printerr ("\n");
}
*/

static const gchar *scatmat_ui =
"<ui>"
"	<menubar>"
"		<menu action='Options'>"
"			<menuitem action='ShowPoints'/>"
"		</menu>"
"	</menubar>"
"</ui>";

#if 0
static void
scatmat_display_menus_make (displayd *display, GtkAccelGroup *accel_group,
  GtkSignalFunc func, GtkWidget *mbar, ggobid *gg)
{
  GtkWidget *options_menu, *submenu, *item;

  /*-- Options menu --*/
  submenu = submenu_make ("_Options", 'O', accel_group);
  /*-- add a tooltip --*/
  gtk_tooltips_set_tip (GTK_TOOLTIPS (gg->tips), submenu,
    "Options menu for this display", NULL);

  options_menu = gtk_menu_new ();

  item = CreateMenuCheck (options_menu, "Show points",
    func, GINT_TO_POINTER (DOPT_POINTS), on, gg);
  g_object_set_data(G_OBJECT (item), "display", (gpointer) display);

/*  -- once the scatterplot is working, consider this --
  item = CreateMenuCheck (options_menu, "Show edges (undirected)",
    func, GINT_TO_POINTER (DOPT_EDGES_U), off, gg);
  g_object_set_data(G_OBJECT (item), "display", (gpointer) display);
  item = CreateMenuCheck (options_menu, "Show 'arrowheads'",
    func, GINT_TO_POINTER (DOPT_EDGES_U), off, gg);
  g_object_set_data(G_OBJECT (item), "display", (gpointer) display);
  item = CreateMenuCheck (options_menu, "Show edges (directed)",
    func, GINT_TO_POINTER (DOPT_EDGES_D), off, gg);
  g_object_set_data(G_OBJECT (item), "display", (gpointer) display);
*/

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (submenu), options_menu);
  submenu_append (submenu, mbar);
  gtk_widget_show (submenu);
}
#endif

#if 0
displayd *
scatmat_new (gboolean missing_p, gint numRows, gint *rows,
	     gint numCols, gint *cols, datad *d, ggobid *gg) 
{
  GtkWidget *vbox, *frame;
  GtkWidget *mbar, *w;
  GtkItemFactory *factory;
  gint i, j, ctr;
  gint width, height;
  gint scr_width, scr_height;
  gint scatmat_nrows, scatmat_ncols;
  splotd *sp;
  displayd *display;
  windowDisplayd *wdpy;
  GtkAccelGroup *scatmat_accel_group;

  display = g_object_new(GGOBI_TYPE_SCATMAT_DISPLAY, NULL);
  display_set_values (display, d, gg);
  wdpy = GGOBI_WINDOW_DISPLAY(display);

  /* If the caller didn't specify the rows and columns, 
     use the default which is the number of variables
     in the dataset or the maximum number of columns
     within a scatterplot matrix.
     ! Need to check rows and cols are allocated. !
   */
  if (numRows == 0 || numCols == 0) {
    scatmat_nrows = scatmat_ncols = MIN (d->ncols, sessionOptions->info->numScatMatrixVars);
    if(scatmat_nrows < 0) {
	scatmat_nrows = scatmat_ncols = d->ncols;
    }
    for (j=0; j<scatmat_nrows; j++)
      rows[j] = cols[j] = j;
  } else { 
    scatmat_nrows = numRows;
    scatmat_ncols = numCols;
  }

  display->p1d_orientation = HORIZONTAL;

  scatmat_cpanel_init (&display->cpanel, gg);

  display_window_init (GGOBI_WINDOW_DISPLAY(display), 5, gg);

/*
 * Add the main menu bar
*/
  vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 1);
  gtk_container_add (GTK_CONTAINER (wdpy->window), vbox);

  scatmat_accel_group = gtk_accel_group_new ();
  factory = get_main_menu (menu_items,
    sizeof (menu_items) / sizeof (menu_items[0]),
    scatmat_accel_group, wdpy->window, &mbar,
    (gpointer) display);

  /*-- add a tooltip to the file menu --*/
  w = gtk_item_factory_get_widget (factory, "<main>/File");
  gtk_tooltips_set_tip (GTK_TOOLTIPS (gg->tips),
    gtk_menu_get_attach_widget (GTK_MENU(w)),
    "File menu for this display", NULL);

  /*
   * After creating the menubar, and populating the file menu,
   * add the Options and Link menus another way
  */
  scatmat_display_menus_make (display, scatmat_accel_group,
                              G_CALLBACK(display_options_cb), mbar, gg);
  gtk_box_pack_start (GTK_BOX (vbox), mbar, false, true, 0);

/*
 * splots in a table 
*/
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 3);
  gtk_box_pack_start (GTK_BOX (vbox), frame, true, true, 1);

  gtk_widget_show (frame);

  /*
   * make the matrix take up no more than some fraction
   * the screen by default, and make the plots square.
  */
  scr_width = gdk_screen_width () / 2;
  scr_height = gdk_screen_height () / 2;
  width = (WIDTH * scatmat_ncols > scr_width) ?
    (scr_width / scatmat_ncols) : WIDTH;
  height = (HEIGHT * scatmat_nrows > scr_height) ?
    (scr_height / scatmat_nrows) : HEIGHT;
  width = height = MIN (width, height);
  /* */

  display->table = gtk_table_new (scatmat_ncols, scatmat_nrows, false);
  gtk_container_add (GTK_CONTAINER (frame), display->table);
  display->splots = NULL;
  ctr = 0;
  for (i=0; i<scatmat_ncols; i++) {
    for (j=0; j<scatmat_nrows; j++, ctr++) {
/* Can we use SCATTER_SPLOT or do we need SCATMAT_SPLOT. */
      sp = g_object_new(GGOBI_TYPE_SCATMAT_SPLOT, NULL);
      splot_init(sp, display, width, height, gg);

      sp->xyvars.x = rows[i]; 
      sp->xyvars.y = cols[j]; 
      sp->p1dvar = (rows[i] == cols[j]) ? cols[j] : -1;

      display->splots = g_list_append (display->splots, (gpointer) sp);

      gtk_table_attach (GTK_TABLE (display->table), sp->da, i, i+1, j, j+1,
			(GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND), 
			(GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND),
			1, 1);
      gtk_widget_show (sp->da);
    }
  }

  display->scatmat_cols = NULL;
  for (j=0; j<scatmat_ncols; j++)
    display->scatmat_cols = g_list_append (display->scatmat_cols,
                                           GINT_TO_POINTER (cols[j]));
  display->scatmat_rows = NULL;
  for (i=0; i<scatmat_nrows; i++)
    display->scatmat_rows = g_list_append (display->scatmat_rows,
                                           GINT_TO_POINTER (cols[i]));

  gtk_widget_show (display->table);

  /*-- position the display toward the lower left of the main window --*/
  display_set_position (wdpy, gg);
  gtk_widget_show_all (wdpy->window);

  return display;
}

#else

displayd *
scatmat_new (displayd *display,
	       gboolean missing_p, gint numRows, gint *rows,
	       gint numCols, gint *cols, datad *d, ggobid *gg) 
{
  GtkWidget *vbox, *frame;
  gint i, j, ctr;
  gint width, height;
  gint scr_width, scr_height;
  gint scatmat_nrows, scatmat_ncols;
  splotd *sp;
  windowDisplayd *wdpy = NULL;
  
  if(!display)
     display = g_object_new(GGOBI_TYPE_SCATMAT_DISPLAY, NULL);

  display_set_values (display, d, gg);
  if(GGOBI_IS_WINDOW_DISPLAY(display))
    wdpy = GGOBI_WINDOW_DISPLAY(display);

  /* If the caller didn't specify the rows and columns, 
     use the default which is the number of variables
     in the dataset or the maximum number of columns
     within a scatterplot matrix.
     ! Need to check rows and cols are allocated. !
   */

  if (numRows == 0 || numCols == 0) {

    scatmat_nrows = scatmat_ncols = MIN (d->ncols, sessionOptions->info->numScatMatrixVars);
    if(scatmat_nrows < 0) {
	scatmat_nrows = scatmat_ncols = d->ncols;
    }

    /* Initialize display with the plotted variables in the current
       display, if appropriate */
    if (gg->current_display != NULL && gg->current_display != display && 
        gg->current_display->d == d && 
        GGOBI_IS_EXTENDED_DISPLAY(gg->current_display))
    {
      gint k, nplotted_vars;
      gint *plotted_vars = (gint *) g_malloc(d->ncols * sizeof(gint));
      displayd *dsp = gg->current_display;

      nplotted_vars = GGOBI_EXTENDED_DISPLAY_GET_CLASS(dsp)->plotted_vars_get(dsp, plotted_vars, d, gg);

      scatmat_ncols = scatmat_nrows = MAX (scatmat_nrows, nplotted_vars);
      for (j=0; j<nplotted_vars; j++)
        rows[j] = cols[j] = plotted_vars[j];
      j = nplotted_vars;
      for (k=0; k<d->ncols; k++) {
        if (!in_vector(k, plotted_vars, nplotted_vars)) {
          rows[j] = cols[j] = k;
          j++;
          if (j == scatmat_ncols)
            break;
        }
      }
      g_free (plotted_vars);

    } else {
      
      for (j=0; j<scatmat_nrows; j++)
        rows[j] = cols[j] = j;
    }

  } else { 
    scatmat_nrows = numRows;
    scatmat_ncols = numCols;
  }

  display->p1d_orientation = HORIZONTAL;

  scatmat_cpanel_init (&display->cpanel, gg);

  /*
   * make the matrix take up no more than some fraction
   * the screen by default, and make the plots square.
  */
  scr_width = gdk_screen_width () / 2;
  scr_height = gdk_screen_height () / 2;
  width = (WIDTH * scatmat_ncols > scr_width) ?
    (scr_width / scatmat_ncols) : WIDTH;
  height = (HEIGHT * scatmat_nrows > scr_height) ?
    (scr_height / scatmat_nrows) : HEIGHT;
  width = height = MIN (width, height);
  /* */
  
  if(wdpy && wdpy->useWindow)
    display_window_init (GGOBI_WINDOW_DISPLAY(display), 
  		width*scatmat_ncols, height*scatmat_nrows, 5, gg);

/*
 * Add the main menu bar
*/
  vbox = gtk_vbox_new (FALSE, 1);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 1);
  if(wdpy && wdpy->useWindow) {
    gtk_container_add (GTK_CONTAINER (wdpy->window), vbox);

  display->menu_manager = display_menu_manager_create(display);
  display->menubar = create_menu_bar(display->menu_manager, scatmat_ui, wdpy->window);
/*
  scatmat_accel_group = gtk_accel_group_new ();
  factory = get_main_menu (menu_items,
    sizeof (menu_items) / sizeof (menu_items[0]),
    scatmat_accel_group, wdpy->window, &display->menubar,
    (gpointer) display);
*/
  /*-- add a tooltip to the file menu --*/
/*  w = gtk_item_factory_get_widget (factory, "<main>/File");
  gtk_tooltips_set_tip (GTK_TOOLTIPS (gg->tips),
    gtk_menu_get_attach_widget (GTK_MENU(w)),
    "File menu for this display", NULL);
*/
  /*
   * After creating the menubar, and populating the file menu,
   * add the Options and Link menus another way
  */
/*   scatmat_display_menus_make (display, scatmat_accel_group,
       G_CALLBACK(display_options_cb), display->menubar, gg);*/
   gtk_box_pack_start (GTK_BOX (vbox), display->menubar, false, true, 0);
  }
/*
 * splots in a table 
*/
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 3);
  gtk_box_pack_start (GTK_BOX (vbox), frame, true, true, 1);

  gtk_widget_show (frame);

  display->table = gtk_table_new (scatmat_ncols, scatmat_nrows, false);
  gtk_container_add (GTK_CONTAINER (frame), display->table);
  display->splots = NULL;
  ctr = 0;
  for (i=0; i<scatmat_ncols; i++) {
    for (j=0; j<scatmat_nrows; j++, ctr++) {
/* Can we use SCATTER_SPLOT or do we need SCATMAT_SPLOT. */
      sp = g_object_new(GGOBI_TYPE_SCATMAT_SPLOT, NULL);
      splot_init(sp, display, gg);

      sp->xyvars.x = rows[i]; 
      sp->xyvars.y = cols[j]; 
      sp->p1dvar = (rows[i] == cols[j]) ? cols[j] : -1;

      display->splots = g_list_append (display->splots, (gpointer) sp);

      gtk_table_attach (GTK_TABLE (display->table), sp->da, i, i+1, j, j+1,
			(GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND), 
			(GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND),
			1, 1);
      gtk_widget_show (sp->da);
    }
  }

  display->scatmat_cols = NULL;
  for (j=0; j<scatmat_ncols; j++)
    display->scatmat_cols = g_list_append (display->scatmat_cols,
                                           GINT_TO_POINTER (cols[j]));
  display->scatmat_rows = NULL;
  for (i=0; i<scatmat_nrows; i++)
    display->scatmat_rows = g_list_append (display->scatmat_rows,
                                           GINT_TO_POINTER (cols[i]));

  gtk_widget_show (display->table);

  /*-- position the display toward the lower left of the main window --*/
  if(wdpy && wdpy->useWindow) {
    display_set_position (wdpy, gg);
    gtk_widget_show_all (wdpy->window);
  } else {
    gtk_container_add(GTK_CONTAINER(display), vbox);
  }


  return display;
}

#endif


/*
 * check symmetry assumption -- the plot could have been rendered
 * asymmetric by actions in the API.
*/
static gboolean
scatmat_symmetric (displayd *display) 
{
  GList *lcols, *lrows;
  gint scatmat_nvars, j;
  gboolean symmetric = true;

  if ((scatmat_nvars = g_list_length (display->scatmat_rows)) !=
      g_list_length (display->scatmat_cols))
  {
    symmetric = false;
  } else {
    lrows = display->scatmat_rows;
    lcols = display->scatmat_cols;
    for (j=0; j<scatmat_nvars; j++) {
      if (GPOINTER_TO_INT (lcols->data) != GPOINTER_TO_INT (lrows->data)) {
        symmetric = false;
        break;
      }
      lcols = lcols->next;
      lrows = lrows->next;
    }
  }
  return symmetric;
}

/*
 * Assuming symmetry, find out whether a variable is selected
*/
static gboolean
scatmat_var_selected (gint jvar, displayd *display)
{
  gboolean selected = false;
  gint j;

  GList *l = display->scatmat_cols;
  while (l) {
    j = GPOINTER_TO_INT (l->data);
    if (j == jvar) {
      selected = true;
      break;
    }
    l = l->next;
  }
  return selected;
}

static splotd *
scatmat_add_plot (gint xvar, gint yvar, gint col, gint row,
		  displayd *display, ggobid *gg)
{
  splotd *sp_new;

  sp_new = g_object_new(GGOBI_TYPE_SCATMAT_SPLOT, NULL);
  splot_init(sp_new, display, gg);

  sp_new->xyvars.x = xvar;
  sp_new->xyvars.y = yvar;
  sp_new->p1dvar = (sp_new->xyvars.x == sp_new->xyvars.y) ? xvar : -1;

  gtk_table_attach (GTK_TABLE (display->table),
		    sp_new->da, col, col+1, row, row+1,
		    (GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND), 
		    (GtkAttachOptions) (GTK_SHRINK|GTK_FILL|GTK_EXPAND),
		    1, 1);
  gtk_widget_show (sp_new->da);

  /* We don't care where, I think */
  display->splots = g_list_append (display->splots, (gpointer) sp_new);

  return sp_new;
}

gboolean
scatmat_varsel_simple (cpaneld *cpanel, splotd *sp, gint jvar,
  gint *jvar_prev, ggobid *gg)
{
  gboolean redraw = true;
  gboolean Delete = false;
  gint k;
  GList *l;
  splotd *s, *sp_new;
  GtkWidget *da;
  GtkTableChild *child;
  displayd *display = gg->current_display;

  /* the number of variables in the current layout -- assuming symmetry */
  gint scatmat_nvars = g_list_length (display->scatmat_cols);

  /* Check the assumption of layout symmetry */
  if (!scatmat_symmetric) {
	  return  (false);
  }

  //splot_get_dimensions (gg->current_splot, &width, &height);

/*
 * If jvar is selected, delete a row and a column.  Delete the
 * variable that's selected; we have no interest in the current splot.
*/

  /*-- VAR_DELETE --*/
  if (cpanel->scatmat_selection_mode == VAR_DELETE) {
    if (scatmat_var_selected (jvar, display)) {
      /* if jvar is one of the plotted variables, its row and column */
      gint jvar_rc;
      jvar_rc = g_list_index (display->scatmat_cols, GINT_TO_POINTER (jvar));
#if 0
      ratio = (gfloat) scatmat_nvars / (gfloat) (scatmat_nvars-1);
      width = (gint) (ratio * (gfloat) width);
#endif
      l = (GTK_TABLE (display->table))->children;
      while (l) {
        Delete = false;
        child = (GtkTableChild *) l->data;
        l = l->next;
        da = child->widget;

        if (child->left_attach == jvar_rc)
          Delete = true;
        else if (child->left_attach > jvar_rc) {
          child->left_attach--;
          child->right_attach--;
        }
        if (child->top_attach == jvar_rc) {
          Delete = true;
        } else if (child->top_attach > jvar_rc) {
          child->top_attach--;
          child->bottom_attach--;
        }

        if (Delete) {

          s = (splotd *) g_object_get_data(G_OBJECT (da), "splotd");
          display->splots = g_list_remove (display->splots, (gpointer) s);
          /*
           * add a reference to da here, because it's going to be
           * destroyed in splot_free, and we don't want it destroyed
           * as a result of gtk_container_remove.
          */
          gtk_widget_ref (da);
          gtk_container_remove (GTK_CONTAINER (display->table), da);

          if (s == gg->current_splot)
            sp_event_handlers_toggle (s, off, display->cpanel.pmode, display->cpanel.imode);
          splot_free (s, display, gg);
        }
      }

      /*
       * Delete the list elements for the row&column that are being deleted
      */
      display->scatmat_cols = g_list_remove_nth (display->scatmat_cols,
                                                 jvar_rc);
      display->scatmat_rows = g_list_remove_nth (display->scatmat_rows,
                                                 jvar_rc);

      gtk_table_resize (GTK_TABLE (display->table),
                        g_list_length (display->scatmat_rows),
                        g_list_length (display->scatmat_cols));

      /*-- when finished, adjust the sizes of the remaining plots --*/
	  #if 0
      l = (GTK_TABLE (display->table))->children;
      while (l) {
        child = (GtkTableChild *) l->data;
        l = l->next;
        da = child->widget;
        gtk_widget_set_usize (da, -1, -1);
        gtk_widget_set_usize (da, width, height);
      }
	  #endif
      /*
       * I'm not sure this is necessary -- am I checking whether the
       * gg.current_splot was deleted?
      */
      gg->current_splot = (splotd *) g_list_nth_data (display->splots, 0);
      display->current_splot = gg->current_splot;

      redraw = false;  /*-- individual plots don't need a redraw --*/
    }
  }

/*
 * Otherwise, replace, insert or append a row <and> a column --
 * depending on the value of scatmat_selection_mode.
*/
  /*-- VAR_REPLACE or VAR_INSERT or VAR_APPEND  --*/
  else if (!scatmat_var_selected (jvar, display)) {

    /* the row and column of gg.current_splot */
    gint sprow = 1, spcol = -1;

    /*
     * Find the row and column index of the currently selected plot
    */
    for (l=(GTK_TABLE (display->table))->children; l; l=l->next) {
      child = (GtkTableChild *) l->data;
      da = child->widget;
      s = (splotd *) g_object_get_data(G_OBJECT (da), "splotd");
      if (s == gg->current_splot) {
        sprow = child->top_attach;
        spcol = child->left_attach;
      }
    }

    if (sprow != spcol) {
/*XXX */
	gg_write_to_statusbar("Please select one of the plots on the diagonal", gg);
/*      g_printerr ("Please select one of the plots on the diagonal\n"); */
	return false;
    }

    if (cpanel->scatmat_selection_mode == VAR_REPLACE) {

      redraw = true;
      l = (GTK_TABLE (display->table))->children;
      while (l) {
        child = (GtkTableChild *) l->data;
        l = l->next;
        da = child->widget;
        s = (splotd *) g_object_get_data(G_OBJECT (da), "splotd");

        if (child->left_attach == spcol) {
          *jvar_prev = s->xyvars.x;
          s->xyvars.x = jvar;
          s->p1dvar = (s->xyvars.x == s->xyvars.y) ? jvar : -1;
        }

        if (child->top_attach == sprow) {
          *jvar_prev = s->xyvars.y;
          s->xyvars.y = jvar;
          s->p1dvar = (s->xyvars.x == s->xyvars.y) ? jvar : -1;
        }
      }

      display->scatmat_cols = g_list_replace_nth (display->scatmat_cols,
        GINT_TO_POINTER (jvar), spcol);
        /*scatmat_cols_print (display);*/

      display->scatmat_rows = g_list_replace_nth (display->scatmat_rows,
        GINT_TO_POINTER (jvar), sprow);
        /*scatmat_rows_print (display);*/

    } else {  /* VAR_INSERT or VAR_APPEND */
      gint newvar;
      gint row = -1, col = -1;

      /*-- prepare to reset the current plot --*/
      sp_event_handlers_toggle (sp, off, display->cpanel.pmode, display->cpanel.imode);

      /*
       * First adjust the table, inserting or appending a row
       * and resetting the attachment values.
      */

      /*
       * Prepare to decrease the size of each plot: symmetry says
       * that some of these variables are redundant.
      */
      col = (cpanel->scatmat_selection_mode == VAR_INSERT) ? spcol :
                                                             scatmat_nvars;
      row = (cpanel->scatmat_selection_mode == VAR_INSERT) ? sprow :
                                                             scatmat_nvars;
#if 0
      ratio = (gfloat) scatmat_nvars / (gfloat) (scatmat_nvars+1);
      width = (gint) (ratio * (gfloat) width);
      height = (gint) (ratio * (gfloat) height);
#endif
      /*
       * Fix up the attachments of the rows below and columns to the right
       * of the inserted/appended row/column.
      */
      for (l=(GTK_TABLE (display->table))->children; l; l=l->next) {
        child = (GtkTableChild *) l->data;
        da = child->widget;
        //gtk_widget_set_usize (da, -1, -1);
        //gtk_widget_set_usize (da, width, height);

        if (child->left_attach >= col) {
          child->left_attach++;
          child->right_attach++;
        } 
        if (child->top_attach >= row) {
          child->top_attach++;
          child->bottom_attach++;
        }
      }

      /*
       * Now create the new plots and fill in the new row/column.
       * Work out the correct p1dvar/xyvars values for each new plot.
      */

      scatmat_nvars++;
      display->scatmat_cols = g_list_insert (display->scatmat_cols,
                                             GINT_TO_POINTER (jvar),
                                             col);
      display->scatmat_rows = g_list_insert (display->scatmat_rows,
                                             GINT_TO_POINTER (jvar),
                                             row);

      /*-- note the strong assumption of symmetry here --*/
      for (k=0; k<scatmat_nvars; k++) {
        /* which variable is plotting in the k'th intersecting row/column? */
        newvar = GPOINTER_TO_INT (g_list_nth_data (display->scatmat_rows, k));

        sp_new = scatmat_add_plot (jvar, newvar, col, k, display, gg);

        if (k != row) {  /*-- except at the intersection, do it twice --*/
          sp_new = scatmat_add_plot (newvar, jvar, k, row, display, gg);
        }
      }

      gtk_table_resize (GTK_TABLE (gg->current_display->table),
                        scatmat_nvars, scatmat_nvars);

      gg->current_splot = sp->displayptr->current_splot = sp;
      sp_event_handlers_toggle (sp_new, on, gg->current_display->cpanel.pmode, gg->current_display->cpanel.imode);
      redraw = true;
    }
  }

  return redraw;
}
#if 0
/**
  This creates the scatter matrix window contents using static
  variables within this file.

  The corresponding code in scatmat_new should be deprecated and
  that routine should be made to call this one.
  -- I think this has already been done - mfl
 */
gint *
createScatmatWindow(gint nrows, gint ncols, displayd *display, ggobid *gg, gboolean useWindow)
{
  GtkWidget *vbox, *frame;
  GtkWidget *mbar;
  gint width, height;
  gint scr_width, scr_height;
  gint scatmat_nrows=1, scatmat_ncols=1; /* Not used really! */
  GtkAccelGroup *scatmat_accel_group;
  gint *dims = NULL;


  display->p1d_orientation = HORIZONTAL;

  scatmat_cpanel_init (&display->cpanel, gg);

  if(useWindow) {
    display_window_init (GGOBI_WINDOW_DISPLAY(display), 5, gg);


/*
 * Add the main menu bar
*/
    vbox = GTK_WIDGET(display); 
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 1);
    gtk_container_add (GTK_CONTAINER (GGOBI_WINDOW_DISPLAY(display)->window), vbox);

  scatmat_accel_group = gtk_accel_group_new ();
  get_main_menu (menu_items, sizeof (menu_items) / sizeof (menu_items[0]),
                 scatmat_accel_group, GGOBI_WINDOW_DISPLAY(display)->window, &mbar,
                 (gpointer) display);
  /*
   * After creating the menubar, and populating the file menu,
   * add the Options and Link menus another way
  */
  scatmat_display_menus_make (display, scatmat_accel_group,
                               G_CALLBACK(display_options_cb), mbar, gg);
  gtk_box_pack_start (GTK_BOX (vbox), mbar, false, true, 0);

/*
 * splots in a table 
*/
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 3);
  gtk_box_pack_start (GTK_BOX (vbox), frame, true, true, 1);

  gtk_widget_show (frame);
  }
  /*
   * make the matrix take up no more than some fraction
   * the screen by default, and make the plots square.
  */
  scr_width = gdk_screen_width () / 2;
  scr_height = gdk_screen_height () / 2;
  width = (WIDTH * scatmat_ncols > scr_width) ?
    (scr_width / scatmat_ncols) : WIDTH;
  height = (HEIGHT * scatmat_nrows > scr_height) ?
    (scr_height / scatmat_nrows) : HEIGHT;
  width = height = MIN (width, height);
  /* */

  display->table = gtk_table_new (nrows, ncols, false);
  if(useWindow)
    gtk_container_add (GTK_CONTAINER (frame), display->table);


  dims = (gint *) g_malloc(sizeof(gint) * 2);
  dims[0] = width;
  dims[1] = height;

  return(dims);
}
#endif

#undef WIDTH
#undef HEIGHT
#undef MAXNVARS