#include "./input/input.h"
#include "./interface/interface.h"
#include "./cenarios/criacao.h"
#include "./cenarios/manipulacao.h"
#include "./types/types.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define VIDA_INICIAL 100

void inicia_o_jogo(Jogo *jogo) {
  EstadoJogo estado = {
    .rodada = 1,
    .tem_bran = 0,
    .vida = VIDA_INICIAL,
    .determinacao = 0,
    .obsidiana = 0,
    .venceu = 0,
    .fim = 0,
  };
  jogo->estado = estado;

  ContextoAcao contexto = {
    .elemento_alvo = VAZIO,
    .comando = 0,
    .dano_sofrido = 0,
  };
  jogo->contexto = contexto;

  cria_castelo_negro(jogo);
  cria_alem_da_muralha(jogo);
  cria_fortaleza_congelada(jogo);

  // Envia todos os cenários para preencher com seus portões
  cria_todos_portoes(jogo->cenarios);

  // Interliga os portões
  conecta_os_cenarios(jogo->cenarios);

  jogo->cenario_atual = &jogo->cenarios[CASTELO_NEGRO];
}

void mover_jogador(Jogo *jogo) {
  TipoElemento elemento_alvo = VAZIO;

  int moveu = move_elemento(
    jogo->cenario_atual,
    &jogo->jon,
    jogo->contexto.comando,
    &elemento_alvo
  );

  if (!moveu) return;
  // o elemento alvo foi preenchido dentro da função move_elemento
  jogo->contexto.elemento_alvo = elemento_alvo;

  // Efetiva a movimentação
  coloca_elemento_no_cenario(&jogo->jon, jogo->cenario_atual);
}

void mover_bran(Jogo *jogo) {
  char comandos[] = {CIMA, ESQUERDA, BAIXO, DIREITA};
  int escolhe_comando = rand() % 4;
  Elemento bran_antes = jogo->bran;
  TipoElemento elemento_alvo = VAZIO;

  int moveu = move_elemento(jogo->cenario_atual, &jogo->bran, comandos[escolhe_comando], &elemento_alvo);

  if (!moveu) return;
  if (elemento_alvo == CAMINHANTE_BRANCO) {
    // Se o bran andou pra cima de um caminhante branco, faz um rollback pra posicao original
    jogo->bran = bran_antes;
    return;
  }

  // Se não, efetiva a mudança
  coloca_elemento_no_cenario(&jogo->bran, jogo->cenario_atual);
}

int troca_de_cenario(Jogo *jogo, Portao portao) {
  jogo->cenario_atual = portao.destino.vai_para;

  // o portão 0 sempre será o portão de entrada
  Portao *ponteiro_portao_alvo = busca_portao_por_posicao(portao.destino.posicao, jogo->cenario_atual);
  if (ponteiro_portao_alvo == NULL) return EXIT_FAILURE;

  Portao portao_alvo = *ponteiro_portao_alvo;

  jogo->jon.posicao = portao_alvo.posicao;
  coloca_elemento_no_cenario(&jogo->jon, jogo->cenario_atual);

  return EXIT_SUCCESS;
}

void tenta_abrir_portao(Jogo *jogo) {
  if (jogo->contexto.elemento_alvo != PORTAO) return;
  Portao *ponteiro_portao = busca_portao_por_posicao(jogo->jon.posicao, jogo->cenario_atual);

  if (ponteiro_portao == NULL) return;
  Portao portao_atual = *ponteiro_portao;

  if (jogo->cenario_atual->etapa == CASTELO_NEGRO) {
    jogo->estado.determinacao = rand() % 101;

    if (jogo->estado.determinacao >= 70) {
      troca_de_cenario(jogo, portao_atual);
      return;
    } else {
      jogo->estado.vida -= 15;
    }
  }
  troca_de_cenario(jogo, portao_atual);
}

void lidar_comando(Jogo *jogo) {

  switch (jogo->contexto.comando) {
  case CIMA:
  case BAIXO:
  case ESQUERDA:
  case DIREITA:
    mover_jogador(jogo);
    break;

  case ABRIR_PORTA:
    tenta_abrir_portao(jogo);
    break;

  default:
    break;
  }
}

void combater_caminhante(EstadoJogo *estado, ContextoAcao *contexto) {
  int dano = 0;

  if (estado->obsidiana > 0) {
    estado->obsidiana -= 1;
    dano = 5;
  } else {
    dano = 25;
  }
  estado->vida -= dano;
  contexto->dano_sofrido = dano;
}

void recuperar_vida(EstadoJogo *estado) {
  if (estado->vida <= VIDA_INICIAL - 10) {
    estado->vida += 10;
  } else {
    estado->vida = VIDA_INICIAL;
  }
}

void aplicar_efeito_elemento(EstadoJogo *estado, ContextoAcao *contexto) {
  switch (contexto->elemento_alvo) {

  case CAMINHANTE_BRANCO:
    combater_caminhante(estado, contexto);
    return;

  case VIDRO_DRAGAO:
    estado->obsidiana += 1;
    return;

  case FOGUEIRA:
    recuperar_vida(estado);
    return;

  default:
    break;
  }

  contexto->dano_sofrido = 0;
}

void processa_a_rodada(Jogo *jogo) {

  if (jogo->cenario_atual->etapa == ALEM_DA_MURALHA) {
    jogo->estado.rodada += 1;
    jogo->estado.vida -= 1;

    int distancia_bran_jon = mede_distancia(jogo->bran.posicao, jogo->jon.posicao);
    if (distancia_bran_jon == 0) {
      jogo->estado.tem_bran = 1;
    }

    if (!jogo->estado.tem_bran) mover_bran(jogo);

    if (distancia_bran_jon < 4 && distancia_bran_jon < jogo->estado.distancia_anterior) {
      gera_caminhantes_ao_redor(jogo->cenario_atual, jogo->jon.posicao, 2);
    }

    jogo->estado.distancia_anterior = distancia_bran_jon;
  }

  if (jogo->cenario_atual == &jogo->cenarios[CASTELO_NEGRO] && jogo->estado.tem_bran) {
    jogo->estado.venceu = 1;
  }

  if (jogo->estado.vida <= 0 || jogo->estado.venceu) {
    jogo->estado.fim = 1;
  }
}

int main() {
  srand(time(NULL));

  Jogo jogo = {0};
  inicia_o_jogo(&jogo);

  imprime_cenario(jogo);
  while (!jogo.estado.fim) {
    jogo.contexto.comando = capturar_tecla();

    lidar_comando(&jogo);
    aplicar_efeito_elemento(&jogo.estado, &jogo.contexto);

    processa_a_rodada(&jogo);
    imprime_cenario(jogo);
  }

  imprimir_fim_jogo(jogo.estado.venceu);

  return 0;
}