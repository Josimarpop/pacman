// Listas de Exibição
unsigned int corpo;
unsigned int olhos;

// Instancia os fantasmas
Fantasma *criaFantasmas() {
	int i;
	Fantasma *fant;
	GLUquadricObj *cilindro;
	
	fant = (Fantasma *) malloc(PAC_FANTASMAS * sizeof(Fantasma));
	if (!fant) return 0;
	
	for (i = 0; i < PAC_FANTASMAS; i++) {
		fant[i].nome = i;
		fant[i].estado = PAC_ESTADO_ESPALHAR;
		fant[i].capturado = PAC_CAPTURA_PRISAO;
		fant[i].direcao = PAC_DIRECAO_CIMA;
		fant[i].mov = 0;
		fant[i].velocidade = 0;
		fant[i].tele = 0;
		fant[i].atual = 0;
		fant[i].destino = 0;
		fant[i].inicio = 0;
		fant[i].alvo = 0;
	}
	
	cilindro = gluNewQuadric();
	gluQuadricDrawStyle(cilindro, GLU_FILL);
	
	corpo = glGenLists(1);
	glNewList(corpo, GL_COMPILE);
	gluCylinder(cilindro, 0.6, 0.6, 0.6, 30, 1);
	glTranslated(0, 0, 0.6);
	glutSolidSphere(0.6, 30, 30);
	glEndList();
	
	olhos = glGenLists(1);
	glNewList(olhos, GL_COMPILE);
	glColor3f(1, 1, 1);
	glTranslated(0.275, 0.5, 0.65);
	glScaled(1, 0.5, 1.75);
	glutSolidSphere(0.15, 30, 30);
	glColor3f(0, 0, 1);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.08, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslated(-0.275, 0.5, 0.65);
	glScaled(1, 0.5, 1.75);
	glutSolidSphere(0.15, 30, 30);
	glColor3f(0, 0, 1);
	glTranslated(0, 0.1, 0);
	glutSolidSphere(0.08, 30, 30);
	glEndList();
	
	return fant;
}

// Realiza um passo de movimento com o fantasma
void moveFantasmas(Fantasma *fant, Mapa *map, Pacman *pac) {
	int i;
	
	for (i = 0; i < PAC_FANTASMAS; i++) {
		// Atualiza o alvo
		fant[i].alvo = defineAlvo(fant[i], map, pac, fant);
		// Pega o próximo tile
		fant[i].destino = proximoTile(map, fant[i].atual, fant[i].direcao, fant[i].tele);
		// Verifica se pode mover-se para ele
		if (fantasmaPodeAndarSobre(fant[i], fant[i].destino)) { // Se é permitido
			// Anda
			fant[i].velocidade = velocidadeFantasma(fant[i], pac);
			// Verifica se completou o movimento
			if (fant[i].mov < 1) {
				// Dá um passo
				fant[i].mov += fant[i].velocidade;
			} else {
				// Permite ser capturado quando sai da prisão
				if (fant[i].capturado == PAC_CAPTURA_PRISAO
					&& !ePrisao(fant[i].destino))
					fant[i].capturado = PAC_CAPTURA_NORMAL;
				// Sai da prisão quando volta da captura
				else if (fant[i].capturado == PAC_CAPTURA_CAPTURADO
						 && fant[i].destino == fant[i].inicio)
					fant[i].capturado = PAC_CAPTURA_PRISAO;
				
				// Atualiza a posição
				if (fant[i].destino->tele)
					fant[i].tele = !fant[i].tele;
				fant[i].atual = fant[i].destino;
				fant[i].mov = 0;
				fant[i].direcao = melhorCaminho(fant[i], map, pac);
			}
		} else {
			// Pára
			fant[i].velocidade = 0;
			fant[i].direcao = melhorCaminho(fant[i], map, pac);
		}
	}
}

// Exibe os fantasmas
void mostrarFantasmas(Fantasma *fant, double interpolacao) {
	int i;
	
	for (i = 0; i < PAC_FANTASMAS; i++) {
		double posX = fant[i].atual->pos[X];
		double posY = fant[i].atual->pos[Y];
		double delta = min(1.0, fant[i].mov + fant[i].velocidade * interpolacao);
		
		switch (fant[i].direcao) {
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
		
		switch (fant[i].nome) {
			case PAC_FANTASMA_BLINKY:
				glColor3f(1, 0, 0);
				break;
			case PAC_FANTASMA_PINKY:
				glColor3f(1, 0.75, 1);
				break;
			case PAC_FANTASMA_INKY:
				glColor3f(0, 1, 1);
				break;
			case PAC_FANTASMA_CLYDE:
				glColor3f(1, 0.75, 0.3);
				break;
		}
		
		glPushMatrix();
		
		if (fant[i].capturado == PAC_CAPTURA_AZUL)
			glColor3f(0, 0, 1);
		
		glTranslated(posX + 1, PAC_MAPA_ALTURA - posY, 0);
		glRotated(90 * fant[i].direcao, 0, 0, 1);
		
		if (fant[i].capturado != PAC_CAPTURA_CAPTURADO) {
			glPushMatrix();
			glCallList(corpo);
			glPopMatrix();
		}
		
		glPushMatrix();
		glCallList(olhos);
		glPopMatrix();
		
		glPopMatrix();
	}
}

// Retorna a velocidade do fantasma
double velocidadeFantasma(Fantasma fant, Pacman *pac) {
	if (pac->fase <= 5)
		return PAC_VELOCIDADE_FANTASMA_A;
	else if (pac->fase <= 10)
		return PAC_VELOCIDADE_FANTASMA_B;
	else if (pac->fase <= 20)
		return PAC_VELOCIDADE_FANTASMA_C;
	else
		return PAC_VELOCIDADE_FANTASMA_D;
}

// Calcula o alvo do fantasma
Tile *defineAlvo(Fantasma fant, Mapa *map, Pacman *pac, Fantasma *blinky) {
	// Sair da prisão
	if (fant.capturado == PAC_CAPTURA_PRISAO) {
		return &map->tiles[8][7];
	// Fugir
	} else if (fant.capturado == PAC_CAPTURA_AZUL) {
		return &map->tiles[rand() % PAC_MAPA_ALTURA]
						  [rand() % PAC_MAPA_LARGURA];
	// Voltar à prisão
	} else if (fant.capturado == PAC_CAPTURA_CAPTURADO) {
		return fant.inicio;
	// Espalhar
	} else if (fant.estado == PAC_ESTADO_ESPALHAR) {
		return &map->tiles[(fant.nome/2)*(PAC_MAPA_ALTURA-1)]
						  [((fant.nome+1)%2)*(PAC_MAPA_LARGURA-1)];
	// Perseguir
	} else if (fant.estado == PAC_ESTADO_PERSEGUIR) {
		Tile *referencia;
		int delta[2];
		switch (fant.nome) {
			case PAC_FANTASMA_BLINKY:
				return pac->atual;
			case PAC_FANTASMA_PINKY:
				return proximoTileEm(4, map, pac->atual, pac->direcao);
			case PAC_FANTASMA_INKY:
				referencia = proximoTileEm(2, map, pac->atual, pac->direcao);
				delta[X] = referencia->pos[X] - blinky->atual->pos[X];
				delta[Y] = referencia->pos[Y] - blinky->atual->pos[Y];
				
				if (delta[X] > 0)
					referencia = proximoTileEm(2 * delta[X], map, blinky->atual, PAC_DIRECAO_DIREITA);
				else
					referencia = proximoTileEm(-2 * delta[X], map, blinky->atual, PAC_DIRECAO_ESQUERDA);
				
				if (delta[Y] > 0)
					referencia = proximoTileEm(2 * delta[Y], map, referencia, PAC_DIRECAO_BAIXO);
				else
					referencia = proximoTileEm(-2 * delta[Y], map, referencia, PAC_DIRECAO_CIMA);
				
				return referencia;
			case PAC_FANTASMA_CLYDE:
				if (distanciaEntre(fant.atual, pac->atual) > 8)
					return pac->atual;
				else
					return &map->tiles[PAC_MAPA_ALTURA-1][0];
		}
	}
}

// Verifica se é permitido passar sobre este tile
int fantasmaPodeAndarSobre(Fantasma fant, Tile *tile) {
	return tile != 0
		   && tile->tipo != PAC_TILE_PAREDE
		   && (!ePrisao(tile)
			   || fant.capturado == PAC_CAPTURA_PRISAO
			   || fant.capturado == PAC_CAPTURA_CAPTURADO)
		   && tile->tipo != PAC_TILE_NADA;
}

int fantasmaPodeMudarDirecao(Fantasma fant, Mapa *map) {
	Tile *destino;
	
	destino = proximoTile(map, fant.atual, fant.direcao, fant.tele);
	return fant.mov == 0
		   && destino != 0
		   && fantasmaPodeAndarSobre(fant, destino)
		   && !destino->tele;
}

// Indica a direção que o fantasma deve seguir
int melhorCaminho(Fantasma fant, Mapa *map, Pacman *pac) {
	int i;
	int podeMudar[4];
	double distancia[4];
	int menorDistancia = PAC_MAPA_LARGURA + PAC_MAPA_ALTURA; // Número maior que o possível
	int melhorDirecao = 4;
	int direcaoOposta = (fant.direcao + 2) % 4;
	Tile *destino;
	
	// Verifica as direções possíveis e a distância de cada uma do alvo
	for (i = 0; i < 4; i++) {
		fant.direcao = i;
		destino = proximoTile(map, fant.atual, fant.direcao, fant.tele);
		if (i != direcaoOposta && destino)
			podeMudar[i] = fantasmaPodeMudarDirecao(fant, map);
		else
			podeMudar[i] = 0;
		distancia[i] = distanciaEntre(destino, fant.alvo);
	}
	
	// Encontra a que tem menor distância do alvo
	for (i = 0; i < 4; i++) {
		if (podeMudar[i] && distancia[i] < menorDistancia) {
			melhorDirecao = i;
			menorDistancia = distancia[i];
		}
	}
	
	// Se foi encontrada uma direção, segue-a; se não, dá a volta
	if (melhorDirecao < 4)
		return melhorDirecao;
	else
		return direcaoOposta;
}
