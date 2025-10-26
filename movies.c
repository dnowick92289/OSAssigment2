#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS and EXIT_FAILURE
#define MAX_LANGS 5
#define MAX_LANG_LEN 20

struct movie {
    char *title;
    int year;
    char *langugages[MAX_LANGS];
    int lang_count;
    double rating;
    struct movie *next;
};

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
    printf("Enter a choice from 1 to 4");
}
void processMovieFile(char* filePath){
char *currLine = NULL;
size_t len = 0;
ssize_t nread;
int movieCount = 0;

// Open the specified file for reading only
FILE *movieFile = fopen(filePath, "r");
if (!movieFile) {
    perror("fopen");
    return;
}

// Read the file and discard header line
nread = getline(&currLine, &len, movieFile);
if (nread <= 0) {
    free(currLine);
    fclose(movieFile);
    fprintf(stderr, "Empty or unreadable file\n");
    return;
}

while((nread =getline(&currLine, &len, movieFile)) != -1){
    movieCount++;
}

// Free the memory allocated by getline for currLine
free(currLine);
// Close the file
fclose(movieFile);

printf("\nProcessed file %s and parsed data for %d movies \n\n", filePath, movieCount);

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
        printf("\n");
    } else if (choice == 2) {
        printf("\n");
    } else if (choice == 3){
        printf("Enter the language for which you want to see movies: ");
        char lang[64]; scanf("%63s", lang);
        printf("\n");
    }
}
    

}
/**
*
*/
int main ( int argc, char **argv ){
if (argc < 2)
{
printf("You must provide the name of the file to process\n");
printf("Example usage: ./movies movies.csv\n");
return EXIT_FAILURE;
}
processMovieFile(argv[1]);
return EXIT_SUCCESS;
}