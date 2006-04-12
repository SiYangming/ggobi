/* identify.c */
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

gint
find_nearest_point (icoords * lcursor_pos, splotd * splot, GGobiData * d,
                    ggobid * gg)
{
/*
 * Returns index of nearest un-hidden point
*/
  gint i, k, sqdist, near, xdist, ydist, npoint;

  npoint = -1;
  near = 20 * 20;               /* If nothing is close, don't show any label */

  for (i = 0; i < d->nrows_in_plot; i++) {
    if (!ggobi_data_get_attr_hidden(d, k = d->rows_in_plot.els[i])) {
      xdist = splot->screen[k].x - lcursor_pos->x;
      ydist = splot->screen[k].y - lcursor_pos->y;
      sqdist = xdist * xdist + ydist * ydist;
      if (sqdist < near) {
        near = sqdist;
        npoint = k;
      }
    }
  }
  return (npoint);
}

/*-- still having trouble getting identify turned off properly --*/
RedrawStyle
identify_activate (gint state, displayd * display, ggobid * gg)
{
  RedrawStyle redraw_style = NONE;
  GGobiData *d = display->d;

/* At the moment, do the same thing whether identify is turning on or off */
  if (state == on || state == off) {
    if (d->nearest_point != -1)
      redraw_style = QUICK;
    d->nearest_point = -1;
  }

  return redraw_style;
}

void
sticky_id_toggle (GGobiData * d, ggobid * gg)
{
  gint i = 0;
  gboolean i_in_list = false;
  gpointer ptr = NULL;

  if (d->nearest_point != -1) {

    if (d->sticky_ids && g_slist_length (d->sticky_ids) > 0) {
      GSList *l;
      for (l = d->sticky_ids; l; l = l->next) {
        i = GPOINTER_TO_INT (l->data);
        if (i == d->nearest_point) {
          i_in_list = true;
          ptr = l->data;
          break;
        }
      }
    }

    if (i_in_list) {
      d->sticky_ids = g_slist_remove (d->sticky_ids, ptr);
      sticky_id_link_by_id (STICKY_REMOVE, d->nearest_point, d, gg);
      /* This will become an event on the datad when we move to
         Gtk objects (soon now!) */
      g_signal_emit (G_OBJECT (gg),
                     GGobiSignals[STICKY_POINT_REMOVED_SIGNAL], 0,
                     d->nearest_point, (gint) STICKY_REMOVE, d);
    }
    else {
      ptr = GINT_TO_POINTER (d->nearest_point);
      d->sticky_ids = g_slist_append (d->sticky_ids, ptr);
      sticky_id_link_by_id (STICKY_ADD, d->nearest_point, d, gg);
      /* This will become an event on the datad when we move to
         Gtk objects (soon now!) */
      g_signal_emit (G_OBJECT (gg),
                     GGobiSignals[STICKY_POINT_ADDED_SIGNAL], 0,
                     d->nearest_point, (gint) STICKY_ADD, d);
    }
  }
}

/*----------------------------------------------------------------------*/
/*                Linking to other datad's by id                        */
/*----------------------------------------------------------------------*/

void
identify_link_by_id (gint k, GGobiData * source_d, ggobid * gg)
{
  GGobiData *d;
  GSList *l;
  gboolean inrange;

  /*-- k is the row number in source_d --*/

  if (k < 0) {  /*-- handle this case separately --*/
    for (l = gg->d; l; l = l->next) {
      d = (GGobiData *) l->data;
      if (d != source_d)
        d->nearest_point_prev = d->nearest_point = -1;
    }
    return;
  }

  for (l = gg->d; l; l = l->next) {
    d = (GGobiData *) l->data;
    inrange = false;

    if (d == source_d)
      continue;        /*-- skip the originating datad --*/

    guint id = ggobi_data_get_row_for_id(d, source_d->rowIds[k]);
    if (id != -1) {
      inrange = true;
      d->nearest_point_prev = d->nearest_point;
      d->nearest_point = id;
    }

    if (!inrange) {
      d->nearest_point_prev = d->nearest_point;
      d->nearest_point = -1;
    }
  }
  return;
}

void
sticky_id_link_by_id (gint whattodo, gint k, GGobiData * source_d,
                      ggobid * gg)
{
  GGobiData *d;
  GSList *l;
  gint i, n, id = -1;
  gboolean i_in_list = false;
  GSList *ll;
  gpointer ptr = NULL;

  /*-- k is the row number in source_d --*/
  id = ggobi_data_get_row_for_id(source_d, source_d->rowIds[k]);

  for (l = gg->d; l; l = l->next) {
    d = (GGobiData *) l->data;
    if (d == source_d)
      continue;        /*-- skip the originating datad --*/

    i = ggobi_data_get_row_for_id(d, source_d->rowIds[k]);

    if (i < 0)          /*-- then no cases in d have this id --*/
      continue;

    if (g_slist_length (d->sticky_ids) > 0) {
      for (ll = d->sticky_ids; ll; ll = ll->next) {
        n = GPOINTER_TO_INT (ll->data);
        if (n == i) {  /*-- the row number of the id --*/
          i_in_list = true;
          ptr = ll->data;
          break;
        }
      }
    }

    if (i_in_list && whattodo == STICKY_REMOVE) {
      d->sticky_ids = g_slist_remove (d->sticky_ids, ptr);
    }
    else if (!i_in_list && whattodo == STICKY_ADD) {
      ptr = GINT_TO_POINTER (i);
      d->sticky_ids = g_slist_append (d->sticky_ids, ptr);
    }
  }
}

/*----------------------------------------------------------------------*/
/*                Called from sp_plot.c                                 */
/*----------------------------------------------------------------------*/

gchar *
identify_label_fetch (gint k, cpaneld * cpanel, GGobiData * d, ggobid * gg)
{
  gchar *lbl = NULL;
  GList *labels = NULL, *l;
  gint id_display_type = cpanel->id_display_type;

/*
 * How can I tell if the current page of the notebook
 * corresponds to the data?
*/
  if (id_display_type & ID_VAR_LABELS) {
    GtkWidget *pnl =
      mode_panel_get_by_name (ggobi_getIModeName (IDENT), gg);
    GtkWidget *tree_view;
    GGobiData *tree_view_d;

    tree_view = get_tree_view_from_object (G_OBJECT (pnl));
    tree_view_d =
      (GGobiData *) g_object_get_data (G_OBJECT (tree_view), "datad");

    if (tree_view_d != d) {
      id_display_type = ID_RECORD_LABEL;
      /*-- this will be caught below --*/
    }
    else {
      gint *vars;               // = (gint *) g_malloc (d->ncols * sizeof(gint));
      gint j, nvars;
      gchar *colname = NULL, *value = NULL;
      
      vars = get_selections_from_tree_view (tree_view, &nvars);

      for (j = 0; j < nvars; j++) {
        if (vars[j] < 0) continue;

        value = ggobi_data_get_string_value(d, k, vars[j], TRUE);
        colname = ggobi_data_get_transformed_col_name(d, vars[j]);
        lbl = g_strdup_printf ("%s=%s", colname, value);
        labels = g_list_append (labels, lbl);

      }
      g_free (vars);
    }
  }

  if (id_display_type & ID_RECORD_NO) {
    if (id_display_type & ~ID_RECORD_NO)
      lbl = g_strdup_printf ("num=%d", k);
    else
      lbl = g_strdup_printf ("%d", k);
    labels = g_list_append (labels, lbl);
  }

  if (id_display_type & ID_RECORD_LABEL) {
    if (id_display_type & ~ID_RECORD_LABEL)
      lbl = g_strdup_printf ("id=%s", ggobi_data_get_row_id(d, (guint) k));
    else
      lbl = ggobi_data_get_row_id(d, (guint) k);
    labels = g_list_append (labels, lbl);
  }

  if (lbl) {
    lbl = (gchar *) g_list_first (labels)->data;
    for (l = labels->next; l; l = l->next) {
      gchar *tmp_lbl = g_strdup_printf ("%s, %s", lbl, (gchar *)l->data);
      g_free (l->data);
      g_free (lbl);
      lbl = tmp_lbl;
    }
    return lbl;
  }
  
  return g_strdup("");
}

