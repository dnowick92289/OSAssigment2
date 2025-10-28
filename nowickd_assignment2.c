#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>

/* 5 languages per movie and max length of each language*/
#define MAX_LANGS 5
#define MAX_LANG_LEN 20

/*singly linked list of movies node*/
struct movie {
    char *title;
    int year;
    char *languages[MAX_LANGS];
    int lang_count;
    double rating;
    struct movie *next;
};

/*parsing helpers*/

/*allocate and anf copy C string*/
static char *dupstr(const char *s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

/*parse langugages column from CSV*/
static void parseLanguages(const char *src, char *out[], int *count) {
    *count = 0;
    size_t n =strlen(src);
    if (n < 2 || src[0] != '[' || src[n - 1] != ']')return;

    char *buf = dupstr(src + 1);
    buf[n - 2] = '\0';

    char *save = NULL;
    char *tok = strtok_r(buf, ";", &save);
    while (tok && *count < MAX_LANGS) {
        while (isspace((unsigned char)*tok)) tok++;
        char *end = tok + strlen(tok) - 1;
        while (end >= tok && isspace((unsigned char)*end)) *end-- = '\0';

        
        size_t L = strlen(tok);
        if (L > MAX_LANG_LEN) L = MAX_LANG_LEN;

        char *p = malloc(L + 1);
        if (!p) { perror("malloc"); exit(1); }
        strncpy(p, tok, L);
        p[L] = '\0';

        out[(*count)++] = p;
        tok = strtok_r(NULL, ";", &save);
    }
    free(buf);
}
/*parse one csv row into newly allocated struct movie*/
static struct movie *parseLineToMovie(const char *lineIn) {
    char *line = dupstr(lineIn); 
    char *save = NULL;

    char *title = strtok_r(line, ",", &save);
    char *year  = strtok_r(NULL, ",", &save);
    char *langs = strtok_r(NULL, ",", &save);
    char *rate  = strtok_r(NULL, ",", &save);

    if (!title || !year || !langs || !rate) { free(line); return NULL; }

    struct movie *m = calloc(1, sizeof(*m));
    if (!m) { perror("calloc"); exit(1); }

    m->title = dupstr(title);
    m->year = atoi(year);
    parseLanguages(langs, m->languages, &m->lang_count);
    m->rating = strtod(rate, NULL);
  

    free(line);
    return m;
}

/*free single movie node and all its heap allocated strings*/
static void freeMovie(struct movie *m) {
    if (!m) return;
    free(m->title);
    for (int i = 0; i < m->lang_count; i++) free(m->languages[i]);
    free(m);
}

/*walk down list and free each node*/
static void freeMovieList(struct movie *head) {
    while (head) {
        struct movie *next = head->next;
        freeMovie(head);
        head = next;
    }
}

/*UI helpers*/

/*print exact menu text*/
static void printMenu(void) {
    printf("1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of releae of all movies in a specific language\n");
    printf("4. Exit from the program\n\n");
    printf("Enter a choice from 1 to 4: ");
}

/*option 1-print all titles by year*/
static void showByYear(struct movie *head, int year) {
    int found = 0;
    for (struct movie *m = head; m; m = m->next) {
        if (m->year == year) {
            printf("%s\n", m->title);
            found = 1;
        }
    }
    if (!found) {
        printf("No data about movies released in the year %d\n", year);
    }
    printf("\n"); 
}

/*option 2 print highest rated movie for each year*/
static void showHighestPerYear(struct movie *head) {
    const int YMIN = 1900, YMAX = 2021;
    const int RANGE = (YMAX - YMIN + 1);

    double bestRating[RANGE];
    struct movie *bestMovie[RANGE];


    for (int i = 0; i < RANGE; i++) {
        bestRating[i] = -1.0;   
        bestMovie[i] = NULL;
    }


    for (struct movie *m = head; m; m = m->next) {
        if (m->year < YMIN || m->year > YMAX) continue; 
        int idx = m->year - YMIN;
        if (m->rating > bestRating[idx]) {
            bestRating[idx] = m->rating;
            bestMovie[idx] = m;
        }
    }

  
    for (int i = 0; i < RANGE; i++) {
        if (bestMovie[i]) {
            int year = YMIN + i;
            printf("%d %.1f %s\n", year, bestRating[i], bestMovie[i]->title);
        }
    }
    printf("\n");
}

/*option 3 - language searcg*/
static void showByLanguage(struct movie *head, const char *lang) {
    int found = 0;
    for (struct movie *m = head; m; m = m->next) {
        for (int i = 0; i < m->lang_count; i++) {
            if (strcmp(m->languages[i], lang) == 0) {
                printf("%d %s\n", m->year, m->title);
                found = 1;
                break; 
            }
        }
    }
    if (!found) {
        printf("No data about movies released in %s\n", lang);
    }
    printf("\n"); 
}

/********main code**********/


/*reads csv, builds linked list of struct movie, prints processed file line
enters interactive menu in inmemory list
frees list at end*/
void processMovieFile(char* filePath){
    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;



    FILE *movieFile = fopen(filePath, "r");
    if (!movieFile) {
        perror("fopen");
        return;
    }


    nread = getline(&currLine, &len, movieFile);
   
    if (nread <= 0) {
        free(currLine);
        fclose(movieFile);
        fprintf(stderr, "Empty or unreadable file\n");
        return;
    }

   
    struct movie *head = NULL;
    struct movie *tail = NULL;
    int movieCount = 0;

    while ((nread = getline(&currLine, &len, movieFile)) != -1) {
        struct movie *m = parseLineToMovie(currLine);
        if (!m) continue;  

     
        if (!head) {
            head = tail = m;
        } else {
            tail->next = m;
            tail = m;
        }
        movieCount++;
    }

    free(currLine);
    fclose(movieFile);

    printf("Processed file %s and parsed data for %d movies\n\n", filePath, movieCount);

/*interactive menu*/
    int choice;
    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) return;

        if (choice == 4) break;
        if (choice < 1 || choice > 4) {
            printf("You've entered an incorrect choice, try again \n\n");
            continue;

        }
        if (choice == 1){
            printf("Enter the year you want to see movies: ");
            int year; scanf("%d", &year);
            showByYear(head, year);

        } else if (choice == 2) {
            showHighestPerYear(head);

        } else if (choice == 3) {
            printf("Enter the language for which you want to see movies: ");
            char lang[64];                
            scanf("%63s", lang);           
            showByLanguage(head, lang);
        }
    }
    
    freeMovieList(head);
}

/*program entry point*/
int main ( int argc, char **argv ){
if (argc < 2){
    printf("You must provide the name of the file to process\n");
    printf("Example usage: ./movies movies.csv\n");
    return EXIT_FAILURE;
}
processMovieFile(argv[1]);
return EXIT_SUCCESS;
}