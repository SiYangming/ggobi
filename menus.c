/*-- menus.c: where the modes change entries in the main menubar --*/
/*
    This software may only be used by you under license from AT&T Corp.
    ("AT&T").  A copy of AT&T's Source Code Agreement is available at
    AT&T's Internet website having the URL:
    <http://www.research.att.com/areas/stat/ggobi/license.html>
    If you received this software without first entering into a license
    with AT&T, you have an infringing copy of this software and cannot use
    it without violating AT&T's intellectual property rights.
*/

#include <gtk/gtk.h>
#include "vars.h"
#include "externs.h"

/*-- every mode has an options meny --*/
gboolean
mode_has_options_menu (gint mode)
{
  return (mode == P1PLOT ||
          mode == XYPLOT ||
          mode == SCALE  ||
          mode == BRUSH  ||
          mode == TOUR1D ||
          mode == TOUR2D ||
          mode == COTOUR ||
          mode == IDENT  ||
          mode == MOVEPTS ||
          mode == PCPLOT ||
          mode == TSPLOT);
}

gboolean
mode_has_reset_menu (gint mode)
{
  return (mode == SCALE  ||
          mode == BRUSH);
}

gboolean
mode_has_io_menu (gint mode)
{
  return (mode == TOUR1D ||
          mode == TOUR2D ||
          mode == COTOUR);
}


/*--------------------------------------------------------------------*/
/*                   Plot1D: Options menu                             */
/*--------------------------------------------------------------------*/

void
p1dplot_menus_make (ggobid *gg)
{
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                     XYPlot: Options menu                           */
/*--------------------------------------------------------------------*/

void
xyplot_menus_make (ggobid *gg)
{
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                   Tour 1D: I/O and Options menus                   */
/*--------------------------------------------------------------------*/

void
tour1d_menus_make (ggobid *gg) {
  GtkWidget *item;
  extern void varcircles_layout_cb (GtkCheckMenuItem *w, guint action);
  void tour1d_io_cb (GtkWidget *w, gpointer *cbd);

  /*-- I/O menu --*/
  gg->menus.io_item = submenu_make ("_I/O", 'I',
    gg->main_accel_group);
  gg->menus.io_menu = gtk_menu_new ();

/*-- these could use CreateMenuItem --*/
  item = gtk_menu_item_new_with_label ("Save coefficients");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tour1d_io_cb),
                      (gpointer) "write_coeffs");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  item = gtk_menu_item_new_with_label ("Save history");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tour1d_io_cb),
                      (gpointer) "write_history");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  item = gtk_menu_item_new_with_label ("Read history");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tour1d_io_cb),
                      (gpointer) "read_history");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);
/*-- --*/

  gtk_widget_show_all (gg->menus.io_menu);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.io_item),
    gg->menus.io_menu); 
  submenu_insert (gg->menus.io_item, gg->main_menubar, 4);

  /*-- Options menu --*/ 
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  /* Add a separator before the mode-specific items */
  CreateMenuItem (gg->menus.options_menu, NULL,
    "", "", NULL, NULL, NULL, NULL, gg);

  CreateMenuCheck (gg->menus.options_menu, "Lay out variable circles by row",
    GTK_SIGNAL_FUNC (varcircles_layout_cb), NULL,
    gg->varpanel_ui.layoutByRow, gg);


  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                   Tour 2D: I/O and Options menus                   */
/*--------------------------------------------------------------------*/

void
tour2d_menus_make (ggobid *gg)
{
  GtkWidget *item;
  extern void varcircles_layout_cb (GtkCheckMenuItem *w, guint action);
  void tour2d_io_cb (GtkWidget *w, gpointer *cbd);

  /*-- I/O menu --*/
  gg->menus.io_menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_label ("Save coefficients");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tour2d_io_cb),
                      (gpointer) "write_coeffs");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  item = gtk_menu_item_new_with_label ("Save history");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tour2d_io_cb),
                      (gpointer) "write_history");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  item = gtk_menu_item_new_with_label ("Read history");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tour2d_io_cb),
                      (gpointer) "read_history");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  gtk_widget_show_all (gg->menus.io_menu);

  gg->menus.io_item = submenu_make ("_I/O", 'I',
    gg->main_accel_group);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.io_item),
    gg->menus.io_menu); 
  submenu_insert (gg->menus.io_item, gg->main_menubar, 4);

  /*-- Options menu --*/
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  /* Add a separator before the mode-specific items */
  CreateMenuItem (gg->menus.options_menu, NULL,
    "", "", NULL, NULL, NULL, NULL, gg);

  CreateMenuCheck (gg->menus.options_menu, "Lay out variable circles by row",
    GTK_SIGNAL_FUNC (varcircles_layout_cb), NULL,
    gg->varpanel_ui.layoutByRow, gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                 Corr Tour: I/O and Options menus                   */
/*--------------------------------------------------------------------*/

void
tourcorr_menus_make (ggobid *gg)
{
  GtkWidget *item;
  extern void varcircles_layout_cb (GtkCheckMenuItem *w, guint action);
  void tourcorr_io_cb (GtkWidget *w, gpointer *cbd);

  /*-- I/O menu --*/
  gg->menus.io_menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_label ("Save coefficients");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tourcorr_io_cb),
                      (gpointer) "write_coeffs");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  item = gtk_menu_item_new_with_label ("Save history");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tourcorr_io_cb),
                      (gpointer) "write_history");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  item = gtk_menu_item_new_with_label ("Read history");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (tourcorr_io_cb),
                      (gpointer) "read_history");
  gtk_menu_append (GTK_MENU (gg->menus.io_menu), item);

  gtk_widget_show_all (gg->menus.io_menu);

  gg->menus.io_item = submenu_make ("_I/O", 'I',
    gg->main_accel_group);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.io_item),
    gg->menus.io_menu); 
  submenu_insert (gg->menus.io_item, gg->main_menubar, 4);

  /*-- Options menu --*/
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  /* Add a separator before the mode-specific items */
  CreateMenuItem (gg->menus.options_menu, NULL,
    "", "", NULL, NULL, NULL, NULL, gg);

  CreateMenuCheck (gg->menus.options_menu, "Lay out variable circles by row",
    GTK_SIGNAL_FUNC (varcircles_layout_cb), NULL,
    gg->varpanel_ui.layoutByRow, gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                   Scaling: Reset and Options menus                 */
/*--------------------------------------------------------------------*/

void
scale_menus_make (ggobid *gg) {
  GtkWidget *item;
  void pan_reset_cb (GtkWidget *w, ggobid *gg);
  void zoom_reset_cb (GtkWidget *w, ggobid *gg);

  /*-- Reset menu --*/
  gg->menus.reset_menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_label ("Reset pan");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (pan_reset_cb),
                      (gpointer) gg);
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);

  item = gtk_menu_item_new_with_label ("Reset zoom");
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (zoom_reset_cb),
                      (gpointer) gg);
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);

  gtk_widget_show_all (gg->menus.reset_menu);

  gg->menus.reset_item = submenu_make ("_Reset", 'R',
    gg->main_accel_group);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.reset_item),
    gg->menus.reset_menu); 
  submenu_insert (gg->menus.reset_item, gg->main_menubar, 4);

  /*-- Options menu --*/

  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*               Brushing: Reset and Options menus                    */
/*--------------------------------------------------------------------*/

void
brush_menus_make (ggobid *gg)
{
  GtkWidget *item;
  void brush_reset_cb (GtkWidget *w, gpointer cbd);
  void brush_update_set_cb (GtkCheckMenuItem *w, guint action);

  /*-- Reset menu --*/
  gg->menus.reset_menu = gtk_menu_new ();

  item = gtk_menu_item_new_with_label ("Show all points");
  GGobi_widget_set (item, gg, true);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (brush_reset_cb),
                      (gpointer) GINT_TO_POINTER (0));
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);

/*
  item = gtk_menu_item_new_with_label ("Reset point colors");
  GGobi_widget_set (item, gg, true);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (brush_reset_cb),
                      (gpointer) GINT_TO_POINTER (1));
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);

  item = gtk_menu_item_new_with_label ("Reset glyphs");
  GGobi_widget_set (item, gg, true);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (brush_reset_cb),
                      (gpointer) GINT_TO_POINTER (2));
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);
*/

  item = gtk_menu_item_new_with_label ("Show all edges");
  GGobi_widget_set (item, gg, true);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (brush_reset_cb),
                      (gpointer) GINT_TO_POINTER (3));
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);

/*
  item = gtk_menu_item_new_with_label ("Reset edgecolors");
  GGobi_widget_set (item, gg, true);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (brush_reset_cb),
                      (gpointer) GINT_TO_POINTER(4));
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);
*/

  item = gtk_menu_item_new_with_label ("Reset brush size");
  GGobi_widget_set (item, gg, true);
  gtk_signal_connect (GTK_OBJECT (item), "activate",
                      GTK_SIGNAL_FUNC (brush_reset_cb),
                      (gpointer) GINT_TO_POINTER (5));
  gtk_menu_append (GTK_MENU (gg->menus.reset_menu), item);

  gtk_widget_show_all (gg->menus.reset_menu);


  gg->menus.reset_item = submenu_make ("_Reset", 'R',
    gg->main_accel_group);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.reset_item),
    gg->menus.reset_menu);
  submenu_insert (gg->menus.reset_item, gg->main_menubar, 4);

  /*-- Options menu --*/
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  /* Add a separator before the mode-specific items */
  CreateMenuItem (gg->menus.options_menu, NULL,
    "", "", NULL, NULL, NULL, NULL, NULL);

  CreateMenuCheck (gg->menus.options_menu, "Update brushing continuously",
    GTK_SIGNAL_FUNC (brush_update_set_cb), NULL,
    gg->brush.updateAlways_p, gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                   Identify: Options menu                           */
/*--------------------------------------------------------------------*/

void
identify_menus_make (ggobid *gg)
{
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                   Movepts: Options menu                            */
/*--------------------------------------------------------------------*/

void
movepts_menus_make (ggobid *gg) {
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}


/*--------------------------------------------------------------------*/
/*                   Edge edit: Options menu                          */
/*--------------------------------------------------------------------*/

void
edgeedit_menus_make (ggobid *gg) {

  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);

}

/*--------------------------------------------------------------------*/
/*                   Scatmat: Options menu                            */
/*--------------------------------------------------------------------*/

void
scatmat_menus_make (ggobid *gg) {
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}

/*--------------------------------------------------------------------*/
/*                   Parcoords: Options menu                          */
/*--------------------------------------------------------------------*/

void
pcplot_menus_make (ggobid *gg)
{
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}


/*--------------------------------------------------------------------*/
/*                   Time series: Options menu                        */
/*--------------------------------------------------------------------*/

void
tsplot_menus_make (ggobid *gg) {
  gg->menus.options_item = submenu_make ("_Options", 'O',
    gg->main_accel_group);
  gg->menus.options_menu = gtk_menu_new ();

  CreateMenuCheck (gg->menus.options_menu, "Show tooltips",
    GTK_SIGNAL_FUNC (tooltips_show_cb), NULL,
    GTK_TOOLTIPS (gg->tips)->enabled, gg);

  CreateMenuCheck (gg->menus.options_menu, "Show control panel",
    GTK_SIGNAL_FUNC (cpanel_show_cb), NULL,
    GTK_WIDGET_VISIBLE (gg->mode_frame), gg);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (gg->menus.options_item),
    gg->menus.options_menu);
  submenu_insert (gg->menus.options_item, gg->main_menubar, OPTIONS_MENU_POS);
}
