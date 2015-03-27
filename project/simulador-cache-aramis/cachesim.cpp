#include "cachesim.h"
#include "list.h"
#include "string.h"
#include "time.h"

irr::core::stringw str;

ender *memoria;

int bus_end;
int bus_end_type; // dado ou instrucao?
int offset_size;

int bus_ready = 1; // BUS livre? [1 - sim] & [0 - nao]
int gen_clock = 0; //general clock - clock geral do simulador.
int clock_ref_cycle = 0;
int clock_ref_cycle_count_down;

/* clocks de espera de cada cache (com finalidade de barrar o clock geral e auxiliar o BUS)*/
int fa_clock = 0;
int dmi_clock = 0;
int dmd_clock = 0;
int sa2w_clock = 0;
int sa4w_clock = 0;
int sa8w_clock = 0;

// int lru = 0;
int ffa_lru = 0,
	fsa2w_lru = 0,
	fsa4w_lru = 0,
	fsa8w_lru = 0;


//hit miss data
int fa_hit = 0;
int fa_miss = 0;

int dmi_hit = 0;
int dmi_miss = 0;

int dmd_hit = 0;
int dmd_miss = 0;

int sa2w_hit = 0;
int sa2w_miss = 0;

int sa4w_hit = 0;
int sa4w_miss = 0;

int sa8w_hit= 0;
int sa8w_miss = 0;


// 0 = não respondeu e 1 = respondeu
int fa_r = 0;
int dmi_r = 0;
int dmd_r = 0;
int sa2w_r = 0;
int sa4w_r = 0;
int sa8w_r = 0;

// aux functions
/* 8 Bit bynary number only */
char *dec_to_bin(int dec)
{
	int i, n;
	char *b = (char*)malloc(sizeof(char)*9);
	n = dec;
	for(i = 7; i >= 0; i--) 
	{ 
		b[i]= (n%2)+48; 
		n=n/2; 

		if(n==0 && i == 0)
			break; 
	}b[8] = 0;

	return b;
}	

// Algoritmo de simulação do cache fully associative (Aramis)
// hit retorna 1 & miss retorna 0
int FA_cycle()
{

	int blc; // indice para o laco (for) de blocos
			// pega endereço
			
	for(blc = 0; blc < nbfa; blc++) // ver bloco a bloco
	{
		if(FA[blc].bv == 1 && bus_end == FA[blc].tag && bus_end_type == FA[blc].nature)
		{
				//fa_clock = 1;
				if(bus_end_type == 'd')
				{
					FA[blc].d = 1;
					str = L"FA> Re-referencing a data type address. Switching dirty bit to true on address "; str+=bus_end; str+=" on line "; str+=blc;
					update_log(str);
				}
				FA[blc].r = 1;
				fa_r = 1;
				fa_hit++;

				str = L"FA> Hit the address: "; str+=bus_end; str+=". On line "; str+=blc;
				update_log(str);

				return 1; // Hit
		}
	}

	// nao possuimos a tag, entao vamos aramazenar
	if(ffa_lru) // substituicao por lru (caso precisarmos substituir)
	{
		//cria lsita D
		int lista_D_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
		list *list_D = AllocateList();
		for(blc = 0; blc < nbfa; blc++)
		{
			if(FA[blc].d == 0)
			{
				InsertLast(list_D, blc);
				lista_D_len++;
			}
		}

		if(IsListEmpty(list_D))
		{
			// escolhe um randomicamente da FA toda
			srand (time(NULL));
			int choose = rand()%nbfa;
			FA[choose].bv = 1;
			FA[choose].r = 1;
			FA[choose].d = 0;
			FA[choose].tag = bus_end;
			FA[choose].nature = bus_end_type;

			fa_clock = fa_clock + 4; // pq todos os d são 1 atualiza

			str = L"";
			str += "FA> Updating data on memory (clock +4)";
			update_log(str);

		}
		else // cria lista R, da lista da
		{
			node *aux = list_D->first;
			int lista_R_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
			list *list_R = AllocateList();

			do
			{
				if(FA[aux->data].r == 0)
				{
					InsertLast(list_R, aux->data);
					lista_R_len++;
				}
				aux = aux->next;
			}while(aux != NULL);

			if(IsListEmpty(list_R))
			{
						// escolhe um randomicamente da lista D
				srand (time(NULL));
				int choose = rand() % lista_D_len;

				FA[choose].bv = 1;
				FA[choose].r = 1;
				FA[choose].d = 0;
				FA[choose].tag = bus_end;
				FA[choose].nature = bus_end_type;
			}
			else // senao
			{
				// escolhe um randomicamente da lista R
				srand (time(NULL));
				int choose = rand() % lista_R_len;
						
				FA[choose].bv = 1;
				FA[choose].r = 1;
				FA[choose].d = 0;
				FA[choose].tag = bus_end;
				FA[choose].nature = bus_end_type;
			}
		}
	}
	else// randomico
	{
		// escolhe um randomicamente da FA toda
		srand (time(NULL));
		int choose = rand()%nbfa;

		FA[choose].bv = 1;
		FA[choose].r = 0;
		FA[choose].d = 0;
		FA[choose].tag = bus_end;
		FA[choose].nature = bus_end_type;
	}

	fa_clock = fa_clock + 4; // atualiza na mem
	bus_ready = 0;
	fa_miss++;
	fa_r = 1;

	str = L"";
	str += "FA> Miss address: "; str += bus_end; str += ". Reading memory (clock + 4)";
	update_log(str);

	return 0; //miss
}

int DMI_cycle()
{
	int index;
	
	index = bus_end%(sdmi-1);
	index = index%(nbdmi-1);

	if(index > nbdmi - 1)
	{
		do
		{
			index--;
		}while(index > nbdmi - 1);
	}abs(index);

	str = L"";
	str += "DMI> calculating index... index: ";str += index;
	update_log(str);

	if(DM_I[index].bv == 1 && DM_I[index].tag == bus_end)
	{
		dmi_r = 1;
		dmi_hit++;

		str = L"";
		str += "DMI> Hit: ";str += DM_I[index].tag;str += "found on line: "; str += index;
		update_log(str);

		return 1; // hit
	}
	else
	{
		str = L"";
				str += "DMI> No addres found on line ";str += index; str += ". Passing address: '";str += bus_end;str += "' to FA...";
				update_log(str);

		FA_cycle();
		DM_I[index].bv = 1;
		DM_I[index].r = 1;
		DM_I[index].tag = bus_end;
		dmi_miss++;
		dmi_r = 1;

		str = L"";
		str += "DMI> Miss: "; str += bus_end; str += " on line "; str += index; str += "reading new data from FA.";
		update_log(str);

		return 0; //miss
	}
}

int DMD_cycle()
{
	int index;

	index = bus_end%(sdmd-1);
	index = index%(nbdmd-1);

	if(index > nbdmd - 1)
	{
		do
		{
			index--;
		}while(index > nbdmd - 1);
	}abs(index);

	str = L"";
	str += "DMD> calculating index... index: ";str += index;
	update_log(str);

	if(DM_D[index].bv == 1 && DM_D[index].tag == bus_end)
	{
		dmd_hit++;
		dmd_r = 1;
		DM_D[index].d = 1;

		str = L"";
				str += "DMD> Hit: ";str += DM_D[index].tag;str += "found on line: "; str += index;
				update_log(str);

		return 1; // hit
	}
	else
	{
		str = L"";
				str += "DMD> No addres found on line ";str += index; str += ". Passing address: '";str += bus_end;str += "' to FA...";
				update_log(str);

		FA_cycle();
		DM_D[index].bv = 1;
		DM_D[index].r = 1;

		if(DM_D[index].d == 1) // atualiz. se nescessario
		{
			str = L"";
				str += "DMD> The address: "; str += bus_end; str += " on line "; str += index; str += "is with dirty bit true. Writig on memory. (clock + 4)";
				update_log(str);

			DM_D[index].d = 0;
			dmd_clock = dmd_clock + 4;
			bus_ready = 0;
		}

		DM_D[index].tag = bus_end;
		dmd_miss++;
		dmd_r = 1;

		str = L"";
				str += "DMD> Miss: "; str += bus_end; str += " on line "; str += index; str += " reading new data from FA.";
				update_log(str);

		return 0; //miss
	}
}

int SA2W_cycle()
{
	int index, way, way_len = 2;

	index = bus_end%(ssa-1);
	index = index%(int)((nbsa-1)/(way_len-1));

	// mecanismo de segurança para nao violar range de vetores
	if(index > (nbsa/way_len)-1)
	{
		do
		{
			index--;
		}while(index > ((nbsa/way_len) - 1));
	}
	abs(index);

	str = L"";
	str += "2W> calculating index... index: ";str += index;
	update_log(str);

	for(way = 0; way < way_len; way++)
	{
		if(SA2W[index][way].bv == 1 && SA2W[index][way].tag == bus_end && SA2W[index][way].nature == bus_end_type)
		{
			if(SA2W[index][way].nature == 'd')
				SA2W[index][way].d = 1;
			SA2W[index][way].r = 1;

			sa2w_hit++;
			sa2w_r = 1;

			str = L"";
			str += "2W> Hit on line: ";str += index;str += " col: ";str += way;str += " the address: ";str += bus_end;str += (char)bus_end_type;
			update_log(str);

			return 1; // hit
		}
	}
	if(fsa2w_lru) // substituicao por lru (caso precisarmos substituir)
	{
		//cria lsita D
		int lista_D_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
		list *list_D = AllocateList();
		for(way = 0; way < way_len; way++)
		{
			if(SA2W[index][way].d == 0)
			{
				InsertLast(list_D, way);
				lista_D_len++;
			}
		}

		if(IsListEmpty(list_D))
		{
			// escolhe um randomicamente da SA toda
			srand (time(NULL));
			int choose = rand()%way_len;

			str = L"";
			str += "2W> Replacing a dirty true data... Updating data from adress '"; str += SA2W[index][choose].tag; str+="'  into memory (clock +4).";
			update_log(str);

			SA2W[index][choose].bv = 1;
			SA2W[index][choose].r = 1;
			SA2W[index][choose].d = 0;
			SA2W[index][choose].tag = bus_end;
			SA2W[index][choose].nature = bus_end_type;

			sa2w_clock = sa2w_clock + 4; // pq todos os d são 1 atualiza
		}
		else // cria lista R da lista R
		{
			node *aux = list_D->first;
			int lista_R_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
			list *list_R = AllocateList();

			do
			{
				if(SA2W[index][aux->data].r == 0)
				{
					InsertLast(list_R, aux->data);
					lista_R_len++;
				}
				aux = aux->next;
			}while(aux != NULL);

			if(IsListEmpty(list_R))
			{
				node *aux = list_D->first; int lcaux = 0;

				// escolhe um randomicamente da lista D
				srand (time(NULL));
				int choose = rand() % lista_D_len;
				// navegar nas lsitas ate o item selecionado
				while(lcaux != choose && aux != NULL)
				{
					lcaux++;
					aux = aux->next;
				}

				SA2W[index][aux->data].bv = 1;
				SA2W[index][aux->data].r = 1;
				SA2W[index][aux->data].d = 0;
				SA2W[index][aux->data].tag = bus_end;
				SA2W[index][aux->data].nature = bus_end_type;
			}
			else // senao
			{
				node *aux = list_R->first; int lcaux = 0;

				// escolhe um randomicamente da lista R
				srand (time(NULL));
				int choose = rand() % lista_R_len;
				// navegar nas lsitas ate o item selecionado
				while(lcaux != choose && aux != NULL)
				{
					lcaux++;
					aux = aux->next;
				}

				SA2W[index][aux->data].bv = 1;
				SA2W[index][aux->data].r = 1;
				SA2W[index][aux->data].d = 0;
				SA2W[index][aux->data].tag = bus_end;
				SA2W[index][aux->data].nature = bus_end_type;
			}
		}
	}
	else// randomico
	{
		// escolhe um randomicamente da FA toda
		srand (time(NULL));
		int choose = rand()%way_len;

		SA2W[index][choose].bv = 1;
		SA2W[index][choose].r = 1;
		SA2W[index][choose].d = 0;
		SA2W[index][choose].tag = bus_end;
		SA2W[index][choose].nature = bus_end_type;
	}

	sa2w_clock += 4; // atualiza na mem
	bus_ready = 0;
	sa2w_miss++;
	sa2w_r = 1;

	str = L"";
	str += "2W> Missed "; str += bus_end; str+="  reading data from meory clock + 4.";
			update_log(str);

	return 0; //miss
}

int SA4W_cycle()
{
	int index, way, way_len = 4;

	index = bus_end%(ssa-1);
	index = index%(int)((nbsa-1)/(way_len-1));

	// mecanismo de segurança para nao violar range de vetores
	// mecanismo de segurança para nao violar range de vetores
	if(index > (nbsa/way_len)-1)
	{
		do
		{
			index--;
		}while(index > ((nbsa/way_len) - 1));
	}
	abs(index);

	str = L"";
	str += "4W> calculating index... index: ";str += index;
	update_log(str);

	for(way = 0; way < way_len; way++)
	{
		if(SA4W[index][way].bv == 1 && SA4W[index][way].tag == bus_end && SA4W[index][way].nature == bus_end_type)
		{
			if(SA4W[index][way].nature == 'd')
				SA4W[index][way].d = 1;
			SA4W[index][way].r = 1;

			sa4w_hit++;
			sa4w_r = 1;

			str = L"";
			str += "4W> Hit on line: ";str += index;str += " col: ";str += way;str += " the address: ";str += bus_end;str += (char)bus_end_type;
			update_log(str);

			return 1; // hit
		}
	}
	if(fsa4w_lru) // substituicao por lru (caso precisarmos substituir)
	{
		//cria lsita D
		int lista_D_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
		list *list_D = AllocateList();
		for(way = 0; way < way_len; way++)
		{
			if(SA4W[index][way].d == 0)
			{
				InsertLast(list_D, way);
				lista_D_len++;
			}
		}

		if(IsListEmpty(list_D))
		{
			// escolhe um randomicamente da SA toda
			srand (time(NULL));
			int choose = rand()%way_len;

			str = L"";
			str += "4W> Replacing a dirty true data... Updating data from adress '"; str += SA4W[index][choose].tag; str+="'  into memory (clock +4).";
			update_log(str);

			SA4W[index][choose].bv = 1;
			SA4W[index][choose].r = 1;
			SA4W[index][choose].d = 0;
			SA4W[index][choose].tag = bus_end;
			SA4W[index][choose].nature = bus_end_type;

			sa4w_clock = sa4w_clock + 4; // pq todos os d são 1 atualiza
		}
		else // cria lista R da lista R
		{
			node *aux = list_D->first;
			int lista_R_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
			list *list_R = AllocateList();

			do
			{
				if(SA4W[index][aux->data].r == 0)
				{
					InsertLast(list_R, aux->data);
					lista_R_len++;
				}
				aux = aux->next;
			}while(aux != NULL);

			if(IsListEmpty(list_R))
			{
				node *aux = list_D->first; int lcaux = 0;

				// escolhe um randomicamente da lista D
				srand (time(NULL));
				int choose = rand() % lista_D_len;
				// navegar nas lsitas ate o item selecionado
				while(lcaux != choose && aux != NULL)
				{
					lcaux++;
					aux = aux->next;
				}

				SA4W[index][aux->data].bv = 1;
				SA4W[index][aux->data].r = 1;
				SA4W[index][aux->data].d = 0;
				SA4W[index][aux->data].tag = bus_end;
				SA4W[index][aux->data].nature = bus_end_type;
			}
			else // senao
			{
				node *aux = list_R->first; int lcaux = 0;

				// escolhe um randomicamente da lista R
				srand (time(NULL));
				int choose = rand() % lista_R_len;
				// navegar nas lsitas ate o item selecionado
				while(lcaux != choose && aux != NULL)
				{
					lcaux++;
					aux = aux->next;
				}

				SA4W[index][aux->data].bv = 1;
				SA4W[index][aux->data].r = 1;
				SA4W[index][aux->data].d = 0;
				SA4W[index][aux->data].tag = bus_end;
				SA4W[index][aux->data].nature = bus_end_type;
			}
		}
	}
	else// randomico
	{
		// escolhe um randomicamente da FA toda
		srand (time(NULL));
		int choose = rand()%way_len;

		SA4W[index][choose].bv = 1;
		SA4W[index][choose].r = 1;
		SA4W[index][choose].d = 0;
		SA4W[index][choose].tag = bus_end;
		SA4W[index][choose].nature = bus_end_type;
	}

	sa4w_clock += 4; // atualiza na mem
	bus_ready = 0;
	sa4w_miss++;
	sa4w_r = 1;

	str = L"";
	str += "4W> Missed "; str += bus_end; str+="  reading data from meory clock + 4.";
			update_log(str);

	return 0; //miss
}

int SA8W_cycle()
{
		int index, way, way_len = 8;

	index = bus_end%(ssa-1);
	index = index%(int)((nbsa-1)/(way_len-1));

	// mecanismo de segurança para nao violar range de vetores
	// mecanismo de segurança para nao violar range de vetores
	if(index > (nbsa/way_len)-1)
	{
		do
		{
			index--;
		}while(index > ((nbsa/way_len) - 1));
	}
	abs(index);

	str = L"";
	str += "8W> calculating index... index: ";str += index;
	update_log(str);

	for(way = 0; way < way_len; way++)
	{
		if(SA8W[index][way].bv == 1 && SA8W[index][way].tag == bus_end && SA8W[index][way].nature == bus_end_type)
		{
			if(SA8W[index][way].nature == 'd')
				SA8W[index][way].d = 1;
			SA8W[index][way].r = 1;

			sa8w_hit++;
			sa8w_r = 1;

			str = L"";
			str += "8W> Hit on line: ";str += index;str += " col: ";str += way;str += " the address: ";str += bus_end;str += (char)bus_end_type;
			update_log(str);

			return 1; // hit
		}
	}
	if(fsa8w_lru) // substituicao por lru (caso precisarmos substituir)
	{
		//cria lsita D
		int lista_D_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
		list *list_D = AllocateList();
		for(way = 0; way < way_len; way++)
		{
			if(SA8W[index][way].d == 0)
			{
				InsertLast(list_D, way);
				lista_D_len++;
			}
		}

		if(IsListEmpty(list_D))
		{
			// escolhe um randomicamente da SA toda
			srand (time(NULL));
			int choose = rand()%way_len;

			str = L"";
			str += "8W> Replacing a dirty true data... Updating data from adress '"; str += SA8W[index][choose].tag; str+="'  into memory (clock +4).";
			update_log(str);

			SA8W[index][choose].bv = 1;
			SA8W[index][choose].r = 1;
			SA8W[index][choose].d = 0;
			SA8W[index][choose].tag = bus_end;
			SA8W[index][choose].nature = bus_end_type;

			sa8w_clock = sa8w_clock + 4; // pq todos os d são 1 atualiza
		}
		else // cria lista R da lista R
		{
			node *aux = list_D->first;
			int lista_R_len = 0; // tamanho da lista gerada, usamos isto para selecionarmos randomicamente um bloco
			list *list_R = AllocateList();

			do
			{
				if(SA8W[index][aux->data].r == 0)
				{
					InsertLast(list_R, aux->data);
					lista_R_len++;
				}
				aux = aux->next;
			}while(aux != NULL);

			if(IsListEmpty(list_R))
			{
				node *aux = list_D->first; int lcaux = 0;

				// escolhe um randomicamente da lista D
				srand (time(NULL));
				int choose = rand() % lista_D_len;
				// navegar nas lsitas ate o item selecionado
				while(lcaux != choose && aux != NULL)
				{
					lcaux++;
					aux = aux->next;
				}

				SA8W[index][aux->data].bv = 1;
				SA8W[index][aux->data].r = 1;
				SA8W[index][aux->data].d = 0;
				SA8W[index][aux->data].tag = bus_end;
				SA8W[index][aux->data].nature = bus_end_type;
			}
			else // senao
			{
				node *aux = list_R->first; int lcaux = 0;

				// escolhe um randomicamente da lista R
				srand (time(NULL));
				int choose = rand() % lista_R_len;
				// navegar nas lsitas ate o item selecionado
				while(lcaux != choose && aux != NULL)
				{
					lcaux++;
					aux = aux->next;
				}

				SA8W[index][aux->data].bv = 1;
				SA8W[index][aux->data].r = 1;
				SA8W[index][aux->data].d = 0;
				SA8W[index][aux->data].tag = bus_end;
				SA8W[index][aux->data].nature = bus_end_type;
			}
		}
	}
	else// randomico
	{
		// escolhe um randomicamente da FA toda
		srand (time(NULL));
		int choose = rand()%way_len;

		SA8W[index][choose].bv = 1;
		SA8W[index][choose].r = 1;
		SA8W[index][choose].d = 0;
		SA8W[index][choose].tag = bus_end;
		SA8W[index][choose].nature = bus_end_type;
	}

	sa8w_clock += 4; // atualiza na mem
	bus_ready = 0;
	sa8w_miss++;
	sa8w_r = 1;

	str = L"";
	str += "8W> Missed "; str += bus_end; str+="  reading data from meory clock + 4.";
			update_log(str);

	return 0; //miss
}

ender *read_address_file(FILE *fpin)
{
	char line[256];
	char *end;
	char *end_type;
	int i = 0;
	ender *enderecos;
	fgets(line, 256, fpin);
	end = strtok(line,"-");
	end_type = strtok(NULL,"\n");
	while(!feof(fpin))
	{
		i++;
		printf("end %s, nature: %s", end, end_type);
		fgets(line, 256, fpin);
		
	}
	rewind(fpin);
	enderecos = (struct ender*)malloc(sizeof(struct ender)*i);
	i = 0;

	char* str;
	while(!feof(fpin))
	{
		fgets(line, 256, fpin);

		str = strtok(line, "-");
		enderecos[i].endereco = atoi(str);

		str = strtok(NULL, "\n");
		char tempstr[2];
		if(str)
		{
			tempstr[0] =  str[0];
			tempstr[1] = '\0';

			if(!strcmp(tempstr, "i"))
				enderecos[i].type = 'i';
			else if(!strcmp(tempstr, "d"))
				enderecos[i].type = 'd';
			else
				enderecos[i].type = '?';
			i++;
		}
	}add_number = i;
	end = strtok(line," ,\n");
	end_type = strtok(NULL," ,\n");
	return enderecos;
}

int binary_exponential_index(int number)
{
	int aux = 1, expind = 0;
	do
	{
		aux =  aux*2;
		expind++;
	}while(aux - number < 0);
	return expind;
}

ender *generate_add(int number, int range, int cache_line_size) // generate address
{
	FILE *fpout;
	int i, aux = 1, offsetindex = 0, rangeindex = 0;
	int endtype;
	ender *enderecos = (struct ender*)malloc(sizeof(struct ender)*number);
	int seed;
	//offsetindex = binary_exponential_index(cache_line_size);

	//rangeindex = binary_exponential_index(range);

	fpout = fopen("input.txt", "w");
	for(i = 0; i < number; i++ )
	{
		int seedexp;
		seed = 0;
		do
		{
			seed = rand()%range + (cache_line_size-1); // [cache-line to (range-1)]
			seedexp = binary_exponential_index(seed);
			seed = seed<<(31 - seedexp);
		}while(seed < 0);

		enderecos[i].endereco = seed;

		//strcpy(enderecos[i].endereco,charbin);
		//enderecos[i].endereco = charbin;

		endtype = rand()%2;
		if(endtype == 1)
			endtype = 'i';
		else
			endtype = 'd';

		enderecos[i].type = endtype;
		fprintf(fpout,"%d-%c\n",seed,endtype);

		//printf("%d - %d %c\n",i, enderecos[i].endereco, enderecos[i].type);
		// aqui seria um bom lugar apra escrever essa informacao no arquivo
		//fflush(stdin); fgetchar();
	}
	fclose(fpout);
	return enderecos;
}

void cache_simulation_ui()
{
	//do{
		// CACHE MULTI-NIVEL (FA;DMI;DMD)---------------------------
		if(!fa_clock)
		{
			if(!dmd_clock)
			{
				//if(bus_ready)// simula DM
				//{
					if(bus_end_type == 'd')
					{
						//DMD- cycle
						if(!dmd_r) // nao respondeu
							DMD_cycle();
					}
					else if(bus_end_type == 'i')
					{
						//DMI - cycle
						if(!dmi_r) // nao respondeu
							DMI_cycle();
					}
				//}
			}
			else
			{/*
				if(dmi_clock && !fa_clock)
				{
					dmi_clock--;
				}*/
				if(!fa_clock)
				{
					dmd_clock--;

					if(!dmd_clock && !sa2w_clock && !sa4w_clock && !sa8w_clock)
						bus_ready = 1;
				}
				/*
				if(!dmd_clock)
				{
					if(!sa2w_clock && !sa4w_clock && !sa8w_clock)// verifica tambem se as outra em paralelo nao estou usando o barramento
						bus_ready = 1;
				}*/
			}
		}
		else
		{
			if(fa_clock)
				fa_clock--;

			if(!fa_clock)
			{
				if(!dmd_clock)
				{
					if(!sa2w_clock && !sa4w_clock && !sa8w_clock)
					{
						bus_ready = 1;
					}
				}
			}
		}

		// SERIES SAS (2W;4W;8W)---------------------------

		if(!sa2w_clock) // 2W
		{
			//if(bus_ready)
				if(!sa2w_r) // nao respondeu
					SA2W_cycle(); // simula sa2w
		}
		else if(!fa_clock && !dmd_clock)
		{
			sa2w_clock--;
			if(!sa2w_clock && !dmd_clock && !fa_clock)
			{
				if(!sa4w_clock && !sa8w_clock)
				{
					bus_ready = 1;
				}
			}
		}


		if(!sa4w_clock) // 4W
		{
			//if(bus_ready)
				if(!sa4w_r)
					SA4W_cycle(); // simula sa4w
		}
		else if(!fa_clock && !dmd_clock && !sa2w_clock)
		{
			sa4w_clock--;
			if(!sa4w_clock)
			{
				if(!sa4w_clock && !sa8w_clock)
				{
					bus_ready = 1;
				}
			}
		}


		if(!sa8w_clock) // 8W
		{
			//if(bus_ready)
				if(!sa8w_r)
					SA8W_cycle(); // simula sa2w
		}
		else if(!fa_clock && !dmd_clock && !sa2w_clock && !sa4w_clock)
		{
			sa8w_clock--;
			if(!sa8w_clock )
			{
				bus_ready = 1;
			}
		}

		str = L"------------------------clock "; str+= gen_clock; str+="-----------------------";update_log(str);
		gen_clock++;
		clock_ref_cycle_count_down--;

		if(!clock_ref_cycle_count_down)
		{
			int i, j;

			//dms
			for(i = 0; i < nbdmi; i++)
				DM_I[i].r = 0;

			for(i = 0; i < nbdmd; i++)
				DM_D[i].r = 0;

			//fa
			for(i = 0; i < nbfa; i++)
				FA[i].r = 0;

			//sas
			for(i = 0; i < (nbsa/2); i++)
				for(j = 0; j < 2; j++)
					SA2W[i][j].r = 0;

			for(i = 0; i < (nbsa/4); i++)
				for(j = 0; j < 4; j++)
					SA4W[i][j].r = 0;

			for(i = 0; i < (nbsa/8); i++)
				for(j = 0; j < 8; j++)
					SA8W[i][j].r = 0;

			clock_ref_cycle_count_down = clock_ref_cycle;
		}

		if(enderecos_ui[end_index_ui+1].endereco < 0)
		{
			update_log(L"-------------------END OF THE SIMULATION------------------");
			sok = 0;
		}

		else if((fa_r || dmi_r || dmd_r) && sa2w_r && sa4w_r && sa8w_r && sok)
		{
			end_index_ui++;
			bus_end = enderecos_ui[end_index_ui].endereco;
			bus_end_type = enderecos_ui[end_index_ui].type;
			fa_r = dmi_r = dmd_r = sa2w_r = sa4w_r = sa8w_r = 0; // resposta voltam a zero
			str = L"ADDR>   "; str+=bus_end; str+=bus_end_type;
			update_log(str);
		}
	//}while(end_index < number); // ate que chege ao ultimo endereco
}

void clear_simulation()
{
	// limpar todas as variaveis
	free(memoria);

	bus_ready = 1; // BUS livre? [1 - sim] & [0 - nao]
	gen_clock = 0; //general clock - clock geral do simulador.
	clock_ref_cycle = 0;

	fa_clock = 0;
	dmi_clock = 0;
	dmd_clock = 0;
	sa2w_clock = 0;
	sa4w_clock = 0;
	sa8w_clock = 0;

	//hit miss data
	fa_hit = 0;
	fa_miss = 0;
	dmi_hit = 0;
	dmi_miss = 0;
	dmd_hit = 0;
	dmd_miss = 0;
	sa2w_hit = 0;
	sa2w_miss = 0;
	sa4w_hit = 0;
	sa4w_miss = 0;
	sa8w_hit= 0;
	sa8w_miss = 0;

	// 0 = não respondeu e 1 = respondeu
	fa_r = 0;
	dmi_r = 0;
	dmd_r = 0;
	sa2w_r = 0;
	sa4w_r = 0;
	sa8w_r = 0;

	add_number = 1000; // numero de enderecos
	//if(enderecos_ui)
	//	free(enderecos_ui); // simulacao do que seria os enderecos que a cpu nos solicita
	end_index_ui = 0; // auxilia no termino da simulacao; ou seja, ate o numero de endereços solicitados
}