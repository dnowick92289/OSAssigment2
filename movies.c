#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS and EXIT_FAILURE
#include <string.h>
#include <ctype.h>
#define MAX_LANGS 5
#define MAX_LANG_LEN 20

struct movie {
    char *title;
    int year;
    char *languages[MAX_LANGS];
    int lang_count;
    double rating;
    struct movie *next;
};

/*Helpers for parsing*/

static char *dupstr(const char *s) {
    char *p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

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

        // cap to MAX_LANG_LEN (assignment says max 20)
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
// CSV columns: Title,Year,Languages,Rating
static struct movie *parseLineToMovie(const char *lineIn) {
    char *line = dupstr(lineIn); // make writable for strtok_r
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
    // m->next already NULL from calloc

    free(line);
    return m;
}

static void freeMovie(struct movie *m) {
    if (!m) return;
    free(m->title);
    for (int i = 0; i < m->lang_count; i++) free(m->languages[i]);
    free(m);
}

static void freeMovieList(struct movie *head) {
    while (head) {
        struct movie *next = head->next;
        freeMovie(head);
        head = next;
    }
}
/*
* Function: processMovieFile
* Opens a file, reads and prints each line
* filePath: path to the file
*
* This function shows sample code that opens a file, then in a loop reads and
prints each line in that file.
* You can use this code however you want in your Prog Assignment 2.
*/

static void printMenu(void) {
    printf("1. Show movies released in the specified year\n");
    printf("2. Show highest rated movie for each year\n");
    printf("3. Show the title and year of releae of all movies in a specific language\n");
    printf("4. Exit from the program\n\n");
    printf("Enter a choice from 1 to 4: ");
}

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
    printf("\n"); // keep the blank line like the sample runs
}


void processMovieFile(char* filePath){
    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;


    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");
    if (!movieFile) {
        perror("fopen");
        return;
    }

    // Read the file and discard header line
    nread = getline(&currLine, &len, movieFile);
    // --- read & discard the header line ---
    if (nread <= 0) {
        free(currLine);
        fclose(movieFile);
        fprintf(stderr, "Empty or unreadable file\n");
        return;
    }

    // --- build linked list instead of just counting ---
    struct movie *head = NULL;
    struct movie *tail = NULL;
    int movieCount = 0;

    while ((nread = getline(&currLine, &len, movieFile)) != -1) {
        struct movie *m = parseLineToMovie(currLine);
        if (!m) continue;  // skip if malformed (safety)

        // append to list
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
            printf("\n");
        } else if (choice == 3){
            printf("Enter the language for which you want to see movies: ");
            char lang[64]; scanf("%63s", lang);
            printf("\n");
        }
    }
    
    freeMovieList(head);
}
/**
*
*/
int main ( int argc, char **argv ){
if (argc < 2){
    printf("You must provide the name of the file to process\n");
    printf("Example usage: ./movies movies.csv\n");
    return EXIT_FAILURE;
}
processMovieFile(argv[1]);
return EXIT_SUCCESS;
}