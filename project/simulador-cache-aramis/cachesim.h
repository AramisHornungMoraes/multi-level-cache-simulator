#ifndef CACHESIM_H
#define CACHESIM_H
#include "cachedef.h";
#include "irrlicht.h"

typedef struct ender
{
	int endereco;
	int type;// 'd' ou 'i'
}ender;

extern int bus_end, bus_end_type;
extern int fa_hit, fa_miss, dmi_hit, dmi_miss, dmd_hit, dmd_miss, sa2w_hit, sa2w_miss, sa4w_hit, sa4w_miss, sa8w_hit, sa8w_miss;
extern int clock_ref_cycle, bus_ready, gen_clock;

void read_bus(FILE *fpin);
ender *generate_add(int number, int range, int cache_line_size); // generate address
ender *read_address_file(FILE *fpin);
void cache_simulation(int number, int range, int cache_line_size);
void cache_simulation_ui();

// variaveis do main.cpp
extern ender *enderecos_ui; // simulacao do que seria os enderecos que a cpu nos solicita
extern int end_index_ui; // auxilia no termino da simulacao; ou seja, ate o numero de endereços solicitados
extern int sok;

extern int add_number; // numero de enderecos
extern ender *enderecos_ui; // simulacao do que seria os enderecos que a cpu nos solicita
extern int end_index_ui; // auxilia no termino da simulacao; ou seja, ate o numero de endereços solicitados

void clear_simulation();

void update_log(irr::core::stringw str);
#endif