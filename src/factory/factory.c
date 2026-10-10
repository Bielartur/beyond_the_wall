#include "../types/types.h"
#include <stdlib.h>

Cenario cria_cenario(int linhas, int colunas, Etapa etapa) {
  Cenario cenario = {
    .linhas = linhas,
    .colunas = colunas,
    .etapa = etapa,
  };

  // Preenche com os ponteiros para o tipo vazio
  for (int i = 0; i < cenario.linhas; i++) {
    for (int j = 0; j < cenario.colunas; j++) {
      cenario.matriz[i][j] = &TIPO_VAZIO;
    }
  }

  return cenario;
}

Elemento cria_elemento(TipoElemento tipo, Posicao *posicao) {
  Posicao posicao_real = {0};

  if (posicao != NULL) {
    posicao_real = *posicao;
  }

  Elemento elemento = {
    .tipo = tipo,
    .posicao = posicao_real,
    .elemento_abaixo = &TIPO_VAZIO,
  };

  return elemento;
}

Portao cria_portao(Posicao posicao, Cenario *vai_para) {
  Portao portao = {
    .tipo = PORTAO,
    .posicao = posicao,
    .vai_para = vai_para,
  };

  return portao;
}