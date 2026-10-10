#ifndef FACTORY_H
#define FACTORY_H

#include "../types/types.h"

Cenario cria_cenario(int linhas, int colunas, Etapa etapa);

Elemento cria_elemento(TipoElemento tipo, Posicao *posicao);

Portao cria_portao(Posicao posicao, Cenario *vai_para);

#endif