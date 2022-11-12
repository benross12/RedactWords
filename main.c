#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

char toUpper(char c) {
  if (c >= 'a' && c <= 'z') return c - 32;
  return c;
}

// check if a character is alphabetical
int isAlpha(char c) {
  if (c <= 'Z' && c >= 'A') return 1;
  if (c <= 'z' && c >= 'a') return 1;
  return 0;
}

char *readFileContents(const char *fn) {
  // setup variables
  FILE *inp = fopen(fn, "r");
  char currentCharacter;
  char *output = malloc(0);
  int count = 0;

  // read from the file
  do {
    // read the current character
    currentCharacter = fgetc(inp);
    
    // exit if end of file is reached
    if (currentCharacter == EOF) break;
    
    // increase the size of output string by 1
		output = realloc(output, (count + 1) * sizeof(char));    
    
    // add the character to output
    output[count++] = currentCharacter;
  } while(1);  

  // remove the final \n from the file  
  output[count - 1] = '\0';
  
  // close the file and return
  fclose(inp);
  return output;
}

// get the number of words in a string
int numberOfWordsF(char words[]) {
  int numberOfWords = 0, currentIndex = 0;
  char current = words[numberOfWords];
  while (current != '\0') {
    if (current == ',') numberOfWords++;
    
    currentIndex++;
    current = words[currentIndex];
  }
  return numberOfWords + 1;
}

void redact_words(const char *text_filename, const char *redact_words_filename) {
  // read both strings from the file
  char *text = readFileContents(text_filename);
  char *words = readFileContents(redact_words_filename);
  printf("Read from text - %s\n", text);
  printf("Read from words - %s\n", words);

  // find the number of words in words
  int numberOfWords = numberOfWordsF(words);

  // capitalise the redact words, place them on the heap and store an array of pointers
  char *wordsArray[numberOfWords];
  int index = 0;
  char delimiters[] = ", ";
  char *ptr = strtok(words, delimiters);
  
  while (ptr != NULL) {    
    // get the word as upper case
    int length = strlen(ptr);
    char *upperWord = malloc(length + 1);
    for (int i = 0; i < length; i++) upperWord[i] = toUpper(ptr[i]);
    upperWord[length] = '\0';  

    // store a pointer to the heap
    wordsArray[index++] = upperWord;

    // move onto the next word
    ptr = strtok(NULL, delimiters);
  }

  // print out the words to redact
  printf("Words array contents - ");
  for (int i = 0; i < numberOfWords; i++) {
    printf("%s  ", wordsArray[i]);
  }
  printf("\n");
  
  // loop through the text and replace the words
  char *textPointer = strtok(text, " ");
  FILE *outputFile = fopen("result.txt", "w");
  
  printf("Output Sentence - ");
  while (textPointer != NULL) {
    // get a copy of the uppercase and original word
    int length = strlen(textPointer);
    char *originalWord = textPointer;
    originalWord[length] = '\0';

    // get the number of letters in originalWord
    int letters = 0;
    for (int i = 0; i < length; i++) if (isAlpha(originalWord[i])) letters++;
    char formattedWord[letters + 1];
    formattedWord[letters] = '\0';
    int wordIndex = 0;
    char removedPunctuation = '|';
    for (int i = 0; i < length; i++) {
      if (isAlpha(originalWord[i])) {
        formattedWord[wordIndex++] = toUpper(originalWord[i]);
      }
      else removedPunctuation = originalWord[i];
    }

    // check if the current word should be redacted
    int redacted = 0;
    for (int i = 0; i < numberOfWords; i++) {
      if (strcmp(formattedWord, wordsArray[i]) == 0) {
        // mark the current word as redacted
        redacted = 1;

        // write the correct number of *'s to the file
        for (int j = 0; j < letters; j++) {
          fprintf(outputFile, "*");
        }        

        // if any punctuation was removed from the end of a word, write it in
        if (removedPunctuation != '|') {
          fprintf(outputFile, "%c", removedPunctuation);
        }
        
        fprintf(outputFile, " ");        
      }

      // don't check the rest of the words if the current one is already redacted
      if (redacted) break;
    }
    
    // if not redacted, write the word to the file
    if (!redacted) {
      fprintf(outputFile, "%s ", originalWord);
    }

    // move onto the next word
    textPointer = strtok(NULL, delimiters);
  }

  //add a final clear line to the file and close it
  fprintf(outputFile, "\n");
  fclose(outputFile);
  
  // free the memory on the heap
  free(text);
  free(words);
  text = NULL;
  words = NULL;
  for (int i = 0; i < numberOfWords; i++) {
    free(wordsArray[i]);
    wordsArray[i] = NULL;
  }
}

int main(void) {
  redact_words("textFile.txt", "readactedWordsFile.txt");
  return 0;
}
