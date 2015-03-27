#include "cachedef.h";
// cache definition variables
int sdmi, sdmd, sfa, ssa;// lsdmi, lsdmd, lsfa, lssa;
int nbdmi, nbdmd, nbfa, nbsa;
// cache arrays
carr *DM_I, *DM_D, *FA;
carr **SA2W, **SA4W, **SA8W;

int cache_size = 0;
int cache_line_size = 0;

int define_cache_size(int cs)
{
	int aux = 1;

	cache_size = cs;

	if(cache_size > 2048)
		return 0;


	cache_size = cache_size*1024;
	// convert the cache_size to binary exponential.
	do
	{
		aux =  aux*2;
	}while(aux - cache_size < 0);
	cache_size = aux;


	if(cache_size%8 >= 1)
		return 0; // fail printf("ERRO>\tValor nao atende os requisitos da arquitetura. O valor precisa ser multiplo de oito. Tente novamente.\n\n");
	else
	{
		sfa = cache_size*2; // FA
		sdmi = sdmd = ssa = cache_size/8; // DM, SA
	}
	return 1; // accept
}

int define_cache_line_size(int cls)
{
	int i, j;

	cache_line_size = cls;
	if(cache_line_size > 1024)
		return 0;

	if(cache_line_size%2 != 0 || cache_line_size > cache_size/64)
		return (cache_size/64); //printf("ERRO>\tValor nao atende os requisitos. O valor deve ser multiplo de dois e deve ser menor ou igual %d bytes.\nTente novamente.\n\n", cache_size/64);


	nbdmi = (sdmi/cache_line_size); // DM I
	nbdmd = (sdmd/cache_line_size); // DM D
	nbfa = (sfa/cache_line_size); // FA
	nbsa = ssa/cache_line_size; // SA

	DM_I = (struct carr*)malloc(sizeof(struct carr)*nbdmi);
	for(i = 0; i<nbdmi; i++)
	{
		DM_I[i].bv = 0;
		DM_I[i].r = 0;
		DM_I[i].d = 0;
		DM_I[i].nature = NULL;
		DM_I[i].tag = NULL;
	}

	DM_D = (struct carr*)malloc(sizeof(struct carr)*nbdmd);
	for(i = 0; i<nbdmd; i++)
	{
		DM_D[i].bv = 0;
		DM_D[i].r = 0;
		DM_D[i].d = 0;
		DM_D[i].nature = NULL;
		DM_D[i].tag = NULL;
	}

	FA = (struct carr*)malloc(sizeof(struct carr)*nbfa);
	for(i = 0; i<nbfa; i++)
	{
		FA[i].bv = 0;
		FA[i].r = 0;
		FA[i].d = 0;
		FA[i].nature = NULL;
		FA[i].tag = NULL;
	}

	SA2W = (struct carr**)malloc(sizeof(struct carr*)*(nbsa/2));
	for(i = 0; i < (nbsa/2); i++)
		SA2W[i] = (struct carr*)malloc(sizeof(struct carr)*2);
	for(i = 0; i<(nbsa/2); i++)
	{
		for(j = 0; j < 2; j++)
		{
			SA2W[i][j].bv = 0;
			SA2W[i][j].r = 0;
			SA2W[i][j].d = 0;
			SA2W[i][j].nature = NULL;
			SA2W[i][j].tag = NULL;
		}
	}

	SA4W = (struct carr**)malloc(sizeof(struct carr*)*(nbsa/4));
	for(i = 0; i < (nbsa/4); i++)
		SA4W[i] = (struct carr*)malloc(sizeof(struct carr)*4);
	for(i = 0; i<(nbsa/4); i++)
	{
		for(j = 0; j < 4; j++)
		{
			SA4W[i][j].bv = 0;
			SA4W[i][j].r = 0;
			SA4W[i][j].d = 0;
			SA4W[i][j].nature = NULL;
			SA4W[i][j].tag = NULL;
		}
	}

	SA8W = (struct carr**)malloc(sizeof(struct carr*)*(nbsa/8));
	for(i = 0; i < (nbsa/8); i++)
		SA8W[i] = (struct carr*)malloc(sizeof(struct carr)*8);
	for(i = 0; i<(nbsa/8); i++)
	{
		for(j = 0; j < 8; j++)
		{
			SA8W[i][j].bv = 0;
			SA8W[i][j].r = 0;
			SA8W[i][j].d = 0;
			SA8W[i][j].nature = NULL;
			SA8W[i][j].tag = NULL;
		}
	}

	return 1;
	printf(">DM: %d lines.\n>FA: %d lines.\n>SA-2W: %d lines.\n>SA-4W: %d lines.\n>SA-8W: %d lines.\n\n", nbdmi, nbfa, (nbsa/2), (nbsa/4), (nbsa/8));
}

void define_cache() // Define o tamanho de cada cache
{
	int i, j;
	// cache variables
	sdmi  = 0; sdmd  = 0; sfa  = 0; ssa  = 0; // cache size
//	lsdmi = 0; lsdmd  = 0; lsfa  = 0; lssa = 0; // cache line size


	// Definições da cahe (apenas numeros que comportam bytes, tais como 8, 16, 32 serão aceitos.) ---------------------------
	do{
		int aux = 1;
		printf("CS>\tTamanho da cache (KB) - sujeito a arredondamento - : "); fflush(stdin); scanf("%d", &cache_size);

		cache_size = cache_size*1024;
		// convert the cache_size to binary exponential.
		do
		{
			aux =  aux*2;
		}while(aux - cache_size < 0);
		cache_size = aux;


		if(cache_size%8 >= 1)
			printf("ERRO>\tValor nao atende os requisitos da arquitetura. O valor precisa ser multiplo de oito. Tente novamente.\n\n");
		else
		{
			sfa = cache_size*2; // FA
			sdmi = sdmd = ssa = cache_size/8; // DM, SA
		}
	}while(cache_size%8 >= 1);
	printf("> cache size: %d bytes\n\n", cache_size);

	do{
		printf("CLS> Tamanho da linha de cache (bytes): "); fflush(stdin); scanf("%d", &cache_line_size);
		if(cache_line_size%2 != 0 || cache_line_size > cache_size/64)
			printf("ERRO>\tValor nao atende os requisitos. O valor deve ser multiplo de dois e deve ser menor ou igual %d bytes.\nTente novamente.\n\n", cache_size/64);
		//else
		//{
		//	lsdmi = lsdmd = lsfa = lssa = cache_line_size;
		//}
	}while(cache_line_size%2 != 0 || cache_line_size > (cache_size/64));
	printf("> cache line size: %d bytes\n\n", cache_line_size);


	// ------------------- DEFINICOES DE CACHE

	//calculate the number of blocs for each cache organization.
	nbdmi = (sdmi/cache_line_size); // DM I
	nbdmd = (sdmd/cache_line_size); // DM D
	nbfa = (sfa/cache_line_size); // FA
	nbsa = ssa/cache_line_size; // SA

	DM_I = (struct carr*)malloc(sizeof(struct carr)*nbdmi);
	for(i = 0; i<nbdmi; i++)
	{
		DM_I[i].bv = 0;
		DM_I[i].r = 0;
		DM_I[i].d = 0;
		DM_I[i].nature = NULL;
		DM_I[i].tag = NULL;
	}

	DM_D = (struct carr*)malloc(sizeof(struct carr)*nbdmd);
	for(i = 0; i<nbdmd; i++)
	{
		DM_D[i].bv = 0;
		DM_D[i].r = 0;
		DM_D[i].d = 0;
		DM_D[i].nature = NULL;
		DM_D[i].tag = NULL;
	}

	FA = (struct carr*)malloc(sizeof(struct carr)*nbfa);
	for(i = 0; i<nbfa; i++)
	{
		FA[i].bv = 0;
		FA[i].r = 0;
		FA[i].d = 0;
		FA[i].nature = NULL;
		FA[i].tag = NULL;
	}

	SA2W = (struct carr**)malloc(sizeof(struct carr*)*(nbsa/2));
	for(i = 0; i < (nbsa/2); i++)
		SA2W[i] = (struct carr*)malloc(sizeof(struct carr)*2);
	for(i = 0; i<(nbsa/2); i++)
	{
		for(j = 0; j < 2; j++)
		{
			SA2W[i][j].bv = 0;
			SA2W[i][j].r = 0;
			SA2W[i][j].d = 0;
			SA2W[i][j].nature = NULL;
			SA2W[i][j].tag = NULL;
		}
	}

	SA4W = (struct carr**)malloc(sizeof(struct carr*)*(nbsa/4));
	for(i = 0; i < (nbsa/4); i++)
		SA4W[i] = (struct carr*)malloc(sizeof(struct carr)*4);
	for(i = 0; i<(nbsa/4); i++)
	{
		for(j = 0; j < 4; j++)
		{
			SA4W[i][j].bv = 0;
			SA4W[i][j].r = 0;
			SA4W[i][j].d = 0;
			SA4W[i][j].nature = NULL;
			SA4W[i][j].tag = NULL;
		}
	}

	SA8W = (struct carr**)malloc(sizeof(struct carr*)*(nbsa/8));
	for(i = 0; i < (nbsa/8); i++)
		SA8W[i] = (struct carr*)malloc(sizeof(struct carr)*8);
	for(i = 0; i<(nbsa/8); i++)
	{
		for(j = 0; j < 8; j++)
		{
			SA8W[i][j].bv = 0;
			SA8W[i][j].r = 0;
			SA8W[i][j].d = 0;
			SA8W[i][j].nature = NULL;
			SA8W[i][j].tag = NULL;
		}
	}

	printf(">DM: %d lines.\n>FA: %d lines.\n>SA-2W: %d lines.\n>SA-4W: %d lines.\n>SA-8W: %d lines.\n\n", nbdmi, nbfa, (nbsa/2), (nbsa/4), (nbsa/8));
}