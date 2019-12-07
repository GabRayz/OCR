#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hunspell/hunspell.h>
#include "linkedlist.h"



char *spellcheck_word(Hunhandle *h, char *w)
{
    // Check if the word w is wrongly spelled
    if(!Hunspell_spell(h, w))
    {
        // Hunspell suggest severals correct words
        char ***s = malloc(sizeof(char));
        Hunspell_suggest(h, s, w);
        
        // Return the first suggested value
        if (*s != NULL)
            return s[0][0]; 
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
        if(isSpecial(*word_array))
        {
            *c = *word_array;
            *(c+1) = '\0';
            strcat(res,c);
        }
        
        w = spellcheck_word(h, word_array);
        strcat(res,w);

        if(isSpecial(*(word_array+strlen(word_array)-1)) 
            & (*(w+strlen(w)-1) != '.'))
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

