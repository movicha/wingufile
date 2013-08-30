#ifndef PACK_DIR_H
#define PACK_DIR_H

/* Pack a wingufile directory to a zipped archive, saved in a temporary file.
   Return the path of this temporary file.
 */
char *pack_dir (const char *dirname,
                const char *root_id,
                SeafileCrypt *crypt,
                gboolean is_windows);
#endif
