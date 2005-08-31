/* tsPlot.c */
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
 *
 * Contributing author of time series code:  Nicholas Lewin-Koh
*/


#include "ggobi.h"
#include "tsdisplay.h"

#include <string.h>
#include "externs.h"


static void tsWorldToPlane(splotd *sp, datad *d, ggobid *gg);
static void tsWithinPlaneToScreen(splotd *sp, displayd *display, datad *d, ggobid *gg);
static gboolean tsDrawEdge_p(splotd *sp, gint m, datad *d, datad *e, ggobid *gg);
static gboolean tsDrawCase_p(splotd *sp, gint m, datad *d, ggobid *gg);
static void tsAddPlotLabels(splotd *sp, GdkDrawable *drawable, ggobid *gg) ;
static void tsWithinDrawBinned(splotd *sp, gint m, GdkDrawable *drawable, GdkGC *gc);
static void tsShowWhiskers(splotd *sp, gint m, GdkDrawable *drawable, GdkGC *gc);
static GdkSegment * tsAllocWhiskers(GdkSegment *, splotd *sp, gint nrows, datad *d);
static gchar *tsTreeLabel(splotd *sp, datad *d, ggobid *gg);



void
tsWorldToPlane(splotd *sp,  datad *d, ggobid *gg)
{
      xy_reproject (sp, d->world.vals, d, gg);
}

splotd *
gtk_time_series_splot_new(displayd *dpy, gint width, gint height, ggobid *gg)
{
  timeSeriesSPlotd *bsp;
  splotd *sp;

  bsp = gtk_type_new(GTK_TYPE_GGOBI_TS_SPLOT);
  sp = GTK_GGOBI_SPLOT(bsp);


  splot_init(sp, dpy, width, height, gg);

  return(sp);
}

void
tsDestroy(splotd *sp)
{
      g_free ((gpointer) sp->whiskers);
}

void
tsWithinPlaneToScreen(splotd *sp, displayd *display, datad *d, ggobid *gg)
{
      tsplot_whiskers_make (sp, display, gg);
}

gboolean
tsDrawEdge_p(splotd *sp, gint m, datad *d, datad *e, ggobid *gg)
{
   gboolean draw_edge = true;

   draw_edge = !(e->missing.vals[m][sp->xyvars.y] ||  e->missing.vals[m][sp->xyvars.x]);
   return(draw_edge);
}

gboolean
tsDrawCase_p(splotd *sp, gint m, datad *d, ggobid *gg)
{
  return !(d->missing.vals[m][sp->xyvars.y] || d->missing.vals[m][sp->xyvars.x]);
}

void
tsAddPlotLabels(splotd *sp, GdkDrawable *drawable, ggobid *gg) 
{
    displayd *display = sp->displayptr;
    GList *l = display->splots;
    gint lbearing, rbearing, width, ascent, descent;
    GtkStyle *style = gtk_widget_get_style (sp->da);
    vartabled *vtx, *vty;

    if (l->data == sp) {
      vtx = vartable_element_get (sp->xyvars.x, display->d);
      gdk_text_extents (
#if GTK_MAJOR_VERSION == 2
        gtk_style_get_font (style),
#else
        style->font,
#endif
        vtx->collab_tform, strlen (vtx->collab_tform),
        &lbearing, &rbearing, &width, &ascent, &descent);
      gdk_draw_string (drawable,
#if GTK_MAJOR_VERSION == 2
        gtk_style_get_font (style),
#else
        style->font,
#endif
        gg->plot_GC,
        sp->max.x - width - 5,
        sp->max.y - 5,
        vtx->collab_tform);
    }
    vty = vartable_element_get (sp->xyvars.y, display->d);
    gdk_text_extents (
#if GTK_MAJOR_VERSION == 2
      gtk_style_get_font (style),
#else
      style->font,
#endif
      vty->collab_tform, strlen (vty->collab_tform),
      &lbearing, &rbearing, &width, &ascent, &descent);
    gdk_draw_string (drawable,
#if GTK_MAJOR_VERSION == 2
      gtk_style_get_font (style),
#else
      style->font,
#endif
      gg->plot_GC,
      5, 5 + ascent + descent,
      vty->collab_tform);
}

void
tsWithinDrawBinned(splotd *sp, gint m, GdkDrawable *drawable, GdkGC *gc)
{
  gdk_draw_line (drawable, gc,
    sp->whiskers[m].x1, sp->whiskers[m].y1,
    sp->whiskers[m].x2, sp->whiskers[m].y2);
}


void
tsShowWhiskers(splotd *sp, gint m, GdkDrawable *drawable, GdkGC *gc)
{
  displayd *dpy = sp->displayptr;
     /*-- there are n-1 whiskers --*/
  if (dpy->options.whiskers_show_p && m < dpy->d->nrows_in_plot-1) 
     gdk_draw_line (drawable, gc,
       sp->whiskers[m].x1, sp->whiskers[m].y1,
       sp->whiskers[m].x2, sp->whiskers[m].y2);
}


GdkSegment * 
tsAllocWhiskers(GdkSegment *whiskers, splotd *sp, gint nrows, datad *d)
{
  return((GdkSegment *) g_realloc (whiskers, (nrows-1) * sizeof (GdkSegment)));
}

gchar *
tsTreeLabel(splotd *sp, datad *d, ggobid *gg)
{
  vartabled *vty;
  int n;
  char *buf;

    vty = vartable_element_get (sp->xyvars.y, d);
    n = strlen (vty->collab);
    buf = (gchar*) g_malloc(n* sizeof (gchar*));
   sprintf(buf, "%s", vty->collab);

   return(buf);
}


static gint
splotVariablesGet(splotd *sp, gint *cols, datad *d)
{
	cols[0] = sp->xyvars.x;
	cols[1] = sp->xyvars.y;
	return(2);
}

/*
XX Incomplete. Need to finish off the construction of splotd's directly
 from command line language rather than as part of the displayd.
*/
splotd *
tsplotCreateWithVars(displayd *display, gint *vars, gint nvar, gint width, gint height, ggobid *gg)
{
   splotd *sp;
   if(nvar < 1) {
      g_printerr("not enough variables specified to create time series plot\n");
      return(NULL);
   }

   sp = gtk_time_series_splot_new(display, width, height, gg);
   if(nvar > 1) {
      sp->xyvars.y = vars[1];
      sp->xyvars.x = vars[0];
   } else {
      sp->xyvars.y = vars[0];
      sp->xyvars.x = 0;
   }

   return(sp);
}

void 
timeSeriesSPlotClassInit(GtkGGobiTimeSeriesSPlotClass *klass)
{
    klass->extendedSPlotClass.splot.redraw = QUICK;
    klass->extendedSPlotClass.tree_label = tsTreeLabel;

    klass->extendedSPlotClass.within_draw_to_binned = tsWithinDrawBinned;
    klass->extendedSPlotClass.within_draw_to_unbinned = tsShowWhiskers;

    klass->extendedSPlotClass.draw_edge_p = tsDrawEdge_p;
    klass->extendedSPlotClass.draw_case_p = tsDrawCase_p;

    klass->extendedSPlotClass.add_plot_labels = tsAddPlotLabels;

    klass->extendedSPlotClass.sub_plane_to_screen = tsWithinPlaneToScreen;
    klass->extendedSPlotClass.alloc_whiskers = tsAllocWhiskers;

    klass->extendedSPlotClass.world_to_plane = tsWorldToPlane;

    klass->extendedSPlotClass.plotted_vars_get = splotVariablesGet;

    klass->extendedSPlotClass.createWithVars = tsplotCreateWithVars;
}


void 
timeSeriesClassInit(GtkGGobiTimeSeriesDisplayClass *klass)
{
    klass->parent_class.binning_ok = false;

    klass->parent_class.treeLabel =  klass->parent_class.titleLabel = "Time Series";
    klass->parent_class.create = timeSeriesDisplayCreate;
    klass->parent_class.createWithVars = tsplot_new_with_vars;
    klass->parent_class.variable_select = tsplot_varsel;
    klass->parent_class.variable_plotted_p = tsplotIsVarPlotted;
    klass->parent_class.cpanel_set = tsplotCPanelSet;
    klass->parent_class.display_unset = NULL;
    klass->parent_class.display_set = tsplotDisplaySet;
    klass->parent_class.varpanel_refresh = tsplotVarpanelRefresh;

    klass->parent_class.handles_interaction = tsplotHandlesInteraction;

    klass->parent_class.xml_describe = add_xml_tsplot_variables;

    klass->parent_class.varpanel_tooltips_set = tsplotVarpanelTooltipsSet;
    klass->parent_class.plotted_vars_get = tsplotPlottedColsGet;


    klass->parent_class.imode_control_box = tsplotCPanelWidget;
    klass->parent_class.menus_make = tsplotMenusMake;

    klass->parent_class.event_handlers_toggle = tsplotEventHandlersToggle;

    klass->parent_class.splot_key_event_handled = tsplotKeyEventHandled;


    klass->parent_class.add_plot_labels = NULL; 
}
