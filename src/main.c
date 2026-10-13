#include "./input/input.h"
#include "./interface/interface.h"
#include "./factory/factory.h"
#include "./types/types.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Configurações globais do jogo
#define VIDA_INICIAL 100
#define QTD_ARVORES 8
#define QTD_ROCHAS 6
#define QTD_CABANAS_SELVAGENS 4
#define QTD_CAMINHANTES_BRANCOS 5
#define QTD_FOGUEIRAS 8
#define QTD_VIDROS_DRAGAO 4

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
  if (cenario->portoes[cenario->qtd_portoes_colocados].tipo == PORTAO) return EXIT_FAILURE;
  cenario->portoes[cenario->qtd_portoes_colocados] = portao;
  cenario->qtd_portoes_colocados += 1;

  cenario->matriz[portao.posicao.linha][portao.posicao.coluna] = PORTAO;
  return EXIT_SUCCESS;
}

Portao *busca_portao_por_posicao(Posicao posicao, Cenario *cenario) {
  for (int i = 0; i < cenario->qtd_portoes_colocados; i++) {
    int tem_mesma_linha = cenario->portoes[i].posicao.linha == posicao.linha;
    int tem_mesma_coluna = cenario->portoes[i].posicao.coluna == posicao.coluna;

    if (tem_mesma_linha && tem_mesma_coluna) {
      return &cenario->portoes[i];
    }
  }

  return NULL;
}

int cria_alem_da_muralha(Jogo *jogo) {
  jogo->alem_da_muralha = cria_cenario(10, 10, ALEM_DA_MURALHA);
  Posicao posicao_portao = gera_posicao_na_borda(jogo->alem_da_muralha);

  // O destino será definido na main, após o cenário estar em seu endereço definitivo.
  Portao portao = cria_portao(posicao_portao, &jogo->castelo_negro);
  coloca_portao_no_cenario(portao, &jogo->alem_da_muralha);

  // jon->posicao = posicao_portao;
  // coloca_elemento_no_cenario(*jon, &alem_da_muralha);
  Elemento bran = cria_elemento(BRAN, gera_posicao_aleatoria(jogo->alem_da_muralha));
  jogo->bran = bran;

  distribui_tipos_no_cenario(ARVORE, QTD_ARVORES, &jogo->alem_da_muralha);
  distribui_tipos_no_cenario(ROCHA, QTD_ROCHAS, &jogo->alem_da_muralha);
  distribui_tipos_no_cenario(CABANA_SELVAGEM, QTD_CABANAS_SELVAGENS, &jogo->alem_da_muralha);
  distribui_tipos_no_cenario(CAMINHANTE_BRANCO, QTD_CAMINHANTES_BRANCOS, &jogo->alem_da_muralha);
  distribui_tipos_no_cenario(FOGUEIRA, QTD_FOGUEIRAS, &jogo->alem_da_muralha);
  distribui_tipos_no_cenario(VIDRO_DRAGAO, QTD_VIDROS_DRAGAO, &jogo->alem_da_muralha);

  return EXIT_SUCCESS;
}

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

  // Cria a primeira sala
  jogo->castelo_negro = cria_cenario(3, 3, CASTELO_NEGRO);

  // Cria o elemento que representa Jon
  Elemento jon = cria_elemento(JOGADOR, gera_posicao_aleatoria(jogo->castelo_negro));

  Portao portao1 = cria_portao(gera_posicao_aleatoria(jogo->castelo_negro), &jogo->alem_da_muralha);
  coloca_portao_no_cenario(portao1, &jogo->castelo_negro);
  
  jogo->jon = jon;
  
  coloca_elemento_no_cenario(&jogo->jon, &jogo->castelo_negro);

  cria_alem_da_muralha(jogo);
  jogo->cenario_atual = &jogo->castelo_negro;
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
  jogo->cenario_atual = portao.vai_para;

  // o portão 0 sempre será o portão de entrada
  Portao portao_alvo = jogo->cenario_atual->portoes[0];
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

  if (jogo->cenario_atual == &jogo->castelo_negro && jogo->estado.tem_bran) {
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