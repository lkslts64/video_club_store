#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct date
{
    int day,month,year;
}date;

typedef struct director_info
{
    char *director_surname;
    char *director_name;
}director_info;

typedef struct movie
{
    int id;
    char *title;
    director_info *director;
    date *release_date;
}movie;


int count_file_lines(char *filename) {
    FILE* fp = fopen(filename, "r"); 
    if (fp == NULL) {
        printf("Incorrect file. Please try again..\n");
        exit(-1);
    }
    int lines = 0;
    char c;
    //Extract characters from file and store in character c 
    c = getc(fp);
    while (c != EOF) {
        //if we meet a newline, increment 'lines'.
        if (c == '\n') {
            lines++;
        }
        c = getc(fp); //get the next character
    }
    fclose(fp);
    return lines;
}

void get_line(char *line,FILE *file) {
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("Error????line=%s\n",line);
    } else {
        line[strlen(line) - 1] = 0; //trim new line character.
    } 
}

//allocate memory for the fields of struct movie.
movie *new_movie(void) {
    movie *m = (malloc(1 * sizeof(movie)));
    m->title = malloc(256 * sizeof(char));
    m->director = malloc(sizeof(director_info));
    m->director->director_surname = malloc(256 * sizeof(char));
    m->director->director_name = malloc(256 * sizeof(char));
    m->release_date = malloc(sizeof(date));
    return m;
}

void print_movie(movie m) {
    printf("id = %d,title = %s,director_surname = %s,director_name = %s,day = %d,month = %d, year = %d\n",m.id,m.title,m.director->director_surname,m.director->director_name,m.release_date->day,m.release_date->month,m.release_date->year);
}

//Used to by qsort to compare two objects of type movie to determine which to sort first.
//The function signature needs to be the following (meaning the arguements should be of type void)
//so we have to cast them to movie later.
int compareSort(const void *m1,const void *m2) {



    movie *_m1 = (movie *)m1; //cast void to movie type.
    movie tmp1 = *_m1;

    movie *_m2 = (movie *)m2;
    movie tmp2 = *_m2;



    return strcmp(tmp1.director->director_surname,tmp2.director->director_surname);
}

//sort movies based on directors surname
//This function uses the C's standard library qsort function.
void sort(movie *movies,int num_movies) {
    qsort(movies,num_movies,sizeof(movie),compareSort);
}

int binarySearch(movie *movies, int start, int end, char *targetName) { 
    while (start <= end) { 
        int m = start + (end -start) / 2; 
        int tmp = strcmp(movies[m].director->director_surname,targetName);
        if (tmp == 0) { 
            return m; 
        }
  
        if (tmp < 0)  {
            start = m + 1; 
        }
        else {
            end = m - 1; 
        }
    } 
    return -1; 
} 


movie *load_file(char *filename,int *sz) {
    char line[256];
    //count the number of movies in the input file.
    int movies_num = count_file_lines(filename) / 7;
    FILE* file = fopen(filename, "r"); 
    if (file == NULL) {
        printf("can not open file. Exiting\n");
        exit(-1);
    }
    //allocate the movies array
    movie *movies = malloc(movies_num * sizeof(movie));
    //allocate memory for the fields of each movie.
    for (int i = 0; i < movies_num; i++) {
        movies[i] = *new_movie();
    }
    //fill the allocated memory with data from the file.
    for (int i = 0; i < movies_num; i++) {
        get_line(line, file);
        movies[i].id = atoi(line);
        get_line(movies[i].title,file);
        get_line(movies[i].director->director_surname,file);
        get_line(movies[i].director->director_name,file);
        get_line(line, file);
        movies[i].release_date->day = atoi(line);
        get_line(line, file);
        movies[i].release_date->month = atoi(line);
        get_line(line, file);
        movies[i].release_date->year = atoi(line);
        print_movie(movies[i]);
    }
    *sz = movies_num;
    fclose(file);
    sort(movies,movies_num);
    return movies;
}

//standard algorithm to find maximum value from an array
int find_max_id(movie *movies,int num_movies) {
    int max_id = -1;
    for (int i = 0; i < num_movies; i++) {
        if (movies[i].id >  max_id ) {
            max_id = movies[i].id;
        }
    }
    return max_id;
}


//update the file on exit.
void save_to_file_on_exit(movie *movies,int movies_num) {
    char buffer[100000]; //assume the movies data are less than 100Kb
    char temp[256];
    for (int i = 0; i < movies_num; i++ ) {
        sprintf(temp,"%d",movies[i].id); //convert an int to string
        strcat(buffer,strcat(temp,"\n")); //concatenate two strings
        strcat(buffer,strcat(movies[i].title,"\n"));
        strcat(buffer,strcat(movies[i].director->director_surname,"\n"));
        strcat(buffer,strcat(movies[i].director->director_name,"\n"));
        sprintf(temp,"%d",movies[i].release_date->day);
        strcat(buffer,strcat(temp,"\n"));
        sprintf(temp,"%d",movies[i].release_date->month);
        strcat(buffer,strcat(temp,"\n"));
        sprintf(temp,"%d",movies[i].release_date->year);
        strcat(buffer,strcat(temp,"\n"));
    }
    FILE *fp = fopen("movies.txt","w");
    fprintf(fp,"%s",buffer); //write new contents to file, overwriting the previous ones.
    fclose(fp);
}

void menu(movie *movies,int sz) {
    int choice;
    movie m;
    int num_movies = sz;
    while(1) {
        printf("1. Insert a new movie\n2. Delete a movie based on movie's id\n3. Search a movie based on movie's title\n4. Search a movie based on movie's director\n5. Exit\n");   
        scanf("%d",&choice);
        switch (choice)
        {
        //insert
        case 1: 
            m = *new_movie();
            //id = 1 if we dont have any movies 
            //else is max_id + 1
            if (num_movies == 0) {
                m.id = 1;
            } else {
                m.id = find_max_id(movies,num_movies) + 1;
            }
            num_movies++;
            //if we dont have any memory allocatd for the insertion, we should.
            if (num_movies >= sz) {
                //allocate twice as we have (we don't want to make a new allocation every time a new movie is inserted this is not performant)
                sz = sz * 2;
                movies = realloc(movies,sz * sizeof *movies);
            }
            //always insert at the end.
            movies[num_movies-1] = m;
            printf("Please type a movie title\n");
            scanf("%s",m.title);
            printf("Please type the director's surname\n");
            scanf(" %s",m.director->director_surname);
            printf("Please type the director's name\n");
            scanf("%s",m.director->director_name);
            printf("Please type the release day\n");
            scanf("%d",&m.release_date->day);
            printf("Please type the release month\n");
            scanf("%d",&m.release_date->month);
            printf("Please type the release year\n");
            scanf("%d",&m.release_date->year);
            //don't forget to sort after insertion
            sort(movies,num_movies);
            break;
        //delete
        case 2:
            printf("Type a movie id\n");
            int id;
            char found = 0;
            scanf("%d",&id);
            for (int i = 0; i < num_movies; i++) {
                if (id == movies[i].id) {
                    //delete the movie. Just override it with the last one (fill the gaps).
                    movies[i] = movies[num_movies - 1];
                    num_movies--;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Sorry couldn't find the specified movie\n");
            }
            //don't forget to sort after deletion
            sort(movies,num_movies);
            break;
        case 3: //search based on movie title.
            printf("Type a movie title\n");
            char title[256];
            char found2 = 0;
            scanf("%s",title);
            for (int i = 0; i < num_movies; i++) {
                if (strcmp(title,movies[i].title) == 0) {
                    print_movie(movies[i]);
                    found2 = 1;
                    break;
                }
            }
            if (!found2) {
                printf("Sorry couldn't fine the specified movie\n");
            }
            break;
        //search by director's surname
        case 4:
            printf("Type a director's surname\n");
            char name[256];
            scanf("%s",name);
            int res = binarySearch(movies,0,num_movies,name);
            if (res < 0) {
                printf("Sorry couldn't find the specified director's name\n");
            } else {
                print_movie(movies[res]);
            }
            break;
        case 5:
            save_to_file_on_exit(movies,num_movies);
            exit(0);
        case 6: //print the movies,for debugging only
            for (int i = 0; i < num_movies; i++) {
                print_movie(movies[i]);
            }
        default:
            break;
        }
    }
}


int main(void) {
    int movies_sz;
    movie *movies = load_file("movies.txt",&movies_sz);
    menu(movies,movies_sz);
}
