#include "manipulacao.h"
#include "../factory/factory.h"
#include <stdlib.h>
#include <stdbool.h>

int eh_obstaculo(TipoElemento tipo_elemento) {
  switch (tipo_elemento) {
  case ARVORE:
    return 1;
  case ROCHA:
    return 1;
  case CABANA_SELVAGEM:
    return 1;

  default:
    return 0;
  }
}

int mede_distancia(Posicao posicao1, Posicao posicao2) {
  int distancia_em_linhas = abs(posicao1.linha - posicao2.linha);
  int distancia_em_colunas = abs(posicao1.coluna - posicao2.coluna);

  return distancia_em_linhas + distancia_em_colunas;
}

Posicao gera_posicao_aleatoria(Cenario cenario) {
  Posicao posicao = {0};

  do {
    posicao.linha = (rand() % cenario.linhas);   // Já exclui o último elemento, então vai estar sempre em um espaço válido
    posicao.coluna = (rand() % cenario.colunas); // (se a entrada estiver correta)
  } while (cenario.matriz[posicao.linha][posicao.coluna] != VAZIO); // Só retorna posições disponíveis

  return posicao;
}

Posicao gera_posicao_na_borda(Cenario cenario) {
  Posicao posicao = {0};

  int na_linha = rand() % 2;
  int no_final = rand() % 2;

  do {
    if (na_linha) {
      posicao.linha = rand() % cenario.linhas;
      posicao.coluna = no_final ? cenario.colunas - 1 : 0;
    } else {
      posicao.coluna = rand() % cenario.colunas;
      posicao.linha = no_final ? cenario.linhas - 1 : 0;
    }
  } while (cenario.matriz[posicao.linha][posicao.coluna] != VAZIO);

  return posicao;
}

void coloca_elemento_no_cenario(Elemento *elemento, Cenario *cenario) {
  TipoElemento elemento_abaixo = cenario->matriz[elemento->posicao.linha][elemento->posicao.coluna];

  if (elemento_abaixo == PORTAO) {
    // Guarda só se for o portão
    elemento->elemento_abaixo = elemento_abaixo;
  }
  cenario->matriz[elemento->posicao.linha][elemento->posicao.coluna] = elemento->tipo;
}

// Tipos identificam o conteúdo da célula; elementos possuem estado e podem se mover.
int coloca_tipo_no_cenario(Posicao posicao, TipoElemento tipo, Cenario *cenario) {
  if (tipo == PORTAO) return EXIT_FAILURE;

  cenario->matriz[posicao.linha][posicao.coluna] = tipo;
  return EXIT_SUCCESS;
}

void distribui_tipos_no_cenario(TipoElemento tipo, int qtd_elementos, Cenario *cenario) {
  for (int i = 0; i < qtd_elementos; i++) {
    Posicao posicao_aleatoria = gera_posicao_aleatoria(*cenario);
    coloca_tipo_no_cenario(posicao_aleatoria, tipo, cenario);
  }
}

int busca_posicoes_ao_redor(Cenario *cenario, Posicao centro, Posicao posicoes_disponiveis[]) {
  // Esse daqui vai ser o indice que vai interar no array de posicoes_disponiveis
  int quantidade = 0;
  for (int linha = centro.linha - 1; linha <= centro.linha + 1; linha++) {
    for (int coluna = centro.coluna - 1; coluna <= centro.coluna + 1; coluna++) {

      // Ignora as posições fora do cenário
      if (linha < 0 || linha >= cenario->linhas || coluna < 0 || coluna >= cenario->colunas) {
        continue;
      }

      // Ignora a posiçao do Jon
      if (linha == centro.linha && coluna == centro.coluna) {
        continue;
      }

      // Guarda as posições vazias
      if (cenario->matriz[linha][coluna] == VAZIO) {
        posicoes_disponiveis[quantidade].linha = linha;
        posicoes_disponiveis[quantidade].coluna = coluna;

        quantidade++;
      }
    }
  }

  return quantidade;
}

void gera_caminhantes_ao_redor(Cenario *cenario, Posicao centro, int qtd_caminhantes) {
  // 8 se não houver nada ao redor de jon
  Posicao posicoes_disponiveis[8] = {0};

  int quantidade = busca_posicoes_ao_redor(cenario, centro, posicoes_disponiveis);
  if (quantidade == 0) return;

  for (int i = 0; i < qtd_caminhantes; i++) {
    int sorteio_posicao = rand() % quantidade;
    coloca_tipo_no_cenario(posicoes_disponiveis[sorteio_posicao], CAMINHANTE_BRANCO, cenario);
  }
}

int coloca_portao_no_cenario(Portao portao, Cenario *cenario) {
  // retorna se já tem um portão nessa posição
  if (cenario->portoes[portao.direcao].tipo == PORTAO) return EXIT_FAILURE;
  cenario->portoes[portao.direcao] = portao;

  cenario->matriz[portao.posicao.linha][portao.posicao.coluna] = PORTAO;
  return EXIT_SUCCESS;
}

DirecoesPortao obtem_direcoes_disponiveis(Etapa etapa) {
  DirecoesPortao direcoes_disponiveis = {
    .ida = true,
    .volta = true
  };

  if (etapa == 0) {
    direcoes_disponiveis.volta = false;
  }

  if (etapa == QTD_ETAPAS - 1) {
    direcoes_disponiveis.ida = false;
  }

  return direcoes_disponiveis;
}

int tem_direcao_disponivel(Etapa etapa, Direcao direcao) {
  DirecoesPortao direcoes_disponiveis = obtem_direcoes_disponiveis(etapa);

  if (direcao == IDA && direcoes_disponiveis.ida) return true;
  if (direcao == VOLTA && direcoes_disponiveis.volta) return true;

  return false;
}

Portao *busca_portao_por_posicao(Posicao posicao, Cenario *cenario) {
  for (int direcao = 0; direcao < MAX_PORTOES; direcao++) {
    if (
      !tem_direcao_disponivel(cenario->etapa, direcao)
      || cenario->portoes[direcao].tipo != PORTAO
    ) continue;

    int tem_mesma_linha = cenario->portoes[direcao].posicao.linha == posicao.linha;
    int tem_mesma_coluna = cenario->portoes[direcao].posicao.coluna == posicao.coluna;

    if (tem_mesma_linha && tem_mesma_coluna) {
      return &cenario->portoes[direcao];
    }
  }

  return NULL;
}

int cria_todos_portoes(Cenario cenarios[]) {
  Portao portoes[QTD_ETAPAS][MAX_PORTOES] = {0};

  for (int etapa_atual = 0; etapa_atual < QTD_ETAPAS - 1; etapa_atual++) {
    Posicao posicao = {0};
    posicao = etapa_atual == 0 ? gera_posicao_aleatoria(cenarios[etapa_atual]) : gera_posicao_na_borda(cenarios[etapa_atual]);

    for (int direcao = 0; direcao < MAX_PORTOES; direcao++) {
      if (!tem_direcao_disponivel(etapa_atual, direcao)) continue;

      portoes[etapa_atual][direcao] = cria_portao(posicao, NULL);
      Portao portao_criado = portoes[etapa_atual][direcao];

      coloca_portao_no_cenario(portao_criado, &cenarios[etapa_atual]);
    }
  }

  return EXIT_SUCCESS;
}

int conecta_os_cenarios(Cenario cenarios[]) {
  for (int etapa_atual = 0; etapa_atual < QTD_ETAPAS; etapa_atual++) {
    DirecoesPortao direcoes_disponiveis = obtem_direcoes_disponiveis(etapa_atual);

    for (int direcao = 0; direcao < MAX_PORTOES; direcao++) {
      Etapa proxima_etapa = etapa_atual + 1;
      Etapa etapa_anterior = etapa_atual - 1;
      
      if (direcao == IDA && direcoes_disponiveis.ida) {
        // Ao avançar eu vou para o portão da posição VOLTA do próximo cenário
        Portao portao_ida = cenarios[proxima_etapa].portoes[VOLTA];

        Destino destino = {
          .posicao = portao_ida.posicao,
          .vai_para = &cenarios[proxima_etapa]
        };
        // Atualiza o registro do portão no cenário
        cenarios[etapa_atual].portoes[IDA].destino = destino;

      } else if (direcao == VOLTA && direcoes_disponiveis.volta) {
        // Ao voltar eu volto para o portão da posição IDA do cenário anterior
        Portao portao_volta = cenarios[etapa_anterior].portoes[IDA];

        Destino destino = {
          .posicao = portao_volta.posicao,
          .vai_para = &cenarios[etapa_anterior]
        };
        // Atualiza o registro do portão no cenário
        cenarios[etapa_atual].portoes[VOLTA].destino = destino;
      }
    }
  }

  return EXIT_SUCCESS;
}

int move_elemento(Cenario *cenario, Elemento *elemento, Comando direcao, TipoElemento *elemento_alvo) {
  // Esse asterisco permite que eu passe o conteúdo desse ponteiro pra essa minha variável
  Elemento elemento_antes = *elemento;

  switch (direcao) {
  case CIMA:
    if (elemento->posicao.linha == 0) return 0;
    elemento->posicao.linha -= 1;
    break;

  case BAIXO:
    if (elemento->posicao.linha == cenario->linhas - 1) return 0; // linhas - 1 porque se tem 3 linhas o último indice é o 2
    elemento->posicao.linha += 1;
    break;

  case ESQUERDA:
    if (elemento->posicao.coluna == 0) return 0;
    elemento->posicao.coluna -= 1;
    break;

  case DIREITA:
    if (elemento->posicao.coluna == cenario->colunas - 1) return 0; // colunas - 1 porque se tem 3 colunas o último indice é o 2
    elemento->posicao.coluna += 1;
    break;

  default:
    return 0;
  }

  // Guarda o conteúdo no elemento_alvo passado
  *elemento_alvo = cenario->matriz[elemento->posicao.linha][elemento->posicao.coluna];

  // Compara se o conteúdo que esse ponteiro aponta é um obstáculo
  if (eh_obstaculo(*elemento_alvo)) {
    *elemento = elemento_antes;
    return 0;
  }

  // Salva o que havia na posição que o elemento está indo
  elemento->elemento_abaixo =
    *elemento_alvo == PORTAO ? *elemento_alvo : VAZIO;

  // Restaura o que havia abaixo dele
  cenario->matriz
    [elemento_antes.posicao.linha]
    [elemento_antes.posicao.coluna] = elemento_antes.elemento_abaixo;

  return 1;
}
