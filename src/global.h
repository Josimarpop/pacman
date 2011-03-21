// Eixos
#define X 0
#define Y 1

// Velocidade do jogo em frames por segundo
#define PAC_FPS 24

// Máximo de frames pulados
#define PAC_PULOS_MAX 4

// Tempo (em ms) para verificar se um estado do jogo mudou
#define PAC_TIMER 500 

// Tamanho do Mapa
#define PAC_TAMANHO_MAPA 50
#define PAC_TAMANHO_MINIATURA 10
#define PAC_MAPA_ALTURA 21
#define PAC_MAPA_LARGURA 19

// Tipos de Tiles
#define PAC_TILE_CAMINHO 0
#define PAC_TILE_PAREDE 1
#define PAC_TILE_TELETRANSPORTE 2
#define PAC_TILE_BLINKY 3
#define PAC_TILE_PINKY 4
#define PAC_TILE_INKY 5
#define PAC_TILE_CLYDE 6
#define PAC_TILE_PACMAN 7
#define PAC_TILE_NADA 9

// Tipos de bolinhas
#define PAC_BOLINHA_NENHUMA 0
#define PAC_BOLINHA_NORMAL 1
#define PAC_BOLINHA_ESPECIAL 2

// Direções de movimento
#define PAC_DIRECAO_CIMA 0
#define PAC_DIRECAO_ESQUERDA 1
#define PAC_DIRECAO_BAIXO 2
#define PAC_DIRECAO_DIREITA 3

// Quantidade de Fantasmas
#define PAC_FANTASMAS 4

// Tipos de fantasmas
#define PAC_FANTASMA_BLINKY 0
#define PAC_FANTASMA_PINKY 1
#define PAC_FANTASMA_INKY 2
#define PAC_FANTASMA_CLYDE 3

// Estados dos fantasmas
#define PAC_ESTADO_ESPALHAR 0
#define PAC_ESTADO_PERSEGUIR 1

// Estados de captura dos fantasmas
#define PAC_CAPTURA_PRISAO -2
#define PAC_CAPTURA_NORMAL -1
#define PAC_CAPTURA_AZUL 0
#define PAC_CAPTURA_CAPTURADO 1

// Velocidade dos personagens
#define PAC_VELOCIDADE_PACMAN_A 0.167 // 1/6 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_A 0.125 // 1/8 tiles por frame
#define PAC_VELOCIDADE_PACMAN_B 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_B 0.167 // 1/6 tiles por frame
#define PAC_VELOCIDADE_PACMAN_C 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_C 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_PACMAN_D 0.2 // 1/5 tiles por frame
#define PAC_VELOCIDADE_FANTASMA_D 0.25 // 1/4 tiles por frame

// Número inicial de vidas
#define PAC_VIDAS 3

// Ângulo de abertura máxima da boca do pac-man
#define PAC_BOCA_MAX 45

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct tile {
	int tipo; // Tipo
	int bolinha; // Bolinha que está dentro
	int pos[2]; // Posição
	struct tile *tele; // Para onde teletransporta
} Tile;

typedef struct mapa {
	struct tile **tiles; // Matriz de tiles
	int bolinhas; // Bolinhas restantes
	unsigned int lista; // Lista de exibição
} Mapa;

typedef struct pacman {
	int vidas; // Quantidade de vidas
	int pontos; // Pontuação
	int fase; // Fase atual
	int morto; // Está morto
	int capturados; // Fantasmas capturados
	int direcao; // Direção de movimento
	int pre; // Direção do pré-move
	int boca; // Abertura da boca
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade
	int tele; // Se foi teletransportado
	struct tile *atual; // Posição atual
	struct tile *destino; // Próxima posição
	struct tile *inicio; // Posição inicial
} Pacman;

typedef struct fantasma {
	int nome; // Qual fantasma é
	int estado; // Estado
	int capturado; // Se pode ser capturado
	int direcao; // Direção de movimento
	double mov; // Porcentagem de movimento
	double velocidade; // Velocidade
	int tele; // Se foi teletransportado
	struct tile *atual; // Posição atual
	struct tile *destino; // Próxima posição
	struct tile *inicio; // Posição inicial
	struct tile *alvo; // Aonde vai
} Fantasma;

// Tiles do Mapa
int tTipos[][PAC_MAPA_LARGURA] =
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,0,1,
1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,
1,1,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,1,
9,9,9,1,0,1,0,0,0,0,0,0,0,1,0,1,9,9,9,
1,1,1,1,0,1,0,1,1,3,1,1,0,1,0,1,1,1,1,
2,0,0,0,0,0,0,1,5,4,6,1,0,0,0,0,0,0,2,
1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,
9,9,9,1,0,1,0,0,0,0,0,0,0,1,0,1,9,9,9,
1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,
1,0,0,1,0,0,0,0,0,7,0,0,0,0,0,1,0,0,1,
1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,
1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,
1,0,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

// Posições das Bolinhas
int tBolinhas[][PAC_MAPA_LARGURA] =
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
0,2,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,2,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
0,1,0,0,1,0,1,0,0,0,0,0,1,0,1,0,0,1,0,
0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
0,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,1,0,
0,2,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,2,0,
0,0,1,0,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,
0,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,0,
0,1,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,1,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

// Variáveis do jogo
int especiais = 0; // Quantidade de bolinhas especiais ativadas
int estados = 1; // Número de vezes nas quais o estado dos fantasmas mudou
int maiorPontuacao = 0; // Recorde atual do jogo

// gui.c
void inicializar();
void desenhar();
void mostrarMenu();
void atualizarJogo();
void mostrarJogo();
void centralizarCamera(Pacman *, double);
void reiniciarJogo();
void reiniciar(Pacman *, Fantasma *);
void mudarEstado(int);
void iniciou(int);
void morreu(int);
void passou(int);
void terminarEspecial(int);
void mover(int, int, int);
void teclado(unsigned char, int, int);
void redimensionar(int, int);
void escrever(char *, int, int, int, int);
void carregarBitmap(char *, unsigned int, int, int);

// mapa.h
Mapa *criaMapa(int [][PAC_MAPA_LARGURA], int [][PAC_MAPA_LARGURA], Pacman *, Fantasma *);
void mostrarMapa(Mapa *);
void reiniciarMapa(Mapa *, int [][PAC_MAPA_LARGURA]);
Tile *proximoTile(Mapa *, Tile *, int, int);
Tile *proximoTileEm(int, Mapa *, Tile *, int);
int ePrisao(Tile *);
double distanciaEntre(Tile *, Tile *);
void desenhaQuadrado(float);
#include "mapa.h"

// pacman.h
Pacman *criaPacman();
void movePacman(Pacman *, Mapa *, Fantasma *);
void mostrarPacman(Pacman *, double);
double velocidadePacman(Pacman *);
int pacmanPodeAndarSobre(Tile *);
int pacmanPodeMudarDirecao(Pacman *, Mapa *);
void estaVivo(Pacman *, Fantasma *);
void ganharPontos(Pacman *, int, int *);
void hemisferio(double);
#include "pacman.h"

// fantasma.h
Fantasma *criaFantasmas();
void moveFantasmas(Fantasma *, Mapa *, Pacman *);
void mostrarFantasmas(Fantasma *, double);
double velocidadeFantasma(Fantasma, Pacman *);
Tile *defineAlvo(Fantasma, Mapa *, Pacman *, Fantasma *);
int fantasmaPodeAndarSobre(Fantasma, Tile *);
int fantasmaPodeMudarDirecao(Fantasma, Mapa *);
int melhorCaminho(Fantasma, Mapa *, Pacman *);
#include "fantasma.h"
