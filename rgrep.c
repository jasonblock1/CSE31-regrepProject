/*Jason Block
Collaborated: Alex la Torre & David Wang
Laboratory Section: Friday 4:30*/
#include <stdio.h>
#define MAXSIZE 4096
int index;
int c = sizeof(char);																//	global variable for length in bytes of char
int question(char *pattern);
int escape(char *pattern);
int plus(char *pattern);
int bool = 0;

char next(char *arr) {
    return *(arr + c);
}

char prev(char *arr) {
    return *(arr - c);
}

int identifier(char pattern) {														//	switch statement to check wildcard character
	switch(pattern) {
		default:
			return 0;
		case '.':
			return 1;
		case '+':
			return 2;
		case '\?':
			return 3;
		case '\\':
			return 4;
		
	}
}

/**
 * You can use this recommended helper function 
 * Returns true if partial_line matches pattern, starting from
 * the first char of partial_line.
 */
int matches_leading(char *partial_line, char *pattern) {
  if(*partial_line == *pattern && !identifier(*pattern)) {						//	check normal match
  	return 1;
  }
  if(*pattern == '.' && !escape(pattern)) {										//	 check if '.' wildcard
  	return 1;
  } 
  
  if(*partial_line == *pattern && escape(pattern) && identifier(*pattern)) {// check normal match if pattern character is a wildcard and was escaped
  	return 1;
  }
  if(*pattern == '\\') {														//check for escape character 
  		if(*partial_line != 0) 
			return matches_leading(partial_line, pattern + c);
		return 0;
  }
  if(question(pattern)) {														//check question wildcard
  	return 1;
  }
  return 0;
}

int plus(char* pattern) {														
	if(next(pattern) == '+') {
		return 1;
	}
	return 0;
}

int question(char *pattern) {
	if(next(pattern) == '\?') {
		return 1;
	}
	return 0;
}

int escape(char* pattern) {
	if(prev(pattern) == '\\') {
		return 1;
	}
	return 0;
}


/**
 * You may assume that all strings are properly null terminated 
 * and will not overrun the buffer set by MAXSIZE 
 *
 * Implementation of the rgrep matcher function
 */
int rgrep_matches(char *line, char *pattern) { 
	static int index = 0;									//	keeps track of pattern depth
	if(*pattern == '\0') {							//	checks if the pattern is empty						
			pattern -= index * c;					//	moves pattern pointer to the beginning of the pattern
		
		index = 0;
		return 1;
	}
	if(*line == '\0') {								//	if we reach the end of the line then line is not a match
		return 0;
	}
	if(*line == '\n') {								//	if we reach a new line, then reset pattern pointer and index to beginning of pattern
		pattern -= index * c;
		index = 0;
		return 0;
	}
	if((*pattern == '\\') && identifier(next(pattern))) {	//	checks for escape 	
		pattern += c;
		index++;
	}
	if(matches_leading(line,pattern)) {						//	check for match
		if(plus(pattern)){									//	check for plus wildcard
			int counter = 1;
            if(*pattern == *(pattern + 2 * c))
                counter = 2;
            while(*(line + counter * c) == *pattern && !(*pattern == '.' && !escape(pattern))) {
                line += c;
            }
            if(*(pattern ) == '.' && *(pattern + c) == '+') {
                
                while(*line != *(pattern + 2 * c)) {
                        line += c;
                 }

                 bool = 1; // a .+_ is present

                return rgrep_matches(line, pattern + 2 * c);
             }
           
			pattern += c;												//	skips wildcard character if not escaped
			index++;
			if(bool == 1) {
				line += c;
			}													//	''
		} 
		if(question(pattern)) {											//check for question mark wildcard
			 if(*line == *pattern && *(line + c) == *(pattern + 2 * c) && !(escape(pattern))) {			//wo?t == wot either there
                index+= 2;
                return rgrep_matches(line + c, pattern + 2 * c); 
            }else if (*line == *pattern && *(line + c) != *(pattern + 2 * c) && !(escape(pattern))) {	 //a?a == a or not there
                index+= 2;
                return rgrep_matches(line, pattern + 2 * c);
            }else if(*line != *pattern && *line == *(pattern + 2 * c) && !(escape(pattern))) {			//wo?t == wt
                index+= 2;
                return rgrep_matches(line, pattern + 2 * c);
            }else if (*(pattern + c) == '\\' && *(pattern + 2 * c) == '.') {								// presence of '\.'
                index += 2;
                return rgrep_matches(line, pattern + c); 							//since the \\ already increments by one, only increment by one
            }else if (*pattern == '.' && *(pattern + c) == '\?') {										//presence of a '.?', skip over 3
          		index += 3;
                return rgrep_matches(line, pattern + 3 * c);
            }else if (*(pattern + c) == '\\' && *(pattern + 2 * c) == '.' && *(pattern + 3 * c) == '\?') {	// presence of a '\.?'
                index += 3;
                return rgrep_matches(line, pattern + 3 * c);
            }else {
                return 0;
            }
		}
		
		pattern += c;										
		index++;
											
	}else if(*(pattern - index * c) != '\0' && index != 0) {					//resets pattern pointer and index if partial match 
		pattern -= index * c;
		index = 0;
	}
	
	if(*line != '\0') {
		line += c;										//	increment through line
	}
    return rgrep_matches(line,pattern);					//recursive function call
}
	

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PATTERN>\n", argv[0]);
        return 2;
    }

    /* we're not going to worry about long lines */
    char buf[MAXSIZE];

    while (!feof(stdin) && !ferror(stdin)) {
        if (!fgets(buf, sizeof(buf), stdin)) {
            break;
        }
        if (rgrep_matches(buf, argv[1])) {
            fputs(buf, stdout);
            fflush(stdout);
        }
    }

    if (ferror(stdin)) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
