#ifndef __TREMEAU_LISTE_H__
#define __TREMEAU_LISTE_H__

void initList(list_t * plist);
int insert(list_t * plist, char * nom, int note);
void displayByKey(list_t list);
void displayByValue(list_t list);
void freeList(list_t * plist);


#endif
