/*-- sphere.c --*/

#include <math.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include "vars.h"
#include "externs.h"

extern void sphere_variance_set (gfloat x, datad *, ggobid*);


/*-------------------------------------------------------------------------*/
/*      dynamic memory allocation routines                                 */
/*-------------------------------------------------------------------------*/

void
sphere_init (datad *d) {
  vectori_init (&d->sphere.vars);
  vectori_init (&d->sphere.pcvars);
  vectorf_init (&d->sphere.eigenval);

  arrayf_init (&d->sphere.eigenvec);
  arrayf_init (&d->sphere.vc);

  vectorf_init (&d->sphere.tform_mean);
}

void
sphere_free (datad *d) {
  /*-- don't free d->sphere.pcvars, because I need it to check history --*/

  vectori_free (&d->sphere.vars);
  vectorf_free (&d->sphere.eigenval);

  arrayf_free (&d->sphere.eigenvec, 0, 0);
  arrayf_free (&d->sphere.vc, 0, 0);

  vectorf_free (&d->sphere.tform_mean);
}

void
sphere_malloc (gint nc, datad *d, ggobid *gg) 
{
  if (d->sphere.vars.nels != 0)
    sphere_free (d);

  if (nc > 0) {
    vectori_alloc_zero (&d->sphere.vars, nc);
    vectorf_alloc_zero (&d->sphere.eigenval, nc);

    arrayf_alloc_zero (&d->sphere.eigenvec, nc, nc);
    arrayf_alloc_zero (&d->sphere.vc, nc, nc);

    vectorf_alloc_zero (&d->sphere.tform_mean, nc);
  }
}

/*-------------------------------------------------------------------------*/
/*         test the number of variables sphered last time                  */
/*-------------------------------------------------------------------------*/

void
variable_set_label (datad *d, gint j, gchar *lbl) 
{
  extern void vartable_collab_set_by_var (gint j, datad *d);
  extern void varcircle_label_set (gint jvar, datad *d);

  d->vartable[j].collab = g_strdup (lbl);
  d->vartable[j].collab_tform = g_strdup (lbl);
  vartable_collab_set_by_var (j, d);
  varlabel_set (j, d);         /*-- checkboxes --*/
  varcircle_label_set (j, d);  /*-- variable circles --*/
}

/*
 * before sphering 
 * if (npcvars > 0 && npcvars != npcs) {
 *   delete the variables in sphere.pcvars (or just as many as needed?)
 *   add npcs new variables (or ditto?); populate pcvars 
 * } else if (sphere.npcvars == 0) {
 *   add npcs new variables; populate pcvars
 * }
*/
void
spherize_set_pcvars (datad *d, ggobid *gg)
{
  gint ncols_prev = d->ncols;
  gint j, k;
  gchar *lbl;
  /*-- for updating the clist --*/
  GtkCList *clist = GTK_CLIST (gg->sphere_ui.clist);
  gchar *row[] = {""};
  /*-- --*/

/*g_printerr ("npcs=%d\n", d->sphere.npcs);*/
  if (d->sphere.npcs == 0)
    return;

  /*-- the null case: sphering for the first time --*/
  if (d->sphere.pcvars.els == NULL || d->sphere.pcvars.nels == 0) {
    vectori_realloc (&d->sphere.vars_sphered, d->sphere.vars.nels);
    vectori_copy (&d->sphere.vars, &d->sphere.vars_sphered);  /* from, to */

    vectori_realloc (&d->sphere.pcvars, d->sphere.npcs);
    clone_vars (d->sphere.vars.els, d->sphere.npcs, d, gg);

    for (j=ncols_prev, k=0; j<d->ncols; j++) {
      d->sphere.pcvars.els[k++] = j;
    }

/*
 * sphering after the first time
*/

  /*-- if the number hasn't changed --*/
  } else if (d->sphere.pcvars.nels == d->sphere.npcs) {

    if (d->sphere.vars_sphered.nels != d->sphere.vars.nels)
      vectori_realloc (&d->sphere.vars_sphered, d->sphere.vars.nels);
    vectori_copy (&d->sphere.vars, &d->sphere.vars_sphered);  /* from, to */

  /*-- if the number has grown --*/
  } else if (d->sphere.pcvars.nels < d->sphere.npcs) {
    /*-- add just the additional required variables? --*/

    /*-- try deleting them all and starting fresh? --*/
    delete_vars (d->sphere.pcvars.els, d->sphere.pcvars.nels, d, gg);
    ncols_prev = d->ncols;

    vectori_realloc (&d->sphere.vars_sphered, d->sphere.vars.nels);
    vectori_copy (&d->sphere.vars, &d->sphere.vars_sphered);  /* from, to */

    vectori_realloc (&d->sphere.pcvars, d->sphere.npcs);
    clone_vars (d->sphere.vars.els, d->sphere.npcs, d, gg);

    for (j=ncols_prev, k=0; j<d->ncols; j++) {
      d->sphere.pcvars.els[k++] = j;
    }

  /*-- if the number has decreased --*/
  } else if (d->sphere.pcvars.nels > d->sphere.npcs) {
    /*-- delete the last few variables --*/
    gint ncols = d->sphere.pcvars.nels - d->sphere.npcs;
    gint *cols = (gint *) g_malloc (ncols * sizeof (gint));
    for (j=d->sphere.pcvars.nels-1, k=ncols-1; j>=d->sphere.npcs; j--)
      cols[k--] = d->sphere.pcvars.els[j];
    delete_vars (cols, ncols, d, gg);

    /*-- then behave as above, when the lengths were the same --*/
    if (d->sphere.vars_sphered.nels != d->sphere.vars.nels)
      vectori_realloc (&d->sphere.vars_sphered, d->sphere.vars.nels);
    vectori_copy (&d->sphere.vars, &d->sphere.vars_sphered);  /* from, to */

    g_free (cols);
  }

  for (k=0; k<d->sphere.pcvars.nels; k++) {
    j = d->sphere.pcvars.els[k];
    lbl = g_strdup_printf ("PC%d", (k+1));
    variable_set_label (d, j, lbl);
    g_free (lbl);
  }

  /*-- clear the clist --*/
  gtk_clist_clear (clist);
  /*-- add the new labels to the clist --*/
  gtk_clist_freeze (clist);
  for (j=0; j<d->sphere.vars_sphered.nels; j++) {
    row[0] = g_strdup (d->vartable[d->sphere.vars_sphered.els[j]].collab);
    gtk_clist_append (clist, row);
    g_free (row[0]);
  }
  gtk_clist_thaw (clist);
}

/*-------------------------------------------------------------------------*/
/*      get and set the number of principal components;                    */
/*      changed when the spin button is reset                              */
/*-------------------------------------------------------------------------*/

void pca_diagnostics_set (datad *d, ggobid *gg) {
/*
 * Compute and set the total variance and the condition number
 * of the principal components analysis
*/
  gint j;
  gfloat ftmp1=0.0, ftmp2=0.0;
  gfloat firstpc, lastpc;

  g_assert (d->sphere.npcs > 0);
  g_assert (d->sphere.eigenval.nels >= d->sphere.npcs);

  firstpc = d->sphere.eigenval.els[0];
  lastpc = d->sphere.eigenval.els[d->sphere.npcs-1];

  for (j=0; j<d->sphere.npcs; j++)
    ftmp1 += d->sphere.eigenval.els[j];
  for (j=0; j<d->sphere.vars.nels; j++)
    ftmp2 += d->sphere.eigenval.els[j];

  if (ftmp2 != 0)
    sphere_variance_set (ftmp1/ftmp2, d, gg);
  if (lastpc != 0)
    sphere_condnum_set (firstpc/lastpc, gg);
}

void
sphere_npcs_set (gint n, datad *d, ggobid *gg)
{
  d->sphere.npcs = n;
  if (!scree_mapped_p (gg)) {
    return;
  }

  if (d->sphere.npcs<1) {
     quick_message ("Need to choose at least 1 PC.", false);
     sphere_enable (false, gg);

  } else if (d->sphere.npcs > d->sphere.vars.nels) {
     gchar *msg = g_strdup_printf ("Need to choose at most %d PCs.\n",
       d->sphere.vars.nels);
     quick_message (msg, false);
     sphere_enable (false, gg);
     g_free (msg);

  } else {
    pca_diagnostics_set (d, gg);
    sphere_enable (true, gg);
  }
}
gint
npcs_get (datad *d, ggobid *gg)
{
  return d->sphere.npcs;
}

/*-------------------------------------------------------------------------*/
/*    get and set the sphered variables:                                   */
/*    the user chooses the first npcs of these as principal components     */
/*-------------------------------------------------------------------------*/

void
spherevars_set (datad *d, ggobid *gg) {
  extern void vars_stdized_send_event (datad *d, ggobid *gg);
  extern void sphere_npcs_range_set (gint n, ggobid *gg);
  gint *cols = (gint *) g_malloc (d->ncols * sizeof (gint));
  gint j, ncols;

  ncols = selected_cols_get (cols, d, gg);
  if (ncols == 0)
    ncols = plotted_cols_get (cols, d, gg);

  if (d->sphere.vars.els == NULL || d->sphere.vars.nels != ncols) {
    sphere_malloc (ncols, d, gg);
  }

  for (j=0; j<ncols; j++)
    d->sphere.vars.els[j] = cols[j];

  /*-- update the "vars stdized?" text entry --*/
  vars_stdized_send_event (d, gg);

  /*-- reset the spinner so that its max is the number of sphered vars --*/
  sphere_npcs_range_set (ncols, gg);

  g_free (cols);
}

/*-------------------------------------------------------------------------*/
/*    eigenvector and eigenvalues routines                                 */
/*-------------------------------------------------------------------------*/

void eigenvals_get (gfloat *els, datad *d)
{
  gint j;
  for (j=0; j<d->sphere.vars.nels; j++)
    els[j] = d->sphere.eigenval.els[j];
}

void
eigenval_zero (datad *d)
{
  vectorf_zero (&d->sphere.eigenval);
}

void
eigenvec_zero (datad *d, ggobid *gg)
{
  arrayf_zero (&d->sphere.eigenvec);
}

/*
 * this routine overwrites the var-cov matrix of the old active variables
 * with the new active variables. then this matrix is put through
 * singular value decomposition and overwritten with the matrix of
 * eigenvectors, and the eigenvalues are returned in a.
*/
void
eigenvec_set (datad *d, ggobid *gg)
{
  gint i, j;
  gint nels = d->sphere.vars.nels;
  gfloat **eigenvec = d->sphere.eigenvec.vals;
  gfloat **vc = d->sphere.vc.vals;

  for (i=0; i<nels; i++)
    for (j=0; j<nels; j++)
      eigenvec[i][j] = vc[i][j];
}


/*-------------------------------------------------------------------------*/
/*       variance-covariance matrix routines                               */
/*-------------------------------------------------------------------------*/

void
sphere_varcovar_set (datad *d, ggobid *gg)
{
  gint i, j, k, m, var;
  gfloat tmpf = 0.;
  gint n = d->nrows_in_plot;
  gfloat *tform_mean = d->sphere.tform_mean.els;

  for (k=0; k<d->sphere.vars.nels; k++) {
    var = d->sphere.vars.els[k];

    g_assert (var < d->ncols);
    g_assert (k < d->sphere.tform_mean.nels);

/*
 * This may not be necessary:  isn't tform_mean already
 * stored in vartabled?  dfs ...  Yes, but Andreas thinks
 * maybe it shouldn't be.
*/
    tmpf = 0.;
    for (i=0; i<n; i++)
      tmpf += d->tform.vals[d->rows_in_plot[i]][var];
    tform_mean[k] = tmpf / ((gfloat)n);

    tmpf = 0.;
    for (j=0; j<d->sphere.vc.ncols; j++) {
      for (m=0; m<n; m++) {
        i = d->rows_in_plot[m];
        tmpf = tmpf +
          (d->tform.vals[i][var] - tform_mean[k]) *
          (d->tform.vals[i][j] - tform_mean[j]);
      }
      tmpf /= ((gfloat)(n - 1));
      d->sphere.vc.vals[k][j] = d->sphere.vc.vals[j][k] = tmpf;
    }
  }
}

/*
 * returns true if the nxn variance-covariance matrix 'matrx'
 * is within tol of being equal to the identity matrix
*/
gboolean
vc_identity_p (gfloat **matrx, gint n)
{
  gint i, j;
  gboolean retn_val = true;
  gfloat tol=0.001;

  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      if ((i==j) && (fabs ((gdouble)(1.000-matrx[i][j])) > tol)) {
        retn_val = false;
        break;
      }
      else if ((i!=j) && (fabs ((gdouble)matrx[i][j]) > tol)) {
        retn_val = false;
        break;
      }
    }
  }
  return (retn_val);
}

/*-------------------------------------------------------------------------*/
/*       Here's the beef:  sphering routines                               */
/*-------------------------------------------------------------------------*/

/*
 * As long as eigenvec is not equal to the identity matrix, 
 * perform a singular value decomposition.
*/
gboolean sphere_svd (datad *d, ggobid *gg)
{
  gint i, j, k, rank;
  gint nels = d->sphere.vars.nels;
  gfloat **eigenvec = d->sphere.eigenvec.vals;
  gfloat *eigenval = d->sphere.eigenval.els;

  gboolean vc_equals_I = vc_identity_p (eigenvec, nels);
  paird *pairs = (paird *) g_malloc (nels * sizeof (paird));
  /*-- scratch vector and array --*/
  gfloat *e = (gfloat *) g_malloc (nels * sizeof (gfloat));
  gfloat **b = (gfloat **) g_malloc (nels * sizeof (gfloat *));

  for (j=0; j<nels; j++)
    b[j] = (gfloat *) g_malloc0 (nels * sizeof (gfloat));

  if (!vc_equals_I) {
    eigenval_zero (d);  /*-- zero out the vector of eigenvalues --*/
    dsvd (eigenvec, nels, nels, d->sphere.eigenval.els, b);
    for (j=0; j<nels; j++) {
      eigenval[j] = (gfloat) sqrt ((gdouble) eigenval[j]);
    }
  }

  /*-- obtain ranks to use in sorting eigenvals and eigenvec --*/
  for (i=0; i<d->sphere.vars.nels; i++) {
    pairs[i].f = (gfloat) eigenval[i];
    pairs[i].indx = i;
  }
  qsort ((gchar *) pairs, nels, sizeof (paird), pcompare);

  /*-- sort the eigenvalues and eigenvectors into temporary arrays --*/
  for (i=0; i<nels; i++) {
    k = (nels - i) - 1;  /*-- to reverse the order --*/
    rank = pairs[i].indx;
    e[k] = eigenval[rank];
    for (j=0; j<nels; j++)
      b[j][k] = eigenvec[j][rank];
  }

  /*-- copy the sorted eigenvalues and eigenvectors back --*/
  for (i=0; i<nels; i++) {
    eigenval[i] = e[i];
    for (j=0; j<nels; j++)
      eigenvec[j][i] = b[j][i];
  }

  /*-- free temporary variables --*/
  g_free ((gpointer) pairs);
  for (j=0; j<nels; j++)
    g_free (b[j]);
  g_free (b);
  g_free (e);

  return (!vc_equals_I);
}

/*-- sphere data from svars[] into pcvars[] --*/
void
spherize_data (vector_i *svars, vector_i *pcvars, datad *d, ggobid *gg)
{
  gint i, j, k, m;
  gfloat tmpf;
  gfloat *b = (gfloat *) g_malloc (svars->nels * sizeof (gfloat));

  gfloat *tform_mean = d->sphere.tform_mean.els;
  gfloat **eigenvec = d->sphere.eigenvec.vals;
  gfloat *eigenval = d->sphere.eigenval.els;

  for (m=0; m<d->nrows_in_plot; m++) {
    i = d->rows_in_plot[m];

    for (j=0; j<pcvars->nels; j++) {
      tmpf = 0.;
      for (k=0; k<svars->nels; k++) {
        tmpf = tmpf + eigenvec[k][j] *
          (d->tform.vals[i][svars->els[k]] - tform_mean[svars->els[k]]);
      }
      b[j] = tmpf / eigenval[j]; 
    }
    for (j=0; j<pcvars->nels; j++)
      d->tform.vals[i][pcvars->els[j]] = b[j];
  }

  g_free (b);
}

/*-------------------------------------------------------------------------*/
/*          Principal components analysis routines:                        */
/*  executed when the sphere button is pressed and the scree plot opened   */
/*-------------------------------------------------------------------------*/

gboolean
pca_calc (datad *d, ggobid *gg) {
  gboolean svd_ok;

  eigenvec_zero (d, gg);
/*spherevars_set (d, gg);*/

  sphere_varcovar_set (d, gg);
  
   /* If nspherevars > 1 use svd routine, otherwise just standardize */
  if (d->sphere.vars.nels > 1) {
    eigenvec_set (d, gg);
    svd_ok = sphere_svd (d, gg);
  }

  return svd_ok;
}
