#include "libpq-fe.h"
#include "ggobi.h"

#include "GGobiAPI.h"

#include "dbms_ui.h"

#include <stdlib.h>
#include <string.h>

gboolean postgres_read(InputDescription *desc, ggobid *gg);
int read_postgres_data(DBMSLoginInfo *info, gboolean init, ggobid *gg);

PGresult *query(const char * const query, PGconn *conn);
int processResult(PGresult *result, ggobid *gg);
PGconn* makeConnection(DBMSLoginInfo *info);

/**
  This creates and populates an InputDescription object
  which will cause GGobi to use the routines in this file
  to read data. Specifically, GGobi will call postgres_read()
  below which will open a GUI to get the DBMS inputs from the user,
  including the login information, query for the data, etc.

  @see postgres_read.
 */
InputDescription *
postgres_input_description(const char * const fileName, const char * const modeName, 
                             ggobid *gg, GGobiInputPluginInfo *info)
{
  InputDescription *desc;
  desc = (InputDescription*) g_malloc(sizeof(InputDescription));
  memset(desc, '\0', sizeof(InputDescription));

  desc->fileName = g_strdup("Postgres table");
  desc->mode = unknown_data;
  desc->read_input = postgres_read;

  return(desc);
}

/**
 This is the initial entry point for reading data from 
 Postgres. This only performs the initial setup, specifically
 requesting the inputs from the user for parameterizing
 the connection and the SQL queries for the different
 data information. It does this by using the DBMS GUI
 input routines in GGobi.
 */
gboolean 
postgres_read(InputDescription *desc, ggobid *gg)
{
    DBMSLoginInfo *info ;
    info = initDBMSLoginInfo(NULL);
     /* We would read these values from a file. */

    info->dataQuery = g_strdup("select  width1, width2, maxheadwidth  from flea;");

    info->desc = desc;
    info->read_input = read_postgres_data;

    GGOBI(get_dbms_login_info)(info, gg);

    return(false);
}

/**
 This is the routine that actually reads the data, using the inputs
 from the user gathered by the GUI into the `info' structure.
 */
int
read_postgres_data(DBMSLoginInfo *info, gboolean init, ggobid *gg)
{
    PGconn *conn;
    PGresult *result;

    conn = makeConnection(info);    
    if(!conn) {
	return(-1);
    }

    result = query(info->dataQuery, conn);
    processResult(result, gg);
    PQclear(result);
    PQfinish(conn);
    
    start_ggobi(gg, true, init);
 
    return(1);
}


PGconn*
makeConnection(DBMSLoginInfo *info)
{
  PGconn *con;
  char port[10];
  if(info->port > 0) {
      sprintf(port, "%d", info->port);
  }
  con = PQsetdbLogin(info->host, info->port > 0 ? port : NULL, NULL, NULL, info->dbname, info->user, info->password);

  return(con);
}

PGresult *
query(const char * const query, PGconn *conn)
{
    PGresult *result;
    ExecStatusType status;
    char *msg;

    result = PQexec(conn, query);

    status = PQresultStatus(result);
    msg = PQresultErrorMessage(result);
    if(msg && msg[0]) {
	fprintf(stderr, "Error from query %s: %s\n", query, msg);fflush(stderr);
    }
    return(result);
}

int
processResult(PGresult *result, ggobid *gg)
{
  int i, j;
  int nr, nc;
  datad *d;

  nr = PQntuples(result);
  nc = PQnfields(result);
 
  d = datad_create(nr, nc, gg);

  for(i = 0; i < nr; i++) {
      float f;
      char *tmp;
      char *l;
      char buf[10];
      sprintf(buf, "%d", 1 + i);
      l = g_strdup(buf);
      g_array_append_val (d->rowlab, l);

      for(j = 0; j < nc; j++) {
	  if(i == 0) {
	  tmp = PQfname(result, j);
#if 0
	  vartabled *vt;
	  vt = vartable_element_get (j, d);
	  vt->collab = g_strdup(tmp);
	  vt->collab_tform = g_strdup(tmp);
#else
	  GGOBI(setVariableName)(j, g_strdup(tmp), false, d, gg);
#endif
	  }

	  tmp  = PQgetvalue(result, i, j);
          if(tmp)
	      f = atof(tmp);
	  else
	      f = 0;
	  d->raw.vals[i][j] = f;
      }
  }

  return(1);
}

#ifdef STANDALONE
int
main(int argc, char *argv[])
{
  PGconn *conn;
  PGresult *result;
  float *data;
  int nr, nc;
  DataFrame frame;
  DBMSLoginInfo info;
  memset(&info, '\0', sizeof(DBMSLoginInfo));

    conn = makeConnection(&info);
    result = query(conn);
    processResult(result, &frame);

    fprintf(stderr, "Result: %d, %d\n", frame.nr, frame.nc);
    return(0); 
}
#endif