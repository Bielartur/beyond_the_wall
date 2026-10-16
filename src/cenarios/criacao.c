#include "criacao.h"
#include "manipulacao.h"
#include "../factory/factory.h"
#include <stdlib.h>

// Configurações globais do jogo
#define VIDA_INICIAL 100

typedef struct {
    int qtdArvores;
    int qtdRochas;
    int qtdCabanasSelvagens;
    int qtdCaminhantesBrancos;
    int qtdFogueiras;
    int qtdVidrosDragao;
} ElementosAlemDaMuralha;

const ElementosAlemDaMuralha CONFIG_ALEM_DA_MURALHA = {
    .qtdArvores            = 8,
    .qtdRochas             = 6,
    .qtdCabanasSelvagens   = 4,
    .qtdCaminhantesBrancos = 5,
    .qtdFogueiras          = 8,
    .qtdVidrosDragao       = 4
};

int cria_castelo_negro(Jogo *jogo) {
  // Cria a primeira sala
  jogo->cenarios[CASTELO_NEGRO] = cria_cenario(3, 3, CASTELO_NEGRO);

  // Cria o elemento que representa Jon
  Elemento jon = cria_elemento(JOGADOR, gera_posicao_aleatoria(jogo->cenarios[CASTELO_NEGRO]));

  jogo->jon = jon;
  coloca_elemento_no_cenario(&jogo->jon, &jogo->cenarios[CASTELO_NEGRO]);

  return EXIT_SUCCESS;
}

int cria_alem_da_muralha(Jogo *jogo) {
  const ElementosAlemDaMuralha *config = &CONFIG_ALEM_DA_MURALHA;
  
  jogo->cenarios[ALEM_DA_MURALHA] = cria_cenario(10, 10, ALEM_DA_MURALHA);
  Elemento bran = cria_elemento(BRAN, gera_posicao_aleatoria(jogo->cenarios[ALEM_DA_MURALHA]));
  jogo->bran = bran;

  distribui_tipos_no_cenario(ARVORE, config->qtdArvores, cenario);
  distribui_tipos_no_cenario(ROCHA, config->qtdRochas, cenario);
  distribui_tipos_no_cenario(CABANA_SELVAGEM, config->qtdCabanasSelvagens, cenario);
  distribui_tipos_no_cenario(CAMINHANTE_BRANCO, config->qtdCaminhantesBrancos, cenario);
  distribui_tipos_no_cenario(FOGUEIRA, config->qtdFogueiras, cenario);
  distribui_tipos_no_cenario(VIDRO_DRAGAO, config->qtdVidrosDragao, cenario);

  return EXIT_SUCCESS;
}

int cria_fortaleza_congelada(Jogo *jogo) {
  jogo->cenarios[FORTALEZA_CONGELADA] = cria_cenario(10, 10, FORTALEZA_CONGELADA);
  
  return EXIT_SUCCESS;
}