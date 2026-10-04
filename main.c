#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_COLUNAS 10
#define VIDA_INICIAL 100
#define TAM_MAX 10
#define QTD_ARVORES 8
#define QTD_ROCHAS 6
#define QTD_CABANAS_SELVAGENS 4
#define QTD_CAMINHANTES_BRANCOS 5
#define QTD_FOGUEIRAS 8
#define QTD_VIDROS_DRAGAO 4


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

#ifdef _WIN32
    #include <conio.h>
    
    int capturar_tecla(void) {
        return getch();
    }
#else
    #include <stdio.h>
    #include <termios.h>
    #include <unistd.h>
    
    int capturar_tecla(void) {
        struct termios oldt, newt;
        int ch;
        // Pega as configurações atuais do terminal
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        // Desativa o modo canônico (Buffer de linha) e o Eco
        newt.c_lflag &= ~(ICANON | ECHO);
        // Aplica as novas configurações imediatamente
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        
        ch = getchar();
        
        // Restaura as configurações originais do terminal
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#endif

#define RESET   "\033[0m"
#define VERMELHO "\033[31m"
#define VERDE    "\033[32m"
#define AMARELO  "\033[33m"
#define AZUL     "\033[34m"
#define MAGENTA  "\033[35m"
#define CIANO    "\033[36m"
#define BRANCO   "\033[97m"
#define CINZA    "\033[90m"

const char *tipo_para_simbolo(TipoElemento tipo) {
  switch (tipo) {
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

void imprime_linha(TipoElemento* linha, int tamanho) {
  printf("| ");
  for (int i=0; i < tamanho; i++) {
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

  #ifdef _WIN32
    system("cls");
  #else
      system("clear");
  #endif

  printf("Rodada %d | Vida: %d | Obsidiana: %d | Bran resgatado: %s\n", jogo.estado.rodada, jogo.estado.vida, jogo.estado.obsidiana, texto);
  printf("\n");
  if (jogo.contexto.elemento_alvo == PORTAO) {
    printf("Determinação: %d\n\n", jogo.estado.determinacao);
  }

  for (int i=0; i < jogo.cenario_atual->linhas; i++) {
    imprime_linha(jogo.cenario_atual->matriz[i], jogo.cenario_atual->colunas);
    printf("\n");
  }
  printf("\n");
  if (jogo.contexto.elemento_alvo == PORTAO) {
    printf("Abrir portão (%c)\n", ABRIR_PORTA);
  }
  printf("Mover-se (%c/%c/%c/%c):", CIMA, ESQUERDA, BAIXO, DIREITA);
}

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
    posicao.linha = (rand() % cenario.linhas); // Já exclui o último elemento, então vai estar sempre em um espaço válido
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

void coloca_elemento_no_cenario(Elemento elemento, Cenario *cenario) {
  TipoElemento elemento_abaixo = cenario->matriz[elemento.posicao.linha][elemento.posicao.coluna];

  if (elemento_abaixo == PORTAO) {
    // Guarda só se for o portão
    elemento.elemento_abaixo = elemento_abaixo;
  }
  cenario->matriz[elemento.posicao.linha][elemento.posicao.coluna] = elemento.tipo;
}

// A diferença entre um tipo e um elemento é que o elemento é movel e o tipo não
void coloca_tipo_no_cenario(Posicao posicao, TipoElemento tipo, Cenario *cenario) {
  cenario->matriz[posicao.linha][posicao.coluna] = tipo;
}

void gera_elementos_no_cenario(TipoElemento tipo, int qtd_elementos, Cenario *cenario) {
  for (int i=0; i < qtd_elementos; i++) {
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

  for (int i=0; i < qtd_caminhantes; i++) {
    int sorteio_posicao = rand() % quantidade;
    coloca_tipo_no_cenario(posicoes_disponiveis[sorteio_posicao], CAMINHANTE_BRANCO, cenario);
  }
}

Jogo inicia_o_jogo() {
  EstadoJogo estado = {
    .rodada = 1,
    .tem_bran = 0,
    .vida = VIDA_INICIAL,
    .determinacao = 0,
    .obsidiana = 0,
    .venceu = 0,
    .fim = 0
  };

  ContextoAcao contexto = {
    .elemento_alvo = VAZIO,
  };

  // Cria a primeira sala
  Cenario sala1 = {
    .linhas = 3,
    .colunas = 3,
    .etapa = CASTELO_NEGRO
  };

  // Cria o array que vai guardar as posições de Jon
  Elemento jon = {
    .tipo = JOGADOR,
    .posicao = gera_posicao_aleatoria(sala1)
  };
  coloca_elemento_no_cenario(jon, &sala1);

  gera_elementos_no_cenario(PORTAO, 1, &sala1);

  Jogo jogo = {
    .estado = estado,
    .contexto = contexto,
    .castelo_negro = sala1,
    .jon = jon,
  };

  return jogo;
}

Cenario monta_alem_da_muralha(Elemento *jon, Elemento *bran) {
  Cenario sala2 = {
    .linhas = 10,
    .colunas = 10,
    .etapa = ALEM_DA_MURALHA
  };

  Posicao posicao_portao = gera_posicao_na_borda(sala2);
  
  Elemento portao = {
    .tipo = PORTAO,
    .posicao = posicao_portao
  };
  coloca_elemento_no_cenario(portao, &sala2);

  jon->posicao = posicao_portao;
  coloca_elemento_no_cenario(*jon, &sala2);
  
  bran->tipo = BRAN;
  bran->posicao = gera_posicao_aleatoria(sala2);

  gera_elementos_no_cenario(ARVORE, QTD_ARVORES, &sala2);
  gera_elementos_no_cenario(ROCHA, QTD_ROCHAS, &sala2);
  gera_elementos_no_cenario(CABANA_SELVAGEM, QTD_CABANAS_SELVAGENS, &sala2);
  gera_elementos_no_cenario(CAMINHANTE_BRANCO, QTD_CAMINHANTES_BRANCOS, &sala2);
  gera_elementos_no_cenario(FOGUEIRA, QTD_FOGUEIRAS, &sala2);
  gera_elementos_no_cenario(VIDRO_DRAGAO, QTD_VIDROS_DRAGAO, &sala2);

  return sala2;
}

int move_elemento(Cenario *cenario, Elemento *elemento, Comando direcao, TipoElemento *elemento_alvo) {
  Elemento elemento_antes = *elemento; // Esse asterisco permite que eu passe o conteúdo desse ponteiro pra essa minha variável

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
    *elemento_alvo == PORTAO ? PORTAO : VAZIO;

  // Restaura o que havia abaixo dele
  cenario->matriz
      [elemento_antes.posicao.linha]
      [elemento_antes.posicao.coluna]
          = elemento_antes.elemento_abaixo;
    
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
    coloca_elemento_no_cenario(jogo->jon, jogo->cenario_atual);
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
  coloca_elemento_no_cenario(jogo->bran, jogo->cenario_atual);
}

void tenta_abrir_portao(Jogo *jogo) {
  if (jogo->contexto.elemento_alvo != PORTAO) return;

  if (jogo->cenario_atual->etapa == CASTELO_NEGRO) {
    jogo->estado.determinacao = rand() % 101;

    if (jogo->estado.determinacao >= 70) {
        jogo->alem_da_muralha = monta_alem_da_muralha(&jogo->jon, &jogo->bran);
        jogo->cenario_atual = &jogo->alem_da_muralha;
    } else {
      jogo->estado.vida -= 15;
    }

  } else if (jogo->cenario_atual->etapa == ALEM_DA_MURALHA) {
    jogo->cenario_atual = &jogo->castelo_negro;
  }
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

void combater_caminhante(EstadoJogo *estado) {
  if (estado->obsidiana > 0) {
    estado->obsidiana -= 1;
    estado->vida -= 5;
  } else {
    estado->vida -= 25;
  }
}

void recuperar_vida(EstadoJogo *estado) {
  if (estado->vida <= VIDA_INICIAL - 10) {
    estado->vida += 10;
  } else {
    estado->vida = VIDA_INICIAL;
  }
}

void aplicar_efeito_elemento(EstadoJogo *estado, ContextoAcao contexto) {
  switch (contexto.elemento_alvo) {

    case CAMINHANTE_BRANCO:
      combater_caminhante(estado);
      break;

    case VIDRO_DRAGAO:
      estado->obsidiana += 1;
      break;

    case FOGUEIRA:
      recuperar_vida(estado);
      break;
    
    default:
      break;
  }
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

  Jogo jogo = inicia_o_jogo();
  jogo.cenario_atual = &jogo.castelo_negro;

  while (!jogo.estado.fim) {
    imprime_cenario(jogo);

    jogo.contexto.comando = capturar_tecla();

    lidar_comando(&jogo);
    aplicar_efeito_elemento(&jogo.estado, jogo.contexto);

    processa_a_rodada(&jogo);
  }

  if (jogo.estado.venceu) {
      printf("\n\n==============================\n");
      printf("          VITORIA!\n");
      printf("==============================\n");
      printf("Jon Snow conseguiu resgatar Bran\n");
      printf("e sobreviver aos perigos alem da Muralha!\n");
      printf("\nParabens, voce venceu!\n");
      printf("==============================\n\n");
  } else {
      printf("\n\n==============================\n");
      printf("         GAME OVER\n");
      printf("==============================\n");
      printf("Jon Snow nao resistiu aos perigos\n");
      printf("alem da Muralha.\n");
      printf("\nA Patrulha da Noite perdeu um de seus irmaos...\n");
      printf("==============================\n\n");
  }

  return 0;
}