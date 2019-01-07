struct character{	
	char name[50];
	int arm;
	int weap;
	int lev;
	long xp;
	int hp;
};

int rollDice(int numDice, int dieSize, int fileDes);
void earthquake(struct character *hero, struct character *(world[]));
void loadSave(struct character *hero, struct character *(world[]));
void newGame(struct character *hero, struct character *(world[]));
void saveAndQuit(struct character *hero, struct character *(world[]));
void statsCommand(struct character *hero);
void lookCommand(struct character *hero, struct character *(world[]));
void fightCommand(struct character *hero, struct character *enemy, int fileDes);