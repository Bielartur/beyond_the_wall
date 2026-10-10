#include "interface.h"
#include <stdio.h>
#include <stdlib.h>

#define RESET "\033[0m"
#define VERMELHO "\033[31m"
#define VERDE "\033[32m"
#define AMARELO "\033[33m"
#define AZUL "\033[34m"
#define MAGENTA "\033[35m"
#define CIANO "\033[36m"
#define BRANCO "\033[97m"
#define CINZA "\033[90m"

const char *tipo_para_simbolo(const TipoElemento *tipo) {
  switch (*tipo) {
  case VAZIO:
    return CINZA "·" RESET;

  case JOGADOR:
    return AZUL "♞" RESET;

  case BRAN:
    return CIANO "♟" RESET;

  case PORTAO:
    return AMARELO "▣" RESET;

  case ARVORE:
    return VERDE "♣" RESET;

  case ROCHA:
    return CINZA "◆" RESET;

  case CABANA_SELVAGEM:
    return AMARELO "⌂" RESET;

  case VIDRO_DRAGAO:
    return MAGENTA "♦" RESET;

  case FOGUEIRA:
    return VERMELHO "♨" RESET;

  case CAMINHANTE_BRANCO:
    return CINZA "." RESET;

  default:
    return "?";
  }
}

void limpar_terminal() {
  #ifdef _WIN32
    system("cls");
  #else
    system("clear");
  #endif
}

void imprime_linha(const TipoElemento **linha, int tamanho) {
  printf("| ");
  for (int i = 0; i < tamanho; i++) {
    if (i != tamanho - 1) {
      printf("%s  ", tipo_para_simbolo(linha[i]));
    } else {
      printf("%s", tipo_para_simbolo(linha[i]));
    }
  }
  printf(" |");
}

void imprime_cenario(Jogo jogo) {
  char *texto; // Aponta para o array de caracteres
  texto = jogo.estado.tem_bran ? "Sim" : "Não";
  limpar_terminal();

  printf("Rodada %d | Vida: %d | Obsidiana: %d | Bran resgatado: %s\n", jogo.estado.rodada, jogo.estado.vida, jogo.estado.obsidiana, texto);
  printf("\n");
  if (*jogo.contexto.elemento_alvo == PORTAO) {
    printf("Determinação: %d\n\n", jogo.estado.determinacao);
  } else if (jogo.contexto.dano_sofrido > 0) {
    printf(
      VERMELHO "Dano sofrido: -%d" RESET "\n",
      jogo.contexto.dano_sofrido
    );
  }

  for (int i = 0; i < jogo.cenario_atual->linhas; i++) {
    imprime_linha(jogo.cenario_atual->matriz[i], jogo.cenario_atual->colunas);
    printf("\n");
  }
  printf("\n");
  if (*jogo.contexto.elemento_alvo == PORTAO) {
    printf("Abrir portão (%c)\n", ABRIR_PORTA);
  }
  printf("Mover-se (%c/%c/%c/%c):", CIMA, ESQUERDA, BAIXO, DIREITA);
}

void imprimir_fim_jogo(int venceu) {
  if (venceu) {
    printf(VERDE "\n\n==============================\n" RESET);
    printf(AMARELO "          VITORIA!\n" RESET);
    printf(VERDE "==============================\n" RESET);
    printf(BRANCO "Jon Snow conseguiu resgatar " CIANO "Bran\n" RESET);
    printf(BRANCO "e sobreviver aos perigos alem da Muralha!\n" RESET);
    printf(AMARELO "\nParabens, voce venceu!\n" RESET);
    printf(VERDE "==============================\n\n" RESET);
  } else {
    printf(VERMELHO "\n\n==============================\n" RESET);
    printf(VERMELHO "         GAME OVER\n" RESET);
    printf(VERMELHO "==============================\n" RESET);
    printf(BRANCO "Jon Snow nao resistiu aos perigos\n" RESET);
    printf(BRANCO "alem da Muralha.\n" RESET);
    printf(CINZA "\nA Patrulha da Noite perdeu um de seus irmaos...\n" RESET);
    printf(VERMELHO "==============================\n\n" RESET);
  }
}