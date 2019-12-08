#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hunspell/hunspell.h>
#include "linkedlist.h"

int hasWhiteSpace(char *s)
{
    size_t length = strlen(s);
    for (size_t i = 0; i < length; i++)
    {
        if(*(s+i) == ' ')
            return 1;
    }
    return 0;
    
}

char *pickWord(char ***s)
{
    if (*s != NULL)
    {
        size_t length = sizeof(s) / sizeof(s[0]);
        for (size_t i = 0; i < length; i++)
        {
            if(!hasWhiteSpace(s[i][0]))
                return s[i][0];
        }
    }

    return NULL;
}

char *spellcheck_word(Hunhandle *h, char *w)
{
    // Check if the word w is wrongly spelled
    if(!Hunspell_spell(h, w))
    {
        // Hunspell suggest severals correct words
        char ***s = malloc(sizeof(char));
        Hunspell_suggest(h, s, w);
        
        // Return the first suggested value
        // if (*s != NULL)
        //     return s[0][0]; 
        char *res = pickWord(s);
        if(res != NULL)
            return res;
    }

    return w;
}

int isSpecial(char c)
{
    char special[24] = " .,;?/!:&\"\'{([-`_)]}=+*";
    for (size_t i = 0; i < 24; i++)
    {
        if(c == special[i])
            return 1;
    }
    return 0;
    
}

void I2l(char *s)
{
    size_t length = strlen(s);
    for (size_t i = 0; i < length; i++)
    {
        if(*(s+i) == 'I')
            *(s+i) = 'l';
        else if (*(s+i) == 'C')
            *(s+i) = 'c';
        else if (*(s+i) == 'O')
            *(s+i) = 'o';
    }
}

char *spellcheck(char *s)
{
    char *res = malloc(sizeof(char) * (strlen(s)+20));

    Hunhandle *h = Hunspell_create(
		"./Dictionnary/en_US.aff",
		"./Dictionnary/en_US.dic"
	);

    char *word_array = strtok(s," ");
    size_t n = strlen(word_array);
    char *w = NULL;
    char *c = malloc(sizeof(char)*2);

    while (word_array != NULL) 
    {
        I2l(word_array);
        if(isSpecial(*word_array))
        {
            *c = *word_array;
            *(c+1) = '\0';
            strcat(res,c);
        }
        
        w = spellcheck_word(h, word_array);
        strcat(res,w);

        if(isSpecial(*(word_array+strlen(word_array)-1)) 
            & (*(w+strlen(w)-1) != '.') 
            & (*(w+strlen(w)-1) != ','))
        {
            *c = *(word_array+strlen(word_array)-1);
            *(c+1) = '\0';
            strcat(res,c);
        }

        strcat(res," ");
        
        // printf("original : %s\n",word_array);
        // printf("suggested : %s\n\n",w);

        word_array = strtok(NULL, " ");
    }

    Hunspell_destroy(h);
    free(s);
    return res+n;
}

