#ifndef DIFF_SIMPLE_H
#define DIFF_SIMPLE_H

#include <glib.h>
#include "index/index.h"
#include "wingufile-session.h"

#define DIFF_TYPE_WORKTREE              'W' /* diff from index to worktree */
#define DIFF_TYPE_INDEX                 'I' /* diff from commit to index */
#define DIFF_TYPE_COMMITS               'C' /* diff between two commits*/

#define DIFF_STATUS_ADDED               'A'
#define DIFF_STATUS_DELETED             'D'
#define DIFF_STATUS_MODIFIED	        'M'
#define DIFF_STATUS_RENAMED             'R'
#define DIFF_STATUS_UNMERGED		'U'
#define DIFF_STATUS_DIR_ADDED           'B'
#define DIFF_STATUS_DIR_DELETED         'C'

enum {
    STATUS_UNMERGED_NONE,
    /* I and others modified the same file differently. */
    STATUS_UNMERGED_BOTH_CHANGED,
    /* I and others created the same file with different contents. */
    STATUS_UNMERGED_BOTH_ADDED,
    /* I removed a file while others modified it. */
    STATUS_UNMERGED_I_REMOVED,
    /* Others removed a file while I modified it. */
    STATUS_UNMERGED_OTHERS_REMOVED,
    /* I replace a directory with a file while others modified files under the directory. */
    STATUS_UNMERGED_DFC_I_ADDED_FILE,
    /* Others replace a directory with a file while I modified files under the directory. */
    STATUS_UNMERGED_DFC_OTHERS_ADDED_FILE,
};

typedef struct DiffEntry {
    char type;
    char status;
    int unmerge_state;
    unsigned char sha1[20];     /* used for resolve rename */
    char *name;
    char *new_name;             /* only used in rename. */
} DiffEntry;

DiffEntry *
diff_entry_new (char type, char status, unsigned char *sha1, const char *name);

void
diff_entry_free (DiffEntry *de);

int
diff_index (struct index_state *istate, SeafDir *root, GList **results);

int
diff_commits (SeafCommit *commit1, SeafCommit *commit2, GList **results);

int
diff_merge (SeafCommit *merge, GList **results);

void
diff_resolve_renames (GList **diff_entries);

void
diff_resolve_empty_dirs (GList **diff_entries);

int 
diff_unmerged_state(int mask);

char *
format_diff_results(GList *results);

#endif
