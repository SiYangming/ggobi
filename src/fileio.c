/* fileio.c */
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

#include "ggobi.h"

#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
#include <unistd.h>
#include <sys/stat.h>
#else
#include <glib.h>
# ifdef __STRICT_ANSI__
# undef   __STRICT_ANSI__
# endif
# include <io.h>
#endif

#include <string.h>
#include <ctype.h>

#include "GGobiAPI.h"
#include "plugin.h"

static gchar *XMLSuffixes[] = { "xml", "xml.gz", "xmlz" };
gchar *ExcelSuffixes[] = { "asc", "csv", "txt" };

ExtensionList xmlFileTypes = {
  xml_data,
  NULL,
  0
};

ExtensionList excelFileTypes = {
  csv_data,
  NULL,
  0
  };

GSList *FileTypeGroups = NULL;

/*--------------------------------------------------------------------*/
/*               Initialization                                       */
/*--------------------------------------------------------------------*/

GSList *initFileTypeGroups(void)
{
  FileTypeGroups = g_slist_alloc();

  excelFileTypes.extensions = ExcelSuffixes;
  excelFileTypes.len = 3;

  xmlFileTypes.extensions = XMLSuffixes;
  xmlFileTypes.len = 3;

  FileTypeGroups->data = (void *) &xmlFileTypes;

  g_slist_append(FileTypeGroups, &excelFileTypes);

  return (FileTypeGroups);
}

static gboolean
isUnknownInputMode(const gchar *modeName)
{
  gboolean status;
  status = !modeName || modeName == "" || strcmp(modeName, DefaultUnknownInputModeName)==0;
  return(status);
}

/*----------------------------------------------------------------------
  Initialize and populate in an InputDescription:  work out the
    mode of the data, the fully expanded name of the file, the
    base name, the name of its directory, ...
----------------------------------------------------------------------*/

InputDescription *
fileset_generate(const gchar * fileName,
		 const gchar *modeName, 
		 GGobiPluginInfo *plugin, 
		 ggobid * gg)
{
  InputDescription *desc;
  DataMode guess = unknown_data;
#ifndef WIN32
  struct stat buf;
#else
  gint ft = 0;
#endif
  gint i, j;
  gint numGroups;
  GSList *groups;
  gboolean isUnknownMode;

  if (FileTypeGroups == NULL)
    initFileTypeGroups();

  if(plugin) {
	  InputDescription *desc;
	  desc = callInputPluginGetDescription(fileName, modeName, plugin, gg);
	  if(desc)
            return(desc);
  }

  groups = FileTypeGroups;

  isUnknownMode = isUnknownInputMode(modeName);

  desc = (InputDescription *) calloc(1, sizeof(InputDescription));

  if (1) {
    GList *els = sessionOptions->info->inputPlugins;
    if (els) {
      gint i, n;
      n = g_list_length(els);
      for (i = 0; i < n; i++) {
        gboolean handlesFile = false;
        GGobiPluginInfo *oplugin;
        GGobiInputPluginInfo *info;
        oplugin = g_list_nth_data(els, i);
        info = oplugin->info.i;

         /* Use the probe only if the user has not given us a 
            specific format/plugin. */
	if(isUnknownMode) {
          if(info->probe) 
     	    handlesFile = info->probe(fileName, gg, oplugin);
	  else
            handlesFile = true;
	}

        if ((isUnknownMode && handlesFile)
	    || (modeName && oplugin && pluginSupportsInputMode(modeName, oplugin))) 
	{
          InputDescription *desc;
	  desc = callInputPluginGetDescription(fileName, modeName, oplugin, gg);
	  if(desc)
  	     return(desc);
        }
      }
    }
  }


#ifndef WIN32
  if (stat(fileName, &buf) != 0) {
#else
  if (access(fileName, ft) != 0) {
#endif
    if (isURL(fileName)) {
      desc->mode = url_data;
      desc->fileName = g_strdup(fileName);
      completeFileDesc(fileName, desc);
      return (desc);
    }

    /* Ok, so the user didn't give a full name and wants us to guess.  
       So we will look through the collections of format types and within
       each of these search through the different extensions for that type.
       If such a file exists, we assume it is of that format.
     */
    numGroups = g_slist_length(groups);
    if (isUnknownMode) {
      for (i = 0; i < numGroups; i++) {
        gchar buf[1000];
        ExtensionList *group;
        group = (ExtensionList *) g_slist_nth_data(groups, i);
        for (j = 0; j < group->len; j++) {
          if (group->extensions[j] && group->extensions[j][0])
            sprintf(buf, "%s.%s", fileName, group->extensions[j]);
          else
            sprintf(buf, "%s", fileName);

          if (check_file_exists(buf)) {
            guess = group->mode;
            desc->mode = guess;
            desc->fileName = g_strdup(buf);
            desc->baseName = g_strdup(fileName);
            desc->givenExtension = g_strdup(group->extensions[j]);
            break;
          }
        }
        if (guess != unknown_data)
          break;
      }
    } else {
      /* Attempt to find a file of this type. */
      /*     guess = unknown_data; */
      for (i = 0; i < numGroups; i++) {
        ExtensionList *group;
        group = (ExtensionList *) g_slist_nth_data(groups, i);
        if (group->mode != guess)
          continue;

        for (j = 0; j < group->len; j++) {
          gchar buf[1000];
          if (endsWith(fileName, group->extensions[j])) {
            g_printerr("%s does not exist!\n", fileName);
            return (NULL);
          }
          if (group->extensions[j] && group->extensions[j][0])
            sprintf(buf, "%s.%s", fileName, group->extensions[j]);
          else
            sprintf(buf, "%s", fileName);
          if (check_file_exists(buf)) {
            DataMode ok = verifyDataMode(buf, group->mode, desc);
            if (ok == guess && ok != unknown_data) {
              desc->fileName = g_strdup(buf);
              desc->baseName = g_strdup(fileName);
              desc->givenExtension = g_strdup(group->extensions[j]);
              desc->mode = guess;
              guess = group->mode;
              break;
            }
          }
        }
        if (desc->fileName)
          break;
      }
    }

    if (desc->fileName == NULL) {
      guess = unknown_data;
    }

  } else {
    desc->fileName = g_strdup(fileName);
    desc->mode = guess = verifyDataMode(desc->fileName, desc->mode, desc);
    if (desc->mode == unknown_data) {
	g_printerr("Cannot determine mode of data file %s\n", desc->fileName);
	return (NULL);
    }
  }

  switch (guess) {
  case unknown_data:
    g_printerr("Cannot find a suitable file %s\n", fileName);
    return (NULL);
    break;
  default:
    break;
  }

  completeFileDesc(desc->fileName, desc);
  return (desc);
}

gint addInputSuffix(InputDescription * desc, const gchar * suffix)
{
  if (desc->extensions) {
    g_slist_append(desc->extensions, g_strdup(suffix));
  } else {
    desc->extensions = g_slist_alloc();
    desc->extensions->data = g_strdup(suffix);
  }

  return (g_slist_length(desc->extensions));
}

gint addInputFile(InputDescription * desc, const gchar * file)
{
  return (addInputSuffix(desc, file));
}

gchar *completeFileDesc(const gchar * fileName, InputDescription * desc)
{
  gint n;
  gchar *tmp = strrchr(fileName, '.');

  ExtensionList *group = getInputDescriptionGroup(desc->mode);

  if (group) {
    gint i;
    const gchar *ext;
    n = strlen(fileName);
    for (i = 0; i < group->len; i++) {
      ext = group->extensions[i];
      if (endsWith(fileName, ext)) {
        gint nbytes = strlen(fileName) - strlen(ext);
        desc->baseName = (gchar *) g_malloc(sizeof(gchar) * nbytes);
        g_snprintf(desc->baseName, nbytes, "%s", fileName);
        desc->givenExtension = g_strdup(ext);
        break;
      }
    }
    if (i == group->len) {

#if 1
      if (tmp) {
        desc->givenExtension = g_strdup(tmp + 1);
        n = (tmp - fileName) + 1;
        desc->baseName = g_malloc(sizeof(char) * n);
        g_snprintf(desc->baseName, n, "%s", fileName);
      } else {
        desc->baseName = g_strdup(fileName);
      }
#else
      return (NULL);
#endif
    }
  }

  if (!desc->baseName) {
    desc->baseName = g_strdup(fileName);
  }

  /* Now compute the directory name. */
  if (desc->baseName) {
    tmp = strrchr(desc->baseName, G_DIR_SEPARATOR);
    if (tmp) {
      n = (tmp - desc->baseName) + 1;
      desc->dirName = g_malloc(sizeof(char) * n);
      g_snprintf(desc->dirName, n, "%s", desc->baseName);
    } else {
      desc->dirName = g_strdup(desc->baseName);
    }
  }
  return (tmp);
}

ExtensionList *getInputDescriptionGroup(DataMode mode)
{
  GSList *groups = FileTypeGroups;
  ExtensionList *el;
  while (groups) {
    el = (ExtensionList *) groups->data;
    if (el == NULL)
      return (NULL);

    if (el->mode == mode)
      return (el);

    groups = groups->next;
  }

  return (NULL);
}

void showInputDescription(InputDescription * desc, ggobid * gg)
{
  FILE *out = stderr;
  gint i;
  fprintf(out, "Input File Information:\n");
  fprintf(out, "\tFile name: %s  (extension: %s)\n",
          desc->fileName, desc->givenExtension);
  fprintf(out, "\tDirectory: %s\n", desc->dirName);
#if 0
  fprintf(out, "\tFormat: %s (%d), verified: %s\n",
          GGOBI(getDataModeDescription) (desc->mode), desc->mode,
          desc->canVerify ? "yes" : "no");
#endif

  if (desc->extensions) {
    fprintf(out, "Auxillary files\n");
    for (i = 0; i < g_slist_length(desc->extensions); i++) {
      fprintf(out, "  %d) %s\n",
              i, (gchar *) g_slist_nth_data(desc->extensions, i));
    }
  }
  fflush(out);
}

/*--------------------------------------------------------------------*/
/*          Determining the mode of the data                          */
/*--------------------------------------------------------------------*/

gboolean isURL(const gchar * fileName)
{
  return ((strncmp(fileName, "http:", 5) == 0 ||
           strncmp(fileName, "ftp:", 4) == 0));
}

DataMode
verifyDataMode(const gchar * fileName, DataMode mode,
               InputDescription * desc)
{
  switch (mode) {
  default:
    mode = guessDataMode(fileName, desc);
  }

  return (mode);
}

gboolean 
isXMLFile(const gchar * fileName, ggobid *gg, GGobiPluginInfo *info)
/*XXX InputDescription * desc) */
{

  FILE *f;
  gint c;
  gchar *tmp;

  if(isURL(fileName))
    return(true);


  tmp = strrchr(fileName, '.');

  if(!tmp) {
     gchar buf[256];
     sprintf(buf, "%s.xml",fileName);
     if(isXMLFile(buf, gg, info))
        return(true);
  }

  if (tmp && (strcmp(tmp, ".xmlz") == 0 || strcmp(tmp, ".gz") == 0)) {
	  /* desc->canVerify = false; */
    return (true);
  }


  f = fopen(fileName, "r");
  if (f == NULL)
    return (false);

/*  desc->canVerify = true; */
  while ((c = getc(f)) != EOF) {
    if (c == ' ' || c == '\t' || c == '\n')
      continue;
    if (c == '<') {
      gchar buf[10];
      fgets(buf, 5, f);
      fclose(f);
      if (strcmp(buf, "?xml") == 0) {
        return (true);
      } else
        return (false);
    }
  }

  return (false);
}


gboolean 
isCSVFile(const gchar * fileName, ggobid *gg, GGobiPluginInfo *plugin)
{
  char tmp[20];
  char extention[20];
  int len;
  int i,inx,inx2;
  memset(tmp,'\0',20);
  memset(extention,'\0',20);	
  len = strlen(fileName);
  inx = 0;
  for(i=len-1;i>=0;i--)
  {
    if(fileName[i] == '.')
      break;
    tmp[inx] = fileName[i];
    inx++;
  }
  tmp[inx] = '\0';
  inx2 = 0;
  for(i=inx-1;i>=0;i--)
  {
    extention[inx2] = tmp[i];
    inx2++;
  }
  if(
  (strcmp(extention, "asc") == 0) ||
  (strcmp(extention, "txt") == 0) ||
  (strcmp(extention, "csv") == 0))
    return true;
  else
    return false;
}

DataMode guessDataMode(const gchar * fileName, InputDescription * desc)
{
  FILE *f;

  f = fopen(fileName, "r");
  if (f == NULL)
    return (unknown_data);

#if 0
  if (isXMLFile(fileName, desc))
    return (xml_data);
#endif

#if 0
  if (isCSVFile(fileName))
      return (csv_data);
#endif

  return (unknown_data);
}

/*--------------------------------------------------------------------*/
/*                 Utility functions                                  */
/*--------------------------------------------------------------------*/

gboolean endsWith(const gchar * str, const gchar * what)
{
  return (strcmp(str + strlen(str) - strlen(what), what) == 0);
}


#ifdef WIN32
gboolean check_file_exists(const gchar * fileName)
{
  gint i = 0;
  return (access(fileName, i) == 0);
}
#else
gboolean check_file_exists(const gchar * fileName)
{
  struct stat buf;
  return (stat(fileName, &buf) == 0);
}
#endif
