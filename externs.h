#ifndef GGOBI_EXTERNS_H
#define GGOBI_EXTERNS_H

#include <stdio.h>

extern ggobid *ValidateGGobiRef (ggobid *gg, gboolean fatal);
extern displayd *display_create(guint action, ggobid *gg);
extern displayd *parcoords_new (gboolean missing_p, splotd **sub_plots, gint numSubPlots, ggobid *gg);
extern GtkWidget* CreateMenuCheck (displayd *display, GtkWidget *, gchar *, GtkSignalFunc, gpointer, gboolean, ggobid *gg);
extern GtkWidget* CreateMenuItem (GtkWidget *, gchar *, gchar *, gchar *, GtkWidget *, GtkAccelGroup *, GtkSignalFunc, gpointer, ggobid *gg) ;
extern ggobid* GGobiFromDisplay (displayd *display);
extern ggobid* GGobiFromSPlot (splotd *sp);
extern ggobid* GGobiFromWidget (GtkWidget *w, gboolean);
extern ggobid* GGobiFromWindow (GdkWindow *w);
extern void GGobi_widget_set (GtkWidget *w, ggobid *gg, gboolean isWindow);
extern void array_read (ggobid *gg);
extern void arrayf_add_cols (array_f *, gint);
extern void arrayf_add_rows (array_f *, gint);
extern void arrayf_alloc (array_f *, gint, gint);
extern void arrayf_alloc_zero (array_f *, gint, gint);
extern void arrayf_copy (array_f *, array_f *);
extern void arrayf_free (array_f *, gint, gint);
extern void arrayf_init (array_f *);
extern void arrayf_zero (array_f *);
extern void arrayl_add_cols (array_l *, gint);
extern void arrayl_add_rows (array_l *, gint);
extern void arrayl_alloc (array_l *, gint, gint);
extern void arrayl_alloc_zero (array_l *, gint, gint);
extern void arrayl_free (array_l *, gint, gint);
extern void arrayl_init (array_l *);
extern void arrayl_zero (array_l *);
extern void arrays_add_cols (array_s *, gint);
extern void arrays_add_rows (array_s *, gint);
extern void arrays_alloc (array_s *, gint, gint);
extern void arrays_alloc_zero (array_s *, gint, gint);
extern void arrays_free (array_s *, gint, gint);
extern void arrays_init (array_s *);
extern void arrays_zero (array_s *);
extern void assign_points_to_bins (ggobid *gg);
extern void br_color_ids_alloc (ggobid *gg);
extern void br_color_ids_init (ggobid *gg);
extern void br_glyph_ids_alloc (ggobid *gg);
extern void br_glyph_ids_init (ggobid *ggvoid);
extern void br_line_color_alloc (ggobid *gg);
extern void br_line_color_init (ggobid *ggvoid);
extern void br_line_vectors_check_size (gint, ggobid *gg);
extern void brush_activate (gboolean, ggobid *gg);
extern void brush_draw_brush (splotd *, ggobid *gg);
extern void brush_draw_label (splotd *, ggobid *gg);
extern void brush_event_handlers_toggle (splotd *, gboolean);
extern void brush_init (ggobid *gg);
extern void brush_menus_make (ggobid *gg);
extern void brush_motion (icoords *, gboolean, gboolean, cpaneld *, ggobid *gg);
extern gboolean brush_once (gboolean force, ggobid *gg);
extern void brush_options_cb (gpointer, guint, GtkCheckMenuItem *);
extern void brush_pos_init (ggobid *);
extern void brush_set_pos (gint, gint, ggobid *gg);
extern void cluster_free (gint, ggobid *gg);
extern void clusters_set (ggobid *gg);
extern gboolean collabels_read (gchar *, gboolean, ggobid *gg);
extern void color_table_init (ggobid *gg);
extern gchar *computeTitle(displayd *display, ggobid *gg);
extern void cpanel_brush_init (cpaneld *, ggobid *);
extern void cpanel_brush_make (ggobid *gg);
extern void cpanel_brush_set (cpaneld *, ggobid *);
extern void cpanel_ctour_make (ggobid *gg);
extern void cpanel_identify_make (ggobid *gg);
extern void cpanel_lineedit_make (ggobid *gg);
extern void cpanel_movepts_make (ggobid *gg);
extern void cpanel_p1d_init (cpaneld *, ggobid *);
extern void cpanel_p1d_set (cpaneld *, ggobid* gg);
extern void cpanel_p1dplot_make (ggobid *gg);
extern void cpanel_parcoords_init (cpaneld *, ggobid *);
extern void cpanel_parcoords_make (ggobid *gg);
extern void cpanel_rotation_init (cpaneld *, ggobid *);
extern void cpanel_rotation_make (ggobid *gg);
extern void cpanel_rotation_set (cpaneld *, ggobid *);
extern void cpanel_scale_init (cpaneld *, ggobid *);
extern void cpanel_scale_make (ggobid *gg);
extern void cpanel_scatmat_init (cpaneld *, ggobid *);
extern void cpanel_scatmat_make (ggobid *gg);
extern void cpanel_set (displayd *, ggobid *gg);
extern void cpanel_tour2d_make (ggobid *gg);
extern void cpanel_tour_init (cpaneld *, ggobid *);
extern void cpanel_xyplot_make (ggobid *gg);
extern void ctour_event_handlers_toggle (splotd *, gboolean);
extern void ctourpp_window_open (ggobid *gg);
extern displayd *dataset_init (ggobid *gg, gboolean cleanup);
extern gint display_add(displayd *display, ggobid *gg);
extern void display_close_cb (displayd *d, guint, GtkWidget *);
extern void display_delete_cb (GtkWidget *, GdkEvent *, displayd *);
extern void display_free (displayd *, gboolean force, ggobid *gg);
extern void display_free_all (ggobid *gg);
extern void display_new (ggobid *, guint action, GtkWidget *widget);
extern void display_options_cb (GtkCheckMenuItem *w, guint action);
extern void display_plot (displayd *display, guint type, ggobid *gg);
extern void display_print_cb (displayd *d, guint, GtkWidget *);
extern void display_set_current (displayd *, ggobid* gg);
extern void display_tailpipe (displayd *, ggobid *gg);
extern void display_tailpipe (displayd *display, ggobid *gg);
extern void display_tour_init (displayd *, ggobid *gg);
extern void display_window_init (displayd *display, gint width, ggobid*);
extern void displays_plot (splotd *sp, ggobid *gg);
extern void displays_tailpipe (gint, ggobid *gg);
extern gint do_ash1d (gfloat *, gint, gint, gint, gfloat *, gfloat *, gfloat *);
extern void draw_glyph (GdkDrawable *, glyphv *, icoords *, gint, ggobid *gg);
extern void eigenvals_get (gfloat *, ggobid *gg);
extern void exclusion_window_open (ggobid *gg);
extern gboolean fileset_read (gchar *, ggobid *gg);
extern gboolean fileset_read_init (gchar *ldata_in, ggobid *gg);
extern void find_glyph_type_and_size (gint, glyphv *);
extern gint find_nearest_point (icoords *, splotd *, ggobid *gg);
extern GList* g_list_remove_nth (GList *, gint);
extern GList* g_list_replace_nth (GList *, gpointer, gint);
extern void get_extended_brush_corners (icoords *, icoords *, ggobid *gg);
extern void get_main_menu (GtkItemFactoryEntry[], gint, GtkAccelGroup *, GtkWidget  *, GtkWidget **, gpointer);
extern gint get_vgroup_cols (gint, gint *, ggobid *gg);
extern void hidden_alloc (ggobid *gg);
extern void hidden_init (ggobid *gg);
extern gboolean hidden_read (gchar *, gboolean, ggobid *gg);
extern void identify_event_handlers_toggle (splotd *, gboolean);
extern void identify_menus_make (ggobid *gg);
extern void init_plot_GC (GdkWindow *, ggobid *gg);
extern void init_var_GCs (GtkWidget *, ggobid *gg);
extern gfloat jitter_randval (gint);
extern void jitter_value_set (gfloat, ggobid *gg);
extern void jitter_vars_init (ggobid *gg);
extern void jitter_window_open (ggobid *gg);
extern void limits_adjust (gfloat *, gfloat *);
extern void line_brush_prev_vectors_update (ggobid *gg);
extern void line_brush_undo (splotd *, ggobid *gg);
extern gboolean line_colors_read (gchar *, gboolean, ggobid *gg);
extern void lineedit_event_handlers_toggle (splotd *, gboolean);
extern void make_symbol_window (ggobid *gg);
extern void make_ui (ggobid *gg);
extern void make_varpanel (GtkWidget *, ggobid *gg);
extern GlyphType mapGlyphName (const gchar *gtype);
extern void missing_alloc (gint, gint);
extern void missing_arrays_add_column (gint, ggobid *gg);
extern void missing_block_alloc (gint, gint);
extern void missing_jitter_value_set (gfloat, ggobid *gg);
extern void missing_lim_set (ggobid *gg);
extern void missing_rejitter (ggobid *gg);
extern void missing_to_world (ggobid *gg);
extern void missing_values_read (gchar *, gboolean, ggobid *gg);
extern void missing_world_alloc (ggobid *gg);
extern void mode_activate (splotd *, gint, gboolean, ggobid *gg);
extern gint mode_get (ggobid* gg);
extern void mode_set (gint, ggobid *gg);
extern void mode_set_cb (GtkWidget  *, gint);
extern void mode_submenus_activate (splotd *, gint, gboolean, ggobid *gg);
extern void mousepos_get_motion (GtkWidget *, GdkEventMotion *, gboolean *, gboolean *, ggobid *gg);
extern void mousepos_get_pressed (GtkWidget *, GdkEventButton *, gboolean *, gboolean *, ggobid *gg);
extern void movepts_event_handlers_toggle (splotd *, gboolean);
extern gint nspherevars_get (ggobid *gg);
extern gint nvgroups (ggobid *gg);
extern FILE* open_ggobi_file_r (gchar *, gint, gchar **, gboolean);
extern void p1d_reproject (splotd *, glong **, ggobid *gg);
extern gboolean p1d_varsel (splotd *, gint, gint *, gint);
extern void pan_by_drag (splotd *, ggobid *gg);
extern void pan_step (splotd *, gint, ggobid *gg);
extern void parcoords_cpanel_init (cpaneld*, ggobid *);
extern void parcoords_main_menus_make (GtkAccelGroup *, GtkSignalFunc, ggobid *gg, gboolean);
extern void parcoords_reset_arrangement (displayd *, gint, ggobid *gg);
extern gboolean parcoords_varsel (cpaneld *, splotd *, gint, gint *, gboolean, ggobid *gg);
extern gboolean pca_calc (ggobid *gg);
extern void pca_diagnostics_set (ggobid *gg);
extern gint pcompare (const void *, const void *);
extern void pipeline_arrays_add_column (gint, ggobid *gg);
extern void pipeline_arrays_alloc (ggobid *gg);
extern void pipeline_init (ggobid *gg);
extern gint plotted_cols_get (gint *, gboolean, ggobid *gg);
extern void point_brush_prev_vectors_update (ggobid *gg);
extern void point_brush_undo (splotd *, ggobid *gg);
extern gboolean point_colors_read (gchar *, gboolean, ggobid *gg);
extern gboolean point_glyphs_read (gchar *, gboolean, ggobid *gg);
extern gboolean point_in_which_bin (gint, gint, gint *, gint *, ggobid *gg);
extern void populate_option_menu (GtkWidget *, gchar **, gint, GtkSignalFunc, ggobid *gg);
extern gint projection_get (ggobid* gg);
extern void quick_message (gchar *, gboolean);
/* extern void quit_ggobi(void); */
extern void quit_ggobi(ggobid *gg, gint action, GtkWidget *w);
extern gdouble randvalue (void);
extern void raw_to_tform_copy (void);
extern void rejitter (ggobid *gg);
extern void rnorm2 (gdouble *, gdouble *);
extern void rotation_event_handlers_toggle (splotd *, gboolean);
extern void rotation_menus_make (ggobid *gg);
extern void rowlabels_alloc (ggobid *gg) ;
extern gboolean rowlabels_read (gchar *, gboolean, ggobid *gg);
extern void rows_in_plot_set (ggobid *gg);
extern void ruler_ranges_set (displayd *, splotd *, ggobid *gg);
extern void scale_event_handlers_toggle (splotd *, gboolean);
extern void scale_menus_make (ggobid *gg);
extern void scaling_visual_cues_draw (splotd *, ggobid *gg);
extern void scatmat_cpanel_init (cpaneld *, ggobid *);
extern void scatmat_main_menus_make (GtkAccelGroup *, GtkSignalFunc, ggobid *gg, gboolean);
extern displayd* scatmat_new (gboolean missing_p, splotd **sub_plots, gint numRows, gint numVertical, ggobid *gg); 
extern gboolean scatmat_varsel (cpaneld *, splotd *, gint, gint *, gint, gboolean, ggobid *gg);
extern void scatterplot_cpanel_init (cpaneld *, gint, ggobid *);
extern void scatterplot_main_menus_make (GtkAccelGroup *, GtkSignalFunc, ggobid *gg, gboolean);
extern displayd* scatterplot_new (gboolean, splotd *sp, ggobid *gg);
extern void scatterplot_show_rulers (displayd *, gint);
extern void segments_alloc (gint, ggobid *gg);
extern void segments_create (ggobid *gg);
extern void segments_free (ggobid *gg);
extern gboolean segments_read (gchar *, gboolean, ggobid *gg);
extern gint selected_cols_get (gint *, gboolean, ggobid *gg);
extern void smooth_window_open (ggobid *gg);
extern void sp_event_handlers_toggle (splotd *, gboolean);
extern void sp_whiskers_make (splotd *, displayd *, ggobid *gg);
extern void sphere_apply_cb (ggobid *gg);
extern gint sphere_npcs_get (ggobid *gg);
extern void sphere_npcs_set (gint, ggobid *gg);
extern void sphere_panel_open (ggobid *gg);
extern void sphere_transform_set (ggobid *gg);
extern gint spherevars_get (gint *, ggobid *gg);
extern void spherevars_set (ggobid *gg);
extern void splot_add_point_label (splotd *, gint, ggobid *gg);
extern void splot_dimension_set(splotd* sp, gint width, gint height);
extern void splot_draw_border (splotd *, ggobid *gg);
extern void splot_expose (splotd *);
extern void splot_free (splotd *, displayd *, ggobid *gg);
extern void splot_get_dimensions (splotd *, gint *, gint *);
extern splotd* splot_new (displayd *, gint, gint, ggobid *gg);
extern void splot_pixmap0_to_pixmap1 (splotd *, gboolean, ggobid *gg);
extern void splot_pixmap1_to_window (splotd *, ggobid *gg);
extern void splot_plane_to_screen (displayd *, cpaneld *, splotd *, ggobid *gg);
extern void splot_plane_to_screen (displayd *, cpaneld *, splotd *, ggobid *gg);
extern void splot_redraw (splotd *sp, gint, ggobid *gg);
extern void splot_screen_to_tform (cpaneld *, splotd *, icoords *, fcoords *, ggobid *gg);
extern void splot_set_current (splotd *, gboolean, ggobid *gg);
extern void splot_set_plot_center (splotd *);
extern void splot_world_to_plane (cpaneld *, splotd *, ggobid *gg);
extern gint sqdist (gint, gint, gint, gint);
extern void strip_suffixes (ggobid *gg);
extern void submenu_append (GtkWidget *, GtkWidget *);
extern void submenu_destroy (GtkWidget *);
extern void submenu_insert (GtkWidget *, GtkWidget *, gint);
extern GtkWidget* submenu_make (gchar *, gint, GtkAccelGroup *);
extern void subset_window_open (ggobid *gg);
extern void textur (gfloat *, gfloat *, gint, gint, gfloat, gint, ggobid *gg);
extern void tform1_to_tform2_copy (void);
extern void tform_label_update (gint, ggobid *gg);
extern void tform_to_world (ggobid *gg);
extern void tour2d_event_handlers_toggle (splotd *, gboolean);
extern void tour2d_menus_make (ggobid *gg);
extern void tour2dpp_window_open (ggobid *gg);
extern void tour_do_step (displayd *, ggobid *gg);
extern void tour_reproject (splotd *, glong **, ggobid *gg);
extern void transform (gint, gint, gfloat, ggobid *gg);
extern void transform_opt_menus_set_history (gint, ggobid *gg);
extern void transform_values_init (gint j, ggobid *gg);
extern void transform_variable (gint, gint, gfloat, gint, ggobid *);
extern void transform_window_open (ggobid *gg);
extern void vardata_alloc (ggobid *gg);
extern void vardata_init (ggobid *gg);
extern void vardata_lim_raw_gp_set (ggobid *gg);
extern void vardata_lim_update (ggobid *gg);
extern void vardata_realloc (gint, ggobid *gg);
extern void vardata_stats_set (ggobid *gg);
extern void varlabel_set (gint, ggobid *gg);
extern void varpanel_layout_init (ggobid *gg);
extern void varpanel_populate (ggobid *gg);
extern void varpanel_refresh (ggobid *gg);
extern void varpanel_size_init (gint, ggobid* gg);
extern void vartable_open (ggobid *);
extern void vartable_row_append (gint, ggobid *);
extern void vartable_select_var (gint, gboolean, ggobid *);
extern void vartable_tform_set (gint, ggobid *);
extern void vartable_unselect_all (ggobid *);
extern void vectorb_copy (vector_b *, vector_b *);
extern void vectorb_free (vector_b *);
extern void vectorb_init (vector_b *);
extern void vectorb_realloc (vector_b *, gint);
extern void vectorb_realloc_zero (vector_b *, gint);
extern void vectori_init (vector_i *);
extern void vectori_realloc (vector_i *, gint);
extern void vectori_realloc_zero (vector_i *, gint);
extern void vectors_copy (vector_s *, vector_s *);
extern void vectors_free (vector_s *);
extern void vectors_init (vector_s *);
extern void vectors_realloc (vector_s *, gint);
extern void vectors_realloc_zero (vector_s *, gint);
extern gint vgroup_cols (gint, gint *);
extern gboolean vgroups_read (gchar *, gboolean, ggobid *gg);
extern void vgroups_sort (ggobid *gg);
extern gboolean write_xml (const gchar *, ggobid *);
extern void xy_reproject (splotd *, glong **, ggobid *gg);
extern gboolean xyplot_varsel (splotd *, gint, gint *, gint);
extern void zoom_by_drag (splotd *, ggobid *gg);
extern void zoom_step (splotd *, gint, gint, rectd *, ggobid* gg);
extern void writeall_window_open (ggobid *);
extern void rowlabels_free (ggobid *);

ggobid * ggobi_get (gint);
displayd * display_alloc_init (enum displaytyped type, gboolean missing_p, ggobid *gg);

/* The new way of handling window closures, so that we don't
   just exit.
 */
void ggobi_close (GtkObject *w, ggobid *gg);
int ggobi_remove_by_index (ggobid *gg, int which);
int ggobi_remove (ggobid *gg);

extern void variable_clone (gint, const gchar *, gboolean, ggobid *);
extern gboolean updateAddedColumn (gint nc, gint jvar, ggobid *gg);

#include "GGobiAPI.h"

#define CHECK_GG(a) ValidateGGobiRef(a, true)

#endif


