#include "../types/types.h"
#include <stdlib.h>

Cenario cria_cenario(int linhas, int colunas, Etapa etapa) {
  Cenario cenario = {
    .linhas = linhas,
    .colunas = colunas,
    .etapa = etapa,
  };

  // Preenche com o tipo vazio
  for (int i = 0; i < cenario.linhas; i++) {
    for (int j = 0; j < cenario.colunas; j++) {
      cenario.matriz[i][j] = VAZIO;
    }
  }

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