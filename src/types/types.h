#ifndef TYPES_H
#define TYPES_H

#define TAM_MAX 10

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

typedef struct {
  int linha;
  int coluna;
} Posicao;

typedef struct {
  Posicao posicao;
  TipoElemento tipo;
  TipoElemento elemento_abaixo;
} Elemento;

typedef enum {
  CASTELO_NEGRO,
  ALEM_DA_MURALHA
} Etapa;

typedef struct {
  int linhas;
  int colunas;
  Etapa etapa;
  TipoElemento matriz[TAM_MAX][TAM_MAX];
} Cenario;

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

typedef struct {
  TipoElemento elemento_alvo;
  Comando comando;
  int dano_sofrido;
} ContextoAcao;

typedef struct {
  EstadoJogo estado;
  ContextoAcao contexto;

  Cenario castelo_negro;
  Cenario alem_da_muralha;

  Cenario *cenario_atual;

  Elemento jon;
  Elemento bran;
} Jogo;

#endif
