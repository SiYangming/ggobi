#define GGOBIINTERN

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "vars.h"
#include "externs.h"

ggobid **all_ggobis;
int num_ggobis;

static gchar *version_date = "July 1, 2000";

const gchar * const ViewTypes[] =
  {"Scatterplot", "Scatterplot Matrix", "Parallel Coordinates"};
const gint ViewTypeIndices[] = {scatterplot, scatmat, parcoords};           
const gchar *const ModeNames[] =
  {"ASCII", "R/S data", "binary", "XML", "MySQL"};

gint
parse_command_line (gint *argc, gchar **av, ggobid *gg)
{
  gboolean stdin_p = false;
  

/*
 * Now parse the command line.
*/
  for ( ; *argc>1 && av[1][0]=='-'; (*argc)--,av++) {
    /*
     * -s:  ggobi initiated from inside S
    */
    if (strcmp (av[1], "-s") == 0)
      gg->data_mode = Sprocess;

#ifdef USE_XML
    else if (strcmp (av[1], "-xml") == 0)
      gg->data_mode = xml;
    else if (strcmp (av[1], "-v") == 0 || strcmp (av[1], "--validate") == 0) {
      extern int xmlDoValidityCheckingDefaultValue;
      xmlDoValidityCheckingDefaultValue = 1;
    }
#endif
#ifdef USE_MYSQL
    else if (strcmp (av[1], "-mysql") == 0) {
      gg->data_mode = mysql;
    }
#endif

    /*
     * -:  look to stdin for the input data
    */
    else if (strcmp (av[1], "-") == 0) {
      stdin_p = true;
    }

    /*
     * -version:  print version date, return
    */
    else if (strcmp (av[1], "-version") == 0) {
      g_printerr ("This version of GGobi is dated %s\n", version_date);
      exit (0);
    }

  }

  (*argc)--;
  av++;

/*
 * Test the values
*/

  /* (gg->data_mode == ascii || gg->data_mode == binary) */
  if (*argc == 0)
    gg->data_in = (stdin_p) ? g_strdup_printf ("stdin") : NULL;
  else
    gg->data_in = g_strdup_printf (av[0]);

  return 1;
}

gint GGOBI(main)(gint argc, gchar *argv[], gboolean processEvents);

int main (gint argc, gchar *argv[])
{ 
 GGOBI(main)(argc, argv, true);
 return (0);
}

gint
ggobi_remove (ggobid *gg)
{ 
  gint i;
  for (i = 0; i < num_ggobis; i++) {
    if (all_ggobis[i] == gg) {
      return (ggobi_remove_by_index (gg, i));
    }
  }

  return (-1);
}

/*
      Also, need to close and free the displays associated with the ggobi.
 */
gint
ggobi_remove_by_index (ggobid *gg, gint which)
{
  GSList *l;
  datad *d;
  int numDatasets, i;

  /* Move all the entries after the one being removed
     down by one in the array to compact it.
   */
  if(which < num_ggobis -1) {
    memcpy(all_ggobis + which, all_ggobis + which + 1, num_ggobis-which);
  }
  /* Now patch up the array so that it has the correct number of elements. */
  num_ggobis--;
  if(num_ggobis > 0)
    all_ggobis = (ggobid**) g_realloc (all_ggobis, sizeof(ggobid*) * num_ggobis);
  else
    all_ggobis = NULL;

  /* 
      This was crashing in R. Probably because when we exhaust the list
      and remove the final element, we get back garbage.
      This isn't a problem in stand-alone as it never gets called.
   */
  numDatasets = g_slist_length(gg->d);
  for (i=0,l = gg->d; l != NULL && i < numDatasets; i++, l = l->next) {
    d = (datad *) l->data;
    datad_free (d, gg);
    gg->d = g_slist_remove (gg->d, d);
  }

  g_free (gg);

  return (which);
}

ggobid*
ggobi_alloc()
{
 ggobid *tmp;

  tmp = (ggobid*) g_malloc (sizeof (ggobid));

  memset (tmp, '\0', sizeof (ggobid));
  tmp->xyplot.direction = FORWARD;
  tmp->xyplot.cycle_p = false;
  tmp->firsttime = true;
  tmp->brush.firsttime = true;
  tmp->mode = XYPLOT;
  tmp->prev_mode = XYPLOT;
  tmp->projection = XYPLOT;
  tmp->prev_projection = XYPLOT;

  tmp->mode_menu.firsttime_reset = tmp->mode_menu.firsttime_link = 
      tmp->mode_menu.firsttime_io =  true;

  tmp->color_ui.margin = 10;
  tmp->tour2d.idled = 0;

  all_ggobis = (ggobid **)
    g_realloc (all_ggobis, sizeof(ggobid*)*(num_ggobis+1));
  all_ggobis[num_ggobis] = tmp;
  num_ggobis++;

  return (tmp);
}

  /* Available so that we can call this from R
     without any confusion between which main().
   */
gint GGOBI (main)(gint argc, gchar *argv[], gboolean processEvents)
{
  extern void make_ggobi (gchar *, gboolean, ggobid *);
  GdkVisual *vis;
  ggobid *gg;

  gtk_init (&argc, &argv);

  vis = gdk_visual_get_system ();

  gg = ggobi_alloc();

  gg->mono_p = (vis->depth == 1 ||
               vis->type == GDK_VISUAL_STATIC_GRAY ||
               vis->type == GDK_VISUAL_GRAYSCALE);

  g_print ("progname = %s\n", g_get_prgname());

  gg->data_mode = ascii;

  parse_command_line (&argc, argv, gg);
  g_print ("data_in = %s\n", gg->data_in);

  make_ggobi (gg->data_in, processEvents, gg);

  g_free (gg->data_in);

  return (num_ggobis);
}


/*
  Called in response to a window being destroyed.
 */
void
ggobi_close(GtkObject *w, ggobid *gg)
{
  GGOBI(close)(gg, false);
}


/*
   Key for storing a reference to a ggobid instance in a widget
   so that we can retrieve it within a callback.
*/
const gchar * const GGobiGTKey = "GGobi";

const gchar* const key_get (void) {
  return GGobiGTKey;
}

/*
  Computes the ggobid pointer associated with the specified
  widget. It does so by looking in the window associated with the widget
  and then looking for an entry in the window's association table.
  This assumes that the ggobid reference was stored in the window 
  when it was created.
 */
ggobid*
GGobiFromWidget (GtkWidget *w, gboolean useWindow)
{
  /*
   GdkWindow *win = gtk_widget_get_parent_window(w);
   return(GGobiFromWindow(win));
  */
  ggobid *gg = NULL;
  gg = (ggobid*) gtk_object_get_data (GTK_OBJECT(w), GGobiGTKey);
  ValidateGGobiRef (gg, true);

  return (gg);
}

ggobid* GGobiFromWindow (GdkWindow *win)
{
  ggobid *gg = NULL;
  gg = (ggobid*) gtk_object_get_data(GTK_OBJECT(win), GGobiGTKey);
  ValidateGGobiRef (gg, true);

  return(gg);
}

ggobid* 
GGobiFromSPlot(splotd *sp)
{
 return(sp->displayptr->ggobi);
}

ggobid* 
GGobiFromDisplay(displayd *display)
{
  return(display->ggobi);
}

void
GGobi_widget_set (GtkWidget *w, ggobid *gg, gboolean asIs)
{
  GtkObject *obj;
  if (asIs)
    obj = GTK_OBJECT (w);
  else 
    obj = GTK_OBJECT (gtk_widget_get_parent_window (w));

  gtk_object_set_data (obj, GGobiGTKey, gg);
}


ggobid *
ggobi_get (gint which)
{
 extern ggobid** all_ggobis;
 return (all_ggobis[which]);
}

ggobid*
ValidateGGobiRef(ggobid *gg, gboolean fatal)
{ 
 extern ggobid** all_ggobis;
 extern int num_ggobis;
  int i;
  for(i = 0; i < num_ggobis ; i++) {
   if(all_ggobis[i] == gg)
    return(gg);
  }

  fprintf(stderr, "Incorrect reference to ggobid.");

 if (fatal)
  exit(10);

 return(NULL);
}

