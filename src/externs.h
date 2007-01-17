/* externs.h */
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

#ifndef GGOBI_S_H
#define GGOBI_S_H

#include <stdio.h>
#ifndef DEFINES_H
#include "defines.h"
#endif

#include "barchartDisplay.h"

#ifdef __cplusplus
extern "C" {
#endif

GtkWidget * mode_panel_get_by_name(const gchar *, GGobiSession *);
InteractionMode imode_get(GGobiSession *);  /* this should probably also
					 change */
ProjectionMode pmode_get(displayd *, GGobiSession *);
void pmode_set_cb (GtkWidget *widget, gint action);
void imode_set_cb (GtkWidget *widget, gint action);
void main_miscmenus_update (ProjectionMode, InteractionMode, displayd *, GGobiSession *gg);
void viewmode_set(ProjectionMode, InteractionMode, GGobiSession *);
gint ggobi_full_viewmode_set(ProjectionMode, InteractionMode, GGobiSession*);

RedrawStyle brush_activate (gboolean, displayd *, splotd *);
RedrawStyle edgeedit_activate (gboolean state, displayd *display, GGobiSession *gg);
RedrawStyle identify_activate (gint, displayd *, GGobiSession *);
RedrawStyle p1d_activate (gint, displayd *, GGobiSession *);
RedrawStyle imode_activate (splotd *, ProjectionMode, InteractionMode, gboolean, GGobiSession *);
RedrawStyle xyplot_activate (gint, displayd *, GGobiSession *);


/* reverse pipeline */
void pt_screen_to_plane (icoords *screen, gint id, gboolean horiz, gboolean vert, gcoords *eps, gcoords *planar, splotd *sp);
void pt_plane_to_world (splotd *sp, gcoords *planar, gcoords *eps, greal *world);
void pt_world_to_raw_by_var (gint j, greal *world, greal *raw, GGobiStage *d);

/* sort -t":" -k1.12 */
gboolean   array_contains (gint* arr, gint n, gint el);
void       edgeedit_init (GGobiSession * gg);
GtkWidget* CreateMenuCheck (GtkWidget *, gchar *, GtkSignalFunc, gpointer, gboolean, GGobiSession *);
GtkWidget* CreateMenuItem (GtkWidget *, gchar *, gchar *, gchar *, GtkWidget *, GtkAccelGroup *, GtkSignalFunc, gpointer, GGobiSession *) ;
GtkWidget* CreateMenuItemWithCheck (GtkWidget *, gchar *, gchar *, gchar *, GtkWidget *, GtkAccelGroup *, GtkSignalFunc, gpointer, GGobiSession *, GSList *, gboolean check) ;
GGobiSession*    GGobiFromDisplay (displayd *display);
GGobiSession*    GGobiFromSPlot (splotd *sp);
GGobiSession*    GGobiFromWidget (GtkWidget *w, gboolean);
GGobiSession*    GGobiFromWindow (GdkWindow *w);
void ggobi_addToolAction (GtkActionEntry *entry, gpointer *data, GGobiSession *gg);
GtkWidget* ggobi_addToolsMenuItem (gchar *label, GGobiSession *gg);
gboolean   ggobi_addToolsMenuWidget(GtkWidget *entry, GGobiSession *gg);
void       ggobi_widget_set (GtkWidget *, GGobiSession *gg, gboolean isWindow);
GGobiStage*     ValidateDatadRef (GGobiStage *d, GGobiSession *gg, gboolean fatal);
displayd*  ValidateDisplayRef(displayd *d, GGobiSession *gg, gboolean fatal);
GGobiSession*    ValidateGGobiRef (GGobiSession *gg, gboolean fatal);
#ifdef STORE_SESSION_ENABLED
xmlNodePtr XML_addVariable(xmlNodePtr node, gint j, GGobiStage *d);
#endif
void       addvar_propagate (gint ncols_prev, gint ncols_added, GGobiStage *);
gint       alloc_optimize0_p (optimize0_param *op, gint nrows, gint ncols, gint ndim);
gint       alloc_pp (pp_param *pp, gint nrows, gint ncols, gint ndim);
void       arrayd_add_cols (array_d *, gint);
void       arrayd_add_rows (array_d *, gint);
void       arrayd_alloc (array_d *, gint, gint);
void       arrayd_alloc_zero (array_d *, gint, gint);
void       arrayd_copy (array_d *, array_d *);
void       arrayd_delete_cols (array_d *, GSList *);
void       arrayd_free (array_d *, gint, gint);
void       arrayd_init_null (array_d *);
void       arrayd_zero (array_d *);
void       arrayf_add_cols (array_f *, gint);
void       arrayf_add_rows (array_f *, gint);
void       arrayf_alloc (array_f *, gint, gint);
void       arrayf_alloc_zero (array_f *, gint, gint);
void       arrayf_copy (array_f *, array_f *);
void       arrayf_delete_cols (array_f *, GSList *);
void       arrayf_free (array_f *, gint, gint);
void       arrayf_init_null (array_f *);
void       arrayf_zero (array_f *);
void       arrayg_add_cols (array_g *, gint);
void       arrayg_add_rows (array_g *, gint);
void       arrayg_alloc (array_g *, gint, gint);
void       arrayg_alloc_zero (array_g *, gint, gint);
void       arrayg_delete_cols (array_g *, GSList *);
void       arrayg_free (array_g *, gint, gint);
void       arrayg_init_null (array_g *);
void       arrayg_zero (array_g *);
void       arrayl_add_cols (array_l *, gint);
void       arrayl_add_rows (array_l *, gint);
void       arrayl_alloc (array_l *, gint, gint);
void       arrayl_alloc_zero (array_l *, gint, gint);
void       arrayl_delete_cols (array_l *, GSList *);
void       arrayl_free (array_l *, gint, gint);
void       arrayl_init_null (array_l *);
void       arrayl_zero (array_l *);
void       arrays_add_cols (array_s *, gint);
void       arrays_add_rows (array_s *, gint);
void       arrays_alloc (array_s *, gint, gint);
void       arrays_alloc_zero (array_s *, gint, gint);
void       arrays_delete_cols (array_s *, GSList *);
void       arrays_free (array_s *, gint, gint);
void       arrays_init_null (array_s *);
void       arrays_zero (array_s *);
void       ash_baseline_set (icoords *, splotd *sp);
void       assign_points_to_bins (GGobiStage *, splotd *, GGobiSession *);
gboolean   br_edge_vectors_check_size (gint, GGobiStage *);
void       brush_draw_brush (splotd *, GdkDrawable *, GGobiStage *, GGobiSession *);
void       brush_draw_label (splotd *, GdkDrawable *, GGobiStage *, GGobiSession *);
void       brush_event_handlers_toggle (splotd *, gboolean);
gboolean   brush_motion (icoords *, gboolean, gboolean, cpaneld *, splotd *, GGobiSession *);
void       brush_on_set (gboolean, displayd *, GGobiSession *);
gboolean   brush_once (gboolean force, splotd *, GGobiSession *);
gboolean   brush_once_and_redraw (gboolean binningp, splotd *sp, displayd *display, GGobiSession *gg);
void       brush_pos_init (splotd *);
void       brush_reset(displayd *display, gint action);
void       brush_set_pos (gint, gint, splotd *);
void       brush_undo (GGobiStage *);
void       brush_update_set (gboolean, displayd *, GGobiSession *);
gboolean   brush_all_matching_cv (cpaneld *, GGobiStage *, GGobiSession *);
gdouble    calc_norm (gdouble *, gint);
gboolean   checkequiv(gdouble **u0, gdouble **u1, gint nc, gint nd);
void       clone_vars (gint *cols, gint ncols, GGobiStage *);
void       cluster_free (gint, GGobiStage *, GGobiSession *);
void       cluster_table_labels_update (GGobiStage *d, GGobiSession *gg);
void       cluster_table_update (GGobiStage *, GGobiSession *);
void       cluster_window_open (GGobiSession *);
void       clusters_set(GGobiStage *);
void       collab_tform_update (gint j, GGobiStage *d);
gboolean   colors_remap (colorschemed *scheme, gboolean force, GGobiSession *gg);
void       colorscheme_init (colorschemed *scheme);
gchar*     computeTitle (gboolean, displayd *, GGobiSession *);
void       copy_mat(gdouble **, gdouble **, gint, gint);
void       cpanel_brush_init (cpaneld *, GGobiSession *);
void       cpanel_brush_make (GGobiSession *);
void       cpanel_brush_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_ctour_make (GGobiSession *);
void       cpanel_edgeedit_init (cpaneld *, GGobiSession *);
void       cpanel_edgeedit_make (GGobiSession *);
void       cpanel_edgeedit_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_identify_init (cpaneld *, GGobiSession *);
void       cpanel_identify_make (GGobiSession *);
void       cpanel_identify_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_movepts_make (GGobiSession *);
void       cpanel_p1d_init (cpaneld *, GGobiSession *);
void       cpanel_p1d_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_p1dplot_make (GGobiSession *);
GtkWidget *cpanel_parcoords_make (GGobiSession *);
void       cpanel_parcoords_set (displayd *, cpaneld *, GtkWidget *panel, GGobiSession *);
void       cpanel_scale_init (cpaneld *, GGobiSession *);
void       cpanel_scale_make (GGobiSession *);
void       cpanel_scale_set (displayd *, cpaneld *, GGobiSession *);
GtkWidget *cpanel_scatmat_make (GGobiSession *);
void       cpanel_scatmat_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_set (displayd *, GGobiSession *);
void       cpanel_show (gboolean show, GGobiSession *gg);
void       cpanel_t1d_init (cpaneld *, GGobiSession *);
void       cpanel_t2d3_init (cpaneld *, GGobiSession *);
void       cpanel_t2d_init (cpaneld *, GGobiSession *);
void       cpanel_tcorr_init (cpaneld *, GGobiSession *);
void       cpanel_tour1d_make (GGobiSession *);
void       cpanel_tour1d_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_tour2d3_make (GGobiSession *);
void       cpanel_tour2d3_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_tour2d_make (GGobiSession *);
void       cpanel_tour2d_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_tourcorr_set (displayd *, cpaneld *, GGobiSession *);
void       cpanel_xyplot_init (cpaneld *, GGobiSession *);
void       cpanel_xyplot_make (GGobiSession *);
void       cpanel_xyplot_set (displayd *, cpaneld *, GGobiSession *);
displayd * createDisplayFromDescription (GGobiSession *, GGobiDisplayDescription *desc);
GtkWidget* create_variable_notebook (GtkWidget *box, GtkSelectionMode mode, GGobiVariableType vartype, datatyped dtype, GtkSignalFunc func, gpointer func_data, GGobiSession *);
typedef const gchar ** (*GGobiVariableNotebookPrefixFunc) (GtkWidget *notebook, GGobiStage *d, gint *sel_prefix, gint *n_prefices);
GtkWidget* create_prefixed_variable_notebook (GtkWidget *box, GtkSelectionMode mode, GGobiVariableType vartype, datatyped dtype, GtkSignalFunc func, gpointer func_data, GGobiSession *, GGobiVariableNotebookPrefixFunc prefix_func);
void       ctour_event_handlers_toggle (splotd *, gboolean);
void       ctourpp_window_open (GGobiSession *);
colorschemed* default_scheme_init ();
gushort    datad_colors_used_get (gint *ncolors_used, gushort *colors_used, GGobiStage *, GGobiSession *); 
GGobiStage*     datad_get_from_notebook(GtkWidget *notebook);
void       datad_record_id_add (gchar *ids);
gint       delete_vars (gint *, gint, GGobiStage *);
void       disconnect_button_press_signal (splotd *sp);
void       disconnect_button_release_signal (splotd *sp);
void       disconnect_key_press_signal (splotd *sp);
void       disconnect_scroll_signal (splotd *sp);
void       disconnect_motion_signal (splotd *sp);
gint       display_add(displayd *display, GGobiSession *);
displayd*  display_alloc_init (gboolean, GGobiStage *, GGobiSession *);
void       display_close (displayd *d);
gboolean   display_copy_edge_options (displayd *dsp, displayd *dspnew);
void       display_delete_cb (GtkWidget *, GdkEvent *, displayd *);
void       display_free (displayd *, gboolean force, GGobiSession *);
void       display_free_all (GGobiSession *);
void       display_mode_menus_update (ProjectionMode, InteractionMode, displayd *, GGobiSession *);
void       display_menu_build (GGobiSession *);
void       display_menu_init (GGobiSession *);
void       display_new (GGobiSession *, guint action, GtkWidget *widget);
void       display_plot (displayd *display, RedrawStyle type, GGobiSession *);
void       display_print (displayd *d);
void       display_set_current (displayd *, GGobiSession *);
void       display_set_position (windowDisplayd *d, GGobiSession *gg);
GtkUIManager *display_menu_manager_create(displayd *d);
void       show_display_control_panel (displayd *display);
void       display_tailpipe (displayd *, RedrawStyle, GGobiSession *);
void       display_tour1d_init (displayd *dsp, GGobiSession *gg);
void       display_tour1d_init_null (displayd *dsp, GGobiSession *gg);
void       display_tour2d3_init (displayd *, GGobiSession *);
void       display_tour2d3_init_null (displayd *, GGobiSession *);
void       display_tour2d_init (displayd *dsp, GGobiSession *gg);
void       display_tour2d_init_null (displayd *dsp, GGobiSession *gg);
void       display_tourcorr_init (displayd *dsp, GGobiSession *gg);
void       display_tourcorr_init_null (displayd *dsp, GGobiSession *gg);
gboolean   display_type_handles_projection (displayd *, ProjectionMode);
gboolean     display_type_handles_interaction (displayd *, InteractionMode);

void       display_window_init (windowDisplayd *, gint, gint, gint, GGobiSession *);
void       displays_plot (splotd *, RedrawStyle, GGobiSession *);
void       displays_tailpipe (RedrawStyle, GGobiSession *);
void       set_display_options(displayd *display, GGobiSession *gg);
void	     set_display_option(gboolean active, guint action, displayd *display);
void       set_time(GGobiSession *);
void       run_timing_tests(GGobiSession *);
gint       do_ash1d (gfloat *, gint, gint, gint, gfloat *, gfloat *, gfloat *, gfloat *);
void       do_last_increment (vector_f, vector_f, gfloat, gint);
void       draw_3drectangle (GtkWidget *w, GdkDrawable *drawable, gint x, gint y, gint width, gint height, GGobiSession *gg);
void       draw_glyph (GdkDrawable *, glyphd *, icoords *, gint, GGobiSession *);
gint       dsvd (gdouble **a, gint m, gint n, gfloat *w, gdouble **v);

/* pango utils */
void layout_text(PangoLayout *layout, const gchar *text, PangoRectangle *rect);
void underline_text(PangoLayout *layout);

ProjectionMode pmode_get(displayd *, GGobiSession *);
InteractionMode imode_get(GGobiSession *);

gboolean   edge_add (gint, gint, gchar *, gchar *, GGobiStage *, GGobiStage *, GGobiSession *);
gboolean   edge_endpoints_get (gint k, gint *a, gint *b, GGobiStage *d, endpointsd *endpoints, GGobiStage *e);
void       edgeedit_event_handlers_toggle (splotd *, gboolean);
void       edges_free (GGobiStage *, GGobiSession *);
void       edgeset_add_cb (GtkAction *action, GGobiStage *e);
gint       edgesets_count (GGobiSession *gg);
void       eigenvals_get (gfloat *, GGobiStage *);
gint       fcompare (const void *x1, const void *x2);
void       filename_get_r (GGobiSession *);
void       filename_get_w (GtkWidget *, GGobiSession *);
guint      *find_keepers (gint ncols_current, GSList *cols, guint *nkeepers);
gint       find_nearest_edge (splotd *sp, displayd *display, GGobiSession *gg);
gint       find_nearest_point (icoords *, splotd *, GGobiStage *, GGobiSession *);
gint       free_optimize0_p (optimize0_param *op);
gint       free_pp (pp_param *pp);
GList*     g_list_remove_nth (GList *, gint);
GList*     g_list_replace_nth (GList *, gpointer, gint);
gint       getPreviousDisplays(xmlNodePtr node, GGobiDescription *desc);
GtkWidget* get_tree_view_from_notebook (GtkWidget *);
GtkWidget* get_tree_view_from_object (GObject *);
void       get_extended_brush_corners (icoords *, icoords *, GGobiStage *, splotd *);
gint  	   get_one_selection_from_tree_view (GtkWidget *tree_view, GGobiStage *d);
gint*      get_selections_from_tree_view (GtkWidget *, gint *);
void	   select_tree_view_row(GtkWidget *tree_view, gint row);
gint	   tree_selection_get_selected_row(GtkTreeSelection *tree_sel);
void       gg_write_to_statusbar (gchar *message, GGobiSession *gg);
void    ggobi_alloc (GGobiSession *tmp);
gboolean   ggobi_file_set_create (gchar *rootname, GGobiStage *, GGobiSession *);
GGobiSession*    ggobi_get (gint);
gint       ggobi_getIndex(GGobiSession *gg);
void       ggobi_sleep(guint);
void       globals_init (GGobiSession *);
gboolean   gram_schmidt (gdouble *, gdouble*, gint);
void       gt_basis (array_d, gint, vector_i, gint, gint);
gboolean   hasEdgePoints(GGobiStage *e, GGobiStage *d);
void       identify_event_handlers_toggle (splotd *, gboolean);
gchar *    identify_label_fetch (gint k, cpaneld *cpanel, GGobiStage *d, GGobiSession *);
void       identify_link_by_id (gint k, GGobiStage *source_d, GGobiSession *gg);
gboolean   impute_fixed (ImputeType, gfloat val, int nvars, gint *vars, GGobiStage *);
gboolean   impute_mean_or_median (gint, gint, gint *, GGobiStage *);
void       impute_random (GGobiStage *, gint nvars, gint *vars);
void       impute_window_open (GGobiSession *);
gboolean   in_vector (gint k, gint *vec, gint nels);
gint       include_hiddens (gboolean include, GGobiStage *d, GGobiSession *gg);
void       increment_tour(vector_f, vector_f, gfloat, gfloat, gfloat *, gint);
void       init_plot_GC (GdkWindow *, GGobiSession *);
void       init_var_GCs (GtkWidget *, GGobiSession *);
gdouble    inner_prod (gdouble *, gdouble *, gint);
gboolean   isCrossed (gdouble, gdouble, gdouble, gdouble, gdouble, gdouble, gdouble, gdouble);
gfloat     jitter_randval (gint);
void       jitter_value_set (gfloat, GGobiStage *, GGobiSession *);
void       jitter_vars_init (GGobiStage *);
void       jitter_window_open (GGobiSession *);
void       limits_adjust (gfloat *, gfloat *);
void       limits_display_set_by_var (GGobiStage *d, gint j, gboolean);
void       limits_set (GGobiStage *, gboolean);  
void       limits_set_by_var (GGobiStage *, guint, gboolean);  
gint       lines_intersect (glong, glong, glong, glong, glong, glong, glong, glong);
gint       lwidth_from_gsize(gint size);
gint       ltype_from_gtype(gint type);
gint       set_lattribute_from_ltype(gint, GGobiSession *);
void       linkby_current_page_set (displayd *, GtkWidget *w, GGobiSession *);
void       linking_method_set (displayd *, GGobiStage *, GGobiSession *);
void       make_ggobi (GGobiOptions *, gboolean, GGobiSession *);
void       make_symbol_window (GGobiSession *);
void       make_ui (GGobiSession *);
GlyphType  mapGlyphName (const gchar *gtype);
void       missing_arrays_add_cols (GGobiStage *d);
void       missings_datad_cb (GtkWidget *w, GGobiSession *gg);
gboolean   mouseinwindow (splotd *);
void       mousepos_get_motion (GtkWidget *, GdkEventMotion *, gboolean *, gboolean *, splotd *);
void       mousepos_get_pressed (GtkWidget *, GdkEventButton *, gboolean *, gboolean *, splotd *);
void       move_pt (gint id, gint x, gint y, splotd *sp, GGobiStage *d, GGobiSession *);
void       movepts_event_handlers_toggle (splotd *, gboolean);
void       movepts_history_add (gint id, splotd *sp, GGobiStage *, GGobiSession *);
void       movepts_history_delete_last (GGobiStage *, GGobiSession *);
gint       ndatad_with_vars_get (GGobiSession *gg);
void       norm (gdouble *, gint);
GtkWidget* create_menu_bar (GtkUIManager *, const gchar *, GtkWidget *);
void       p1d_event_handlers_toggle (splotd *, gboolean);
void       p1d_reproject (splotd *, greal **, GGobiStage *, GGobiSession *);
gboolean   p1d_varsel (splotd *, gint, gint *, gint, gint);
gint       p1dcycle_func (GGobiSession *gg);
void       pan_by_drag (splotd *, GGobiSession *);
void       parcoords_cpanel_init (cpaneld*, GGobiSession *);
const gchar *parcoords_mode_ui_get(displayd *dsp);
displayd*  parcoords_new_with_vars (gboolean, gint, gint *, GGobiStage *, GGobiSession *);
displayd*  parcoords_new (displayd *dpy, gboolean, gint, gint *, GGobiStage *, GGobiSession *);
void       parcoords_reset_arrangement (displayd *, gint, GGobiSession *);
gboolean   parcoords_varsel (cpaneld *, splotd *, gint, gint *, GGobiSession *);
void       parcoordsDragAndDropEnable(displayd *dsp, gboolean active);
gboolean   pca_calc (GGobiStage *, GGobiSession *);
void       pca_diagnostics_set (GGobiStage *d, GGobiSession *);
gint       pcompare (const void *, const void *);
gint       rank_compare (gconstpointer, gconstpointer, gpointer);
gint       plotted_cols_get (gint *, GGobiStage *, GGobiSession *);
gboolean   point_in_which_bin (gint, gint, gint *, gint *, GGobiStage *, splotd *);
void       populate_combo_box (GtkWidget *, gchar **, gint, GCallback, gpointer);
void       populate_tree_view(GtkWidget *tree_view, gchar **lbl, gint nitems, gboolean headers, GtkSelectionMode mode, GCallback func, gpointer obj);
gboolean   processRestoreFile(const gchar * const fileName, GGobiSession *gg);
void       procs_activate(gboolean state, ProjectionMode pmode, displayd *display, GGobiSession *gg);
gboolean   projection_ok (ProjectionMode m, displayd *display);
void       quick_message (const gchar * const, gboolean);
void       quit_ggobi(GGobiSession *gg);
gdouble    randvalue (void);
void       range_unset (GGobiSession *gg);
gboolean   reached_target(gfloat, gfloat, gint, gfloat *, gfloat *);
gint       realloc_optimize0_p(optimize0_param *, gint, vector_i);
void       recenter_data (gint, GGobiStage *, GGobiSession *);
gboolean   record_add (eeMode, gint a, gint b, gchar *lbl, gchar *id, gchar **vals, GGobiStage * d, GGobiStage * e, GGobiSession *gg);
void       reinit_transient_brushing (displayd *, GGobiSession *);
void       rejitter (gint *, gint, GGobiStage *, GGobiSession *);
void       reset_pp(GGobiStage *, GGobiSession *);
void       rnorm2 (gdouble *, gdouble *);
void       rotation_event_handlers_toggle (splotd *, gboolean);
void       ruler_ranges_set (gboolean force, displayd *, splotd *, GGobiSession *);
void       scale_event_handlers_toggle (splotd *, gboolean);
void       scale_set_default_values (GtkScale *scale);
void	   scale_zoom_reset (displayd *dsp);
void	   scale_pan_reset (displayd *display);
void       scale_update_set (gboolean, displayd *, GGobiSession *);
void       scaling_visual_cues_draw (splotd *, GdkDrawable *, GGobiSession *);
void       scatmat_cpanel_init (cpaneld *, GGobiSession *);
displayd*  scatmat_new (displayd *, gboolean, gint, gint *, gint, gint *, GGobiStage *, GGobiSession *);
gboolean   scatmat_varsel (GtkWidget *, cpaneld *, splotd *, gint, gint, gint *, gint, gboolean, GGobiSession *);
gboolean   scatmat_varsel_simple (cpaneld *, splotd *, gint, gint *, GGobiSession *);
const gchar* scatmat_mode_ui_get(displayd *display);

void       scatterplot_cpanel_init (cpaneld *, ProjectionMode, InteractionMode, GGobiSession *); 

void       scatterplot_display_edge_menu_update (displayd *, GtkAccelGroup *accel_group, GGobiSession *gg);
const gchar * scatterplot_mode_ui_get(displayd *display);		
displayd*  scatterplot_new (gboolean, splotd *sp, GGobiStage *d, GGobiSession *);
void       scatterplot_show_hrule (displayd *, gboolean show);
void       scatterplot_show_rulers (displayd *, gint);
void       scatterplot_show_vrule (displayd *, gboolean show);
gboolean   scree_mapped_p (GGobiSession *);
void       scree_plot_make (GGobiSession *);
gint       selected_cols_get (gint **, GGobiStage *d, GGobiSession *);
void       smooth_window_open (GGobiSession *);
void       sp_event_handlers_toggle (splotd *, gboolean, ProjectionMode, InteractionMode);
void       sp_whiskers_make (splotd *, displayd *, GGobiSession *);
void       special_colors_init (GGobiSession *);
void       speed_set (gfloat, gfloat *, gfloat *);
void       sphere_condnum_set (gfloat x, GGobiSession *);
void       sphere_enable (gboolean sens, GGobiSession *);
void       sphere_free (GGobiStage *);
void       sphere_init (GGobiStage *);
void       sphere_npcs_range_set (gint n, GGobiSession *gg);
void       sphere_npcs_set (gint, GGobiStage *, GGobiSession *);
void       sphere_panel_open (GGobiSession *);
void       sphere_varcovar_set (GGobiStage *, GGobiSession *);
void       sphere_variance_set (gfloat x, GGobiStage *, GGobiSession*);
void       spherevars_set (GGobiSession *);
void       spherize_data (vector_i *svars, vector_i *pcvars, GGobiStage *, GGobiSession *);
gboolean   spherize_set_pcvars (GGobiStage *, GGobiSession *);
void       splash_show (GGobiSession *gg);
void       splot_add_diamond_cue (gint k, splotd *sp, GdkDrawable *drawable, GGobiSession *gg);
void       splot_add_edge_highlight_cue (splotd *, GdkDrawable *, gint k, gboolean nearest, GGobiSession *);
void       splot_add_edge_label (splotd *, GdkDrawable *, gint k, gboolean nearest, GGobiSession *);
void       splot_add_edgeedit_cues (splotd *, GdkDrawable *, gint k, gboolean nearest, GGobiSession *);
void       splot_add_identify_edge_cues (splotd *sp, GdkDrawable *drawable, gint k, gboolean nearest, GGobiSession *gg);
void       splot_add_point_label (gboolean, gint, gboolean, splotd *, GdkDrawable *, GGobiSession *);
void       splot_cursor_set (gint jcursor, splotd *sp);
void       splot_dimension_set(splotd* sp, gint width, gint height);
void       splot_draw_tour_axes(splotd *sp, GdkDrawable *, GGobiSession *);
void       splot_edges_draw (splotd *sp, gboolean hiddens_p, GdkDrawable *drawable, GGobiSession *gg);
void       splot_edges_realloc (gint, splotd *, GGobiStage *);
gboolean   splot_event_handled (GtkWidget *, GdkEventKey *, cpaneld *, splotd *, GGobiSession *);
void       splot_expose (splotd *);
void       splot_free (splotd *, displayd *, GGobiSession *);
void       splot_get_dimensions (splotd *, gint *, gint *);
splotd*    splot_new (displayd *, gint, gint, GGobiSession *);
void       splot_pixmap0_to_pixmap1 (splotd *, gboolean, GGobiSession *);
void       splot_plane_to_screen (displayd *, cpaneld *, splotd *, GGobiSession *);
gboolean   splot_plot_case (gint m, GGobiStage *, splotd *, displayd *, GGobiSession *);
void       splot_points_realloc (gint, splotd *, GGobiStage *);
void       splot_redraw (splotd *sp, RedrawStyle, GGobiSession *);
void       splot_set_current (splotd *, gboolean, GGobiSession *);
void       splot_world_to_plane (cpaneld *, splotd *, GGobiSession *);
void       splot_zoom (splotd *sp, gfloat xsc, gfloat ysc) ;
gint       sqdist (gint, gint, gint, gint);
void       statusbar_show (gboolean show, GGobiSession *gg);
void       sticky_id_link_by_id (gint, gint, GGobiStage *, GGobiSession *);
void       sticky_id_toggle (GGobiStage *, GGobiSession *);
void       submenu_destroy (GtkWidget *);
void       submenu_insert (GtkWidget *, GtkWidget *, gint);
GtkWidget* submenu_make (gchar *, guint, GtkAccelGroup *);
void       subset_window_open (GGobiSession *);
gboolean brush_all_matching_id (GGobiStage * sd, gint k, gboolean condition, BrushTargetType brush_mode, GGobiAttrSetMethod brush);
gint       symbol_table_populate (GGobiStage *d);
void       symbol_window_redraw (GGobiSession *);
void       t1d_clear_ppda (displayd *, GGobiSession *);
void       t1d_optimz (gint, gboolean *, gint *, displayd *);
void       t1d_pp_reinit(displayd *, GGobiSession *);
void       t1d_ppcool_set(gfloat, displayd *, GGobiSession *);
void       t1d_ppdraw(gfloat, displayd *, GGobiSession *);
void       t1d_pptemp_set(gfloat, displayd *, GGobiSession *);
void       t2d_clear_ppda (displayd *, GGobiSession *);
void       t2d_optimz (gint, gboolean *, gint *, displayd *);
void       t2d_pp_reinit(displayd *, GGobiSession *);
void       t2d_ppcool_set(gfloat, displayd *, GGobiSession *);
void       t2d_ppdraw (gfloat, displayd *, GGobiSession *);
void       t2d_pptemp_set(gfloat, displayd *, GGobiSession *);
void       textur (gfloat *, gfloat *, gint, gint, gfloat, gint, GGobiSession *);
void       tform_to_world (GGobiStage *);
void       tform_to_world_by_var (GGobiStage *, guint j);
void       tooltips_show (gboolean show, GGobiSession *gg);
void       tour1d_all_vars (displayd *);
void       tour1d_do_step (displayd *,GGobiSession *);
void       tour1d_event_handlers_toggle (splotd *, gboolean);
void       tour1d_fade_vars (gboolean, GGobiSession *);
void       tour1d_func (gboolean, displayd *, GGobiSession *);
void       tour1d_io_cb (GtkWidget *w, gpointer *cbd);
void       tour1d_manip (gint, gint, splotd *, GGobiSession *);
void       tour1d_manip_end (splotd *);
void       tour1d_manip_init (gint, gint, splotd *);
void       tour1d_pause (cpaneld *, gboolean, displayd *, GGobiSession *);
void       tour1d_projdata (splotd *, greal **, GGobiStage *, GGobiSession *);
void       tour1d_realloc_down (GSList *cols, GGobiStage *d, GGobiSession *gg);
void       tour1d_reinit (GGobiSession *);
void       tour1d_scramble(GGobiSession *);
void       tour1d_snap(GGobiSession *);
void       tour1d_video(GGobiSession *);
void       tour1d_speed_set (gfloat, GGobiSession *);
gboolean   tour1d_varsel (GtkWidget *, gint jvar, gint toggle, gint btn, GGobiStage *, GGobiSession *);
void       tour1d_vert (cpaneld *, gboolean);
void       tour1dpp_window_open (GGobiSession *);
void       tour2d3_event_handlers_toggle (splotd *, gboolean state);
void       tour2d3_func (gboolean state, displayd *, GGobiSession *);
void       tour2d3_manip (gint, gint, splotd *, GGobiSession *);
void       tour2d3_manip_end (splotd *);
void       tour2d3_manip_init (gint, gint, splotd *);
void       tour2d3_pause (cpaneld *, gint, GGobiSession *);
void       tour2d3_projdata (splotd *, greal **world_data, GGobiStage *, GGobiSession *);
void       tour2d3_reinit (GGobiSession *);
void       tour2d3_scramble (GGobiSession *);
void       tour2d3_speed_set (gfloat, GGobiSession *);
gboolean   tour2d3_varsel (GtkWidget *, gint jvar, gint toggle, gint btn, GGobiStage *, GGobiSession *);
void       tour2d_all_vars (displayd *);
void       tour2d_do_step (displayd *,GGobiSession *);
void       tour2d_event_handlers_toggle (splotd *, gboolean);
void       tour2d_fade_vars (gboolean, GGobiSession *);
void       tour2d_func (gboolean, displayd *, GGobiSession *);
void       tour2d_io_cb (GtkWidget *w, gpointer *cbd);
void       tour2d_manip (gint, gint, splotd *, GGobiSession *);
void       tour2d_manip_end (splotd *);
void       tour2d_manip_init (gint, gint, splotd *);
void       tour2d_pause (cpaneld *, gboolean, displayd *, GGobiSession *);
void       tour2d_projdata (splotd *, greal **, GGobiStage *, GGobiSession *);
void       tour2d_realloc_down (GSList *cols, GGobiStage *d, GGobiSession *gg);
void       tour2d_reinit (GGobiSession *);
void       tour2d_scramble (GGobiSession *);
void       tour2d_snap(GGobiSession *);
void       tour2d_video(GGobiSession *);
void       tour2d_speed_set (gfloat, GGobiSession *);
gboolean   tour2d_varsel (GtkWidget *, gint jvar, gint toggle, gint btn, GGobiStage *, GGobiSession *);
void       tour2dpp_window_open (GGobiSession *);
gint       tour_path (array_d, array_d, array_d, gint, gint, array_d, array_d, array_d, vector_f, array_d, array_d, array_d, vector_f, vector_f, gfloat *, gfloat *);
void tour_realloc_up (GGobiStage *d, gint nc);
void       tour_reproject (vector_f, array_d, array_d, array_d, array_d, array_d, gint, gint);
void       tourcorr_fade_vars (gboolean, GGobiSession *);
void       tourcorr_func (gboolean, displayd *, GGobiSession *);
void       tourcorr_io_cb (GtkWidget *w, gpointer *cbd);
void       tourcorr_manip (gint, gint, splotd *, GGobiSession *);
void       tourcorr_manip_end (splotd *);
void       tourcorr_manip_init (gint, gint, splotd *);
void       tourcorr_pause (cpaneld *, gboolean, GGobiSession *);
void       tourcorr_projdata (splotd *, greal **, GGobiStage *, GGobiSession *);
void       tourcorr_realloc_down (GSList *cols, GGobiStage *d, GGobiSession *gg);
void       tourcorr_reinit (GGobiSession *);
void       tourcorr_scramble (GGobiSession *);
void       tourcorr_snap(GGobiSession *);
void       tourcorr_video(GGobiSession *);
void       tourcorr_speed_set (gfloat, GGobiSession *);
gboolean   tourcorr_varsel (GtkWidget *, gint jvar, gint toggle, gint btn, GGobiStage *, GGobiSession *);
void       transform (gint, gint, gfloat, gint *, gint, GGobiStage *, GGobiSession *);
void       transform0_combo_box_set_value (gint, gboolean, GGobiStage *, GGobiSession *);
void       transform0_values_set (gint, gint, GGobiStage *, GGobiSession *);
gboolean   transform1_apply (gint, GGobiStage *, GGobiSession *);
void       transform1_combo_box_set_value (gint, gboolean, GGobiStage *, GGobiSession *);
void       transform1_values_set (gint, gfloat, gint, GGobiStage *, GGobiSession *);
gboolean   transform2_apply (gint, GGobiStage *, GGobiSession *);
void       transform2_combo_box_set_value (gint, gboolean, GGobiStage *, GGobiSession *);
void       transform2_values_set (gint, gint, GGobiStage *, GGobiSession *);
gboolean   transform_values_compare (gint, gint, GGobiStage *);
void       transform_values_copy (gint jfrom, gint jto, GGobiStage *d);
void       transform_values_init (GGobiVariable *var);
gboolean   transform_variable (gint, gint, gfloat, gint, GGobiStage *, GGobiSession *);
void       varcircle_label_set (GGobiStage *s, gint jvar);
void       varcircles_add (gint ncols, GGobiStage *, GGobiSession *);
void       varcircles_cursor_set_default (GGobiStage *d);
void       varcircles_delete_nth (GGobiStage *, guint j);
void       varcircles_populate (GGobiStage *, GGobiSession *);
void       varcircles_refresh (GGobiStage *, GGobiSession *);
void       varcircles_show (gboolean, GGobiStage *, displayd *, GGobiSession *);
void       varcircles_visibility_set (displayd *display, GGobiSession *gg);
void       variable_notebook_handlers_disconnect (GtkWidget *notebook, GGobiSession *gg);
void       variable_notebook_list_changed_cb(GGobiSession *gg, GGobiStage *d, void *notebook);
void       variable_notebook_subwindow_add (GGobiStage *d, GtkSignalFunc func, gpointer func_data, GtkWidget *notebook, GGobiVariableType, datatyped, const gchar*, GGobiSession *gg);
void       variable_notebook_varchange_cb (GGobiSession *gg, gint which, GGobiStage *, void *notebook);
void       varpanel_clear (GGobiStage *, GGobiSession *);
void       varpanel_delete_nth (GGobiStage *d, gint jvar);
void       varpanel_label_set (GGobiStage *, gint);
void       varpanel_make (GtkWidget *, GGobiSession *);
void       varpanel_populate (GGobiStage *, GGobiSession *);
void       varpanel_refresh (displayd *, GGobiSession *);
void       varpanel_reinit (GGobiSession *gg);
void       varpanel_set_sensitive (GGobiStage *d, gboolean sensitive_p, GGobiSession *);
void       varpanel_show_page (displayd*, GGobiSession*);
void       varpanel_tooltips_set (displayd *, GGobiSession *);
GtkWidget* varpanel_widget_get_nth (gint jbutton, gint jvar, GGobiStage *d);
void       varpanel_widgets_add (gint nc, GGobiStage *d, GGobiSession *gg);
void       vars_stdized_send_event (GGobiStage *d, GGobiSession *gg);
void       varsel (GtkWidget *w, cpaneld *, splotd *, gint jvar, gint toggle, gint btn, gint alt_mod, gint ctrl_mod, gint shift_mod, GGobiStage *, GGobiSession *);
void       vartable_alloc (GGobiStage *);
gint       vartable_index_get_by_name(gchar *name, GGobiStage *d);
gboolean   vartable_iter_from_varno(gint var, GGobiStage *d, GtkTreeModel **model, GtkTreeIter *iter);
gint	   vartable_varno_from_path(GtkTreeModel *model, GtkTreePath *path);
void       vartable_cells_set_by_var (gint j, GGobiStage *d);
GtkWidget*  vartable_tree_view_get (GGobiSession *gg);
void       vartable_collab_set_by_var (GGobiStage *, guint);
void       vartable_collab_tform_set_by_var (GGobiStage *s, guint j);
GGobiVariable *vartable_copy_var (GGobiVariable *var, GGobiVariable *var_to);
GGobiVariable* vartable_element_new (GGobiStage *d);
void       vartable_element_remove (gint, GGobiStage *);
void       vartable_init (GGobiStage *d);
void       vartable_limits_set (GGobiStage *);
void       vartable_limits_set_by_var (GGobiStage *s, guint j);
void       vartable_open (GGobiSession *);
void       vartable_row_append (gint j, GGobiStage *);
void       vartable_show_page (GGobiStage*, GGobiSession*);
void       vartable_stats_set (GGobiStage *);
void       vartable_stats_set_by_var (GGobiStage *, guint j);
void       vectorb_alloc (vector_b *, gint);
void       vectorb_alloc_zero (vector_b *, gint);
void       vectorb_copy (vector_b *, vector_b *);
void       vectorb_delete_els (vector_b *vecp, GSList *els);
void       vectorb_free (vector_b *);
void       vectorb_init_null (vector_b *);
void       vectorb_realloc (vector_b *, gint);
void       vectorb_realloc_zero (vector_b *, gint);
void       vectorb_zero (vector_b *vecp);
void       vectord_alloc (vector_d *, gint);
void       vectord_alloc_zero (vector_d *, gint);
void       vectord_delete_els (vector_d *vecp, GSList *els);
void       vectord_free (vector_d *);
void       vectord_init_null (vector_d *);
void       vectord_realloc (vector_d *, gint);
void       vectord_zero (vector_d *vecp);
void       vectorf_alloc (vector_f *, gint);
void       vectorf_alloc_zero (vector_f *, gint);
void       vectorf_delete_els (vector_f *vecp, GSList *els);
void       vectorf_free (vector_f *);
void       vectorf_init_null (vector_f *);
void       vectorf_realloc (vector_f *, gint);
void       vectorf_zero (vector_f *vecp);
void       vectorg_alloc (vector_g *, gint);
void       vectorg_copy (vector_g *, vector_g *);
void       vectorg_free (vector_g *);
void       vectorg_init_null (vector_g *);
void       vectorg_realloc (vector_g *, gint);
void       vectorg_realloc_zero (vector_g *, gint);
void       vectori_alloc (vector_i *, gint);
void       vectori_alloc_zero (vector_i *, gint);
void       vectori_copy (vector_i *, vector_i *);
void       vectori_delete_els (vector_i *vecp, GSList *els);
void       vectori_free (vector_i *);
void       vectori_init_null (vector_i *);
void       vectori_realloc (vector_i *, gint);
void       vectori_zero (vector_i *vecp);
void       vectors_copy (vector_s *, vector_s *);
void       vectors_free (vector_s *);
void       vectors_init_null (vector_s *);
void       vectors_realloc (vector_s *, gint);
void       vectors_realloc_zero (vector_s *, gint);
GtkWidget* widget_find_by_name (GtkWidget *, gchar *);
void       widget_initialize (GtkWidget *w, gboolean initd);
gboolean   widget_initialized (GtkWidget *w);
void       writeall_window_open (GGobiSession *);
void       svis_init (GGobiSession *gg);
void       svis_window_open (GGobiSession *gg);
gboolean   write_csv(const gchar *, GGobiSession *);
void       wvis_init (GGobiSession *gg);
void       wvis_window_open (GGobiSession *gg);
void       xy_reproject (splotd *, greal **, GGobiStage *, GGobiSession *);
gint       xycycle_func (GGobiSession *gg);
void       xyplot_cycle_activate (gboolean state, cpaneld *cpanel, GGobiSession *gg);
void       xyplot_event_handlers_toggle (splotd *, gboolean);
gboolean   xyplot_varsel (splotd *, gint, gint *, gint, gint);
void       zoom_by_drag (splotd *, GGobiSession *);

/*tsplot functions*/
GtkWidget* cpanel_tsplot_make (GGobiSession *);
void      cpanel_tsplot_set (displayd *, cpaneld *, GtkWidget *, GGobiSession *);
void      tsplot_cpanel_init (cpaneld*, GGobiSession *);

const gchar* tsplot_mode_ui_get(displayd *display);
displayd* tsplot_new (displayd *, gboolean, gint, gint *, GGobiStage *, GGobiSession *);
displayd *tsplot_new_with_vars (gboolean missing_p, gint nvars, gint *vars, GGobiStage *d, GGobiSession *gg) ;
void      tsplot_reset_arrangement (displayd *, gint, GGobiSession *);
gboolean  tsplot_varsel (GtkWidget *, displayd *display, splotd *sp, gint jvar, gint toggle, gint mouse, cpaneld *cpanel,  GGobiSession *gg);
void      tsplot_whiskers_make (splotd *, displayd *, GGobiSession *);
void      zero_tau(vector_f, gint);

/* The new way of handling window closures, so that we don't just exit. */
gboolean  ggobi_close (GGobiSession *gg);
gint      ggobi_remove_by_index (GGobiSession *gg, gint which);
gint      ggobi_remove (GGobiSession *);
void      subset_init (GGobiStage *d, GGobiSession *gg);


displayd *createBarchart(displayd *display, gboolean missing_p, splotd * sp, gint var, GGobiStage * d,  GGobiSession * gg);
void      barchart_scaling_visual_cues_draw (splotd *sp, GdkDrawable *drawable, GGobiSession *gg);
gboolean  barchart_active_paint_points (splotd *sp, GGobiStage *, GGobiSession *); 
void      barchart_add_bar_cues (splotd *sp, GdkDrawable *drawable, GGobiSession *gg);
void      barchart_clean_init (barchartSPlotd *sp);
void      barchart_cpanel_init (cpaneld *, GGobiSession *);
void      barchart_event_handlers_toggle (displayd *, splotd *, gboolean state, ProjectionMode, InteractionMode);
void      barchart_free_structure (barchartSPlotd *sp);
gboolean  barchart_identify_bars (icoords mousepos, splotd *sp, GGobiStage *d, GGobiSession *gg);
void      barchart_init_vectors(barchartSPlotd *sp);
const gchar *barchart_mode_ui_get(displayd *display);
displayd *barchart_new (gboolean missing_p, splotd *sp, GGobiStage *d, GGobiSession *gg);
void      barchart_recalc_counts (barchartSPlotd *sp, GGobiStage *d, GGobiSession *gg);
void      barchart_recalc_dimensions (splotd *sp, GGobiStage *d, GGobiSession *gg);
void      barchart_recalc_group_dimensions (barchartSPlotd *sp, GGobiSession *gg);
gboolean  barchart_redraw (splotd *sp, GGobiStage *d, GGobiSession *gg, gboolean binned);
void      barchart_splot_add_plot_labels (splotd *, GdkDrawable *, GGobiSession *);
GtkWidget* cpanel_barchart_make (GGobiSession *gg);
void      cpanel_barchart_set (displayd *, cpaneld *, GtkWidget *panel, GGobiSession *gg);

void      barchart_scale_event_handlers_toggle(splotd *sp, gboolean state);

displayd *barchart_new_with_vars(gboolean missing_p, gint nvars, gint *vars, GGobiStage * d, GGobiSession * gg);

#ifdef WIN32
void      win32_draw_to_pixmap_binned (icoords *, icoords *, gint, splotd *, gboolean draw_hidden, GGobiSession *gg);
void      win32_draw_to_pixmap_unbinned (gint, splotd *, gboolean draw_hidden, GGobiSession *gg);
void      win32_drawing_arrays_free (splotd *sp);
#endif

#ifdef __cplusplus
}
#endif

#include "GGobiAPI.h"

#define CHECK_GG(a) ValidateGGobiRef(a, true)

/* Made externs for access from display class methods. */
void       varpanel_toggle_set_active (gint jbutton, gint jvar, gboolean active, GGobiStage *d);
GtkWidget *varpanel_widget_set_visible (gint jbutton, gint jvar, gboolean show, GGobiStage *d);

void       display_plot (displayd *display, RedrawStyle type, GGobiSession *gg);
void       scatterXYAddPlotLabels(splotd *sp, GdkDrawable *drawable, GdkGC *gc);
void       scatter1DAddPlotLabels(splotd *sp, GdkDrawable *drawable, GdkGC *gc);
gboolean   processRestoreFile(const gchar * const fileName, GGobiSession *gg);
void       scatterplotMovePointsMotionCb(displayd *display, splotd *sp, GtkWidget *w, GdkEventMotion *event, GGobiSession *gg);
void       scatterplotMovePointsButtonCb(displayd *display, splotd *sp, GtkWidget *w, GdkEventButton *event, GGobiSession *gg);
displayd * scatterplot_new_with_vars(gboolean missing_p, gint numVars, gint *vars, GGobiStage *d, GGobiSession *gg);

GGobiOptions *GGOBI_getSessionOptions();

#ifdef WIN32
/* needed by transform.c */
extern double erf(double x);
extern double erfc(double x);
#endif

void resetDataMode();

gboolean parcoords_add_delete_splot(cpaneld *cpanel, splotd *sp, gint jvar, gint *jvar_prev, GGobiSession *gg, displayd *display);

#ifdef ENABLE_CAIRO
cairo_t*   create_cairo_glitz(GdkDrawable *drawable);
#endif

#endif

void
pt_world_to_raw_by_var (gint j, greal * world, greal * raw, GGobiStage * d);

void
pt_screen_to_raw (icoords * screen, gint id, gboolean horiz, gboolean vert,
                  greal * raw, gcoords * eps, GGobiStage * d, splotd * sp,
                  GGobiSession * gg);
void
pt_plane_to_world (splotd * sp, gcoords * planar, gcoords * eps,
                   greal * world);
void
pt_screen_to_plane (icoords * screen, gint id, gboolean horiz, gboolean vert,
                   gcoords * eps, gcoords * planar, splotd * sp);
endpointsd *
resolveEdgePoints (GGobiStage * e, GGobiStage * d);
void
unresolveAllEdgePoints (GGobiStage * e);
void
edges_alloc (gint nsegs, GGobiStage * d);
