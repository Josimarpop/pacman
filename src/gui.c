#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"

// Variáveis do Game Loop
unsigned int tempoInicial;
unsigned int tempoAtual;
int passos;
int frame = 1000 / PAC_FPS;
double interpolacao;

// Variáveis da tela
int telaW = 800;
int telaH = 500;

// Variáveis do jogo
Pacman *pac;
Fantasma *fant;
Mapa *map;
char pontuacao[10];
char vidas[10];
char recorde[10];

// Variáveis de estado
int menu = 1;
int opcao = 0;
int pause = 0;
int inicio = 1;
int morte = 0;
int fim = 0;

int main() {
	// Configura a janela
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(telaW, telaH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pac-Man");
	//glutFullScreen();
	
	// Inicializa as variáveis
	inicializar();
	
	// Informa as funções de interação
	glutDisplayFunc(desenhar);
	glutSpecialFunc(mover);
	glutKeyboardFunc(teclado);
	glutReshapeFunc(redimensionar);
	
	// Inicia o jogo
	glutMainLoop();
}

// Inicializa as variáveis do jogo
void inicializar() {
	// Cria os objetos do jogo
	pac = criaPacman();
	fant = criaFantasmas(PAC_FANTASMAS);
	map = criaMapa(tTipos, tBolinhas, pac, fant);
	
	// Marca o tempo inicial
	tempoInicial = glutGet(GLUT_ELAPSED_TIME);
	
	// Carrega a imagem de fundo
	carregarBitmap("bg.bmp", 1, 480, 359);
	
	// Habilita o uso de buffer duplo
	glEnable(GL_DEPTH_TEST);
	glDrawBuffer(GL_BACK);
	
	// Inicia o gerador de números
	srand(time(NULL));
	
	glutTimerFunc(PAC_TIMER, mudarEstado, 5000);
}

// Desenha e exibe a imagem atual na tela
void desenhar() {
	// Limpa a tela
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Determina o tamanho da área de visualização
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(0, 0, telaW, telaH);
	
	// Utiliza a projeção ortográfica
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((double) -telaW/2, (double) telaW/2, (double) -telaH/2, (double) telaH/2, -2000, 2000);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (menu) {
		// Desenha o menu
		mostrarMenu();
	} else {
		// Desenha o jogo
		atualizarJogo();
		mostrarJogo();
	}
	
	glutSwapBuffers();
	glutPostRedisplay();
}

// Monta o menu do jogo
void mostrarMenu() {
	int i;
	char itens[4][21];
	char sobre[4][31];
	char controles[3][34];
	
	// Exibe imagem de fundo
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f((double) -telaW/2, (double) -telaH/2, 0);
		glTexCoord2f(0, 1);
		glVertex3f((double) -telaW/2, (double) telaH/2, 0);
		glTexCoord2f(1, 1);
		glVertex3f((double) telaW/2, (double) telaH/2, 0);
		glTexCoord2f(1, 0);
		glVertex3f((double) telaW/2, (double) -telaH/2, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	
	// Exibe o texto
	switch (menu) {
		case 1: // Tela principal
			if (pause)
				strcpy(itens[0], "Continuar Jogo");
			else
				strcpy(itens[0], "Iniciar Jogo");
			strcpy(itens[1], "Controles");
			strcpy(itens[2], "Sobre");
			strcpy(itens[3], "Sair");
			
			// Desenha os ítens
			for (i = 0; i < 4; i++)
				escrever(itens[i], 0, (double) telaH/2 + 25*i + 40, 0, opcao == i);
			
			// Desenha a pontuação recorde
			itoa(maiorPontuacao, recorde, 10);
			escrever("Recorde", 20+3*9, 20, 1, 0);
			escrever(recorde, 20, 35, 1, 0);
			break;
		case 2: // Controles
			strcpy(controles[0], "Setas: Controlam o Pac-Man");
			strcpy(controles[1], "Enter: Pausa (ou continua) o jogo");
			strcpy(controles[2], "Esc: Volta para o menu");
			
			// Escreve o texto
			for (i = 0; i < 3; i++)
				escrever(controles[i], (double) telaW/2 - 15*9, (double) telaH/2 + 25*(i-0.5), -1, 0);
			break;
		case 3: // Sobre
			strcpy(sobre[0], "Este jogo foi desenvolvido por");
			strcpy(sobre[1], "Andre Fonseca Costa como");
			strcpy(sobre[2], "projeto final na disciplina");
			strcpy(sobre[3], "de Computacao Grafica da UFPB.");
			
			// Escreve o texto
			for (i = 0; i < 4; i++)
				escrever(sobre[i], (double) telaW/2 - 15*9, (double) telaH/2 + 25*(i-1), -1, 0);
			break;
	}
}

// Atualiza o jogo um passo
void atualizarJogo() {
	int i;
	tempoAtual = glutGet(GLUT_ELAPSED_TIME);
	passos = 0;
	
	if (!pause) {
		// Atualiza o jogo, se não estiver pausado
		while ((tempoAtual - tempoInicial) > frame
			   && passos < PAC_PULOS_MAX) {
			// Verifica se ainda restam bolinhas
			if (map->bolinhas) {	
				// Se está começando a fase, espera
				if (inicio) {
					pac->boca = 30;
					glutTimerFunc(PAC_TIMER, iniciou, 2000);
				// Se perdeu uma vida, espera
				} else if (pac->morto && !morte) {
					morte = 1;
					glutTimerFunc(PAC_TIMER, morreu, 2000);
				// Se está normal, dá um passo
				} else if (!pac->morto && !fim) {
					movePacman(pac, map, fant);
					estaVivo(pac, fant);
				}
				
				// Move os fantasmas
				if (!inicio && !fim)
						moveFantasmas(fant, map, pac);
			} else {
				map->bolinhas = -1;
				fim = 1;
				glutTimerFunc(PAC_TIMER, passou, 2000);
			}
			
			tempoInicial += frame;
			tempoAtual = glutGet(GLUT_ELAPSED_TIME);
			passos++;
		}
		
		if ((tempoAtual - tempoInicial) > frame)
			tempoInicial = tempoAtual - frame;
		
		// Calcula a interpolação que deve ser usada
		interpolacao = min(1.0, (double) (tempoAtual - tempoInicial) / frame);
	} else {
		interpolacao = 0;
	}
}

// Monta a cena atual do jogo
void mostrarJogo() {
	// Exibe a pontuação atual e o recorde
	itoa(pac->pontos, pontuacao, 10);
	itoa(pac->vidas, vidas, 10);
	itoa(maiorPontuacao, recorde, 10);
	escrever("Pontos", 20, 20, -1, 0);
	escrever(pontuacao, 20+(10-strlen(pontuacao))*9, 35, -1, 0);
	escrever("Vidas", 20+12*9, 20, -1, 0);
	escrever(vidas, 20+12*9+(10-strlen(vidas))*9, 35, -1, 0);
	escrever("Recorde", 20+3*9, 20, 1, 0);
	escrever(recorde, 20, 35, 1, 0);
	
	// Informa quando está pausado
    if (pause)
		escrever("Pausado", 0, telaH/2-50, 0, 0);
	
	// Posição da câmera
	glRotated(35, 1, 0, 0);
	glRotated(-45, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	glScaled(PAC_TAMANHO_MAPA, PAC_TAMANHO_MAPA, PAC_TAMANHO_MAPA);
	
	// Se está morto, mover apenas os fantasmas
	if (morte || fim) {
		centralizarCamera(pac, 0);
		mostrarMapa(map);
		mostrarPacman(pac, 0);
		
		// Se passou de fase, não move os fantasmas
		if (fim)
			mostrarFantasmas(fant, 0);
		else
			mostrarFantasmas(fant, interpolacao);
	} else {
		centralizarCamera(pac, interpolacao);
		mostrarMapa(map);
		mostrarPacman(pac, interpolacao);
		mostrarFantasmas(fant, interpolacao);
	}
	
	// Miniatura
	glMatrixMode(GL_VIEWPORT);
	glLoadIdentity();
	glViewport(10, 10, (PAC_MAPA_LARGURA+1)*PAC_TAMANHO_MINIATURA, (PAC_MAPA_ALTURA+1)*PAC_TAMANHO_MINIATURA);
	
	// Posição da câmera na miniatura
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, PAC_MAPA_LARGURA+1, 0, PAC_MAPA_ALTURA+1, -600, 600);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0, 0, 400);
	
	// Se está morto, mover apenas os fantasmas
	mostrarMapa(map);
	if (morte) {
		mostrarPacman(pac, 0);
	} else {
		mostrarPacman(pac, interpolacao);
	}
	mostrarFantasmas(fant, interpolacao);
}

// Atualiza a posição da câmera, centralizando-a no pac-man
void centralizarCamera(Pacman *pac, double interpolacao) {
	double posX = pac->atual->pos[X];
	double posY = pac->atual->pos[Y];
	double delta = min(1.0, pac->mov + pac->velocidade * interpolacao);
	
	// Aplica a interpolação
	switch (pac->direcao) {
		case PAC_DIRECAO_CIMA:
			posY -= delta;
			break;
		case PAC_DIRECAO_ESQUERDA:
			posX -= delta;
			break;
		case PAC_DIRECAO_BAIXO:
			posY += delta;
			break;
		case PAC_DIRECAO_DIREITA:
			posX += delta;
			break;
	}
	
	// Desloca a câmera
	glTranslated(-posX, -(PAC_MAPA_ALTURA - posY + 1), 0);
}

// Reinicia o mapa e os personagens
void reiniciarJogo() {
	pac->vidas = PAC_VIDAS;
	pac->pontos = 0;
	pac->fase = 1;
	reiniciarMapa(map, tBolinhas);
	reiniciar(pac, fant);
}

// Reinicia os personagens do jogo
void reiniciar(Pacman *pac, Fantasma *fant) {
	int i;
	
	// Reinicia o gerador de números
	srand(time(NULL));
	
	// Reinicia os fantasmas
	for (i = 0; i < PAC_FANTASMAS; i++) {
		fant[i].capturado = PAC_CAPTURA_PRISAO;
		fant[i].direcao = PAC_DIRECAO_CIMA;
		fant[i].mov = 0;
		fant[i].velocidade = 0;
		fant[i].tele = 0;
		fant[i].atual = fant[i].inicio;
		fant[i].destino = 0;
		fant[i].alvo = 0;
	}
	
	// Reinicia o pac-man
	pac->direcao = PAC_DIRECAO_DIREITA;
	pac->pre = PAC_DIRECAO_DIREITA;
	pac->boca = 0;
	pac->mov = 0;
	pac->velocidade = 0;
	pac->tele = 0;
	pac->atual = pac->inicio;
	pac->destino = 0;
}

// Altera o estado dos fantasmas
void mudarEstado(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, mudarEstado, tempo);
	} else if (tempo <= 0) {
		int i;
		// Espalhar
		if (estados % 2 == 0) {
			for (i = 0; i < PAC_FANTASMAS; i++)
				fant[i].estado = PAC_ESTADO_ESPALHAR;
			estados++;
			glutTimerFunc(PAC_TIMER, mudarEstado, 20000);
		// Perseguir
		} else {
			for (i = 0; i < PAC_FANTASMAS; i++)
				fant[i].estado = PAC_ESTADO_PERSEGUIR;
			if (estados < 6) {
				estados++;
				glutTimerFunc(PAC_TIMER, mudarEstado, 5000);
			}
		}
	} else {
		glutTimerFunc(PAC_TIMER, mudarEstado, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de começar
void iniciou(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, iniciou, tempo);
	} else if (tempo <= 0) {
		// Começa o jogo
		inicio = 0;
	} else {
		glutTimerFunc(PAC_TIMER, iniciou, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de reiniciar
void morreu(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, morreu, tempo);
	} else if (tempo <= 0) {
		// Reinicia o pac-man
		pac->vidas--;
		pac->morto = 0;
		reiniciar(pac, fant);
		morte = 0;
		inicio = 1;
		
		// Se não tem mais vidas, volta ao menu
		if (pac->vidas <= 0)
			menu = 1;
	} else {
		glutTimerFunc(PAC_TIMER, morreu, tempo - PAC_TIMER);
	}
}

// Faz o jogador esperar antes de passar de fase
void passou(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, passou, tempo);
	} else if (tempo <= 0) {
		// Passa para a próxima fase
		pac->fase++;
		reiniciarMapa(map, tBolinhas);
		reiniciar(pac, fant);
		fim = 0;
		inicio = 1;
	} else {
		glutTimerFunc(PAC_TIMER, passou, tempo - PAC_TIMER);
	}
}

// Termina o poder especial obtido pelo pac-man
void terminarEspecial(int tempo) {
	if (pause) {
		glutTimerFunc(PAC_TIMER, terminarEspecial, tempo);
	} else if (tempo <= 0) {
		int i;
		// Se tiver mais de um ao mesmo tempo, continua
		if (especiais > 1) {
			especiais--;
		// Se tiver apenas um, acaba-o
		} else {
			for (i = 0; i < PAC_FANTASMAS; i++)
				if (fant[i].capturado == PAC_CAPTURA_AZUL)
					fant[i].capturado = PAC_CAPTURA_NORMAL;
			especiais = 0;
		}
	} else {
		glutTimerFunc(PAC_TIMER, terminarEspecial, tempo - PAC_TIMER);
	}
}

// Identifica a direção de movimento pelas setas do teclado
void mover(int tecla, int x, int y) {
	if (menu) {
		// Move o cursor do menu
		switch (tecla) {
			case GLUT_KEY_UP: // Move para cima
				opcao = (opcao - 1) % 4;
				if (opcao < 0) opcao += 4;
				break;
			case GLUT_KEY_DOWN: // Move para baixo
				opcao = (opcao + 1) % 4;
				break;
		}
	} else if (!pause) {
		// Move o pac-man
		switch (tecla) {
			case GLUT_KEY_UP: // Move para cima
				pac->pre = PAC_DIRECAO_CIMA;
				break;
			case GLUT_KEY_LEFT: // Move para a esquerda
				pac->pre = PAC_DIRECAO_ESQUERDA;
				break;
			case GLUT_KEY_DOWN: // Move para baixo
				pac->pre = PAC_DIRECAO_BAIXO;
				break;
			case GLUT_KEY_RIGHT: // Move para a direita
				pac->pre = PAC_DIRECAO_DIREITA;
				break;
		}
	}
}

// Identifica as teclas apertadas e realiza suas ações
void teclado(unsigned char tecla, int x, int y) {
	switch (tecla) {
		// Enter
		case 13:
			switch (menu) {
				case 0: // No jogo
					pause = !pause;
					break;
				case 1: // Na tela inicial
					switch (opcao) {
						case 0: // Iniciar Jogo
							if (!pause)
								reiniciarJogo();
							pause = 0;
							menu = 0;
							break;
						case 1: // Sobre
							menu = 2;
							break;
						case 2: // Controles
							menu = 3;
							break;
						case 3: // Sair
							exit(0);
							break;
					}
					break;
				default: // Numa seção do menu
					menu = 1;
					break;
			}
			break;
		// Esc
		case 27:
			pause = 1;
			menu = 1;
			break;
	}
}

// Captura as dimensões da tela
void redimensionar(int w, int h) {
	telaW = w;
	telaH = h;
}

// Escreve texto na tela
void escrever(char *texto, int x, int y, int alinhamento, int selecionado) {
	int i;
	double posX;
	
	// Se o texto está selecionado
	if (selecionado)
		glColor3f(1, 1, 0);
	else
		glColor3f(1, 1, 1);
	
	// Define o alinhamento
	if (alinhamento < 0)
		posX = (double) -telaW/2 + x;
	else if (alinhamento > 0)
		posX = (double) telaW/2 - x - 9*strlen(texto);
	else
		posX = (double) -9*strlen(texto)/2 + x;
	glRasterPos3f(posX, (double) telaH/2 - y - 9, 600);
	
	// Escreve o texto
	for (i = 0; i < strlen(texto); i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, texto[i]);
	}
}

// Abre um arquivo bitmap e carrega-o como textura
void carregarBitmap(char *arquivo, unsigned int id, int largura, int altura) {
	int i;
	unsigned char dados[(largura*altura+18)*3];
	unsigned char textura[largura*altura*3];
	unsigned char aux;
	FILE *fp;
	
	fp = fopen(arquivo, "r");
	if (!fp) return;
	fread(dados, (largura*altura+18)*3, 1, fp);
	fclose(fp);
	
	for (i = 18*3-1; i < (largura*altura+18)*3; i++)
		textura[i - 18*3] = dados[i];
	
	for (i = 0; i < largura*altura*3; i += 3) {
		aux = textura[i];
		textura[i] = textura[i+2];
		textura[i+2] = aux;
	}
	
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, largura, altura, 0, GL_RGB, GL_UNSIGNED_BYTE, textura);
}
