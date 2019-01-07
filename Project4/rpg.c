#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "rpg.h"

static char* armors[] = {"cloth","studded leather","ring mail","chain mail","plate"};
static int armorVals[] = {10, 12, 14, 16, 18};
static char* weapons[] = {"dagger","short sword","long sword","great sword","great axe"};
static struct character *hero;
static struct character *(world[10]);

int main(){
	//signal(SIGRTMIN, sigrtmin_handler);
	
	printf("WELCOME TO THE RPG\n\n");	
	
	//allocate memory for character structs
	hero = (struct character *) malloc(sizeof(struct character));
	
	int i;
	for(i=0;i<=9;i++){
		world[i] = (struct character *) malloc(sizeof(struct character));		
	}
	
	if(access("rpg.save", F_OK) != -1){
		
		//user has chance to load save		
		char yn;
		do{
			printf("Found save file. Continue where you left off (y/n)?");;
			scanf("%c", &yn);
			int c;
			while ((c = getchar()) != '\n' && c != EOF) { }
		
			if(yn == 'y' || yn == 'n'){
				break;
			}
			printf("Invalid choice - please select y/n\n");		
		}while(1);
			
		if(yn == 'y'){
			loadSave(hero, world);
		}
		else{
			newGame(hero, world);
		}
		
	} else {
		// start new game
		newGame(hero, world);
	}
	printf("Game initialized\n");
	lookCommand(hero, world);
	
	while(1){
		char choice[25];
		int arg;
		char *quit = "quit";
		char *stats = "stats";
		char *look = "look";
		char *fight = "fight";
		char tempbuf[6];
		
		do{
			printf("\ncommand >> ");
			scanf("%[^\n]s", choice);
			int c;
			while ((c = getchar()) != '\n' && c != EOF) { }
			
			strncpy(tempbuf, choice, 5);
			tempbuf[5] = '\0';
			if(strcmp(fight, tempbuf) == 0 && strlen(choice) == 7){
				arg = (int) choice[6];
				if(arg >= 48 && arg <= 57){
					arg -= 48;
					break;
				}
			}else if(strcmp(look, choice) == 0 || strcmp(stats, choice) == 0 || strcmp(quit, choice) == 0){
				break;
			}else if(strcmp(choice, "\n") == 0){
				printf("Hello\n", choice);
			}			
			printf("Invalid command - please re-enter\n");	
		}while(1);
		
		if(strcmp(look, choice) == 0){
			//look
			lookCommand(hero, world);
		}else if(strcmp(stats, choice) == 0){
			//stats
			statsCommand(hero);
		}else if(strcmp(fight, tempbuf) == 0){
			//fight
			printf("Fight %d command\n", arg);
			fightCommand(hero, world[arg], fd);
		}else{
			//quit
			saveAndQuit(hero, world);
			exit(0);
		}
	}
	
	return 0;
}

void earthquake(struct character *hero, struct character *(world[])){
	printf("EARTH QUAKE!!!\n\n");
	
	//damage world
	int i=0;
	for(i=0;i<10;i++){
		world[i]->hp -= 20;
		if(world[i]->hp <= 0){
			
			//print message
			printf("%s suffers -20 and dies.\n", world[i]->name);
			
			//respawn enemy
			char tempbuf[4];
			strncpy(tempbuf, world[i]->name, 3);
			tempbuf[3] = '\0';
			printf("\nRespawning %s...\n", world[i]->name);
			if(strcmp(tempbuf, "Orc") == 0){
				//build new Orc
				world[i]->arm = rand()%5;
				world[i]->weap = rand()%5;
				world[i]->lev = (rand() % hero->lev) + 1;
				world[i]->xp = 1000 * (1<<world[i]->lev);
				world[i]->hp = 20 + ((world[i]->lev-1)*5);
			}else if(strcmp(world[i]->name, "Gollum") == 0){
				//Gollum has 10 health
				world[i]->hp = 10;
			}else{
				//Sauron
				world[i]->hp = 20 + ((world[i]->lev-1)*5);
			}
			statsCommand(world[i]);
			
		}else{
			//print message
			printf("%s suffers -20 but survives.\n", world[i]->name);
		}
	}
	
	//damage hero
	hero->hp -= 20;
	if(hero->hp <= 0){
			
		//print message
		printf("%s suffers -20 and dies.\n", hero->name);
		
		//respawn hero
		printf("\nRespawning %s...\n", hero->name);
		hero->hp = 20 + ((hero->lev-1)*5);
		hero->xp = 1000 * (1<<hero->lev);
		statsCommand(hero);
		
	}else{
		//print message
		printf("%s suffers -20 but survives.\n", hero->name);
	}
	
	return;
}

int rollDice(int numDice, int dieSize, int fileDes){
	int val = 0;
	//write die size to dice driver
	unsigned char size = (unsigned char) dieSize;
	write(fileDes, &size, 1);
	
	int i;	
	for(i=0;i<numDice;i++){
		unsigned char roll;
		int ret = read(fileDes, &roll, 1);
		val += (int) roll;
	}
	return val;
}

void loadSave(struct character *hero, struct character *(world[])){
	
	// create save file
	FILE *inFile;
	inFile = fopen("rpg.save", "r"); 
	
	//*****read game state from file*****
	
	//read hero state
	fread(hero, sizeof(struct character), 1, inFile);
	
	//read world state
	int i;
	for(i=0;i<10;i++){
		fread(world[i], sizeof(struct character), 1, inFile);
	}
	
	// Close the file
	fclose(inFile);
	
	return;
}

void newGame(struct character *hero, struct character *(world[])){
	
	//*****Player Setup Stage*****
	
	hero->arm = 0;
	hero->weap = 0;
	hero->lev = 1;
	hero->xp = 2000;
	hero->hp = 20 + ((hero->lev -1)*5);	
	
	printf("What is your name?\n");
	scanf("%s", hero->name);
	
	//Select Armor
	printf("\nList of available armors:\n");
	printf("0: cloth (AC=10)\n");
	printf("1: studded leather (AC=12)\n");
	printf("2: ring mail (AC=14)\n");
	printf("3: chain mail (AC=16)\n");
	printf("4: plate (AC=18)\n");
	
	printf("\nChoose %s's Armor (0~4): ", hero->name);
	scanf("%d", &hero->arm);
	fflush(stdin);
	
	//Select Weapon
	printf("\nList of available weapons:\n");
	printf("0: dagger (damage=1d4)\n");
	printf("1: short sword (damage=1d6)\n");
	printf("2: long sword (damage=1d8)\n");
	printf("3: great sword (damage=2d6)\n");
	printf("4: great axe (damage=1d12)\n");
	
	int weap = 0;
	printf("\nChoose %s's Weapon (0~4): ", hero->name);
	scanf("%d", &hero->weap);
	
	int c;
	while ((c = getchar()) != '\n' && c != EOF) { }	
	
	printf("\nPlayer setting complete:\n");
	statsCommand(hero);
	
	//*****populate world*****
	
	//seed random number generator	
	srand((unsigned int)time(NULL));
	
	//build Sauron
	strcpy(world[0]->name, "Sauron");
	world[0]->arm = 4;
	world[0]->weap = 4;
	world[0]->lev = 20;
	world[0]->xp = 1048576000;	
	world[0]->hp = 20 + ((world[0]->lev-1)*5);
	
	//build orcs
	int i;
	for(i=1;i<=8;i++){
		sprintf(world[i]->name, "Orc %d", i);
		world[i]->arm = rand()%5;
		world[i]->weap = rand()%5;
		world[i]->lev = 1;
		world[i]->xp = 1000 * (1<<world[i]->lev);
		world[i]->hp = 20 + ((world[i]->lev -1)*5);		
	}
	
	//build Gollum
	strcpy(world[9]->name, "Gollum");
	world[9]->arm = 0;
	world[9]->weap = 0;
	world[9]->lev = 1;
	world[9]->xp = 2000;
	world[9]->hp = 10;
	
	return;
}

void saveAndQuit(struct character *hero, struct character *(world[])){
	// create save file
	
	FILE *outFile;
	outFile = fopen("rpg.save", "w"); 
	
	// Write game state to file
	
	//write hero state
	fwrite(hero, sizeof(struct character), 1, outFile);
	
	//write world state
	int i;
	for(i=0;i<10;i++){
		fwrite(world[i], sizeof(struct character), 1, outFile);
	}
	
	//free memory for character structs
	free(hero);
	
	for(i=0;i<10;i++){
		free(world[i]);
	}
	
	// Close the file
	fclose(outFile); 
	return;
}

void statsCommand(struct character *hero){
	printf("[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp = %ld]\n", hero->name, hero->hp, armors[hero->arm], weapons[hero->weap], hero->lev, hero->xp);
	return;
}

void lookCommand(struct character *hero, struct character *(world[])){
	printf("All is peaceful in the land of Mordor.\n");
	printf("Sauron and his minions are blissfully going about their business:\n");
	
	int i;
	for(i=0;i<10;i++){
		statsCommand(world[i]);
	}
	
	printf("Also at the scene are some adventurers looking for trouble:\n");
	statsCommand(hero);
		
	return;
}

void fightCommand(struct character *hero, struct character *enemy, int fileDes){
	//Fight Stage
	printf("\nFighting...\n\n");
	
	int gameOver = 0;

	int heroNumDice = 1;
	int enNumDice = 1;
	int heroDieSize = 4;
	int enDieSize = 4;
	
	//set player damage dice specifications based on weapon selection
	
	//if weapon is great sword, increase # damage dice, otherwise calc proper damage amount
	if(hero->weap == 3){
		heroNumDice = 2;
		heroDieSize = 6;
	}
	else{
		int i;
		for(i=0;i<hero->weap;i++){
			heroDieSize += 2;
		}
	}
	
	//if weapon is great sword, increase # damage dice, otherwise calc proper damage amount
	if(enemy->weap == 3){
		enNumDice = 2;
		enDieSize = 6;
	}
	else{
		int i;
		for(i=0;i<enemy->weap;i++){
			enDieSize += 2;
		}
	}
		
	while(gameOver == 0){
		//execute turn
		//printf("\n-------Round %d-------\n",round);
	
		//hero attack roll
		int att = rollDice(1, 20, fileDes);
		
		//hero damage roll (if hit)
		if(att > armorVals[enemy->arm]){
			int dam = rollDice(heroNumDice, heroDieSize, fileDes);
			enemy->hp -= dam;
			printf("%s hits %s for %d damage (attack roll %d)\n", hero->name, enemy->name, dam, att);
		}			
		else{
			printf("%s misses %s (attack roll %d)\n", hero->name, enemy->name, att);
		}		
		
		//enemy attack roll
		att = rollDice(1, 20, fileDes);
	
		//enemy damage roll (if hit)
		if(att > armorVals[hero->arm]){
			int dam = rollDice(enNumDice, enDieSize, fileDes);
			hero->hp -= dam;
			printf("%s hits %s for %d damage (attack roll %d)\n", enemy->name, hero->name, dam, att);
		}			
		else{
			printf("%s misses %s (attack roll %d)\n", enemy->name, hero->name, att);
		}
		
		if(hero->hp <= 0 && enemy->hp <= 0){
			printf("Both %s and %s are killed!!!", hero->name, enemy->name);
			
			//respawn enemy
			char tempbuf[4];
			strncpy(tempbuf, enemy->name, 3);
			tempbuf[3] = '\0';
			printf("\nRespawning %s...\n", enemy->name);
			if(strcmp(tempbuf, "Orc") == 0){
				//build new Orc
				enemy->arm = rand()%5;
				enemy->weap = rand()%5;
				enemy->lev = (rand() % hero->lev) + 1;
				enemy->xp = 1000 * (1<<enemy->lev);
				enemy->hp = 20 + ((enemy->lev-1)*5);
			}else if(strcmp(enemy->name, "Gollum") == 0){
				//Gollum has 10 health
				enemy->hp = 10;
			}else{
				//Sauron
				enemy->hp = 20 + ((enemy->lev-1)*5);
			}
			statsCommand(enemy);
			
			//respawn hero
			printf("\nRespawning %s...\n", hero->name);
			hero->hp = 20 + ((hero->lev-1)*5);
			hero->xp = 1000 * (1<<hero->lev);
			statsCommand(hero);
			
		}else if(hero->hp <= 0){
			printf("%s is killed by %s!", hero->name, enemy->name);
			
			//respawn hero
			printf("\nRespawning %s...\n", hero->name);
			hero->hp = 20 + ((hero->lev-1)*5);
			hero->xp = 1000 * (1<<hero->lev);
			statsCommand(hero);
			
			//heal enemy
			enemy->hp = 20 + ((enemy->lev -1)*5);
			
			gameOver = 1;
			
		}else if(enemy->hp <= 0){
			printf("%s is killed by %s!\n", enemy->name, hero->name);
			
			//option to take armor/weapon
			
			//exchange armor
			char yn;
			do{
				printf("Get %s's %s, exchanging Frodo's current armor %s (y/n)?", enemy->name, armors[enemy->arm], armors[hero->arm]);
				scanf("%c", &yn);
				int c;
				while ((c = getchar()) != '\n' && c != EOF) { }
			
				if(yn == 'y' || yn == 'n'){
					break;
				}
				printf("Invalid choice - please select y/n\n");		
			}while(1);
			
			if(yn == 'y'){
				hero->arm = enemy->arm;
			}						
						
			//exchange weapon		
			do{
				printf("Get %s's %s, exchanging Frodo's current weapon %s (y/n)?", enemy->name, weapons[enemy->weap], weapons[hero->weap]);
				scanf("%c", &yn);
				int c;
				while ((c = getchar()) != '\n' && c != EOF) { }
			
				if(yn == 'y' || yn == 'n'){
					break;
				}
				printf("Invalid choice - please select y/n\n");		
			}while(1);
			
			if(yn == 'y'){
				hero->weap = enemy->weap;
			}
			
			//heal hero
			hero->hp = 20 + ((hero->lev-1)*5);
			
			//give hero xp
			printf("%s gained %ld xp!\n", hero->name, enemy->xp);
			hero->xp += enemy->xp;
			
			//level up as necessary
			while(hero->xp >= (1000 * (1<<(hero->lev+1)))){
				hero->lev++;
				hero->hp = 20 + ((hero->lev-1)*5);
				printf("%s levels up to level %d\n", hero->name, hero->lev);
			}
			
			//show player state
			statsCommand(hero);
			
			//respawn enemy
			char tempbuf[4];
			strncpy(tempbuf, enemy->name, 3);
			tempbuf[3] = '\0';
			printf("\nRespawning %s...\n", enemy->name);
			if(strcmp(tempbuf, "Orc") == 0){
				//build new Orc
				enemy->arm = rand()%5;
				enemy->weap = rand()%5;
				enemy->lev = (rand() % hero->lev) + 1;
				enemy->xp = 1000 * (1<<enemy->lev);
				enemy->hp = 20 + ((enemy->lev-1)*5);
			}else if(strcmp(enemy->name, "Gollum") == 0){
				//Gollum has 10 health
				enemy->hp = 10;
			}else{
				//Sauron
				enemy->hp = 20 + ((enemy->lev-1)*5);
			}
			statsCommand(enemy);
			
			gameOver = 1;
		}
	}
	
	return;
}