/* vartable.c */
/*
    This software may only be used by you under license from AT&T Corp.
    ("AT&T").  A copy of AT&T's Source Code Agreement is available at
    AT&T's Internet website having the URL:
    <http://www.research.att.com/areas/stat/ggobi/license.html>
    If you received this software without first entering into a license
    with AT&T, you have an infringing copy of this software and cannot use
    it without violating AT&T's intellectual property rights.
*/

#include <stdlib.h>
#include <math.h>

#include <gtk/gtk.h>
#include "vars.h"
#include "externs.h"

#include "vartable.h"

#ifdef __cplusplus
extern "C" {
#endif
extern gfloat no_change (gfloat, gfloat);
#ifdef __cplusplus
}
#endif

gboolean vartable_update_cloned_var (gint nc, gint jvar, datad *, ggobid *);

/*-------------------------------------------------------------------------*/
/*                         utilities                                       */
/*-------------------------------------------------------------------------*/

gint
selected_cols_get (gint *cols, datad *d, ggobid *gg)
{
/*
 * Figure out which columns are selected.
*/
  gint j, ncols = 0;

  for (j=0; j<d->ncols; j++) 
    if (d->vartable[j].selected)
      cols[ncols++] = j;

  return (ncols);
}

/*
 * When there aren't any columns in the variable statistics table,
 * this is how we find out which columns are selected for plotting.
 * 
 * I was selecting them in the variable table, and then clearing
 * the table afterwards, but I don't like that -- it triggers callbacks
 * in a mysterious way.
*/
gint
plotted_cols_get (gint *cols, datad *d, ggobid *gg) 
{
  gint mode = mode_get (gg);
  gint ncols = 0;
  splotd *sp = gg->current_splot;
  displayd *display = (displayd *) sp->displayptr;

  switch (display->displaytype) {
    case scatterplot:
      switch (mode) {
        case P1PLOT:
          cols[ncols++] = sp->p1dvar;
          break;
        case XYPLOT:
          cols[ncols++] = sp->xyvars.x;
          cols[ncols++] = sp->xyvars.y;
          break;
      }
      break;
    case scatmat:
      break;
    case parcoords:
      break;
  }

  return ncols;
}

/*-------------------------------------------------------------------------*/
/*                         memory management                               */
/*-------------------------------------------------------------------------*/

void vartable_alloc (datad *d, ggobid *gg)
{
  if (d->vartable != NULL)
    g_free ((gpointer) d->vartable);

  d->vartable = (vartabled *) g_malloc (d->ncols * sizeof (vartabled));
}

void vartable_realloc (gint n, datad *d, ggobid *gg)
{
  d->vartable = (vartabled *) g_realloc ((gpointer) d->vartable,
    n * sizeof (vartabled));
}

/*-------------------------------------------------------------------------*/

void vartable_init (datad *d, ggobid *gg)
{
  gint j;
  for (j=0; j<d->ncols; j++) {
    d->vartable[j].selected = false;
    d->vartable[j].nmissing = 0;

    d->vartable[j].jref = -1;  /*-- not cloned --*/

    d->vartable[j].mean = 0.0;
    d->vartable[j].median = 0.0;

    d->vartable[j].lim_specified_p = false;  /*-- no user-specified limits --*/

    d->vartable[j].lim_raw.min = 0.0;
    d->vartable[j].lim_raw.max = 0.0;
    d->vartable[j].lim_tform.min = 0.0;
    d->vartable[j].lim_tform.max = 0.0;

    d->vartable[j].domain_incr = 0.;
    d->vartable[j].domain_adj = no_change;
    d->vartable[j].inv_domain_adj = no_change;
    d->vartable[j].tform1 = NO_TFORM1;
    d->vartable[j].param = 0.;
	d->vartable[j].tform2 = NO_TFORM2;

    d->vartable[j].jitter_factor = 0.0;

    d->vartable[j].collab = NULL;
    d->vartable[j].collab_tform = NULL;
  }
}

/*-------------------------------------------------------------------------*/
/*                 finding the statistics for the table                    */
/*-------------------------------------------------------------------------*/

void
vartable_stats_print (datad *d, ggobid *gg) 
{
  gint j;
  for (j=0; j<d->ncols; j++) {
    g_printerr ("mean=%f, median=%f\n",
      d->vartable[j].mean, d->vartable[j].median);
    g_printerr ("lims: %7.2f %7.2f %7.2f %7.2f\n",
      d->vartable[j].lim_raw.min, d->vartable[j].lim_raw.max,
      d->vartable[j].lim_tform.min, d->vartable[j].lim_tform.max);
  }
}

/*-- obsolete --*/
/*
 * Sets lim_raw from d->raw, and lim_tform from d->tform.
*/
/*
void
vartable_stats_set (datad *d, ggobid *gg) 
{
  gint j, i, m, np;
  gfloat *sumv = (gfloat *) g_malloc0 (d->ncols * sizeof (gfloat));
  gfloat *x = (gfloat *) g_malloc (d->nrows * sizeof (gfloat));

  *
   * this could be done with less code, but this 
   * minimizes looping and function calls, and should
   * be as as fast as we can make it
  *

  for (j=0; j<d->ncols; j++) {
    d->vartable[j].lim_raw.min = d->raw.vals[0][j];
    d->vartable[j].lim_raw.max = d->raw.vals[0][j];
    d->vartable[j].lim_tform.min = d->tform.vals[0][j];
    d->vartable[j].lim_tform.max = d->tform.vals[0][j];
  }

  for (j=0; j<d->ncols; j++) {
    np = 0;
    for (m=0; m<d->nrows_in_plot; m++) {
      i = d->rows_in_plot[m];
      if (d->nmissing > 0 && MISSING_P(i,j))
        ;
      else {

        if (d->raw.vals[i][j] < d->vartable[j].lim_raw.min)
          d->vartable[j].lim_raw.min = d->raw.vals[i][j];
        else if (d->raw.vals[i][j] > d->vartable[j].lim_raw.max)
          d->vartable[j].lim_raw.max = d->raw.vals[i][j];

        if (d->tform.vals[i][j] < d->vartable[j].lim_tform.min)
          d->vartable[j].lim_tform.min = d->tform.vals[i][j];
        else if (d->tform.vals[i][j] > d->vartable[j].lim_tform.max)
          d->vartable[j].lim_tform.max = d->tform.vals[i][j];

        sumv[j] += d->raw.vals[i][j];
        x[np] = d->raw.vals[i][j];
        np++;
      }
    }
    d->vartable[j].mean = sumv[j] / (gfloat) d->nrows;

    *-- median: sort the temporary vector, and find its center --*
    qsort((void *) x, np, sizeof (gfloat), fcompare);
    d->vartable[j].median = 
      ((np % 2) != 0) ?  x[(np-1)/2] : (x[np/2-1] + x[np/2])/2. ;
  }

  g_free ((gpointer) sumv);
  g_free ((gpointer) x);
}
*/

/*-------------------------------------------------------------------------*/
/*                   adding and deleting variables                         */
/*-------------------------------------------------------------------------*/

void
vartable_row_assemble (gint j, gchar **row, datad *d, ggobid *gg)
{
  gint nrows = GTK_CLIST (d->vartable_clist)->rows;

  if (j == -1) {
    row[CLIST_VARNO] = g_strdup_printf ("%d", nrows);
    row[CLIST_VARNAME] = g_strdup ("");
    row[CLIST_TFORM] = g_strdup ("");
    row[CLIST_USER_MIN] = g_strdup_printf ("%8.3f", 0.0);
    row[CLIST_USER_MAX] = g_strdup_printf ("%8.3f", 0.0);
    row[CLIST_DATA_MIN] = g_strdup_printf ("%8.3f", 0.0);
    row[CLIST_DATA_MAX] = g_strdup_printf ("%8.3f", 0.0);
    row[CLIST_MEAN] = g_strdup_printf ("%8.3f", 0.0);
    row[CLIST_MEDIAN] = g_strdup_printf ("%8.3f", 0.0);
    row[CLIST_NMISSING] = g_strdup_printf ("%d", 0);
  } else {
    row[CLIST_VARNO] = g_strdup_printf ("%d", nrows);
    row[CLIST_VARNAME] = g_strdup (d->vartable[j].collab);
    row[CLIST_TFORM] = g_strdup ("");
    if (d->vartable[j].lim_specified_p) {
      row[CLIST_USER_MIN] = g_strdup_printf ("%8.3f",
        d->vartable[j].lim_specified_tform.min);
      row[CLIST_USER_MAX] = g_strdup_printf ("%8.3f",
        d->vartable[j].lim_specified_tform.max);
    } else {
      row[CLIST_USER_MIN] = g_strdup ("");
      row[CLIST_USER_MAX] = g_strdup ("");
    }
    row[CLIST_DATA_MIN] = g_strdup_printf ("%8.3f",
      d->vartable[j].lim_tform.min);
    row[CLIST_DATA_MAX] = g_strdup_printf ("%8.3f",
      d->vartable[j].lim_tform.max);
    row[CLIST_MEAN] = g_strdup_printf ("%8.3f", d->vartable[j].mean);
    row[CLIST_MEDIAN] = g_strdup_printf ("%8.3f", d->vartable[j].median);
    row[CLIST_NMISSING] = g_strdup_printf ("%d", d->vartable[j].nmissing);
  }
}

void
vartable_row_append (gint j, datad *d, ggobid *gg)
{
  if (d->vartable_clist != NULL) {
    gint k;
    gchar **row = (gchar **) g_malloc (NCOLS_CLIST * sizeof (gchar *));

    vartable_row_assemble (j, row, d, gg);
    gtk_clist_append ((GtkCList *) d->vartable_clist, row);

    for (k=0; k<NCOLS_CLIST; k++)
      g_free ((gpointer) row[k]);
    g_free ((gpointer) row);
  }
}

/*
 * n: the index of the new variable
 * jvar: the index of the variable being cloned
*/
gboolean
vartable_update_cloned_var (gint jvar, gint n, datad *d, ggobid *gg)
{
  if (n >= 0 && jvar > n) {
    d->vartable[jvar].jref = n;
    d->vartable[jvar].collab = g_strdup (d->vartable[n].collab_tform);
    d->vartable[jvar].collab_tform = g_strdup (d->vartable[n].collab_tform);
    d->vartable[jvar].mean = d->vartable[n].mean;
    d->vartable[jvar].median = d->vartable[n].median;
    d->vartable[jvar].lim.min =
      d->vartable[jvar].lim_raw.min =
      d->vartable[jvar].lim_tform.min = d->vartable[n].lim_raw.min;
    d->vartable[jvar].lim.max =
      d->vartable[jvar].lim_raw.max =
      d->vartable[jvar].lim_tform.max = d->vartable[n].lim_raw.max;

    return (true);

  } else return (false); 
}

void
clone_vars (gint *cols, gint ncols, datad *d, ggobid *gg)
{
  gint i, k, n, jvar;

g_printerr ("adding %d columns: ", ncols);
for (k=0; k<ncols; k++) g_printerr ("%d ", cols[k]);
g_printerr ("\n");
g_printerr ("the new ncols will be %d\n", d->ncols+ncols);

  vartable_realloc (d->ncols+ncols, d, gg);
  for (k=0; k<ncols; k++) {
    n = cols[k];       /*-- variable being cloned --*/
    jvar = d->ncols+k; /*-- its new index --*/

    /*-- fill in the values in d->vartable --*/
    vartable_update_cloned_var (jvar, n, d, gg);

    transform_values_init (jvar, d, gg);
  }

  /*-- pipeline --*/
  arrayf_add_cols (&d->raw, d->ncols+ncols);
  arrayf_add_cols (&d->tform, d->ncols+ncols);

  for (k=0; k<ncols; k++) {
    n = cols[k];        /*-- variable being cloned --*/
    jvar = d->ncols+k;  /*-- its new index --*/
    for (i=0; i<d->nrows; i++)
      d->raw.vals[i][jvar] = d->tform.vals[i][jvar] = d->tform.vals[i][n];

    tform_to_world_by_var (jvar, d, gg);

    /*-- update the vartable display --*/
    vartable_row_append (n, d, gg);
  }
  /*-- --*/
/*
    missing_arrays_add_column (jvar, d, gg);
*/

  /*-- variable checkboxes --*/
  for (k=0; k<ncols; k++)
    varpanel_checkbox_add (k, d, gg);

  /*-- variable circles --*/
/*
  varcircles_add (d->ncols+ncols, d, gg);
*/

  d->ncols += ncols;
}

void delete_vars (gint *cols, gint ncols, datad *d, ggobid *gg)
{

}
