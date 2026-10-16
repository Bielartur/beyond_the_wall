#include "../types/types.h"
#include <stdlib.h>

Cenario cria_cenario(int linhas, int colunas, Etapa etapa) {
  Cenario cenario = {
    .linhas = linhas,
    .colunas = colunas,
    .etapa = etapa,
  };

  return cenario;
}

Elemento cria_elemento(TipoElemento tipo, Posicao posicao) {
  Elemento elemento = {
    .tipo = tipo,
    .posicao = posicao,
    .elemento_abaixo = VAZIO,
  };

  return elemento;
}

Portao cria_portao(Posicao posicao, Destino *destino) {
  Portao portao = {
    .tipo = PORTAO,
    .posicao = posicao,
  };
  if (destino) {
    portao.destino = *destino;
  }

  return portao;
}