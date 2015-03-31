/*-- color_ui.c --*/
/*
 * ggobi
 * Copyright (C) AT&T, Duncan Temple Lang, Dianne Cook 1999-2005
 *
 * ggobi is free software; you may use, redistribute, and/or modify it
 * under the terms of the Eclipse Public License, which is distributed
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

#define PSIZE 20

static gint open_colorsel_dialog (GtkWidget * w, ggobid * gg);
static void redraw_fg (GtkWidget * w, gint k, ggobid * gg);

/*------------------------------------------------------------------------*/
/*                    symbol display routines                             */
/*------------------------------------------------------------------------*/

static void
choose_glyph_cb (GtkWidget * w, GdkEventButton * event, ggobid * gg)
{
/*-- Reset glyph_id to the nearest glyph.  --*/
  glyphd g;
  gint i, dsq, nearest_dsq, type, size, rval = false;
  icoords pos, ev;
  splotd *sp = gg->current_splot;
  displayd *display = (displayd *) sp->displayptr;
  GGobiData *d = display->d;
  gint spacing = gg->color_ui.spacing;
  gint margin = gg->color_ui.margin;

  vectorg_copy (&d->glyph, &d->glyph_prev);  /*-- from, to --*/

  ev.x = (gint) event->x;
  ev.y = (gint) event->y;

  pos.y = margin + 3 / 2;
  pos.x = spacing / 2;
  g.type = DOT_GLYPH;
  g.size = 1;
  nearest_dsq = dsq = sqdist (pos.x, pos.y, ev.x, ev.y);
  type = g.type;
  size = g.size;

  pos.y = 0;
  for (i = 0; i < NGLYPHSIZES; i++) {
    g.size = i;
    pos.y += (margin + ((i == 0) ? (3 * g.size) / 2 : 3 * g.size));
    pos.x = spacing + spacing / 2;

    g.type = PLUS;
    if ((dsq = sqdist (pos.x, pos.y, ev.x, ev.y)) < nearest_dsq) {
      nearest_dsq = dsq;
      type = g.type;
      size = g.size;
    }

    pos.x += spacing;
    g.type = X;
    if ((dsq = sqdist (pos.x, pos.y, ev.x, ev.y)) < nearest_dsq) {
      nearest_dsq = dsq;
      type = g.type;
      size = g.size;
    }

    pos.x += spacing;
    g.type = OC;
    if ((dsq = sqdist (pos.x, pos.y, ev.x, ev.y)) < nearest_dsq) {
      nearest_dsq = dsq;
      type = g.type;
      size = g.size;
    }

    pos.x += spacing;
    g.type = OR;
    if ((dsq = sqdist (pos.x, pos.y, ev.x, ev.y)) < nearest_dsq) {
      nearest_dsq = dsq;
      type = g.type;
      size = g.size;
    }

    pos.x += spacing;
    g.type = FC;
    if ((dsq = sqdist (pos.x, pos.y, ev.x, ev.y)) < nearest_dsq) {
      nearest_dsq = dsq;
      type = g.type;
      size = g.size;
    }

    pos.x += spacing;
    g.type = FR;
    if ((dsq = sqdist (pos.x, pos.y, ev.x, ev.y)) < nearest_dsq) {
      nearest_dsq = dsq;
      type = g.type;
      size = g.size;
    }
  }

  gg->glyph_id.type = type;
  gg->glyph_id.size = size;
  redraw_widget (gg->color_ui.symbol_display);
  redraw_widget (gg->color_ui.line_display);
}

static void
find_symbol_selection_circle_pos (icoords * pos, ggobid * gg)
{
  gint i;
  glyphd g;
  gint spacing = gg->color_ui.spacing;
  gint margin = gg->color_ui.margin;

  if (gg->glyph_id.type == DOT_GLYPH) {
    pos->y = margin + 3 / 2;
    pos->x = spacing / 2;

  }
  else {

    pos->y = 0;
    for (i = 0; i < NGLYPHSIZES; i++) {
      g.size = i;
      pos->y += (margin + ((i == 0) ? (3 * g.size) / 2 : 3 * g.size));
      pos->x = spacing + spacing / 2;

      if (gg->glyph_id.type == PLUS && gg->glyph_id.size == g.size)
        break;

      pos->x += spacing;
      if (gg->glyph_id.type == X && gg->glyph_id.size == g.size)
        break;

      pos->x += spacing;
      if (gg->glyph_id.type == OC && gg->glyph_id.size == g.size)
        break;

      pos->x += spacing;
      if (gg->glyph_id.type == OR && gg->glyph_id.size == g.size)
        break;

      pos->x += spacing;
      if (gg->glyph_id.type == FC && gg->glyph_id.size == g.size)
        break;

      pos->x += spacing;
      if (gg->glyph_id.type == FR && gg->glyph_id.size == g.size)
        break;
    }
  }
}

static void
redraw_symbol_display (GtkWidget * w, cairo_t *cr, ggobid * gg)
{
  gint i;
  glyphd g;
  icoords pos;
  gint margin, spacing;
  colorschemed *scheme = gg->activeColorScheme;

  gg->color_ui.spacing = w->allocation.width / NGLYPHTYPES;

  margin = gg->color_ui.margin;
  spacing = gg->color_ui.spacing;

  cairo_set_source(cr, scheme->rgb_bg);
  cairo_paint(cr);
  cairo_set_source (cr, scheme->rgb[gg->color_id]);

  /*
   * The factor of three is dictated by the sizing of circles
   *  ... this should no longer be true; it should be 2*width + 1
   */
  pos.y = margin + 3 / 2;
  pos.x = spacing / 2;
  cairo_rectangle (cr, pos.x, pos.y, 1 , 1);
  cairo_fill(cr);
  
  pos.y = 0;
  for (i = 0; i < NGLYPHSIZES; i++) {
    g.size = i;
    pos.y += (margin + ((i == 0) ? (3 * g.size) / 2 : 3 * g.size));
    pos.x = spacing + spacing / 2;

    g.type = PLUS;
    draw_glyph (cr, &g, &pos, 0, gg);

    pos.x += spacing;
    g.type = X;
    draw_glyph (cr, &g, &pos, 0, gg);

    pos.x += spacing;
    g.type = OC;
    draw_glyph (cr, &g, &pos, 0, gg);

    pos.x += spacing;
    g.type = OR;
    draw_glyph (cr, &g, &pos, 0, gg);

    pos.x += spacing;
    g.type = FC;
    draw_glyph (cr, &g, &pos, 0, gg);

    pos.x += spacing;
    g.type = FR;
    draw_glyph (cr, &g, &pos, 0, gg);
  }

  if (!gg->mono_p) {
    icoords p;
    /*-- 2*(NGLYPHSIZES+1) is the size of the largest glyph; draw_glyph() --*/
    gint radius = (3 * NGLYPHSIZES) / 2 + gg->color_ui.margin / 2;
    find_symbol_selection_circle_pos (&p, gg);

    cairo_set_source (cr, scheme->rgb_accent);
    cairo_set_line_width(cr, 2);
    cairo_arc(cr, p.x, p.y, radius, 0, 2*M_PI);
    cairo_set_line_width(cr, 0);
  }

}

static gint
symbol_display_draw_cb (GtkWidget * w, cairo_t * cr, ggobid * gg)
{
  redraw_symbol_display (w, cr, gg);
  return FALSE;
}

/*------------------------------------------------------------------------*/
/*                      line display routines                             */
/*------------------------------------------------------------------------*/

/*-- use the current glyph type and size to determine the line type --*/
/*
 * point, +, x -> lightest dashed line
 * open circle and rectangle -> middle dashed line
 * filled circle and rectangle -> solid line
*/
static void
find_line_selection_pos (icoords * pos, ggobid * gg)
{
  gint i;
  glyphd g;
  gint spacing = gg->color_ui.spacing;
  gint margin = gg->color_ui.margin;

  if (gg->glyph_id.type == DOT_GLYPH) {
    pos->x = spacing + spacing / 2;
    pos->y = margin;
  }
  else {
    pos->y = 0;
    for (i = 0; i < NGLYPHSIZES; i++) {
      g.size = i;
      pos->y += (margin + ((i == 0) ? (3 * g.size) / 2 : 3 * g.size));
      pos->x = spacing + spacing / 2;

      if (gg->glyph_id.type == PLUS && gg->glyph_id.size == g.size)
        break;
      if (gg->glyph_id.type == X && gg->glyph_id.size == g.size)
        break;

      pos->x += (2 * spacing);
      if (gg->glyph_id.type == OC && gg->glyph_id.size == g.size)
        break;
      if (gg->glyph_id.type == OR && gg->glyph_id.size == g.size)
        break;

      pos->x += (2 * spacing);
      if (gg->glyph_id.type == FC && gg->glyph_id.size == g.size)
        break;
      if (gg->glyph_id.type == FR && gg->glyph_id.size == g.size)
        break;
    }
  }
}

static void
redraw_line_display (GtkWidget * w, ggobid * gg)
{
  gint i, linewidth;
  icoords pos;
  gint margin, spacing;
  double dash_list[2];
  colorschemed *scheme = gg->activeColorScheme;

  margin = gg->color_ui.margin;
  spacing = gg->color_ui.spacing;

  cairo_t *cr = gdk_cairo_create(w->window);

  cairo_set_source (cr, scheme->rgb_bg);
  cairo_paint(cr);
  cairo_set_source (cr, scheme->rgb[gg->color_id]);

  pos.y = 0;
  cairo_save(cr);
  cairo_set_line_width(cr, linewidth);
  cairo_set_line_cap(cr, CAIRO_CAP_BUTT);
  for (i = 0; i < NGLYPHSIZES; i++) {
    linewidth = (i < 3) ? 0 : (i - 2) * 2;
    pos.y += (margin + ((i == 0) ? (3 * i) / 2 : 3 * i));

    pos.x = spacing;
    dash_list[0] = 4;
    dash_list[1] = 2;
    cairo_set_dash (cr, dash_list, 2, 0);
    cairo_move_to(cr, pos.x, pos.y);
    cairo_line_to(cr, pos.x + spacing, pos.y);
    cairo_stroke(cr);
    
    pos.x += (2 * spacing);
    
    dash_list[0] = 8;
    dash_list[1] = 2;
    cairo_set_dash (cr, dash_list, 2, 0);
    cairo_move_to(cr, pos.x, pos.y);
    cairo_line_to(cr, pos.x + spacing, pos.y);
    cairo_stroke(cr);
    
    pos.x += (2 * spacing);
    cairo_set_dash(cr, NULL, 0, 0);
    cairo_move_to(cr, pos.x, pos.y);
    cairo_line_to(cr, pos.x + spacing, pos.y);
    cairo_stroke(cr);
  }

  cairo_restore(cr);
  
  if (!gg->mono_p) {
    icoords p;
    find_line_selection_pos (&p, gg);

    cairo_set_source (cr, scheme->rgb_accent);
    cairo_rectangle (cr, false,
                     p.x - spacing / 2 - margin / 2,
                     p.y - (NGLYPHSIZES + 1) / 2 - margin / 2,
                     spacing + margin, (NGLYPHSIZES + 1) + margin);
    cairo_stroke(cr);
  }

  cairo_destroy(cr);
}

static gint
line_display_draw_cb (GtkWidget * w, cairo_t * cr, ggobid * gg)
{
  redraw_line_display (w, cr, gg);
  return FALSE;
}


/*------------------------------------------------------------------------*/
/*          foreground, background, accent color widgets                  */
/*------------------------------------------------------------------------*/
static void
set_one_color (GtkWidget * w, GdkEventButton * event, ggobid * gg)
{
  if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)
    open_colorsel_dialog (w, gg);
}
static void
set_color_fg (GtkWidget * w, GdkEventButton * event, ggobid * gg)
{
  gint i;
  gint prev = gg->color_id;
  gint k = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (w), "index"));
  splotd *sp = gg->current_splot;
  displayd *display = (displayd *) sp->displayptr;
  GGobiData *d = display->d;

  g_assert (d->color.nels == d->nrows);

  for (i = 0; i < d->nrows; i++)
    d->color_prev.els[i] = d->color.els[i];
  gg->color_id = k;

  if (event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS) {
    open_colorsel_dialog (w, gg);
  }
  else {
    gint rval = false;
    redraw_widget (gg->color_ui.symbol_display);
    redraw_widget (gg->color_ui.line_display);
  }

  redraw_widget (gg->color_ui.fg_da[prev]);
  redraw_widget (w);
}

static gint
set_color_id (GtkWidget * w, GdkEventButton * event, ggobid * gg)
{
  /*
   * So that the same routines can be used to handle both the foreground
   * and background color swatches, keep track of which drawing area
   * was most recently pressed.
   */
  gg->color_ui.current_da = w;

  if (w == gg->color_ui.bg_da ||
      w == gg->color_ui.accent_da || w == gg->color_ui.hidden_da)
    set_one_color (w, event, gg);
  else
    set_color_fg (w, event, gg);

  splot_redraw (gg->current_splot,
                GGOBI_SPLOT_GET_CLASS (gg->current_splot)->redraw, gg);
                      /*-- redraw brush --*/

  return FALSE;
}

/*-- Redraw one of the foreground color swatches --*/
static void
redraw_fg (GtkWidget * w, cairo_t *cr, gint k, ggobid * gg)
{
  colorschemed *scheme = gg->activeColorScheme;

  cairo_set_source (cr, gg->activeColorScheme->rgb[k]);
  cairo_paint(cr);

  /*
   * Draw a background border around the box containing the selected color
   */
  if (k == gg->color_id) {
    cairo_set_source (cr, scheme->rgb_bg);
    cairo_rectangle (cr, 0, 0,
                     w->allocation.width - 1, w->allocation.height - 1);
    cairo_rectangle (cr, 1, 1,
                     w->allocation.width - 2, w->allocation.height - 2);
    cairo_stroke(cr);
  }
}

static gint
color_draw_fg (GtkWidget * w, cairo_t * cr, ggobid * gg)
{
  gint k = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (w), "index"));

  if (k <= gg->activeColorScheme->n)
    redraw_fg (w, cr, k, gg);

  return FALSE;
}

static void
fill_with_pattern(cairo_t *cr, cairo_pattern_t *pattern) {
  cairo_set_source(cr, pattern);
  cairo_paint(cr);
}

static void
redraw_bg (cairo_t *cr, ggobid * gg)
{
  colorschemed *scheme = gg->activeColorScheme;  
  fill_with_pattern(cr, scheme->rgb_bg);
}

static gint
color_draw_bg (GtkWidget * w, cairo_t * cr, ggobid * gg)
{
  redraw_bg (cr, gg);
  return FALSE;
}

static void
redraw_accent (cairo_t *cr, ggobid * gg)
{
  colorschemed *scheme = gg->activeColorScheme;
  fill_with_pattern(cr, scheme->rgb_accent);
}

static gint
color_draw_accent (GtkWidget * w, cairo_t * cr, ggobid * gg)
{
  redraw_accent (cr, gg);
  return FALSE;
}

static void
redraw_hidden (cairo_t *cr, ggobid * gg)
{
  colorschemed *scheme = gg->activeColorScheme;
  fill_with_pattern(cr, scheme->rgb_hidden);
}

static gint
color_draw_hidden (GtkWidget * w, cairo_t * cr, ggobid * gg)
{
  redraw_hidden (cr, gg);
  return FALSE;
}
static void
reverse_video_cb (GtkWidget * ok_button, ggobid * gg)
{
  gulong pixel;
  gdouble r, g, b;
  gint rval = false;
  colorschemed *scheme = gg->activeColorScheme;
  gboolean writeable = false, best_match = true;

  cairo_pattern_t *accent = scheme->rgb_accent;
  scheme->rgb_accent = scheme->rgb_bg;
  scheme->rgb_bg = accent;

  cairo_pattern_get_rgba(scheme->rgb_hidden, &r, &g, &b, NULL);
  cairo_pattern_destroy(scheme->rgb_hidden);
  scheme->rgb_hidden = cairo_pattern_create_rgb(1-r, 1-g, 1-b);
  
  redraw_widget (gg->color_ui.symbol_display);
  redraw_widget (gg->color_ui.line_display);

  redraw_widget (gg->color_ui.bg_da);
  redraw_widget (gg->color_ui.accent_da);
  redraw_widget (gg->color_ui.hidden_da);

  displays_plot ((splotd *) NULL, FULL, gg);
}

/*------------------------------------------------------------------------*/
/*                    color selection dialog routines                     */
/*------------------------------------------------------------------------*/

void
color_changed_cb (GtkWidget * colorsel, ggobid * gg)
{
  GdkRGBA gdk_color;
  splotd *sp = gg->current_splot;

  colorschemed *scheme = gg->activeColorScheme;

  /* Get current color */
  gtk_color_selection_get_current_rgb (GTK_COLOR_SELECTION (colorsel),
                                       &gdk_color);

  /* Allocate color */
  cairo_pattern_t *pattern = cairo_pattern_create_rgb(gdk_color.red,
                                                      gdk_color.green,
                                                      gdk_color.blue);
  
    if (gg->color_ui.current_da == gg->color_ui.bg_da) {
      scheme->rgb_bg = pattern;
    }
    else if (gg->color_ui.current_da == gg->color_ui.accent_da) {
      scheme->rgb_accent = pattern;
    }
    else if (gg->color_ui.current_da == gg->color_ui.hidden_da) {
      scheme->rgb_hidden = pattern;
    }
    else {
      gg->activeColorScheme->rgb[gg->color_id] = pattern;
    }

    fill_with_pattern (gg->color_ui.current_da->window, pattern);
    
    redraw_widget (gg->color_ui.symbol_display);
    redraw_widget (gg->color_ui.line_display);

    if (sp->da != NULL) {
      gboolean rval = false;
      redraw_widget (sp->da);
    }

    displays_plot ((splotd *) NULL, FULL, gg);
}

static void
dlg_response_cb (GtkWidget * dialog, gint id, ggobid * gg)
{
  gg->color_ui.colorseldlg = NULL;
  gtk_widget_destroy(dialog);
}

static gint
open_colorsel_dialog (GtkWidget * w, ggobid * gg)
{
  gint handled = FALSE;
  GtkWidget *colorsel, *ok_button, *cancel_button, *help_button;
  gint i;
  GtkColorSelectionDialog *colordlg;
  colorschemed *scheme = gg->activeColorScheme;

  /* Check if we've received a button pressed event */

  if (gg->color_ui.colorseldlg == NULL) {
    handled = true;

    /* Create color selection dialog */
    gg->color_ui.colorseldlg =
      gtk_color_selection_dialog_new ("Select color");

    /* Get the ColorSelection widget */
    colordlg = GTK_COLOR_SELECTION_DIALOG (gg->color_ui.colorseldlg);
    colorsel =
      GTK_COLOR_SELECTION_DIALOG (gg->color_ui.colorseldlg)->colorsel;

    /*
     * Connect to the "color_changed" signal, set the client-data
     * to the colorsel widget
     */
    g_signal_connect (G_OBJECT (colorsel), "color_changed",
                      G_CALLBACK (color_changed_cb), gg);

    /*
     * Connect up the buttons
     */
    g_signal_connect(G_OBJECT(gg->color_ui.colorseldlg), "response",
      G_CALLBACK(dlg_response_cb), gg);
    
    /*ok_button = colordlg->ok_button;
    cancel_button = colordlg->cancel_button;
    help_button = colordlg->help_button;
    g_signal_connect (G_OBJECT (ok_button), "clicked",
                      G_CALLBACK (dlg_close_cb), gg);
    g_signal_connect (G_OBJECT (cancel_button), "clicked",
                      G_CALLBACK (dlg_close_cb), gg);
    */
  }
  else {
    colorsel =
      GTK_COLOR_SELECTION_DIALOG (gg->color_ui.colorseldlg)->colorsel;
  }

  if (w == gg->color_ui.bg_da) {
    gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (colorsel),
                                           &scheme->rgb_bg);
  }
  else if (w == gg->color_ui.accent_da) {
    gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (colorsel),
                                           &scheme->rgb_accent);
  }
  else if (w == gg->color_ui.hidden_da) {
    gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (colorsel),
                                           &scheme->rgb_hidden);
  }
  else {
    for (i = 0; i < MAXNCOLORS; i++) {
      if (w == gg->color_ui.fg_da[i]) {
        gtk_color_selection_set_current_color (GTK_COLOR_SELECTION (colorsel),
                                               &gg->activeColorScheme->
                                               rgb[i]);
      }
    }
  }

  /* Show the dialog */
  gtk_widget_show (gg->color_ui.colorseldlg);

  return handled;
}


/*------------------------------------------------------------------------*/
/*                    show/hide/delete the window                         */
/*------------------------------------------------------------------------*/

static void
hide_symbol_window (ggobid * gg)
{

  gtk_widget_hide (gg->color_ui.symbol_window);

  if (gg->color_ui.colorseldlg != NULL &&
      GTK_IS_WIDGET (gg->color_ui.colorseldlg) &&
      GTK_WIDGET_VISIBLE (gg->color_ui.colorseldlg)) {
    gtk_widget_hide (gg->color_ui.colorseldlg);
  }
}

/*-- catch a click on the close button --*/
static void
hide_symbol_window_cb (GtkWidget * w, ggobid * gg)
{
  hide_symbol_window (gg);
}

/*-- catch the delete (close) event from the window manager --*/
static void
delete_symbol_window_cb (GtkWidget * w, GdkEventButton * event, ggobid * gg)
{
  hide_symbol_window (gg);
}

/*------------------------------------------------------------------------*/
/*                   redraw the window                                    */
/*------------------------------------------------------------------------*/

void
symbol_window_redraw (ggobid * gg)
{
  gint k;
  splotd *sp = gg->current_splot;
  gint rval = false;

  if (gg->color_ui.symbol_display) {

    redraw_widget (gg->color_ui.symbol_display);
    redraw_widget (gg->color_ui.line_display);
    redraw_widget (gg->color_ui.bg_da);
    redraw_widget (gg->color_ui.accent_da);

    for (k = 0; k < gg->activeColorScheme->n; k++) {
      gtk_widget_show (gg->color_ui.fg_da[k]);
      redraw_widget (gg->color_ui.fg_da[k]);
    }
    for (k = gg->activeColorScheme->n; k < MAXNCOLORS; k++) {
      gtk_widget_hide (gg->color_ui.fg_da[k]);
    }
  }
}

void
close_symbol_window_cb (GtkWidget * w, GdkEventButton * event, ggobid * gg)
{
  fprintf (stderr, "Closing the color scheme window\n");
  fflush (stderr);
  gtk_widget_destroy (gg->color_ui.symbol_window);
  gg->color_ui.symbol_window = NULL;
}

/*------------------------------------------------------------------------*/
/*                    build the window                                    */
/*------------------------------------------------------------------------*/

void
make_symbol_window (ggobid * gg)
{

  GtkWidget *vbox, *fg_frame, *bg_frame, *accent_frame, *hidden_frame, *btn;
  GtkWidget *fg_table, *bg_table, *accent_table, *hidden_table, *ebox, *hbox;
  gint i, j, k;
  gint width, height;

  /*
   * This seems to handle the case where a the window was
   * closed using the window manager -- even though I'm capturing
   * a delete_event.
   */
  if (!GTK_IS_WIDGET (gg->color_ui.symbol_window))
    gg->color_ui.symbol_window = NULL;

  if (gg->color_ui.symbol_window == NULL) {
    gg->color_ui.symbol_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (gg->color_ui.symbol_window),
                          "Color & Glyph Chooser");

    /*
     * I thought this would be enough to prevent the window from
     * being destroyed, but it doesn't seem to be.
     */
    g_signal_connect (G_OBJECT (gg->color_ui.symbol_window),
                      "delete_event",
                      G_CALLBACK (delete_symbol_window_cb), (gpointer) gg);


    /* Track when the ggobid instance is closed and shut this one down too. */
    g_signal_connect (G_OBJECT (gg->main_window),
                      "delete_event",
                      G_CALLBACK (close_symbol_window_cb), (gpointer) gg);

    vbox = gtk_vbox_new (false, 2);
    gtk_container_add (GTK_CONTAINER (gg->color_ui.symbol_window), vbox);

    /*-- to contain the two display areas --*/
    hbox = gtk_hbox_new (false, 2);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, true, true, 0);

/*
 * display of glyph types and sizes
*/
    gg->color_ui.symbol_display = gtk_drawing_area_new ();
    gtk_widget_set_double_buffered (gg->color_ui.symbol_display, false);

    /*-- after this, margin is only used in determining y position --*/
    /*-- 2*(NGLYPHSIZES+1) is the size of the largest glyph --*/
    width =
      NGLYPHTYPES * 2 * (NGLYPHSIZES + 1) +
      gg->color_ui.margin * (NGLYPHTYPES + 1);

    /*-- initialize the spacing that will be used in drawing --*/
    gg->color_ui.spacing = width / NGLYPHTYPES;

    height = gg->color_ui.margin;
    for (i = 0; i < NGLYPHSIZES; i++)
      height += (gg->color_ui.margin + 2 * (i + 2));
    height += gg->color_ui.margin;

    gtk_widget_set_size_request (GTK_WIDGET (gg->color_ui.symbol_display),
                                 width, height);
    gtk_box_pack_start (GTK_BOX (hbox), gg->color_ui.symbol_display,
                        true, true, 0);

    gtk_widget_set_tooltip_text (gg->color_ui.symbol_display,
                          "Click to select glyph type and size -- which also selects the line type");

    g_signal_connect (G_OBJECT (gg->color_ui.symbol_display),
                      "draw",
                      G_CALLBACK (symbol_display_draw_cb), gg);
    g_signal_connect (G_OBJECT (gg->color_ui.symbol_display),
                      "button_press_event", G_CALLBACK (choose_glyph_cb), gg);

    gtk_widget_set_events (gg->color_ui.symbol_display, GDK_EXPOSURE_MASK
                           | GDK_ENTER_NOTIFY_MASK
                           | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);
/*
 * the display of line types and widths
*/
    gg->color_ui.line_display = gtk_drawing_area_new ();
    gtk_widget_set_double_buffered (gg->color_ui.line_display, false);

    width = NEDGETYPES * gg->color_ui.spacing +   /*-- lines --*/
      (NEDGETYPES + 1) * gg->color_ui.spacing;    /*-- space between --*/

    /*-- use the same height we used for the symbol display --*/
    gtk_widget_set_size_request (GTK_WIDGET (gg->color_ui.line_display),
                                 width, height);
    gtk_box_pack_start (GTK_BOX (hbox), gg->color_ui.line_display,
                        true, true, 0);

    gtk_widget_set_tooltip_text (
                          gg->color_ui.line_display,
                          "Shows the line type corresponding to the current glyph selection");

    g_signal_connect (G_OBJECT (gg->color_ui.line_display),
                      "draw",
                      G_CALLBACK (line_display_draw_cb), gg);
/*
    g_signal_connect (G_OBJECT (gg->color_ui.line_display),
      "button_press_event",
      G_CALLBACK (choose_linetype_cb), gg);
*/

    gtk_widget_set_events (gg->color_ui.line_display, GDK_EXPOSURE_MASK
                           | GDK_ENTER_NOTIFY_MASK
                           | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);
/*-- --*/

    fg_frame = gtk_frame_new ("Foreground colors");
    gtk_box_pack_start (GTK_BOX (vbox), fg_frame, false, false, 0);

    ebox = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (fg_frame), ebox);

    /*-- create MAXNCOLORS drawing areas, showing gg->activeColorScheme->n of them --*/
    fg_table = gtk_table_new (1, MAXNCOLORS, true);
    gtk_container_add (GTK_CONTAINER (ebox), fg_table);

    k = 0;
    for (i = 0, j = 0; i < MAXNCOLORS; i++) {
      gg->color_ui.fg_da[k] = gtk_drawing_area_new ();
      gtk_widget_set_double_buffered (gg->color_ui.fg_da[k], false);

      g_object_set_data (G_OBJECT (gg->color_ui.fg_da[k]),
                         "index", GINT_TO_POINTER (k));
      gtk_widget_set_size_request (GTK_WIDGET (gg->color_ui.fg_da[k]),
                                   PSIZE, PSIZE);

      gtk_widget_set_tooltip_text (gg->color_ui.fg_da[k],
                            "Click to select brushing color, double click to reset");

      gtk_widget_set_events (gg->color_ui.fg_da[k],
                             GDK_EXPOSURE_MASK
                             | GDK_ENTER_NOTIFY_MASK
                             | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);

      g_signal_connect (G_OBJECT (gg->color_ui.fg_da[k]),
                        "button_press_event", G_CALLBACK (set_color_id), gg);
      g_signal_connect (G_OBJECT (gg->color_ui.fg_da[k]),
                        "draw", G_CALLBACK (color_draw_fg), gg);
      gtk_table_attach (GTK_TABLE (fg_table),
                        gg->color_ui.fg_da[k], i, i + 1, j, j + 1,
                        GTK_FILL, GTK_FILL, 10, 10);

      k++;
    }

    /*-- hbox to contain bg, accent and hidden color frames --*/
    hbox = gtk_hbox_new (false, 2);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, true, true, 0);

    /*-- Background color --*/
    bg_frame = gtk_frame_new ("Background color");
    gtk_box_pack_start (GTK_BOX (hbox), bg_frame, true, true, 0);

    ebox = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (bg_frame), ebox);

    bg_table = gtk_table_new (1, 5, true);
    gtk_container_add (GTK_CONTAINER (ebox), bg_table);

    gg->color_ui.bg_da = gtk_drawing_area_new ();
    gtk_widget_set_double_buffered (gg->color_ui.bg_da, false);

    gtk_widget_set_size_request (GTK_WIDGET (gg->color_ui.bg_da),
                                 PSIZE, PSIZE);
    gtk_widget_set_tooltip_text (
                          gg->color_ui.bg_da,
                          "Double click to reset background color (Note: your color selection will have no visible effect unless the 'Value' is >>0; look to the right of the color wheel.)");
    gtk_widget_set_events (gg->color_ui.bg_da,
                           GDK_EXPOSURE_MASK
                           | GDK_ENTER_NOTIFY_MASK
                           | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);

    g_signal_connect (G_OBJECT (gg->color_ui.bg_da),
                      "draw", G_CALLBACK (color_draw_bg), gg);
    g_signal_connect (G_OBJECT (gg->color_ui.bg_da),
                      "button_press_event", G_CALLBACK (set_color_id), gg);

    gtk_table_attach (GTK_TABLE (bg_table),
                      gg->color_ui.bg_da, 0, 1, 0, 1,
                      GTK_FILL, GTK_FILL, 10, 10);

    /*-- Accent color --*/
    accent_frame = gtk_frame_new ("Accent color");
    gtk_box_pack_start (GTK_BOX (hbox), accent_frame, true, true, 0);

    ebox = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (accent_frame), ebox);

    accent_table = gtk_table_new (1, 5, true);
    gtk_container_add (GTK_CONTAINER (ebox), accent_table);

    gg->color_ui.accent_da = gtk_drawing_area_new ();
    gtk_widget_set_double_buffered (gg->color_ui.accent_da, false);
    gtk_widget_set_size_request (GTK_WIDGET (gg->color_ui.accent_da),
                                 PSIZE, PSIZE);
    gtk_widget_set_tooltip_text (
                          gg->color_ui.accent_da,
                          "Double click to reset color for labels and axes");
    gtk_widget_set_events (gg->color_ui.accent_da,
                           GDK_EXPOSURE_MASK | GDK_ENTER_NOTIFY_MASK |
                           GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);

    g_signal_connect (G_OBJECT (gg->color_ui.accent_da),
                      "draw", G_CALLBACK (color_draw_accent), gg);
    g_signal_connect (G_OBJECT (gg->color_ui.accent_da),
                      "button_press_event", G_CALLBACK (set_color_id), gg);

    gtk_table_attach (GTK_TABLE (accent_table),
                      gg->color_ui.accent_da, 0, 1, 0, 1,
                      GTK_FILL, GTK_FILL, 10, 10);

    /*-- Shadow (hidden) color --*/
    hidden_frame = gtk_frame_new ("Shadow color");
    gtk_box_pack_start (GTK_BOX (hbox), hidden_frame, true, true, 0);

    ebox = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (hidden_frame), ebox);

    hidden_table = gtk_table_new (1, 5, true);
    gtk_container_add (GTK_CONTAINER (ebox), hidden_table);

    gg->color_ui.hidden_da = gtk_drawing_area_new ();
    gtk_widget_set_double_buffered (gg->color_ui.hidden_da, false);
    gtk_widget_set_size_request (GTK_WIDGET (gg->color_ui.hidden_da),
                                 PSIZE, PSIZE);
    gtk_widget_set_tooltip_text (
                          gg->color_ui.hidden_da,
                          "Double click to reset color for labels and axes");
    gtk_widget_set_events (gg->color_ui.hidden_da,
                           GDK_EXPOSURE_MASK | GDK_ENTER_NOTIFY_MASK |
                           GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK);

    g_signal_connect (G_OBJECT (gg->color_ui.hidden_da),
                      "draw", G_CALLBACK (color_draw_hidden), gg);
    g_signal_connect (G_OBJECT (gg->color_ui.hidden_da),
                      "button_press_event", G_CALLBACK (set_color_id), gg);

    gtk_table_attach (GTK_TABLE (hidden_table),
                      gg->color_ui.hidden_da, 0, 1, 0, 1,
                      GTK_FILL, GTK_FILL, 10, 10);

    /*-- Temporary, perhaps: reverse video button --*/
    btn = gtk_button_new_with_mnemonic ("_Reverse video");
    gtk_box_pack_start (GTK_BOX (vbox), btn, false, false, 0);
    g_signal_connect (G_OBJECT (btn),
                      "clicked",
                      G_CALLBACK (reverse_video_cb), (gpointer) gg);

    /*-- Close button --*/
    btn = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    gtk_box_pack_start (GTK_BOX (vbox), btn, false, false, 0);
    g_signal_connect (G_OBJECT (btn),
                      "clicked",
                      G_CALLBACK (hide_symbol_window_cb), (gpointer) gg);
  }

  gtk_widget_show_all (gg->color_ui.symbol_window);

  /*
   * In case the default colorscheme has fewer colors than the
   * default, hide the extra fg_da's. 
   */
  for (k = gg->activeColorScheme->n; k < MAXNCOLORS; k++)
    gtk_widget_hide (gg->color_ui.fg_da[k]);
}
