// Instancia o mapa
Mapa *criaMapa(int tipos[][PAC_MAPA_LARGURA], int bolinhas[][PAC_MAPA_LARGURA], Pacman *pac, Fantasma *fant) {
	Mapa *map;
	Tile *teletransporte = NULL;
	int i, j, k, l = 0;
	
	// Instancia o mapa
	map = (Mapa *) malloc(sizeof(Mapa));
	if (!map) return 0;
	
	// Aloca memória para os tiles do mapa
	map->tiles = (Tile **) malloc(PAC_MAPA_ALTURA * sizeof(Tile *));
	for (k = 0; k < PAC_MAPA_ALTURA; k++) {
		map->tiles[k] = (Tile *) malloc(PAC_MAPA_LARGURA * sizeof(Tile));
	}
	
	// Preenche as informações dos tiles
	for (j = 0; j < PAC_MAPA_ALTURA; j++) {
		for (i = 0; i < PAC_MAPA_LARGURA; i++) {
			map->tiles[j][i].tipo = tipos[j][i];
			map->tiles[j][i].bolinha = bolinhas[j][i];
			map->tiles[j][i].pos[X] = i;
			map->tiles[j][i].pos[Y] = j;
			
			// Liga os teletransportes
			if (tipos[j][i] == PAC_TILE_TELETRANSPORTE) {
				if (teletransporte) {
					map->tiles[j][i].tele = teletransporte;
					teletransporte->tele = &map->tiles[j][i];
					teletransporte = NULL;
				} else {
					teletransporte = &map->tiles[j][i];
				}
			} else {
				map->tiles[j][i].tele = 0;
			}
		}
	}
	
	// Cria a lista de exibição dos tiles
	map->lista = glGenLists(1);
	glNewList(map->lista, GL_COMPILE);
	glTranslated(0.5, -0.5, 0);
	for (j = PAC_MAPA_ALTURA - 1; j >= 0; j--) {
		glTranslated(0, 1, 0);
		glPushMatrix();
		for (i = 0; i < PAC_MAPA_LARGURA; i++) {
			glBegin(GL_QUADS);
			switch (map->tiles[j][i].tipo) {
				case PAC_TILE_CAMINHO:
					glColor3f(0, 0, 0);
					desenhaQuadrado(1);
					break;
				case PAC_TILE_PAREDE:
					glColor3f(0, 0.1, 0.2);
					desenhaQuadrado(1);
					break;
				case PAC_TILE_TELETRANSPORTE:
					glColor3f(0.3, 0, 0.1);
					desenhaQuadrado(1);
					break;
				case PAC_TILE_BLINKY:
					glColor3f(0, 0, 0);
					desenhaQuadrado(1);
					fant[PAC_FANTASMA_BLINKY].atual = &map->tiles[j][i];
					fant[PAC_FANTASMA_BLINKY].inicio = &map->tiles[j][i];
					break;
				case PAC_TILE_PINKY:
					glColor3f(0, 0, 0);
					desenhaQuadrado(1);
					fant[PAC_FANTASMA_PINKY].atual = &map->tiles[j][i];
					fant[PAC_FANTASMA_PINKY].inicio = &map->tiles[j][i];
					break;
				case PAC_TILE_INKY:
					glColor3f(0, 0, 0);
					desenhaQuadrado(1);
					fant[PAC_FANTASMA_INKY].atual = &map->tiles[j][i];
					fant[PAC_FANTASMA_INKY].inicio = &map->tiles[j][i];
					break;
				case PAC_TILE_CLYDE:
					glColor3f(0, 0, 0);
					desenhaQuadrado(1);
					fant[PAC_FANTASMA_CLYDE].atual = &map->tiles[j][i];
					fant[PAC_FANTASMA_CLYDE].inicio = &map->tiles[j][i];
					break;
				case PAC_TILE_PACMAN:
					glColor3f(0, 0, 0);
					desenhaQuadrado(1);
					pac->atual = &map->tiles[j][i];
					pac->inicio = &map->tiles[j][i];
					break;
			}
			glEnd();
			glTranslated(1, 0, 0);
		}
		glPopMatrix();
	}
	glEndList();
	
	map->bolinhas = 0;
	for (j = 0; j < PAC_MAPA_ALTURA; j++) {
		for (i = 0; i < PAC_MAPA_LARGURA; i++) {
			if (bolinhas[j][i] != PAC_BOLINHA_NENHUMA)
				map->bolinhas++;
		}
	}
	
	return map;
}

// Monta o mapa
void mostrarMapa(Mapa *map) {
	int i, j;
	
	// Exibe o mapa
	glPushMatrix();
	glCallList(map->lista);
	glPopMatrix();
	
	// Exibe as bolinhas
	glPushMatrix();
	glTranslated(1, 0, 0.45);
	for (j = PAC_MAPA_ALTURA - 1; j >= 0; j--) {
		glTranslated(0, 1, 0);
		glPushMatrix();
		for (i = 0; i < PAC_MAPA_LARGURA; i++) {
			glPushMatrix();
			// Exibe as bolinhas restantes
			switch (map->tiles[j][i].bolinha) {
				case PAC_BOLINHA_NORMAL:
					glColor3f(1, 1, 1);
					glutSolidSphere(0.1, 10, 10);
					break;
				case PAC_BOLINHA_ESPECIAL:
					glColor3f(1, 1, 1);
					glutSolidSphere(0.2, 10, 10);
					break;
			}
			glPopMatrix();
			glTranslated(1, 0, 0);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

// Reinicia o mapa
void reiniciarMapa(Mapa *map, int bolinhas[][PAC_MAPA_LARGURA]) {
	int i, j;
	
	// Renova as bolinhas
	map->bolinhas = 0;
	for (j = 0; j < PAC_MAPA_ALTURA; j++) {
		for (i = 0; i < PAC_MAPA_LARGURA; i++) {
			map->tiles[j][i].bolinha = bolinhas[j][i];
			if (bolinhas[j][i] != PAC_BOLINHA_NENHUMA)
				map->bolinhas++;
		}
	}
}

// Retorna um ponteiro para o próximo tile
Tile *proximoTile(Mapa *map, Tile *tile, int direcao, int tele) {
	// Verifica se o tile pertence ao mapa
	if (tile != 0
		&& tile->pos[X] > -1
		&& tile->pos[X] < PAC_MAPA_LARGURA
		&& tile->pos[Y] > -1
		&& tile->pos[Y] < PAC_MAPA_ALTURA) {
		// Se o tile possui um teletransporte
		if (tile->tele && tele) return tile->tele;
		// Procura o próximo
		switch (direcao) {
			case PAC_DIRECAO_CIMA:
				if (tile->pos[Y] == 0) return 0;
				return &map->tiles[tile->pos[Y]-1][tile->pos[X]];
			case PAC_DIRECAO_DIREITA:
				if (tile->pos[X] == PAC_MAPA_LARGURA - 1) return 0;
				return &map->tiles[tile->pos[Y]][tile->pos[X]+1];
			case PAC_DIRECAO_BAIXO:
				if (tile->pos[Y] == PAC_MAPA_ALTURA - 1) return 0;
				return &map->tiles[tile->pos[Y]+1][tile->pos[X]];
			case PAC_DIRECAO_ESQUERDA:
				if (tile->pos[X] == 0) return 0;
				return &map->tiles[tile->pos[Y]][tile->pos[X]-1];
		}
	}
	return 0;
}

// Retorna o n-ésimo tile seguinte
Tile *proximoTileEm(int passos, Mapa *map, Tile *tile, int direcao) {
	Tile *atual = tile, *proximo = NULL;
	int i = 1;
	
	proximo = proximoTile(map, atual, direcao, 0);
	while (i < passos && proximo) {
		atual = proximo;
		proximo = proximoTile(map, atual, direcao, 0);
		i++;
	}
	
	return atual;
}

// Verifica se o tile faz parte da prisão
int ePrisao(Tile *tile) {
	return tile != 0
		   && (tile->tipo == PAC_TILE_BLINKY
			   || tile->tipo == PAC_TILE_PINKY
			   || tile->tipo == PAC_TILE_INKY
			   || tile->tipo == PAC_TILE_CLYDE);
}

// Calcula a distância entre 2 tiles
double distanciaEntre(Tile *a, Tile *b) {
	int deltaX = a->pos[X] - b->pos[X];
	int deltaY = a->pos[Y] - b->pos[Y];
	
	return sqrt((double) deltaX * deltaX + deltaY * deltaY);
}

// Desenha um quadrado na tela
void desenhaQuadrado(float tamanho) {
	glBegin(GL_QUADS);
	glVertex2f(0, tamanho);
	glVertex2f(0, 0);
	glVertex2f(tamanho, 0);
	glVertex2f(tamanho, tamanho);
	glEnd();
}
