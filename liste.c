#ifndef __TREMEAU_LISTE_C__
#define __TREMEAU_LISTE_C__

extern int ERROR;

void initList(list_t * plist) {
	plist->key = plist->value = 0;
}

int insert(list_t * plist, char * nom, int note) {
	int ret = 0;
	cell_t * prec = 0, *cour =0;
	cell_t * cell = (cell_t * )malloc(sizeof(cell_t));

	if (cell) {
		strcpy(cell->key, nom);
		cell->value = note;
		cell->nk = cell->nv = 0;
		LOG(("** INSERT %s %d\n", cell->key, cell->value));

		prec = 0;
		cour = plist->key;
		while (cour &&  strcmp(cell->key, cour->key)>0) {
			prec = cour;
			cour = cour->nk;
		}
		if (prec) {
			cell->nk = prec->nk; 
			prec->nk = cell;
		} else {
			cell->nk = plist->key;
			plist->key = cell;
		}
		prec = 0;
		cour = plist->value;
		while (cour &&  cell->value < cour->value) {
			prec = cour;
			cour = cour->nv;
		}
		if (prec) {
			cell->nv = prec->nv; 
			prec->nv = cell;
		} else {
			cell->nv = plist->value;
			plist->value = cell;
		}
		ret = 1;
	} else ERROR = ERROR_LIST_ALLOC;

	return ret;
}

void displayByKey(list_t list) {
	cell_t * cour = list.key;

	printf("by key\n");
	while (cour) {
		printf("%s %d\n", cour->key, cour->value);
		cour = cour->nk;
	}
}

void displayByValue(list_t list) {
	cell_t * cour = list.value;

	printf("by value\n");
	while (cour) {
		printf("%s %d\n", cour->key, cour->value);
		cour = cour->nv;
	}
}

void freeList(list_t *plist) {
	plist-> key = plist-> value = NULL;
}

#endif
