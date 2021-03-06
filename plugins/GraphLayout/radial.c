#include <gtk/gtk.h>
#include "ggobi.h"
#include "externs.h"
#include "GGobiAPI.h"

#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "plugin.h"
#include "glayout.h"

static void initRadialLayout (glong *vis, gint nvis, ggobid *gg, glayoutd *gl);
static gboolean setParentNodes (glayoutd *gl, GGobiData *d);
static void setNChildren (glayoutd *gl, GGobiData *d);
static gint setSubtreeSize (noded *, glayoutd *, GGobiData *);
static void setSubtreeSpans (glayoutd *, GGobiData *);
static void setNodePositions (glayoutd *, GGobiData *);

/*-- utility --*/
/*
static 
GList * list_concat_uniq (GList *a, GList *b)
{
  GList *l, *ab = NULL;
  noded *n;

  for (l = a; l; l = l->next) {
    n = (noded *) l->data;
    if (g_list_index (ab, n) == -1)
      ab = g_list_append (ab, n);
  }
  for (l = b; l; l = l->next) {
    n = (noded *) l->data;
    if (g_list_index (ab, n) == -1)
      ab = g_list_append (ab, n);
  }

  return ab;
}
*/

static 
GList * list_subset_uniq (GList *full)
{
  GList *l, *uniq = NULL;
/*
 * If a list element is not already a member of the unique list,
 * add it.
*/

  for (l = full; l; l = l->next) {
    if (g_list_position (uniq, l) == -1)
      uniq = g_list_append (uniq, l->data);
  }

  return uniq;
}

static
void list_clear (GList *ab)
{
  GList *l;
  
  for (l = ab; l; l = l->next)
    ab = g_list_remove_link (ab, l);
  ab = NULL;
}

/* unused */
gboolean
hasPathToCenter (noded *n, noded *referringnode, GGobiData *d, GGobiData *e,
  PluginInstance *inst)
{
  gboolean hasPath = false;
  gint k;
  noded *n1;
  glayoutd *gl = glayoutFromInst (inst);
  noded *centerNode = gl->radial->centerNode;
  GList *l, *connectedEdges = list_subset_uniq (n->connectedEdges);
  endpointsd *endpoints;

  endpoints = resolveEdgePoints(e, d);

  for (l = connectedEdges; l; l = l->next) {
    k = GPOINTER_TO_INT (l->data);

    /*-- if edge[k] is included and visible ... --*/
    if (e->sampled.els[k] && !e->hidden.els[k]) {

      /*n1 = &gl->radial->nodes[ d->rowid.idv.els[e->edge.endpoints[k].a] ];*/
      n1 = &gl->radial->nodes[endpoints[k].a];
      if (n1->i == n->i)
        /*n1 = &gl->radial->nodes[d->rowid.idv.els[e->edge.endpoints[k].b]];*/
        n1 = &gl->radial->nodes[endpoints[k].b];

      if (referringnode != NULL && n1->i == referringnode->i)
        continue;  /*-- skip over this node; we've already tested it --*/
      
      /*-- if n1 is included and visible ... --*/
      if (d->sampled.els[n1->i] && !d->hidden.els[n1->i]) {
        /*-- if n1 is no farther from the center than n ... --*/
        if (n1->nStepsToCenter <= n->nStepsToCenter) {
          /*-- if n1 is the center node or has a path to it ... --*/
          if (n1->i == centerNode->i || hasPathToCenter (n1, n, d, e, inst)) {
            hasPath = true;
            break;
          }
        }
      }
    }
  }

  list_clear (connectedEdges);
  return hasPath;
}


/*-----------------------------------------------------------------*/
/*                   callbacks                                     */
/*-----------------------------------------------------------------*/

void radial_auto_update_cb (GtkToggleButton *btn, PluginInstance *inst)
{
  glayoutd *gl = glayoutFromInst (inst);

  gl->radialAutoUpdate = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(btn));
}

void radial_new_data_cb (GtkToggleButton *btn, PluginInstance *inst)
{
  glayoutd *gl = glayoutFromInst (inst);

  gl->radialNewData = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(btn));
}


void do_radial(glayoutd *gl, GGobiData *d, GGobiData *e, displayd *dsp, ggobid *gg);
void 
radial_cb (GtkButton *button, PluginInstance *inst)
{
  ggobid *gg = inst->gg;
  glayoutd *gl = glayoutFromInst (inst);
  displayd *dsp = gg->current_display;
  GGobiData *d = gl->dsrc;
  GGobiData *e = gl->e;

  do_radial(gl, d, e, dsp, gg);
}


void 
do_radial(glayoutd *gl, GGobiData *d, GGobiData *e, displayd *dsp, ggobid *gg)
{
  glong *visible;
  gint nvisible;
/*-- to add variables --*/
  gint i, nP, nC, nS;
  gdouble *x, *y, *depth, *inDegree, *outDegree;
  gdouble *nParents, *nChildren, *nSiblings;
  GList *l, *connectedNodes;
  noded *n, *n1;
/*-- to add the new datad --*/
  gint m, nc;
  InputDescription *desc = NULL;
  GGobiData *dnew;
  gdouble *values;
  gchar **rownames, **colnames, **rowids;
  displayd *dspnew;
  gboolean redisplay_all = false;  /* if points are hidden in this function */
  DisplayOptions *opts = NULL;
  GtkWidget *w;

  if (e == NULL) {
    g_printerr ("Unable to proceed: no edge set is specified\n");
    return;
  }

  /*-- let the first node be the center node if it hasn't been set --*/
  if (gl->centerNodeIndex == -1)
    gl->centerNodeIndex = 0;
  if (!d->sampled.els[gl->centerNodeIndex] ||
       d->hidden.els[gl->centerNodeIndex])
  {
    g_printerr ("Unable to proceed: you've hidden the center node.\n");
    return;
  }

/*-- This may not belong here, but where exactly?  As soon as the
     panel is opened  --*/
/*
  g_signal_connect (G_OBJECT(gg),
    "sticky_point_added", radial_highlight_sticky_edges, inst);
  g_signal_connect (G_OBJECT(gg),
    "sticky_point_removed", radial_highlight_sticky_edges, inst);
*/

  visible = (glong *) g_malloc (d->nrows_in_plot * sizeof (glong));
  nvisible = visible_set (visible, d);
  initRadialLayout (visible, nvisible, gg, gl);

/*
 * As a result of this function, nvisible can change.
*/
  while (setParentNodes (gl, d) == false) {
    for (i=0; i<nvisible; i++) {  /*-- nrows = nnodes --*/
      n = &gl->radial->nodes[i];
      m = visible[i];
      if (n->nStepsToCenter == -1) {
        d->hidden.els[m] = d->hidden_now.els[m] = true;
      }
    }

    nvisible = visible_set (visible, d);
    initRadialLayout (visible, nvisible, gg, gl);
  }

  setNChildren (gl, d);
  setSubtreeSize (gl->radial->centerNode, gl, d);
  setSubtreeSpans (gl, d);
  setNodePositions (gl, d);

/*-- add variables generated by the layout algorithm --*/
  x = g_malloc0 (nvisible * sizeof (gdouble));
  y = g_malloc0 (nvisible * sizeof (gdouble));
  depth = g_malloc0 (nvisible * sizeof (gdouble));
  inDegree = g_malloc0 (nvisible * sizeof (gdouble));
  outDegree = g_malloc0 (nvisible * sizeof (gdouble));
  nParents = g_malloc0 (nvisible * sizeof (gdouble));
  nChildren = g_malloc0 (nvisible * sizeof (gdouble));
  nSiblings = g_malloc0 (nvisible * sizeof (gdouble));

  for (i=0; i<gl->radial->nnodes; i++) {  /*-- nrows = nnodes --*/
    n = &gl->radial->nodes[i];

    /*-- there's no reason to draw these points; they're orphans --*/
    if (n->nStepsToCenter == -1) {
      d->hidden.els[i] = d->hidden_now.els[i] = true;
      redisplay_all = true;
    }

    x[i] = n->pos.x;
    y[i] = n->pos.y;
    depth[i] = (gdouble) n->nStepsToCenter;
    inDegree[i] = (gdouble) n->inDegree;
    outDegree[i] = (gdouble) n->outDegree;

    connectedNodes = list_subset_uniq (n->connectedNodes);

    nP = nC = nS = 0;
    for (l = connectedNodes; l; l = l->next) {
      n1 = (noded *) l->data;

/*
      if (!d->sampled.els[ n1->i ] || d->hidden.els[ n1->i ])
        continue;
*/
      if (n1->nStepsToCenter == -1)
        continue;

      if (n1->nStepsToCenter == n->nStepsToCenter)
        nS++;
      else if (n1->nStepsToCenter < n->nStepsToCenter)
        nP++;
      else if (n1->nStepsToCenter > n->nStepsToCenter)
        nC++;
    }
    list_clear (connectedNodes);

    nChildren[i] = (gdouble) nC;
    nParents[i] = (gdouble) nP;
    nSiblings[i] = (gdouble) nS;
  }

/*
 * create a new datad with the new variables.  include only
 * those nodes that are visible.  these needs some more testing ...
 * and this code could be more efficient -- writing to one set
 * of arrays, then copying to a matrix is probably unnecessary.
*/

  nc = 8;

  rowids = (gchar **) g_malloc (nvisible * sizeof(gchar *));
  for (m=0; m<nvisible; m++) {
    i = visible[m];
    rowids[m] = g_strdup (d->rowIds[i]);
  }

  values = (gdouble *) g_malloc (nvisible * nc * sizeof(gdouble));
  rownames = (gchar **) g_malloc (nvisible * sizeof(gchar *));
  for (i=0; i<nvisible; i++) {
    values[i + 0*nvisible] = (gdouble) x[i];
    values[i + 1*nvisible] = (gdouble) y[i];
    values[i + 2*nvisible] = (gdouble) depth[i];
    values[i + 3*nvisible] = (gdouble) inDegree[i];
    values[i + 4*nvisible] = (gdouble) outDegree[i];
    values[i + 5*nvisible] = (gdouble) nParents[i];
    values[i + 6*nvisible] = (gdouble) nChildren[i];
    values[i + 7*nvisible] = (gdouble) nSiblings[i];

    rownames[i] = (gchar *) g_array_index (d->rowlab, gchar *, visible[i]);
  }

  colnames = (gchar **) g_malloc (nc * sizeof(gchar *));
  colnames[0] = "x";
  colnames[1] = "y";
  colnames[2] = "depth";
  colnames[3] = "in degree";
  colnames[4] = "out degree";
  colnames[5] = "nParents";
  colnames[6] = "nChildren";
  colnames[7] = "nSiblings";

  /*
   * In case there is no initial scatterplot because the datasets
   * have no variables, we don't want creating a datad to trigger
   * the initialization of this plot.   This takes care of it.
  */
  GGOBI_getSessionOptions()->info->createInitialScatterPlot = false;
  /*-- --*/

  /*
   * Determine whether to create a new datad or whether to
   * re-use an existing datad.  For now, only re-use a datad
   * and a display if current_display->d->nrows = nvisible
   */
  if (gl->radialNewData == false && gl->d != NULL &&
      gl->d->nrows == nvisible && gl->d->ncols == 8)
  {
    gint j;
    GGobiData *d = gl->d;

    j = GGOBI(getVariableIndex)("x", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = x[i];

    j = GGOBI(getVariableIndex)("y", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = y[i];

    j = GGOBI(getVariableIndex)("depth", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = y[i];

    j = GGOBI(getVariableIndex)("in degree", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = inDegree[i];

    j = GGOBI(getVariableIndex)("out degree", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = outDegree[i];

    j = GGOBI(getVariableIndex)("nParents", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = nParents[i];

    j = GGOBI(getVariableIndex)("nChildren", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = nChildren[i];

    j = GGOBI(getVariableIndex)("nSiblings", d, gg);
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][j] = d->tform.vals[i][j] = nSiblings[i];
    limits_set(d, true, true, gg->lims_use_visible);

    tform_to_world (d, gg);
  }
  else {
    gl->d = dnew = ggobi_data_new (nvisible, nc);
    dnew->name = g_strdup_printf ("rad:%s", 
      (gchar *) g_array_index (d->rowlab, gchar *, gl->centerNodeIndex));
    dnew->nickname = g_strdup ("rad");

    GGOBI(setData) (values, rownames, colnames, nvisible, nc, dnew, false,
		    gg, rowids, false, desc);

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
    opts = GGOBI(getDefaultDisplayOptions)();
    opts->axes_show_p = false;
    opts->edges_undirected_show_p = true;
    dspnew = GGOBI(newScatterplot) (0, 1, true, dnew, gg);
    opts->axes_show_p = true;  /*-- restore it --*/
    opts->edges_undirected_show_p = false;

    display_add(dspnew, gg);
    varpanel_refresh(dspnew, gg);

    setDisplayEdge (dspnew, e);
  }

  displays_tailpipe (FULL, gg);

  /* enable the check button widget for choosing to re-use a datad and
     displays */
  if (gl && gl->window) {
    w = widget_find_by_name (gl->window, "RADIAL:newdata");
    gtk_widget_set_sensitive (w, true);
  }

  g_free (values);
  g_free (rownames);
  g_free (colnames);
/*
  for (m=0; m<nvisible; m++)
    g_free (rowids[m]);
  g_free (rowids);
*/
  g_free (visible);

  g_free (x);
  g_free (y);
  g_free (depth);
  g_free (inDegree);
  g_free (outDegree);
  g_free (nParents);
  g_free (nChildren);
  g_free (nSiblings);
}

void radial_center_set_cb (ggobid *gg, gint index,
  gint state, GGobiData *d, PluginInstance *inst)
{
  glayoutd *gl = glayoutFromInst (inst);
  GtkWidget *entry;

  if (gl == NULL || gl->window == NULL)
    return;

  entry = (GtkWidget *) g_object_get_data(G_OBJECT(gl->window),
    "CENTERNODE");
  if (entry == NULL)
    return;

  if (state == STICKY && index >= 0) {
    /*-- remove all sticky labels but the last one --*/
    gint i, k, nsticky = g_slist_length (d->sticky_ids);
    for (i = 0; i<nsticky-1; i++) {
      k = (gint) g_slist_nth_data (d->sticky_ids, 0);
      d->sticky_ids = g_slist_remove (d->sticky_ids, 
				      g_slist_nth_data (d->sticky_ids, 0));
      sticky_id_link_by_id (STICKY_REMOVE, k, d, gg);
      g_signal_emit(G_OBJECT(gg),
        getGGobiSignal(STICKY_POINT_REMOVED_SIGNAL), 0,
        (gint) UNSTICKY, d);
    }

    gtk_entry_set_text (GTK_ENTRY (entry),
      (gchar *) g_array_index (d->rowlab, gchar *, index));
    gl->centerNodeIndex = index;

    if (gl->radialAutoUpdate) {
       do_radial(gl, gl->dsrc, gl->e, gg->current_display, gg);
    }
  }
}

#ifdef HIGHLIGHTSTICKY
CHECK_EVENT_SIGNATURE(radial_highlight_sticky_edges, sticky_point_added_f)
CHECK_EVENT_SIGNATURE(radial_highlight_sticky_edges, sticky_point_removed_f)

static void radial_highlight_sticky_edges (ggobid *gg, gint index, gint state,
  GGobiData *d, void *data);
void radial_highlight_sticky_edges (ggobid *gg, gint index, gint state,
  GGobiData *d, void *data)
{
  PluginInstance *inst = (PluginInstance *)data;
  glayoutd *gl = glayoutFromInst (inst);
  GGobiData *e = gl->e;
  noded *n, *n1;
  GList *l, *connectedNodes, *connectedEdges;
  gint k;

  /*-- Do I have to loop over displays, looking for the one(s) with
       edges?  --*/
  if (e == NULL) return;

  n = &gl->radial->nodes[index];

  connectedNodes = list_subset_uniq (n->connectedNodes);
  connectedNodes = g_list_append (connectedNodes, n);
  for (l = connectedNodes; l; l = l->next) {
    n1 = (noded *) l->data;
    d->color.els[n1->i] = d->color_now.els[n1->i] = 
      (state == STICKY) ? gg->color_id : gg->color_0;
    d->glyph.els[n1->i].size = d->glyph_now.els[n1->i].size = 
      (state == STICKY) ? gg->glyph_id.size : gg->glyph_0.size;
    d->glyph.els[n1->i].type = d->glyph_now.els[n1->i].type = 
      (state == STICKY) ? gg->glyph_id.type : gg->glyph_0.type;
  }
  list_clear (connectedNodes);

  connectedEdges = list_subset_uniq (n->connectedEdges);
  for (l = connectedEdges; l; l = l->next) {
    k = GPOINTER_TO_INT (l->data);
    if (k < 0 || k >= e->nrows)
      break;

    if (state == STICKY) {
      e->color.els[k] = e->color_now.els[k] = gg->color_id;
      e->glyph.els[k].size = e->glyph_now.els[k].size = gg->glyph_id.size;
      e->glyph.els[k].type = e->glyph_now.els[k].type = gg->glyph_id.type;
    } else {
      gint a, b;
      endpointsd *endpoints;
      endpoints = resolveEdgePoints(e, d);
      edge_endpoints_get (k, &a, &b, d, endpoints, e);

      if ((a == index &&
           g_slist_index (d->sticky_ids, GINT_TO_POINTER(b)) != -1) ||
          (b == index &&
           g_slist_index (d->sticky_ids, GINT_TO_POINTER(a)) != -1))
      {
        /*
         * if one of the nodes connected to node k has a sticky label,
         * don't downweight the edge
        */
         ;
      } else {
        e->color.els[k] = e->color_now.els[k] = gg->color_0;
        e->glyph.els[k].size = e->glyph_now.els[k].size = gg->glyph_0.size;
        e->glyph.els[k].type = e->glyph_now.els[k].type = gg->glyph_0.type;
      }
    }
  }
  list_clear (connectedEdges);

  displays_plot (NULL, FULL, gg);
}
#endif

/*-- highlighting code that isn't peculiar to radial layouts --*/
void highlight_sticky_edges (ggobid *gg, gint index, gint state,
  GGobiData *d, void *data)
{
}

/*-----------------------------------------------------------------*/

/*
 * Initialize a couple of values for each node.
*/
static void
initRadialLayout (glong *visible, gint nvisible, ggobid *gg,
  glayoutd *gl)
{
  GGobiData *d = gl->dsrc;
  GGobiData *e = gl->e;
  gint i;
  noded *na, *nb;
  gint nedges = e->edge.n;
  gint a, b;
  gint nnodessq = nvisible * nvisible;
  glong *nodeindices = g_malloc (d->nrows * sizeof (glong *));
  endpointsd *endpoints;

  endpoints = resolveEdgePoints(e, d);

  /* used for finding edge endpoints */
  for (i=0; i<d->nrows; i++) 
    nodeindices[i] = -1;

  if (gl->radial != NULL) {
    for (i=0; i < gl->radial->nnodes; i++) {
      g_list_free (gl->radial->nodes[i].connectedEdges);
      g_list_free (gl->radial->nodes[i].connectedNodes);
    }
    g_free (gl->radial->nodes);
  } else {
    gl->radial = (radiald *) g_malloc (sizeof (radiald));
  }

  gl->radial->nodes = (noded *) g_malloc (nvisible * sizeof (noded));
  gl->radial->nnodes = nvisible;

  for (i = 0; i <nvisible; i++) {
    nodeindices[ visible[i] ] = i;
    gl->radial->nodes[i].connectedEdges = NULL;
    gl->radial->nodes[i].connectedNodes = NULL;
    gl->radial->nodes[i].inDegree = 0;
    gl->radial->nodes[i].outDegree = 0;
    gl->radial->nodes[i].subtreeSize = 0;
    gl->radial->nodes[i].nChildren = 0;
    gl->radial->nodes[i].nStepsToCenter = nnodessq;
    gl->radial->nodes[i].i = visible[i];
    gl->radial->nodes[i].parentNode = NULL;

    if (nedges <= 1) {
      gl->radial->nodes[i].nStepsToLeaf = 0;
    } else {
      gl->radial->nodes[i].nStepsToLeaf = nnodessq;
    }
  }


  gl->radial->centerNode = &gl->radial->nodes[gl->centerNodeIndex];
  gl->radial->centerNode->i = gl->centerNodeIndex;

  /*-- initialize the linked lists of edges and nodes --*/

  /*-- loop over the edges --*/
  for (i = 0; i <e->edge.n; i++) {
    if (e->sampled.els[i] && !e->hidden.els[i]) {
/*
      a = d->rowid.idv.els[endpoints[i].a];
      b = d->rowid.idv.els[endpoints[i].b];
*/
      edge_endpoints_get (i, &a, &b, d, endpoints, e);

      if (a != -1 && b != -1 &&
          nodeindices[a] != -1 && nodeindices[b] != -1)
      {
        na = &gl->radial->nodes[ nodeindices[a] ];
        nb = &gl->radial->nodes[ nodeindices[b] ];

        nb->connectedNodes = g_list_append (nb->connectedNodes, na);
        nb->connectedEdges = g_list_append (nb->connectedEdges,
          GINT_TO_POINTER (i));
        nb->inDegree++;

        na->connectedNodes = g_list_append (na->connectedNodes, nb);
        na->connectedEdges = g_list_append (na->connectedEdges,
          GINT_TO_POINTER (i));
        na->outDegree++;
      }
    }
  }

  g_free (nodeindices);
}

void
setNStepsToCenter (noded *n, noded *prevNeighbor, GGobiData *d) {
  noded *n1;
  gint nsteps = n->nStepsToCenter + 1;
  GList *l;
  GList *connectedNodes = list_subset_uniq (n->connectedNodes);

  /*-- source nodes: ie, the edge originates here --*/
  for (l = connectedNodes; l; l = l->next) {
    n1 = (noded *) l->data;
    if (prevNeighbor != NULL && n1->i == prevNeighbor->i)
      continue;

    if (nsteps < n1->nStepsToCenter) {
      n1->nStepsToCenter = nsteps;
      n1->parentNode = n;
      setNStepsToCenter (n1, n, d);
    }
  }

  list_clear (connectedNodes);
}


/*
 * Work out from the center and determine the value of
 * nStepsToCenter and parent node for each node.
*/
gboolean
setParentNodes (glayoutd *gl, GGobiData *d) {
  gint i;
  noded *n;
  gboolean nvisible_ok = true;
  gint nnodessq = gl->radial->nnodes * gl->radial->nnodes;

  noded *centerNode = gl->radial->centerNode;

  centerNode->nStepsToCenter = 0;
  centerNode->parentNode = NULL;
  setNStepsToCenter (centerNode, NULL, d);

  /* find the maximum number of steps from the center */
  gl->radial->nStepsToCenter = 0;
  for (i=0; i<gl->radial->nnodes; i++) {
    n = &gl->radial->nodes[i];

    /*-- these guys have no path to the center; reset nSteps to -1 --*/
    if (n->nStepsToCenter == nnodessq) {
      n->nStepsToCenter = -1;
      nvisible_ok = false;
    } else if (n->nStepsToCenter > gl->radial->nStepsToCenter) {
      gl->radial->nStepsToCenter = n->nStepsToCenter;
    }
  }
  return nvisible_ok;
}


void setNChildren (glayoutd *gl, GGobiData *d)
{
  gint i;
  noded *n;

  for (i=0; i<gl->radial->nnodes; i++) {
    n = &gl->radial->nodes[i];

    if (n->nStepsToCenter != -1)
      if (n->parentNode != NULL)
        n->parentNode->nChildren++;
  }

/*-- debug --*/
/*
  for (i=0; i<d->nrows_in_plot; i++) {
    n = &gl->radial->nodes[i];
    g_printerr ("node %d children %d\n", n->i, n->nChildren);
  }
*/
}

/*
 * This is currently being computed three times; once ought
 * to be enough.
*/
static void
childNodes (GList **children, noded *n) {
  GList *l;
  noded *n1;
  GList *connectedNodes = list_subset_uniq (n->connectedNodes);

  for (l = connectedNodes; l; l = l->next) {
    n1 = (noded *) l->data;

    if (n1->parentNode != NULL && n1->parentNode->i == n->i)
      if (g_list_index (*children, n1) == -1)
        *children = g_list_append (*children, n1);
  }

  list_clear (connectedNodes);
}

/*
 * Once the parent node is irrevocably set (once setParentNodes and
 * setNChildren are through), then it's possible to compute subtreeSize
 * for each node.
 *
 * Work out from the center ...
*/
gint
setSubtreeSize (noded *n, glayoutd *gl, GGobiData *d) {
  noded *nchild;
  GList *l, *children = NULL;

  childNodes (&children, n);

  for (l = children; l; l = l->next) {
    nchild = (noded *) l->data;

/*
    if (!d->sampled.els[ nchild->i ] || d->hidden.els[ nchild->i ])
      continue;
*/
    if (nchild->nStepsToCenter == -1)
      continue;

    if (nchild->nChildren == 0)
      n->subtreeSize += 1;
    else
      n->subtreeSize += setSubtreeSize (nchild, gl, d);
  }
  /*g_printerr ("node %d subtreeSize %d\n", n->i, n->subtreeSize);*/
  return (n->subtreeSize);
}

/*---------------------------------------------------------------------*/

static void
setChildSubtreeSpans (noded *n, glayoutd *gl, GGobiData *d)
{
  noded *nchild;
  GList *l, *children = NULL;

  childNodes (&children, n);

  for (l = children; l; l = l->next) {
    nchild = (noded *) l->data;

/* this should not be possible
    if (!d->sampled.els[ nchild->i ] || d->hidden.els[ nchild->i ])
      continue;
*/
    if (nchild->nStepsToCenter == -1)
      continue;

    nchild->span = n->span * nchild->subtreeSize / n->subtreeSize;
    if (nchild->nChildren > 0) {
      setChildSubtreeSpans (nchild, gl, d);
    }
  }
}


void
setSubtreeSpans (glayoutd *gl, GGobiData *d) {
  gl->radial->centerNode->span = 2*M_PI;
  setChildSubtreeSpans (gl->radial->centerNode, gl, d);
}

/*---------------------------------------------------------------------*/

/* Set the node positions for the 2nd and later rings. */
static void
setChildNodePositions (noded *n, glayoutd *gl, GGobiData *d)
{
  gint i;
  noded *nchild;
  gdouble theta;
  GList *l, *children = NULL;

  /* the initial value of theta is the angle of the boundary of the fan */
  if (n->i == gl->radial->centerNode->i) theta = 0;
  else if (n->nChildren == 1) theta = n->theta;
  else {
    theta = n->theta - n->span/2;
  }

  /* Build an array of the child nodes  */
  childNodes (&children, n);

  i = 0;
  for (l = children; l; l = l->next) {
    nchild = (noded *) l->data;

/* this should not be possible
    if (!d->sampled.els[ nchild->i ] || d->hidden.els[ nchild->i ])
      continue;
*/
    if (nchild->nStepsToCenter == -1)
      continue;

    if (i == 0) {
      nchild->theta = theta;

      if (nchild->span > 0)
        theta += nchild->span/2;
      else  /* if it's a leaf node */
        theta += .5 * (n->span)/(gdouble)(n->subtreeSize-1);

      i++;
    } else {
      nchild->theta = theta + nchild->span/2;

      if (nchild->span > 0)
        theta += nchild->span;
      else  /* if it's a leaf node */
        theta += (n->span)/(gdouble)(n->subtreeSize-1);
    }
      
    nchild->pos.x = nchild->nStepsToCenter * cos(nchild->theta);
    nchild->pos.y = nchild->nStepsToCenter * sin(nchild->theta);

    if (nchild->nChildren > 0)
      setChildNodePositions(nchild, gl, d);
  }
}


void
setNodePositions (glayoutd *gl, GGobiData *d) {

  /* Set the position of the center node */
  gl->radial->centerNode->pos.x = 0;
  gl->radial->centerNode->pos.y = 0;
  gl->radial->centerNode->theta = 0;

  setChildNodePositions (gl->radial->centerNode, gl, d);
}

void
do_radial_plugin (PluginInstance *inst, gint center, GGobiData *d, GGobiData *e, displayd *dsp, ggobid *gg)
{
   glayoutd *gl;
   gl = glayoutFromInst (inst);

   if(!gl) {
    gl = (glayoutd *) g_malloc (sizeof (glayoutd));
    glayout_init (gl);
    inst->data = gl;
   } 

   gl->dsrc = d;
   gl->e = e;
   gl->centerNodeIndex = center;

   do_radial(gl, d, e, dsp, gg);
}

#ifdef WITH_R
#ifdef NEW
#undef NEW
#endif
#include "RSCommon.h"

USER_OBJECT_
S_radial_cb(USER_OBJECT_ plugin, USER_OBJECT_ centerNode, USER_OBJECT_ data, 
              USER_OBJECT_ edges, USER_OBJECT_ display)
{
   GGobiData *d, *e;
   glayoutd *gl;
   ggobid *gg;
   PluginInstance *inst;
   displayd *dsp;

   inst = (PluginInstance *) R_ExternalPtrAddr(plugin);

   gl = glayoutFromInst (inst);

   if(!gl) {
    gl = (glayoutd *) g_malloc (sizeof (glayoutd));
    glayout_init (gl);
    inst->data = gl;
   }

   gl->centerNodeIndex = INTEGER_DATA(centerNode)[0];

   gg = inst->gg;
   gl->dsrc = d = (GGobiData *) R_ExternalPtrAddr(data);
   gl->e = e = (GGobiData *) R_ExternalPtrAddr(edges);

   dsp = (displayd *) R_ExternalPtrAddr(data);

   do_radial(gl, d, e, dsp, gg);

     /* Return the new display. */
   return(NULL_USER_OBJECT);
}
#endif

/*-- API routines --*/
gint radial_auto_update_set (gboolean state, PluginInstance *inst)
{
  glayoutd *gl = glayoutFromInst (inst);
  GtkWidget *w;

  if (gl) {
    w = widget_find_by_name (gl->window, "RADIAL:autoupdate");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), state);
    return 1;
  }
  return 0;
}
gint radial_new_data_set (gboolean state, PluginInstance *inst)
{
  glayoutd *gl = glayoutFromInst (inst);
  GtkWidget *w;

  if (gl) {
    w = widget_find_by_name (gl->window, "RADIAL:newdata");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(w), state);
    return 1;
  }
  return 0;
}
/*  */
