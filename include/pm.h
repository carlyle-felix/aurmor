#ifndef PM_H
#define PM_H

typedef struct node List;

List *foreign_list(void);
void alpm_uninstall(List *pkglist);

#endif 