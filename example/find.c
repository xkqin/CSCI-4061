/*===============================================================
 *   
 *   find.c - find all files residing in the given sub-directory,
 *   whose names contain the given string.
 *   
 ================================================================*/

#include <stdio.h>// Standard input/output routines
#include <stdlib.h>// Standard library routines
#include <dirent.h>// readdir(), etc.
#include <sys/stat.h>// stat(), etc.
#include <string.h>// strstr(), etc.
#include <unistd.h>// getcwd(), etc.

#define MAX_DIR_PATH 2048// Maximal full path length we support

/*===========================================================================
 *   
 *   Function: findfile recusively traverses the current directory, searching
 *   for files with a given string in their name. Hence strstr().
 *   Input:    String to match.
 *   Output:   Any file found, printed to the screen with a full path.
 *   
 ============================================================================*/

void
findfile (char *pattern)
{
  DIR *dir;// Pointer to the scanned directory
  struct dirent *entry;//  Pointer to one directory entry
  char cwd[MAX_DIR_PATH + 1];// Current working directory
  struct stat dir_stat;// Used by stat()

  // First, save the path of current working directory

  if (!getcwd (cwd, MAX_DIR_PATH + 1))
    {
      perror ("getcwd:");
      return;
    }


  // Open the directory to read

  dir = opendir (".");
  if (!dir)
    {
      fprintf (stderr, "Cannot read directory '%s': ", cwd);
      perror ("");
      return;
    }

  /*=======================================================
   * 
   * Scan the directory, traversing each sub-directory, and 
   * matching the pattern for each file / directory name. 
   *
   ========================================================*/

  while ((entry = readdir (dir)))
    {
      // Check if the pattern matches

      if (entry->d_name && strstr (entry->d_name, pattern))
	{
	  printf ("%s/%s\n", cwd, entry->d_name);
	}

      // Check if the given entry is a directory

      if (stat (entry->d_name, &dir_stat) == -1)
	{
	  perror ("stat:");
	  continue;
	}

      // Skip the "." and ".." entries, to avoid loops

      if (strcmp (entry->d_name, ".") == 0)
	continue;

      if (strcmp (entry->d_name, "..") == 0)
	continue;

      /* Is this a directory? */
      if (S_ISDIR (dir_stat.st_mode))
	{
	  /* Change into the new directory */
	  if (chdir (entry->d_name) == -1)
	    {
	      fprintf (stderr, "Cannot chdir into '%s': ", entry->d_name);
	      perror ("");
	      continue;
	    }

	  /* check this directory */
	  findfile (pattern);

	  /* Finally, restore the original working directory. */

	  if (chdir ("..") == -1)
	    {
	      fprintf (stderr, "Cannot chdir back to '%s': ", cwd);
	      perror ("");
	      exit (1);
	    }
	}
    }
} // End findfile()

/*======================================================================
 *
 *   Function: main. find files with a given pattern in their names, in
 *   a given directory tree.
 *   Input:    Path to directory to search, and pattern to match.
 *   Output:   Names of all matching files on the screen.
 *   
 =======================================================================*/

int
main (int argc, char **argv)
{
  char *dir_path;// Path to the directory
  char *pattern;// Pattern to match
  struct stat dir_stat;// Used by stat()

  // Read command line arguments

  if (argc != 3 || !argv[1] || !argv[2])
    {
      fprintf (stderr, "Usage: %s <directory path> <file name string>\n", argv[0]);
      fprintf (stderr, "This program finds all files with name containing the given string.  Does not handle wildcards. \n", argv[0]);
      exit (1);
    }

  dir_path = argv[1];
  pattern = argv[2];

  // Make sure the given path refers to a directory

  if (stat (dir_path, &dir_stat) == -1)
    {
      perror ("stat:");
      exit (1);
    }

  if (!S_ISDIR (dir_stat.st_mode))
    {
      fprintf (stderr, "'%s' is not a directory\n", dir_path);
      exit (1);
    }

  // Change into the given directory

  if (chdir (dir_path) == -1)
    {
      fprintf (stderr, "Cannot change to directory '%s': ", dir_path);
      perror ("");
      exit (1);
    }

  // Recursively scan the directory for the given file name pattern

  findfile (pattern);

} // End main()
