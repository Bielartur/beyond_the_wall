#ifndef MANIPULACAO_H
#define MANIPULACAO_H

#include "../types/types.h"

int eh_obstaculo(TipoElemento tipo_elemento);

int mede_distancia(Posicao posicao1, Posicao posicao2);

Posicao gera_posicao_aleatoria(Cenario cenario);

Posicao gera_posicao_na_borda(Cenario cenario);

void coloca_elemento_no_cenario(Elemento *elemento, Cenario *cenario);

int coloca_tipo_no_cenario(Posicao posicao, TipoElemento tipo, Cenario *cenario);

void distribui_tipos_no_cenario(TipoElemento tipo, int qtd_elementos, Cenario *cenario);

int busca_posicoes_ao_redor(Cenario *cenario, Posicao centro, Posicao posicoes_disponiveis[]);

void gera_caminhantes_ao_redor(Cenario *cenario, Posicao centro, int qtd_caminhantes);

int coloca_portao_no_cenario(Portao portao, Cenario *cenario);

DirecoesPortao obtem_direcoes_disponiveis(Etapa etapa);

int tem_direcao_disponivel(Etapa etapa, Direcao direcao);

Portao *busca_portao_por_posicao(Posicao posicao, Cenario *cenario);

int cria_todos_portoes(Cenario cenarios[]);

int conecta_os_cenarios(Cenario cenarios[]);

int move_elemento(Cenario *cenario, Elemento *elemento, Comando direcao, TipoElemento *elemento_alvo);

#endif
