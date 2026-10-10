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

// Variáveis globais
extern const TipoElemento TIPO_VAZIO;
extern const TipoElemento TIPO_JOGADOR;
extern const TipoElemento TIPO_BRAN;
extern const TipoElemento TIPO_PORTAO;
extern const TipoElemento TIPO_ARVORE;
extern const TipoElemento TIPO_ROCHA;
extern const TipoElemento TIPO_CABANA_SELVAGEM;
extern const TipoElemento TIPO_FOGUEIRA;
extern const TipoElemento TIPO_VIDRO_DRAGAO;
extern const TipoElemento TIPO_CAMINHANTE_BRANCO;

typedef enum {
  CIMA = 'w',
  BAIXO = 's',
  DIREITA = 'd',
  ESQUERDA = 'a',
  ABRIR_PORTA = 'f'
} Comando;

typedef enum {
  CASTELO_NEGRO,
  ALEM_DA_MURALHA
} Etapa;

typedef struct {
  int linhas;
  int colunas;
  Etapa etapa;
  const TipoElemento *matriz[TAM_MAX][TAM_MAX];
} Cenario;

typedef struct {
  int linha;
  int coluna;
} Posicao;

typedef struct {
  const TipoElemento tipo; // deve ser constante do tipo PORTAO
  const Posicao posicao;   // Portão não se mexe também
  const Cenario *vai_para; // Um portão leva sempre pra o mesmo lugar
} Portao;

typedef struct {
  TipoElemento tipo;
  Posicao posicao;
  const TipoElemento *elemento_abaixo;
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
  TipoElemento *elemento_alvo;
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
