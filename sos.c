#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 5

clock_t start, end;
double cpu_time_used;

//ezeknek a felhasznalo adhat erteket a program parameterek altal
int time_limit = -1;
int global_depth = 1;

//ebben taroljuk a jelenlegi allast
unsigned char table[N][N] = {0};

//ebben taroljuk az egy korrel regebbi allast
unsigned char table_old[N][N] = {0};


//ebben kapja meg a program a minimax algoritmus utan a meglependo allapotot
unsigned char global_next_state[N][N] = {0};


//egy allapotot tarol
typedef struct state
{
	unsigned char table[N][N];
	int heuristic_score;//vegul nem lett hasznalva
}state;


//a jatekosok pontjait taroljuk ezekben
int machine_points = 0;
int human_points = 0;


//egy pozicio fuggvenybol valo visszateritesehez kell a struct
typedef struct position
{
	int x ;
	int y ;
} position;


//kiirat egy nyilat
void print_arrow()
{
	printf("\t\t\t|\n");
	printf("\t\t\t|\n");
	printf("\t\t\t|\n");
	printf("\t\t\tv\n");
}


//megmondja, hogy vege van-e a jateknak
int end_of_game()
{
	int end = 1;

	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			if(table[i][j] == ' ')
			{
				end = 0;
			}
		}
	}
	
	return end;
}


//s_s mintat keres
int search_sxs(unsigned char reference_table[N][N])
{
	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			if(reference_table[i][j] == 's')
			{
				int x[8] = {-1,+1,-1,+1,0,1,0,-1};
				int y[8] = {-1,+1,+1,-1,1,0,-1,0};
				
				for(int k = 0; k < 8; ++k)
				{
					int x_index = i + x[k];
					int y_index = j + y[k];

					if((x_index > -1) && (x_index < N) && (y_index > -1) && (y_index < N))
					{
						if(reference_table[x_index][y_index] == ' ')
						{

							int x_index_diff = x_index - i;
							int y_index_diff = y_index - j;
							
							int x_index_pred = x_index + x_index_diff;
							int y_index_pred = y_index + y_index_diff;
										
							if((x_index_pred > -1) && (x_index_pred < N) && (y_index_pred > -1) && (y_index_pred < N))
							{
							
								if(reference_table[x_index_pred][y_index_pred] == 's')
								{		
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}
	
	return 0;
}



//so mintat keres
int search_so(unsigned char reference_table[N][N])
{	
	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			if(reference_table[i][j] == 's')
			{
				int x[8] = {-1,+1,-1,+1,0,1,0,-1};
				int y[8] = {-1,+1,+1,-1,1,0,-1,0};
				
				for(int k = 0; k < 8; ++k)
				{
					int x_index = i + x[k];
					int y_index = j + y[k];

					if((x_index > -1) && (x_index < N) && (y_index > -1) && (y_index < N))
					{
	
						if(reference_table[x_index][y_index] == 'o')
						{
							int x_index_diff = x_index - i;
							int y_index_diff = y_index - j;
							
							int x_index_pred = x_index + x_index_diff;
							int y_index_pred = y_index + y_index_diff;

							if((x_index_pred > -1) && (x_index_pred < N) && (y_index_pred > -1) && (y_index_pred < N))
							{
								if(reference_table[x_index_pred][y_index_pred] == ' ')
								{
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}


	return 0;
}


//sos mintat keres a glboalis tabla matrixban
int search_sos_table()
{
	print_state();

	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			if(table[i][j] == 's')
			{
				int x[8] = {-1,+1,-1,+1,0,1,0,-1};
				int y[8] = {-1,+1,+1,-1,1,0,-1,0};
				
				for(int k = 0; k < 8; ++k)
				{
					int x_index = i + x[k];
					int y_index = j + y[k];


					if((x_index > -1) && (x_index < N) && (y_index > -1) && (y_index < N))
					{
						
						//ha kaptunk egy o-t ami most volt beteve
						if(table[x_index][y_index] == 'o' && table_old[x_index][y_index] == ' ')
						{
							int x_index_diff = x_index - i;
							int y_index_diff = y_index - j;
							
							int x_index_pred = x_index + x_index_diff;
							int y_index_pred = y_index + y_index_diff;
										
							if((x_index_pred > -1) && (x_index_pred < N) && (y_index_pred > -1) && (y_index_pred < N))
							{
								if(table[x_index_pred][y_index_pred] == 's')
								{	
									for(int k = 0; k < N; ++k)
									{
										for(int l = 0; l < N; ++l)
										{
											table_old[k][l] = table[k][l];
										}
									}
									
									return 1;
								}
							}
						}
						else
						//ha kaptunk egy o-t
						if(table[x_index][y_index] == 'o')
						{

							int x_index_diff = x_index - i;
							int y_index_diff = y_index - j;
							
							int x_index_pred = x_index + x_index_diff;
							int y_index_pred = y_index + y_index_diff;
										
							if((x_index_pred > -1) && (x_index_pred < N) && (y_index_pred > -1) && (y_index_pred < N))
							{
								//ha kaptunk egy s-t ami most volt beteve
								if(table[x_index_pred][y_index_pred] == 's' && table_old[x_index_pred][y_index_pred] == ' ')
								{			
									
									for(int k = 0; k < N; ++k)
									{
										for(int l = 0; l < N; ++l)
										{
											table_old[k][l] = table[k][l];
										}
									}
									
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}
	
	printf("\n\n");

	for(int k = 0; k < N; ++k)
	{
		for(int l = 0; l < N; ++l)
		{
			table_old[k][l] = table[k][l];
		}
	}

	return 0;
}



//keres egy sos mintat a parameterkent megadott tomben, hogy meghatarozza ha valaki kell  kapjon pontot
// a heurisztikahoz van hasznalva
int search_sos(unsigned char reference_table[N][N])
{
	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			if(table[i][j] == 's')
			{
				int x[8] = {-1,+1,-1,+1,0,1,0,-1};
				int y[8] = {-1,+1,+1,-1,1,0,-1,0};
				
				for(int k = 0; k < 8; ++k)
				{
					int x_index = i + x[k];
					int y_index = j + y[k];


					if((x_index > -1) && (x_index < N) && (y_index > -1) && (y_index < N))
					{
						
						//ha kaptunk egy o-t ami most volt beteve
						if(reference_table[x_index][y_index] == 'o' && table[x_index][y_index] == ' ')
						{
							int x_index_diff = x_index - i;
							int y_index_diff = y_index - j;
							
							int x_index_pred = x_index + x_index_diff;
							int y_index_pred = y_index + y_index_diff;
										
							if((x_index_pred > -1) && (x_index_pred < N) && (y_index_pred > -1) && (y_index_pred < N))
							{
								if(table[x_index_pred][y_index_pred] == 's')
								{									
									return 1;
								}
							}
						}
						else
						//ha kaptunk egy o-t
						if(table[x_index][y_index] == 'o')
						{

							int x_index_diff = x_index - i;
							int y_index_diff = y_index - j;
							
							int x_index_pred = x_index + x_index_diff;
							int y_index_pred = y_index + y_index_diff;
										
							if((x_index_pred > -1) && (x_index_pred < N) && (y_index_pred > -1) && (y_index_pred < N))
							{
								//ha kaptunk egy s-t ami most volt beteve
								if(reference_table[x_index_pred][y_index_pred] == 's' && table[x_index_pred][y_index_pred] == ' ')
								{													
									return 1;
								}
							}
						}
					}
				}
			}
		}
	}
	
	//printf("\n\n");

	//regi tabla frissitese ha nem jott letre sos akkor is
	// for(int k = 0; k < N; ++k)
	// {
	// 	for(int l = 0; l < N; ++l)
	// 	{
	// 		table_old[k][l] = table[k][l];
	// 	}
	// }

	return 0;
}


//heurisztika, az allapotok ertekenek a meghatarozasara
int heuristics(unsigned char my_table[N][N])
{
	//2 alapeset
		//1# ha tudunk kihozni sos-t

		//2# ha nem tudunk kihozni sos-t
			//ne lehessen kihozni egy lepesbol sos-t


	//ha a jelenlegi allasbol lehet sos-t kihozni akkor pozitiv ertek --> 1
	//ha a jelenlegi allasbol nem lehet kihozni sos-t de nem ad lehetoseget az ellenfelnek a kovetkezo korben akkor kicsi pozitiv ertek ---> 0
	//ha a jelenlegi allas lehetoseget ad az ellenfelnek sos-t kialakitasara akkor  ---> -1
	//ha tobb lehetoseg is kialakul akkor azt az allapotot nagyobb ertekkel vagy kisebb ertekkel latjuk el

	// ha ki lehet hozni sos-t egy lepesbol a jelnlegi korben
	 if(search_sos(my_table))
	 {
		 //a gep a pontok pozitiv kulonbseget akarja maximalizalni
		return 1;
	 }
	 else
	 //ha most jelen van s_s vagy so akkor a kovetkezo jatekos ki tudna hozni sos-t
	 if(search_sxs(my_table) || search_so(my_table))
	 {
		//az ember a pontok negativ kulonbseget akarja minimalizalni
		 return -1;
	 }
	 //ha nem lehet kihozni sos-t
	 else
	 {
		 return 0;
	 }

	//hiba eseten 
	return -1;
}


int max(int one, int two)
{
	if(one > two)
	{
		return one;
	}

	return two;
}


int min(int one, int two)
{
	if(one > two)
	{
		return two;
	}

	return one;
}

//FONTOS
//minket az erdekel, hogy jelen allasban melyik agat kell valasztanunk a legnagyobb biztos pontszam elereseben
//ehhez nem csak a az allapotok heurisztikajat kell visszateriteni, hanem magat az allapotot amibe kerulni szeretnenk
int traverser(unsigned char table_current_state[N][N], int depth, int alpha, int beta, int maximizing_player, int machine_score)
{
	//ha a time_limit implicit erteke le van cserelve a kapcsolo altal megadott ertekre
	if(time_limit != -1)
	{
		//lekerjuk az idot
		end = clock();

		//kiszamoljuk az idot
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

		//ha meghaladja a time_limitet
		if(cpu_time_used > time_limit)
		{	
			//ha maximalizalo jatekos van soron
			if(maximizing_player)
			{	
				//visszateritunke egy olyan erteket ami nem befolyasolja a kimenetet a programnak
				return -1000;
			}
			//ha a minimalizalo jatekos van soron
			else
			{
				return 1000;
			}
		}
	}

	//osszesen ennyi lehetseges allapot letezik a jelenlegibol kiindulva
	int number_of_possible_states = 2 * count_empty_cells();

	//ha elertuk a leveleket vagy a kivant melyseget
	if ((number_of_possible_states == 0) || (depth == 0))
	{
		//print_possible_state(table_current_state);
		int temp_score = heuristics(table_current_state) + machine_score;
		//printf("\nheuristics of current state: %i  +  machine_score : %i = score : %i\n", heuristics(table_current_state), machine_score, temp_score);
		return temp_score;
	}
	
	//elerheto allapotok kigeneralasa
	state array_of_states[number_of_possible_states];
	generate_reachable_states(array_of_states);

	if(maximizing_player)
	{
		int max_eval = -1000;
		for(int i = 0; i < number_of_possible_states; ++i)
		{
			int eval = 0;
			if(depth != global_depth)
			{
				eval = traverser(array_of_states[i].table, depth - 1, alpha, beta, 0, heuristics(table_current_state) + machine_score);
			}
			else
			{
				eval = traverser(array_of_states[i].table, depth - 1, alpha, beta, 0, /*heuristics(table_current_state) +*/ machine_score);
			}
			
			

			//ha innen fog visszaterni a fuggvenyhivashoz akkor azt az allapotot aminek a melysegi kiertekelesebbol kaptam a legkisebb eredmenyt
			//eltarolom hogy a fuggvenyhivas utan tudjunk abba az allapotba lepni
			if(depth == global_depth)
			{
				if(eval > max_eval)
				{
					for(int k = 0; k < N; ++k)
					{
						for(int l = 0; l < N; ++l)
						{	
							global_next_state[k][l] = array_of_states[i].table[k][l];
						} 
						printf("\n");
					} 
				}

				max_eval = max(max_eval, eval);
				alpha = max(alpha, eval);

				if(beta <= alpha)
				{
					break;
				}
			}
			else
			{
				max_eval = max(max_eval, eval);

				alpha = max(alpha, eval);

				if(beta <= alpha)
				{
					break;
				}
			}
		}
		return max_eval;
	}
	else
	{
		int min_eval = 1000;
		for(int i = 0; i < number_of_possible_states; ++i)
		{
			int eval = 0;
			if(depth != global_depth)
			{
				eval = traverser(array_of_states[i].table, depth - 1, alpha, beta, 1, heuristics(table_current_state) + machine_score);
			}
			else
			{
				eval = traverser(array_of_states[i].table, depth - 1, alpha, beta, 1, /*heuristics(table_current_state) +*/ machine_score);
			}


			
			min_eval = min(min_eval, eval);		
			beta = min(beta, eval);

			if(beta <= alpha)
			{
				break;
			}	
		}	
		return min_eval;
	}
	//kiertekeljuk a heurisztika alapjan az allapotokat
	//evaluate_states(array_of_states, number_of_possible_states);
}



void generate_reachable_states(state array_of_states[])
{
	int state_index = 0;
	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			if (table[i][j] == ' ')
			{
				//lehteseges eset s behelyezesevel
				state new_state_s;
				
				//lehteseges eset o behelyezesevel
				state new_state_o;
				
				//atmasoljuk a jelenlei allast
				copy_table(new_state_s.table);
				copy_table(new_state_o.table);
				
				//beteszunk egy s-et
				new_state_s.table[i][j] = 's';
				
				//betesszuk a lehetseges eseteket tarolo tombbe
				array_of_states[state_index] = new_state_s;
				
				//noveljuk az indexet
				state_index++;
				
				//most o-ra csinaljuk meg
				new_state_o.table[i][j] = 'o';
				//print_possible_state(new_state_o.table);
				array_of_states[state_index] = new_state_o;
				state_index++;
			}
		}
	}
}



//table-t masolja a megadott matrixra
void copy_table(unsigned char matrix [N][N])
{
	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			matrix[i][j] = table[i][j];
		}
	}
}



//megszamolja hany kitoltetlen cella van a tablaba
int count_empty_cells()
{
	int number_of_empty_cells = 0;
	for(int i = 0; i < N; ++i)
	{		
		for(int j = 0; j < N; ++j)
		{
			number_of_empty_cells += (table[i][j] == ' ');
		}
	}
	return number_of_empty_cells;
}



//kiiratja a tablat jol atlathato modon
void print_state()
{
	//az oszlopok szamozasainak a kiiratasa
	printf("\n\t");
	for(int i = 0; i < N; ++i)
	{
		printf("%2i  |   ",i);
	}
	printf("\n");


	//horizontalis vonal a szamsor ala
	for(int i = 0; i < N; ++i)
	{
		printf("_________");
	}
	printf("\n");
	
	
	//vertikalis vonalak a szamsor ala
	for(int j = 0; j < N+1; ++j)
	{
		printf("    |   ");
	}
	printf("\n");
	
	
	//a tabla ertekeinek a kiiratasa
	for(int i = 0; i < N; ++i)
	{
		//sorszam szamozasa a sorok elejen
		printf("%2i  |   ",i);
		
		//a tenyleges tarolt ertek kiiratasa a vertikalis vonalakkal egyutt
		for(int j = 0; j < N; ++j)
		{
			printf("%c   |   ",table[i][j]);
		}
		printf("\n");
		
		//horizontalis vonalak kiiratasa
		for(int i = 0; i < N; ++i)
		{
			printf("_________");
		}
		printf("\n");
		
		///hogy ne tegyen plusz vonalakat a vegere
		if(i < N-1)
		{
			for(int j = 0; j < N+1; ++j)
			{
				printf("    |   ");
			}
		}
		printf("\n");
		
	}
	printf("\n");
}



//kiiratja egy allapot tablat jol atlathato modon
void print_possible_state(unsigned char state[N][N])
{
	//az oszlopok szamozasainak a kiiratasa
	printf("\n\t");
	for(int i = 0; i < N; ++i)
	{
		printf("%2i  |   ",i);
	}
	printf("\n");


	//horizontalis vonal a szamsor ala
	for(int i = 0; i < N; ++i)
	{
		printf("_________");
	}
	printf("\n");
	
	
	//vertikalis vonalak a szamsor ala
	for(int j = 0; j < N+1; ++j)
	{
		printf("    |   ");
	}
	printf("\n");
	
	
	//a tabla ertekeinek a kiiratasa
	for(int i = 0; i < N; ++i)
	{
		//sorszam szamozasa a sorok elejen
		printf("%2i  |   ",i);
		
		//a tenyleges tarolt ertek kiiratasa a vertikalis vonalakkal egyutt
		for(int j = 0; j < N; ++j)
		{
			printf("%c   |   ",state[i][j]);
		}
		printf("\n");
		
		//horizontalis vonalak kiiratasa
		for(int i = 0; i < N; ++i)
		{
			printf("_________");
		}
		printf("\n");
		
		///hogy ne tegyen plusz vonalakat a vegere
		if(i < N-1)
		{
			for(int j = 0; j < N+1; ++j)
			{
				printf("    |   ");
			}
		}
		printf("\n");
		
	}
	printf("\n");
}


//a fuggveny meghivasa egy kort jelent a jatekban
//vagy a gepre van meghivva vagy az emberre
int game(unsigned char table[N][N], int id)
{
	if(id)
	{
		printf("The machine's turn : \n");
	}
	else
	{
		printf("Your turn : \n");
	}
	
	
	//ha a jatekos lep 
	//bekerjuk a poziciot es a karaktert
	if(id == 0)
	{	
		print_state();
		
		
		//kiiratjuk az allast
		printf("Machine : %i \n Human : %i\n\n", machine_points, human_points);
		
		unsigned char * chr = 0;
		int column = 0;
		int row = 0;
		
		while(1)
		{
			printf("Row # : ");
			scanf("%i",&row);
			
			printf("Column # : ");
			scanf("%i",&column);
	
			printf("Character : ");		
			scanf("%s", &chr);
			
			if(row > N || row < 0)
			{
				printf("\nIncorrect row number! Please try it again!\n");
				continue;
			}
			
			if(column > N || column < 0)
			{
				printf("\nIncorrect column number! Please try it again!\n");
				continue;
			}
			
			if(chr != 's' && chr != 'o')
			{
				printf("\nIncorrect character! Please try it again!\n");
				continue;
			}
			
			if(table[row][column] != ' ' )
			{
				printf("\nPosition already occupied! Please try it again!\n");
				continue;
			}
			
			break;
		}
		
		
		print_arrow();
		
		
		table[row][column] = chr;
		
	}
	else 
	//ha a gep lep 
	//ha tudjuk akkor olyan helyre rakunk hogy egybol kihozzuk az sos-t
	//ha nem lehet akkor TODO
	if(id == 1)
	{
		int row = 0;
		int column = 0;
		int character = 0;

	
		//kiiratjuk az allast
		print_state();
	
		print_arrow();
		
		//table tomb atmasolasa
		unsigned char my_table[N][N] = {' '};

		for(int i = 0; i < N; ++i)
		{
			for(int j = 0; j < N; ++j)
			{
				my_table[i][j] = table[i][j];
			} 
		} 

		//elinditjuk az orat az ido meresehez
		start = clock();

		//minimax algoritmus alpha beta nyesessel
		int temp_score = traverser(my_table, global_depth, -1000, 1000, 1, machine_points);

		//itt mar be van teve a legjobb allapot a globalis tombe
		for(int i = 0; i < N; ++i)
		{
			for(int j = 0; j < N; ++j)
			{
				table[i][j] = global_next_state[i][j];
			} 
		} 
	}
	
	
	//itt megnezzuk, hogy valakinek sikerult-e az sos-t kirakni
	int search_result = search_sos_table();

	 if(search_result)
	 {
	 	printf("%s scored a point!\n",id ? "Machine" : "You");
	 	return 1;
	 }

	
	//az alap eset, ha nincs vege a jateknak
	return 0;
}




int main(int argc, char * argv[])
{
	//kapcsolok beolvasasa, indo kiirasa
	int i = 0;
	int switch_detpth = 0;
	int switch_time = 0;

	while (i< argc)
	{
		if( (strcmp(argv[i],"-d")==0) && global_depth == 1)
		{
			++i;
			global_depth = atoi(argv[i]);
			switch_detpth = 1;
			printf("Value of depth: %i\n",global_depth);
		}
		else if ((strcmp(argv[i],"-t")==0) && time_limit == -1)
		{
			++i;
			time_limit = atoi(argv[i]);
			switch_time = 1;
			printf("Value of time limit: %i\n",time_limit);
		}
		++i;
	}

	if (!switch_detpth)
	{
		printf("Implicit value of depth: %i\n",global_depth);
	}
	if (!switch_time)
	{
		printf("There is no time limit!\n");
	}
	

	//tabla es a regi tabla es tabla lehetosegek inicializalasa
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j < N; ++j)
		{
			table[i][j] = ' ';
			table_old[i][j] = ' ';
			global_next_state[i][j] = ' ';			
		}
	}

	
	//megkerdezzuk, hogy ki kezdjen
	printf("Who is starting?\n");
	printf("(m/h) : ");
	
	//beolvassuk a kezdo szemelyet
	char starter = 0;
	gets(&starter);
	
	int identifier = 0;
	
	// ha az ember kezd
	if(starter == 'h')
	{
		identifier = 0;
	}
	else
	//ha a gep kezd
	if(starter == 'm')
	{
		identifier = 1;
	}
	else
	{
		//ha mas karaktert ad meg mint az elvartak
		printf("bad input!!!\n");
		return -1;	
	}
	
	
	//amig meg van hely a tablan
	while(!end_of_game())
	{
		printf("\n\n\n\n\n\n\n NEW ROUND : ");


		//meghivjuk a jatekot az adott jatekosra aki sorra kovetkezik
		int round_result = game(table, identifier);
		
		//ha az ebben a korben lepo jatekosnak sikerult kihozni sos-t
		//akkor noveljuk a pontjat es megint o kovetkezik
		if(round_result)
		{
			//megnezzuk ki volt a gyoztes
			if(identifier)
			{
				//novelkuk a pontszamat
				machine_points++;
				
				//ha meg nem lett vege a jateknak
				//vagyis nem telt be a tabla

				//folytatjuk ezzek az identifierrel a jatekot
				continue;
			}
			else
			{
				//noveljuk a pontszamot
				human_points++;
				
				//ha meg nincs vege a jateknak
				//vagyis nem telt be a tabla

				//folytatjuk ezzek az identifierrel a jatekot
				continue;
			}
		}
		
		//ha nem hozott ki sos-t akkor a masik jatekos kovetkezik
		//printf("\nidentifier valtozik\n");
		identifier = (identifier + 1) % 2;	
	}
	
	//kiiratjuk a vegleges allast a jatek vegen
	print_state();
	
	//kiiratjuk a pontokat
	printf("The points : \n Machine : %i \n Human : %i\n", machine_points, human_points);
	
	//az eredmenyek kiirasa
	if(machine_points > human_points)
	{
		printf("\nThe machine won!\n");
	}
	else
	if(human_points > machine_points)
	{
		printf("\nYou won!\n");
	}
	else
	if(machine_points == human_points)
	{
		printf("\nIt's a draw!\n");
	}
	

	return 0;
}


//s-ek felterkepezese, random kereses
//ha egy lepes soran tobb sos is bekerul
