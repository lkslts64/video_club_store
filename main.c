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
        exit(-1);
    }
    int lines = 0;
    char c;
    //Extract characters from file and store in character c 
    c = getc(fp);
    while (c != EOF) {
        if (c == '\n') {
            lines++;
        }
        c = getc(fp);
    }
    fclose(fp);
    return lines;
}

void get_line(char *line,FILE *file) {
    if (fgets(line, sizeof(line), file) == NULL) {
        printf("Error? line=%s\n",line);
    } else {
        line[strlen(line) - 1] = 0; //trim new line character.
    } 
}

movie new_movie(void) {
    movie *movie = malloc(sizeof(movie));
    movie->title = malloc(256 * sizeof(char));
    movie->director = malloc(sizeof(director_info));
    movie->director->director_surname = malloc(256 * sizeof(char));
    movie->director->director_name = malloc(256 * sizeof(char));
    movie->release_date = malloc(sizeof(date));
    return *movie;
}

void print_movie(movie m) {
    printf("id = %d,title = %s,director_surname = %s,director_name = %s,day = %d,month = %d, year = %d\n",m.id,m.title,m.director->director_surname,m.director->director_name,m.release_date->day,m.release_date->month,m.release_date->year);
}

movie *load_file(char *filename,int *sz) {
    char line[256];
    int movies_num = count_file_lines(filename) / 6;
    FILE* file = fopen(filename, "r"); 
    if (file == NULL) {
        printf("can not open file. Exiting\n");
        exit(-1);
    }
    movie *movies = malloc(movies_num * sizeof(movie));
    for (int i = 0; i < movies_num; i++) {
        movies[i] = new_movie();
    }
    int c = 0;
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
    return movies;
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
            m = new_movie();
            if (num_movies == 0) {
                m.id = 1;
            } else {
                m.id = movies[num_movies-1].id + 1;
            }
            num_movies++;
            if (num_movies >= sz) {
                sz = sz * 2;
                movies = realloc(movies,sz * sizeof(movie));
            }
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
            break;
        //delete
        case 2:
            printf("Type a movie id\n");
            int id;
            char found = 0;
            scanf("%d",&id);
            for (int i = 0; i < num_movies; i++) {
                if (id == movies[i].id) {
                    //delete the movie. Just override it with the last one
                    movies[i] = movies[num_movies - 1];
                    num_movies--;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("Sorry couldn't fine the specified movie\n");
            }
            break;
        case 3: //search based on movie title.
            printf("Type a movie title\n");
            char title[256];
            char found2 = 0;
            scanf("%s",title);
            for (int i = 0; i < num_movies; i++) {
                if (!strcmp(title,movies[i].title)) {
                    print_movie(movies[i]);
                    found2 = 1;
                    break;
                }
            }
            if (!found2) {
                printf("Sorry couldn't fine the specified movie\n");
            }
            break;
        case 4:
            break;
        case 5:
            exit(0);
            break;
        case 6: //for debugging only
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
    for (int i = 0; i < movies_sz; i++) {
        print_movie(movies[i]);
    }
    menu(movies,movies_sz);
}
