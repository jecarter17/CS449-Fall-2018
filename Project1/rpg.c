#include <stdio.h>
#include <stdlib.h>

int rollDice(int numDice, int dieSize);

int main(){
	//Player Setup Stage
	
	//Select Armor
	printf("List of available armors:\n");
	printf("0: cloth (AC=10)\n");
	printf("1: studded leather (AC=12)\n");
	printf("2: ring mail (AC=14)\n");
	printf("3: chain mail (AC=16)\n");
	printf("4: plate (AC=18)\n");
	
	int armor1 = 0, armor2 = 0;
	printf("\nChoose Player 1 Armor (0~4): ");
	scanf("%d", &armor1);
	printf("Choose Player 2 Armor (0~4): ");
	scanf("%d", &armor2);
	fflush(stdin);
	
	//Select Weapons
	printf("\nList of available weapons:\n");
	printf("0: dagger (damage=1d4)\n");
	printf("1: short sword (damage=1d6)\n");
	printf("2: long sword (damage=1d8)\n");
	printf("3: great sword (damage=2d6)\n");
	printf("4: great axe (damage=1d12)\n");
	
	int weap1 = 0, weap2 = 0;
	printf("\nChoose Player 1 Weapon (0~4): ");
	scanf("%d", &weap1);
	printf("Choose Player 2 Weapon (0~4): ");
	scanf("%d", &weap2);
	fflush(stdin);
	
	//match selection to item name
	char* armors[] = {"cloth","studded leather","ring mail","chain mail","plate"};
	int armorVals[] = {10, 12, 14, 16, 18};
	char* weapons[] = {"dagger","short sword","long sword","great sword","great axe"};		
	
	printf("\nPlayer setting complete:\n");
	printf("[Player 1: hp=20, armor=%s, weapon=%s]\n", armors[armor1], weapons[weap1]);
	printf("[Player 2: hp=20, armor=%s, weapon=%s]\n", armors[armor2], weapons[weap2]);
	
	char choice;
	
	printf("\nFight? (y/n): ");
	scanf("%c", &choice);
	while(choice != 'y' && choice!= 'n'){
		fflush(stdin);
		printf("\nPlease select y/n");
		printf("\nFight? (y/n): ");
		scanf("%c", &choice);
	}
	
	
	if(choice == 'n'){
		exit(0);
	}	
	
	//seed random number generator
	srand((unsigned int)time(NULL));	
	
	int numDice1 = 1, numDice2 = 1;
	int dieSize1 = 4, dieSize2 = 4;
	
	//set player damage dice specifications based on weapon selection
	
	//if weapon is great sword, increase # damage dice, otherwise calc proper damage amount
	if(weap1 == 3){
		numDice1++;
		dieSize1 = 6;
	}
	else{
		int i;
		for(i=0;i<weap1;i++){
			dieSize1 += 2;
		}
	}
	
	//if weapon is great sword, increase # damage dice, otherwise calc proper damage amount
	if(weap2 == 3){
		numDice2++;
		dieSize2 = 6;
	}
	else{
		int i;
		for(i=0;i<weap2;i++){
			dieSize2 += 2;
		}
	}
	
	while(choice == 'y'){
		
		//Fight Stage
		printf("\nFighting...\n\n");
	
		int gameOver = 0;
		int round = 1;
		int hp1 = 20, hp2 = 20;				
		
		while(gameOver == 0){
			//execute turn
			printf("\n-------Round %d-------\n",round);
		
			//player 1 attack roll
			int att = rollDice(1, 20);
		
			//player 1 damage roll (if hit)
			if(att > armorVals[armor2]){
				int dam = rollDice(numDice1, dieSize1);
				hp2 -= dam;
				printf("Player 1 hits Player 2 for %d damage (attack roll %d)\n", dam, att);
			}			
			else{
				printf("Player 1 misses Player 2 (attack roll %d)\n", att);
			}		
		
			//player 2 attack roll
			att = rollDice(1, 20);
		
			//player 2 damage roll (if hit)
			if(att > armorVals[armor1]){
				int dam = rollDice(numDice2, dieSize2);
				hp1 -= dam;
				printf("Player 2 hits Player 1 for %d damage (attack roll %d)\n", dam, att);
			}			
			else{
				printf("Player 2 misses Player 1 (attack roll %d)\n", att);
			}	
		
			//display player info
			printf("[Player 1: hp=%d, armor=%s, weapon=%s]\n", hp1, armors[armor1], weapons[weap1]);
			printf("[Player 2: hp=%d, armor=%s, weapon=%s]\n", hp2, armors[armor2], weapons[weap2]);
		
			if(hp1 <= 0 && hp2 <= 0){
				printf("DRAW!!!");
				gameOver = 1;
			}else if(hp1 <= 0){
				printf("Player 2 WINS!");
				gameOver = 1;
			}else if(hp2 <= 0){
				printf("Player 1 WINS!");
				gameOver = 1;
			}
			
			round++;
		}	
		
		fflush(stdin);
		printf("\nFight? (y/n): ");
		scanf("%c", &choice);
		while(choice != 'y' && choice!= 'n'){
			fflush(stdin);
			printf("\nPlease select y/n");
			printf("\nFight? (y/n): ");
			scanf("%c", &choice);
		}
		
		if(choice == 'n'){
			exit(0);
		}
	}
	
	return 0;
}

int rollDice(int numDice, int dieSize){
	int val = 0;
	int i;
	for(i=0;i<numDice;i++){
		int roll = (rand() % dieSize) + 1;
		val += roll;
	}
	return val;
}