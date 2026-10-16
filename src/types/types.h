#ifndef TYPES_H
#define TYPES_H
#include <stdbool.h>

#define TAM_MAX 10
#define MAX_PORTOES 2

typedef enum {
  VAZIO,
  JOGADOR,
  BRAN,
  PORTAO,
  ARVORE,
  ROCHA,
  CABANA_SELVAGEM,
  FOGUEIRA,
  VIDRO_DRAGAO,
  CAMINHANTE_BRANCO,
} TipoElemento;

typedef enum {
  CIMA = 'w',
  BAIXO = 's',
  DIREITA = 'd',
  ESQUERDA = 'a',
  ABRIR_PORTA = 'f'
} Comando;

typedef enum {
  CASTELO_NEGRO,
  ALEM_DA_MURALHA,
  FORTALEZA_CONGELADA,
  QTD_ETAPAS
} Etapa;

typedef struct {
  int linha;
  int coluna;
} Posicao;

typedef struct Cenario Cenario;

typedef struct {
  Cenario *vai_para; // indica o cenário de destino
  Posicao posicao; // indica a posição dentro desse cenário
} Destino;

typedef enum {
  IDA,
  VOLTA,
} Direcao;

typedef struct {
  TipoElemento tipo; // deve ser constante do tipo PORTAO
  Direcao direcao;
  Posicao posicao;   // Portão não se mexe também
  Destino destino;   // Um portão leva sempre pra o mesmo lugar
} Portao;

typedef struct {
  bool ida;
  bool volta;
} DirecoesPortao;

struct Cenario {
  int linhas;
  int colunas;
  Etapa etapa;
  Portao portoes[MAX_PORTOES]; // até 5 portões pra evitar problema de faltar espaço

  TipoElemento matriz[TAM_MAX][TAM_MAX];
};

typedef struct {
  TipoElemento tipo;
  Posicao posicao;
  TipoElemento elemento_abaixo;
} Elemento;

typedef struct {
  int rodada;
  int distancia_anterior;
  int tem_bran;
  int vida;
  int determinacao;
  int obsidiana;
  int venceu;
  int fim;
} EstadoJogo;

// O contexto ação ele carrega as informações de uma rodada específica
typedef struct {
  TipoElemento elemento_alvo;
  Comando comando;
  int dano_sofrido;
} ContextoAcao;

typedef struct {
  EstadoJogo estado;
  ContextoAcao contexto;

  Cenario cenarios[QTD_ETAPAS];
  Cenario *cenario_atual;

  Elemento jon;
  Elemento bran;
} Jogo;

const Portao *tipo_para_portao(const TipoElemento *tipo);

#endif
