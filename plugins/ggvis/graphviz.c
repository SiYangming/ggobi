#include <gtk/gtk.h>
#include "ggobi.h"
#include "externs.h"
#include "GGobiAPI.h"

#include <stdio.h>
#include <math.h>

#include "plugin.h"
#include "ggvis.h"

#ifdef GRAPHVIZ
#include <gvconfig.h>
#include <render.h>
#include <graph.h>
#include <dotneato/dotgen/dotprocs.h>
#include <dotneato/neatogen/neatoprocs.h>
#include <dotneato/neatogen/adjust.h>

#define DOT_LAYOUT   0
#define NEATO_LAYOUT 1

#define DATE    "June 2002"
#define VERSION "huh?"
char *Info[] = {
    "ggvis",              /* Program */
    VERSION,              /* Version */
    DATE                  /* Build Date */
};

static void
initGraphviz (ggobid *gg, ggvisd *ggv, datad *d, datad *e, gint kind)
{
  if (ggv->graphviz != NULL) {
    dot_cleanup_graph (ggv->graphviz->graph);
  } else {

    ggv->graphviz = (graphvizd *) g_malloc (sizeof (graphvizd));
    aginit();

    /*-- create new empty graph --*/
    ggv->graphviz->graph = agopen("graph", kind);
  }

  ggv->graphviz->d = d;
}

void dot_neato_layout_cb (GtkWidget *button, PluginInstance *inst)
{
  ggobid *gg = inst->gg;
  ggvisd *ggv = GGVisFromInst (inst);
  datad *d = gg->current_display->d;
  datad *e = gg->current_display->e;
  Agnode_t *node, *head, *tail;
  gchar *name;
  gint kind = AGRAPH;
  gint i, j, m;
  gint a, b;
  gboolean init;
#ifdef DEBUG
  FILE* f;
#endif
  Agraph_t *graph;
  gdouble *x, *y;
  gint layout_type = DOT_LAYOUT;

  if (strcmp (gtk_widget_get_name (button), "neato") == 0)
    layout_type = NEATO_LAYOUT;

  init = (ggv->graphviz == NULL || d->nrows != ggv->graphviz->d->nrows);
  initGraphviz (gg, ggv, d, e, kind);

  graph = ggv->graphviz->graph;

  /*-- create new nodes, add to graph --*/
  for (i=0; i<d->nrows_in_plot; i++) {
    m = d->rows_in_plot[i];
    name = (gchar *) g_array_index (d->rowlab, gchar *, m);
    agnode(graph, name);
  }

  /*-- create new edges, add to graph --*/
  for (i=0; i<e->edge.n; i++) {

    a = d->rowid.idv.els[e->edge.endpoints[i].a];
    name = (gchar *) g_array_index (d->rowlab, gchar *, a);
    tail = agfindnode (graph, name);

    b = d->rowid.idv.els[e->edge.endpoints[i].b];
    name = (gchar *) g_array_index (d->rowlab, gchar *, b);
    head = agfindnode (graph, name);

    agedge(graph, tail, head);
  }

  x = g_malloc0 (d->nrows * sizeof (gdouble));
  y = g_malloc0 (d->nrows * sizeof (gdouble));
  if (layout_type == DOT_LAYOUT) {
    graph_init(graph);
    graph->u.drawing->engine = DOT;
    dot_init_node_edge(graph);  /*-- this might muck with width, height --*/
    dot_rank(graph);
    dot_mincross(graph);
    dot_position(graph);

    /*-- add variables generated by the layout algorithm --*/
    for (i=0; i<d->nrows; i++) {  /*-- nrows = nnodes --*/
      if (!d->sampled.els[i] || d->hidden.els[i]) {
        ;
      } else {
        name = (gchar *) g_array_index (d->rowlab, gchar *, i);
        node = agfindnode (graph, name);
        x[i] = (gdouble) node->u.coord.x;
        y[i] = (gdouble) node->u.coord.y;
      }
    }
  } else {
    gint         nG;
    attrsym_t*  sym;
g_printerr ("doing neato layout\n");
   
    /* setting rankdir=LR is currently undefined in neato,
     * but having it set causes has effects in the common routines.
     * So, we turn it off.
     */
    sym = agfindattr(graph,"rankdir");
    if (sym)
      agxset (graph, sym->index, "");

    graph_init(graph);
    graph->u.drawing->engine = NEATO;
    neato_init_node_edge(graph);
    nG = scan_graph(graph);
	if (Nop) {
	    initial_positions(graph, nG);
	}
	else {
		char *p;
		p = agget(graph,"model");
		if (p && (streq(p,"circuit"))) circuit_model(graph,nG);
		else shortest_path(graph, nG);
    	initial_positions(graph, nG);
    	diffeq_model(graph, nG);
    	solve_model(graph, nG);
    	final_energy(graph, nG); 
    	adjustNodes(graph);
	}
    /*-- add variables generated by the layout algorithm --*/
    for (i=0; i<d->nrows; i++) {  /*-- nrows = nnodes --*/
      if (!d->sampled.els[i] || d->hidden.els[i]) {
        ;
      } else {
        name = (gchar *) g_array_index (d->rowlab, gchar *, i);
        node = agfindnode (graph, name);
        x[i] = (gdouble) node->u.pos[0];
        y[i] = (gdouble) node->u.pos[1];
      }
    }
  }


  /*-- may want to set u.width, u.height to very small values --*/
/*
  for (i=0; i<d->nrows_in_plot; i++) {
    m = d->rows_in_plot[i];
    name = (gchar *) g_array_index (d->rowlab, gchar *, m);
    node = agfindnode (graph, name);
  }
*/


  if (init) {
    name = g_strdup_printf ("x");
    newvar_add_with_values (x, d->nrows, name, d, gg);
    g_free (name);
    g_free (x);

    name = g_strdup_printf ("y");
    newvar_add_with_values (y, d->nrows, name, d, gg);
    g_free (name);
    g_free (y);
  } else {
    j = GGOBI(getVariableIndex)("x", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = x[i];
    limits_set_by_var (j, true, true, d, gg);
    g_free (x);

    j = GGOBI(getVariableIndex)("y", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = y[i];
    limits_set_by_var (j, true, true, d, gg);
    g_free (y);

    tform_to_world (d, gg);
    displays_tailpipe (FULL, gg);
  }

  g_free (x);
  g_free (y);

#ifdef DEBUG
  f = fopen ("test.out", "w");
  agwrite (graph, f);
  fclose(f);
#endif
}

#endif
