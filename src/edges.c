/* edges.c */
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

/* Which include file should this be in? */
void ggobi_edge_menus_update (ggobid * gg);

/* --------------------------------------------------------------- */
/*                   Dynamic allocation section                    */
/* --------------------------------------------------------------- */

/*
 * The brushing arrays are handled in brush_init.c, and the
 * edges and arrowheads arrays are handled in splot.c
*/

void
edges_alloc (gint nsegs, GGobiData * d)
{
  d->edge.n = nsegs;
  d->edge.sym_endpoints = (SymbolicEndpoints *)
    g_realloc (d->edge.sym_endpoints, nsegs * sizeof (SymbolicEndpoints));

  vectorb_alloc (&d->edge.xed_by_brush, nsegs);
}

void
edges_free (GGobiData * d, ggobid * gg)
{
  gpointer ptr;

  vectorb_free (&d->edge.xed_by_brush);
  ptr = (gpointer) d->edge.sym_endpoints;
  g_free (ptr);
  d->edge.n = 0;
}

/* --------------------------------------------------------------- */
/*               Add an edgeset                                    */
/* --------------------------------------------------------------- */


/**
  This sets the data set as the source of the edge information
  for all the plots within the display.
 */
GGobiData *
setDisplayEdge (displayd * dpy, GGobiData * e)
{
  GList *l;
  GGobiData *old = NULL;

  if (resolveEdgePoints (e, dpy->d)) {
    dpy->e = e;
    /* Now update all displays, not just this one. Events could also be used,
       but this is not too bad. */
    ggobi_edge_menus_update (e->gg);
  }

  for (l = dpy->splots; l; l = l->next) {
    splotd *sp;
    sp = (splotd *) l->data;
    splot_edges_realloc (-1, sp, e);
  }
  return (old);
}

/**
 Invoked when the user selected an item in the Edges menu 
 on a scatterplot to control whether edges are displayed or not
 on the plot.
 */
void
edgeset_add_cb (GtkAction * action, GGobiData * e)
{
  ggobid *gg = e->gg;
  displayd *display = GGOBI_DISPLAY (g_object_get_data (G_OBJECT (action), "display"));

  if (GTK_IS_TOGGLE_ACTION (action)
      && !gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action)))
    return;

  if (e != display->e) {
    setDisplayEdge (display, e);
    display_plot (display, FULL, gg);  /*- moving edge drawing */
  }

  /*
   * If no edge option is true, then turn on undirected edges.
   */
  if (!display->options.edges_undirected_show_p &&
      !display->options.edges_directed_show_p &&
      !display->options.edges_arrowheads_show_p) {
    GtkAction *action = gtk_ui_manager_get_action (display->menu_manager,
                                                   "/menubar/Edges/ShowUndirectedEdges");
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), true);
  }
}

/*
 * I don't know who commented this out, but I know that we want to be
 * sure this is done -- and probably after all edges have been read.
 * We can eliminate duplicate edges as we read them in, but these
 * reversed edges also need to be detected. dfs
 */
#if 0
void
ggobi_cleanUpEdgeRelationships (struct _EdgeData *edge, int startPosition)
{
  int k, i, start, end;
  for (i = startPosition; i < edge->n; i++) {
    end = edge->endpoints[i].b;
    start = edge->endpoints[i].a;
    for (k = 0; k < i; k++) {
      if (edge->endpoints[k].a == end && edge->endpoints[k].b == start) {
        edge->endpoints[i].jpartner = k;
        edge->endpoints[k].jpartner = i;
      }
    }
  }
}
#endif


/* --------------------------------------------------------------- */
/*                          Utilities                              */
/* --------------------------------------------------------------- */

gboolean
edge_endpoints_get (gint k, gint * a, gint * b, GGobiData * d,
                    endpointsd * endpoints, GGobiData * e)
{
  gboolean ok;

  *a = endpoints[k].a;
  *b = endpoints[k].b;

  ok = (*a >= 0 && *a < d->nrows && *b >= 0 && *b < d->nrows);

  return ok;
}

gint
edgesets_count (ggobid * gg)
{
  gint k, ne = 0;
  gint nd = g_slist_length (gg->d);
  GGobiData *e;

  for (k = 0; k < nd; k++) {
    e = (GGobiData *) g_slist_nth_data (gg->d, k);
    if (ggobi_data_has_edges(e))
      ne++;
  }

  return ne;
}

/******************************************************************************/

/* A constant used to identify that we have resolved the edgepoints and there were none. */
static endpointsd DegenerateEndpoints;


/*
  Do the computations to lookup the different source and destination
  records in the target dataset (d) to get the record numbers
  given the symbolic names in the edgeset specification (sym).
*/
static endpointsd *
computeResolvedEdgePoints (GGobiData * e, GGobiData * d)
{
  endpointsd *ans = g_malloc (sizeof (endpointsd) * e->edge.n);
  gboolean resolved_p = false;

  for (gint i = 0; i < e->edge.n; i++) {
    gint row_a = ggobi_data_get_row_for_id(d, e->edge.sym_endpoints[i].a);
    gint row_b = ggobi_data_get_row_for_id(d, e->edge.sym_endpoints[i].b);
    g_debug("ids %s->%s : rows %i->%i", e->edge.sym_endpoints[i].a, e->edge.sym_endpoints[i].b, row_a, row_b);

    ans[i].a = (gint) row_a;
    ans[i].b = (gint) row_b;

    if (row_a == -1 || row_b == -1)
      continue;

    ans[i].jpartner = e->edge.sym_endpoints[i].jpartner;
    if(!resolved_p)
      resolved_p = true;
  }
  
  if (resolved_p)
    return ans;

  g_free (ans);
  return &DegenerateEndpoints;
}



static endpointsd *
do_resolveEdgePoints (GGobiData * e, GGobiData * d, gboolean compute)
{
  endpointsd *ans = NULL;
  DatadEndpoints *ptr;
  GList *tmp;


  if (!ggobi_data_has_edges(e))
    return (NULL);

  /* Get the entry in the table for this dataset (d). Use the name for now. */
  for (tmp = e->edge.endpointList; tmp; tmp = tmp->next) {
    ptr = (DatadEndpoints *) tmp->data;
    if (GGOBI_DATA (ptr->data) == d) {
      ans = ptr->endpoints;
      break;
    }
  }

  /* If it is already computed but empty, then return NULL. */
  if (ans == &DegenerateEndpoints)
    return (NULL);
  
  /* So no entry in the table yet. So compute the endpoints and add
     that to the table. */
  if (ans == NULL && compute) {
    /* resolve the endpoints */
    ans = computeResolvedEdgePoints (e, d);
    ptr = (DatadEndpoints *) g_malloc (sizeof (DatadEndpoints));
    ptr->data = G_OBJECT (d);
    ptr->endpoints = ans;       /* (ans == &DegenerateEndpoints) ? NULL : ans; */
    e->edge.endpointList = g_list_append (e->edge.endpointList, ptr);
  }

  if (ans == &DegenerateEndpoints)
    return (NULL);

  return (ans);
}


/*
 Find the resolved edgepoints array associated with
 this dataset (d) by resolving the symbolic edge definitions
 in e.
*/
endpointsd *
resolveEdgePoints (GGobiData * e, GGobiData * d)
{
  return (do_resolveEdgePoints (e, d, true));
}


gboolean
hasEdgePoints (GGobiData * e, GGobiData * d)
{
  return (do_resolveEdgePoints (e, d, false) ? true : false);
}

static void
cleanEdgePoint (gpointer data, gpointer userData)
{
  DatadEndpoints *el = (DatadEndpoints *) data;
  if (el && el->endpoints) {
    g_free (el->endpoints);
  }
}

void
unresolveAllEdgePoints (GGobiData * e)
{
  if (e->edge.endpointList) {
    g_list_foreach (e->edge.endpointList, cleanEdgePoint, NULL);
    g_list_free (e->edge.endpointList);
    e->edge.endpointList = NULL;
  }
}


gboolean
unresolveEdgePoints (GGobiData * e, GGobiData * d)
{
  DatadEndpoints *ptr;
  GList *tmp;

  if (!ggobi_data_has_edges(e))
    return (false);

  for (tmp = e->edge.endpointList; tmp; tmp = tmp->next) {
    ptr = (DatadEndpoints *) tmp->data;
    if (GGOBI_DATA (ptr->data) == d) {
      if (ptr->endpoints)
        g_free (ptr->endpoints);

      /* equivalent to 
         g_list_remove(e->edge.endpointList, tmp) 
         except we don't do the extra looping. Probably
         minute since # of datasets is small!
       */
      if (tmp == e->edge.endpointList) {
        e->edge.endpointList = tmp->next;
      }
      else {
        tmp->prev = tmp->next;
      }
      return (true);
    }
  }

  return (false);
}
