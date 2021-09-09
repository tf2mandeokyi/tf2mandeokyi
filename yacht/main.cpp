#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

namespace YachtGame
{
	namespace ScoreCategory
	{
		enum ScoreCategory : char
		{
			ONES = 0, TWOS, THREES, FOURS, FIVES, SIXES, CHOICE, FOUR_KIND, 
			FULL_HOUSE, SMALL_STRAIGHT, LARGE_STRAIGHT, YACHT
		};
	}
	
	const char* categoryName[] = {
		"Aces", "Deuces", "Threes", "Fours", "Fives", "Sixes", "Choice",
		"4 of a Kind", "Full House", "S. Straight", "L. Straight",
		"Yacht"
	};
	
	const bool dice_dots[6][3][3] = {
		{{ 0, 0, 0 }, { 0, 1, 0 }, { 0, 0, 0 }},
		{{ 1, 0, 0 }, { 0, 0, 0 }, { 0, 0, 1 }},
		{{ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }},
		{{ 1, 0, 1 }, { 0, 0, 0 }, { 1, 0, 1 }},
		{{ 1, 0, 1 }, { 0, 1, 0 }, { 1, 0, 1 }},
		{{ 1, 0, 1 }, { 1, 0, 1 }, { 1, 0, 1 }},
	};
	
	struct Player
	{
		// Using char instead of int here because int is just too big to use
		char scores[12];
		
		Player() {
			for(char i = 0; i < 12; ++i) this->scores[i] = -1;
		}
		
		short get_total_score()
		{
			char score;
			short total = 0;
			for(char i = 0; i < 12; ++i)
			{
				score = this->scores[i];
				if(score != -1) total += score;
			}
			if(this->get_subtotal_score() >= 63)
			{
				total += 35;
			}
			return total;
		}
		
		short get_subtotal_score()
		{
			char score;
			short total = 0;
			for(char i = 0; i < 6; ++i)
			{
				score = this->scores[i];
				if(score != -1) total += score;
			}
			return total;
		}
	};
	
	struct Dices
	{
		char num[5];
		bool should_roll[5];
		char scores[12];
		char rollCount;
		
		Dices()
		{
			for(char i = 0; i < 5; ++i) 
			{
				this->num[i] = 1;
				this->should_roll[i] = true;
			}
			this->calc_scores();
		}
		
		void reset()
		{
			this->rollCount = 0;
			for(char i = 0; i < 5; ++i) 
			{
				this->should_roll[i] = true;
			}
			this->roll();
		}
		
		void roll()
		{
			for(char i = 0; i < 5; ++i)
			{
				if(this->should_roll[i])
				{
					this->num[i] = rand() % 6 + 1;
				}
			}
			this->calc_scores();
			this->rollCount++;
		}
		
		void calc_scores()
		{
			using namespace ScoreCategory;
			
			char count[6] = {0, 0, 0, 0, 0, 0};
			bool two_same_exists, three_same_exists;
			char start = -1, end = 5;
			char sum = 0;
			
			for(char i = 0; i < 5; ++i) 
			{
				++count[this->num[i] - 1];
				sum += this->num[i];
			}
			
			this->scores[ONES] = count[0];
			this->scores[TWOS] = count[1] * 2;
			this->scores[THREES] = count[2] * 3;
			this->scores[FOURS] = count[3] * 4;
			this->scores[FIVES] = count[4] * 5;
			this->scores[SIXES] = count[5] * 6;
			this->scores[CHOICE] = sum;
			this->scores[FOUR_KIND] = 0;
			this->scores[FULL_HOUSE] = 0;
			this->scores[SMALL_STRAIGHT] = 0;
			this->scores[LARGE_STRAIGHT] = 0;
			this->scores[YACHT] = 0;
			
			for(char i = 0; i < 6; ++i)
			{
				if(count[i] >= 1 && start == -1) start = i;
				if(count[i] == 0 && end == 5 && start != -1) end = i - 1;
				
				switch(count[i])
				{
					case 2: two_same_exists = true; break;
					case 3: three_same_exists = true; break;
					case 4: this->scores[FOUR_KIND] = sum; break;
					case 5: this->scores[YACHT] = 50; break;
				}
			}
			
			if(two_same_exists && three_same_exists)
			{
				this->scores[FULL_HOUSE] = sum;
			}
			
			if(end - start >= 3) this->scores[SMALL_STRAIGHT] = 15;
			if(end - start >= 4) this->scores[LARGE_STRAIGHT] = 30;
		}
		
		void print(char lineNum, bool printSaved)
		{
			if(lineNum == 0)
			{
				for(char i = 0; i < 5; ++i)
				{
					if(printSaved != this->should_roll[i])
					{
						printf("+-------+   ");
					}
				}
			}
			else if(lineNum == 4)
			{
				for(char i = 0; i < 5; ++i)
				{
					if(printSaved != this->should_roll[i])
					{
						printf("+-------+ %d ", i+1);
					}
				}
			}
			else
			{
				for(char j = 0; j < 5; ++j)
				{
					if(printSaved != this->should_roll[j])
					{
						printf("| ");
						for(char i = 0; i < 3; ++i)
						{
							if(dice_dots[this->num[j] - 1][lineNum - 1][i] == 1)
							{
								printf("# ");
							}
							else
							{
								printf("  ");
							}
						}
						printf("|   ");
					}
				}
			}
		}
	};
	
	struct Game
	{
		Dices dices;
		Player* players;
		char playerCount, roundTurn, playerTurn;
		const char* message;
		bool gameEnded;
		
		Game() {}
		
		void set_player_count(char playerCount)
		{
			this->playerCount = playerCount;
			this->players = new Player[playerCount];
			this->playerTurn = 0;
			this->message = "";
			this->gameEnded = false;
		}
		
		void draw_scoreboard()
		{
			char p = this->playerCount, i, j;
			char score;
			char buf[5];
			
			// #####################################
			printf("\n                      +");
			for(i = 0; i < p; ++i) printf("-------+");
			
			// #####################################
			printf("\n         턴 %2d/12     |", this->roundTurn + 1);
			for(i = 0; i < p; ++i) printf("       |");
			
			// #####################################
			printf("\n                      |");
			for(i = 0; i < p; ++i)
			{
				if(this->playerTurn == i && !this->gameEnded)
				{
					snprintf(buf, 5, "[P%d]", i+1);
				}
				else
				{
					snprintf(buf, 5, "P%d", i+1);
				}
				printf(" %5s |", buf);
			}
			
			// #####################################
			printf("\n         카테고리     |");
			for(i = 0; i < p; ++i) printf("       |");
			
			// #####################################
			this->draw_horizontal_line();
			if(!this->gameEnded)
			{
				printf("     저장한 주사위:");
			}
			
			// #####################################
			for(j = 0; j < 6; ++j)
			{
				printf("\n   | %2d.  %11s |", j+1, categoryName[j]);
				for(i = 0; i < p; ++i)
				{
					score = this->players[i].scores[j];
					if(score != -1) printf(" %5d |", score);
					else if(this->playerTurn == i)
					{
						snprintf(buf, 5, "(%d)", this->dices.scores[j]);
						printf(" %5s |", buf);
					}
					else printf("       |");
				}
				if(j != 5 && !this->gameEnded)
				{
					printf("      ");
					this->dices.print(j, true);
				}
			}
			
			// #####################################
			this->draw_horizontal_line();
			
			// #####################################
			printf("\n   |        1~6  합산 |");
			for(i = 0; i < p; ++i) 
			{
				printf(" %2d/63 |", this->players[i].get_subtotal_score());
			}
			
			// #####################################
			printf("\n   |       +35 보너스 |");
			for(i = 0; i < p; ++i) 
			{
				if(this->players[i].get_subtotal_score() >= 63)
				{
					printf("    35 |");
				}
				else
				{
					printf("       |");
				}
			}
			if(!this->gameEnded)
			{
				printf("     주사위:");
			}
			
			// #####################################
			this->draw_horizontal_line();
			if(!this->gameEnded)
			{
				printf("      ");
				this->dices.print(0, false);
			}
			
			// #####################################
			printf("\n    63점을 넘길 경우 보너스 추가     ");
			for(char i = 0; i < (p - 1) * 8; ++i)
			{
				printf(" ");
			}
			if(!this->gameEnded)
			{
				this->dices.print(1, false);
			}
			
			// #####################################
			this->draw_horizontal_line();
			if(!this->gameEnded)
			{
				printf("      ");
				this->dices.print(2, false);
			}
			
			// #####################################
			printf("\n   |  7.  %11s |", categoryName[6]);
			for(i = 0; i < p; ++i)
			{
				score = this->players[i].scores[6];
				if(score != -1) printf(" %5d |", score);
				else if(this->playerTurn == i)
				{
					snprintf(buf, 5, "(%d)", this->dices.scores[j]);
					printf(" %5s |", buf);
				}
				else printf("       |");
			}
			if(!this->gameEnded)
			{
				printf("      ");
				this->dices.print(3, false);
			}
			
			// #####################################
			printf("\n   +==================+");
			for(i = 0; i < p; ++i) printf("=======+");
			if(!this->gameEnded)
			{
				printf("      ");
				this->dices.print(4, false);
			}
			
			// #####################################
			for(j = 7; j < 12; ++j)
			{
				printf("\n   | %2d.  %11s |", j+1, categoryName[j]);
				for(i = 0; i < p; ++i)
				{
					score = this->players[i].scores[j];
					if(score != -1) printf(" %5d |", score);
					else if(this->playerTurn == i)
					{
						snprintf(buf, 5, "(%d)", this->dices.scores[j]);
						printf(" %5s |", buf);
					}
					else printf("       |");
				}
				if(j == 9 && !this->gameEnded)
				{
					printf("     굴릴 수 있는 횟수: %d", 
							3 - this->dices.rollCount);
				}
			}
			
			// #####################################
			printf("\n   +==================+");
			for(i = 0; i < p; ++i) printf("=======+");
			
			// #####################################
			printf("\n   |             합산 |");
			for(i = 0; i < p; ++i) 
			{
				printf(" %5d |", this->players[i].get_total_score());
			}
			
			// #####################################
			this->draw_horizontal_line();
			
			// #####################################
		}
		
		void draw_horizontal_line()
		{
			printf("\n   +------------------+");
			for(char i = 0; i < this->playerCount; ++i) printf("-------+");
		}
		
		void draw()
		{
			system("cls");
			this->draw_scoreboard();
			printf("\n\n\n");
			if(this->gameEnded == true)
			{
				this->print_winners();
				system("pause");
				exit(0);
			}
			else
			{
				printf("사용 가능한 커맨드:\n");
				printf("   roll: 주사위를 굴립니다.\n");
				printf("   dice_<번호>: 해당하는 번호의 주사위를 저장하거나 불러옵니다.\n");
				printf("   store_<번호>: 해당하는 번호의 카테고리에 점수를 기록합니다.\n");
				printf("\n\n%s\n", this->message);
				this->await_input();
			}
		}
		
		void print_winners()
		{
			short max = -1, score, result;
			bool win[this->playerCount];
			for(char i = 0; i < this->playerCount; ++i)
			{
				score = this->players[i].get_total_score();
				if(score > max)
				{
					max = score;
					win[i] = true; 
					for(char j = 0; j < i; ++j)
					{
						win[j] = false;
					}
				}
			}
			for(char i = 0; i < this->playerCount; ++i)
			{
				if(win[i])
				{
					printf("P%d ", i+1);
				}
			}
			printf("승!\n\n");
		}
		
		void next_turn()
		{
			this->dices.reset();
			this->playerTurn++;
			if(this->playerTurn >= this->playerCount)
			{
				this->playerTurn = 0;
				if(this->roundTurn >= 11)
				{
					this->gameEnded = true;
				}
				else 
				{
					this->roundTurn++;
				}
			}
		}
		
		void await_input()
		{
			this->message = "";
			
			printf("커맨드: ");
			char input[10];
			char arg;
			
			scanf("%s", input);
			char* pch = strtok(input, "_");
			
			if(strcmp(pch, "roll") == 0)
			{
				if(this->dices.rollCount >= 3)
				{
					this->message = "더이상 주사위를 굴릴 수 없습니다.";
				}
				else
				{
					this->dices.roll();
				}
			}
			else if(strcmp(pch, "dice") == 0)
			{
				pch = strtok(NULL, " ");
				arg = atoi(pch);
				if(arg < 1 || arg > 5)
				{
					this->message = "잘못된 주사위 번호입니다.";
					return;
				}
				bool* b = &this->dices.should_roll[arg - 1];
				*b = !*b;
			}
			else if(strcmp(pch, "store") == 0)
			{
				pch = strtok(NULL, " ");
				arg = atoi(pch);
				if(arg < 1 || arg > 12)
				{
					this->message = "잘못된 카테고리 번호입니다.";
					return;
				}
				char* score = &this->players[this->playerTurn].scores[arg - 1];
				if(*score != -1)
				{
					this->message = "이미 점수가 기록된 카테고리입니다.";
					return;
				}
				*score = this->dices.scores[arg - 1];
				this->next_turn();
			}
			else
			{
				this->message = "알 수 없는 명령어입니다.";
				return;
			}
		}
	};
}

int main() {
	srand(time(NULL));
	YachtGame::Game game;
	char player_count;
	
	while(true)
	{
		printf("플레이어 수 (1~4): ");
		scanf("%d", &player_count);
		if(player_count < 1 || player_count > 4)
		{
			printf("1과 4 사이의 숫자만 입력해주세요.");
		}
		else break;
	}
	
	game.set_player_count(player_count);
	
	game.dices.roll();
	while(true)
	{
		game.draw();
	}
}
