#ifndef __TREMEAU_HISTO_C__
#define __TREMEAU_HISTO_C__

extern int ERROR;

void computeHisto(histogram_t h, list_t l) {
	int i = 0;
	int j = 0;
	cell_t * cour = l.key;

	for (i=0; i < HISTOSIZE ; ++i)
		h[i] = 0.0;

	while (cour) {
		++h[cour->value];
		cour = cour->nk;
		++j;
	}
} 

void displayHisto(histogram_t h) {
	int i = 0;

	for (i=0; i < HISTOSIZE ; ++i)
		printf("[%2d] : %3d\n", i, h[i] );
}

int maxHisto(histogram_t h) {
	int i, max = h[0];

	for(i=1; i< HISTOSIZE; ++i)
		if (h[i]>max) max=h[i];

	return max;
}

float meanHisto(histogram_t h) {
	float som =0.;
	int   tot = 0, i;

	for (i=0; i< HISTOSIZE; ++i) {
		som  = h[i] * i;
		tot += h[i];
	}
	LOG(("MEAN %f / %d\n", som, tot));

	return som/(float)tot;
}

int countHisto(histogram_t h) {
	int   tot = 0, i;
	for (i=0; i< HISTOSIZE; ++i) {
		tot += h[i];
	}
	LOG(("TOT %d\n", tot));

	return tot;
}


#endif
