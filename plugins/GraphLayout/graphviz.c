#include <gtk/gtk.h>
#include "ggobi.h"
#include "externs.h"
#include "GGobiAPI.h"

#include <stdio.h>
#include <math.h>

#include "plugin.h"
#include "glayout.h"

#ifdef GRAPHVIZ
#include <gvconfig.h>
#include <render.h>
#include <graph.h>
#include <dotprocs.h>
#include <neatoprocs.h>
/*
#include <adjust.h>
*/

#define DOT_LAYOUT   0
#define NEATO_LAYOUT 1

#define DATE    "June 2002"
#define VERSION "huh?"
char *Info[] = {
    "glayout",            /* Program */
    VERSION,              /* Version */
    DATE                  /* Build Date */
};


void dot_neato_layout_cb (GtkWidget *button, PluginInstance *inst)
{
  ggobid *gg = inst->gg;
  displayd *dsp = gg->current_display;
  datad *d = gg->current_display->d;
  datad *e = gg->current_display->e;
  Agnode_t *node, *head, *tail;
  gchar *name;
  gint kind = AGRAPH;
  gint i;
  gint a, b;
#ifdef DEBUG
  FILE* f;
#endif
  Agraph_t *graph;
  gdouble *x, *y;
  gint layout_type = DOT_LAYOUT;
/*-- to add the new datad --*/
  gint m, nvisible, nc;
  InputDescription *desc = NULL;
  datad *dnew;
  gdouble *values;
  gchar **rownames, **colnames;
  glong *visible, *rowids;
  displayd *dspnew;
  gboolean edges_displayed;

  if (e == NULL) {
    if (!edgeset_add (dsp)) {
      quick_message ("Please specify an edge set", false);
      return;
    } else e = dsp->e;
  }

  visible = (glong *) g_malloc (d->nrows_in_plot * sizeof (glong));
  nvisible = 0;
  for (m=0; m<d->nrows_in_plot; m++) {
    i = d->rows_in_plot[m];
    if (!d->hidden.els[i]) {
      visible[nvisible++] = i;
    }
  }

  if (strcmp (gtk_widget_get_name (button), "neato") == 0)
    layout_type = NEATO_LAYOUT;

  aginit();

  /*-- create a new empty graph --*/
  graph = agopen("graph", kind);

  /*-- create new nodes, add to graph --*/
  for (i=0; i<nvisible; i++) {
    m = visible[i];
    name = (gchar *) g_array_index (d->rowlab, gchar *, m);
    agnode(graph, name);
  }

  /*-- create new edges, add to graph --*/
  for (i=0; i<e->edge.n; i++) {

/* still need a test for edge visibility */
    a = d->rowid.idv.els[e->edge.endpoints[i].a];
    name = (gchar *) g_array_index (d->rowlab, gchar *, a);
    tail = agfindnode (graph, name);

    b = d->rowid.idv.els[e->edge.endpoints[i].b];
    name = (gchar *) g_array_index (d->rowlab, gchar *, b);
    head = agfindnode (graph, name);

    /*-- if head and tail are both in the visible subset --*/
    if (head && tail)
      agedge(graph, tail, head);
  }

  x = g_malloc0 (nvisible * sizeof (gdouble));
  y = g_malloc0 (nvisible * sizeof (gdouble));
  if (layout_type == DOT_LAYOUT) {
    graph_init(graph);
    graph->u.drawing->engine = DOT;
    dot_init_node_edge(graph);  /*-- this might muck with width, height --*/
    dot_rank(graph);
    dot_mincross(graph);
    dot_position(graph);

    /*-- add variables generated by the layout algorithm --*/
    for (i=0; i<nvisible; i++) {  /*-- nrows = nnodes --*/
      m = visible[i];
      name = (gchar *) g_array_index (d->rowlab, gchar *, m);
      node = agfindnode (graph, name);
      x[i] = (gdouble) node->u.coord.x;
      y[i] = (gdouble) node->u.coord.y;
    }
    dot_cleanup (graph);

  } else {  /* if layout_type == NEATO */
    gint         nG;
    attrsym_t*  sym;
   
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
    for (i=0; i<nvisible; i++) {  /*-- nrows = nnodes --*/
      m = visible[i];
      name = (gchar *) g_array_index (d->rowlab, gchar *, m);
      node = agfindnode (graph, name);
      x[i] = (gdouble) node->u.pos[0];
      y[i] = (gdouble) node->u.pos[1];
    }
#ifdef DEBUG
    f = fopen ("test.out", "w");
    agwrite (graph, f);
    fclose(f);
#endif
    neato_cleanup (graph);
  }


  /*-- may want to set u.width, u.height to very small values --*/
/*
  for (i=0; i<d->nrows_in_plot; i++) {
    m = d->rows_in_plot[i];
    name = (gchar *) g_array_index (d->rowlab, gchar *, m);
    node = agfindnode (graph, name);
  }
*/

/*
 * create a new datad with the new variables.  include only
 * those nodes that are visible.  these needs some more testing ...
 * and this code could be more efficient -- writing to one set
 * of arrays, then copying to a matrix is probably unnecessary.
*/

  nc = 2;

  rowids = (glong *) g_malloc (nvisible * sizeof(glong));
  for (m=0; m<nvisible; m++) {
    i = visible[m];
    rowids[m] = (glong) d->rowid.id.els[i];
  }

  values = (gdouble *) g_malloc (nvisible * nc * sizeof(gdouble));
  rownames = (gchar **) g_malloc (nvisible * sizeof(gchar *));
  for (m=0; m<nvisible; m++) {
    i = visible[m];
    values[m + 0*nvisible] = (gdouble) x[i];
    values[m + 1*nvisible] = (gdouble) y[i];

    rownames[m] = (gchar *) g_array_index (d->rowlab, gchar *, i);
  }

  colnames = (gchar **) g_malloc (nc * sizeof(gchar *));
  colnames[0] = "x";
  colnames[1] = "y";

  dnew = datad_create (nvisible, nc, gg);
  dnew->name = (layout_type == DOT_LAYOUT) ?
    g_strdup ("dot") : g_strdup ("neato");

  GGOBI(setData) (values, rownames, colnames, nvisible, nc, dnew, false,
    gg, rowids, desc);

  /*-- copy the color and glyph vectors from d to dnew --*/
  for (i=0; i<nvisible; i++) {
    dnew->color.els[i] = dnew->color_now.els[i] = dnew->color_prev.els[i] =
      d->color.els[visible[i]];
    dnew->glyph.els[i].type = dnew->glyph_now.els[i].type =
      dnew->glyph_prev.els[i].type = d->glyph.els[visible[i]].type;
    dnew->glyph.els[i].size = dnew->glyph_now.els[i].size =
      dnew->glyph_prev.els[i].size = d->glyph.els[visible[i]].size;
  }

/*
 * open a new scatterplot with the new data, and display edges
 * as they're displayed in the current datad.
*/
  dspnew = GGOBI(newScatterplot) (0, 1, dnew, gg);
  setDisplayEdge (dspnew, e);
  edges_displayed = display_copy_edge_options (dsp, dspnew);
  if (!edges_displayed) {
    GGOBI(setShowLines)(dspnew, true);
/*
    GtkWidget *item;
    dspnew->options.edges_undirected_show_p = true;
    item = widget_find_by_name (dspnew->edge_menu,
            "DISPLAY MENU: show directed edges");
    if (item)
      gtk_check_menu_item_set_active ((GtkCheckMenuItem *) item,
        dspnew->options.edges_directed_show_p);
*/
  }

/*
    name = g_strdup_printf ("x");
    newvar_add_with_values (x, d->nrows, name, d, gg);
    g_free (name);

    name = g_strdup_printf ("y");
    newvar_add_with_values (y, d->nrows, name, d, gg);
    g_free (name);
*/

  g_free (x);
  g_free (y);
  agclose (graph);
}

#endif
