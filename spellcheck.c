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
        if (*(s + i) == ' ')
            return 1;
    }
    return 0;
}

char *pickWord(char ***s)
{
    if (*s != NULL)
    {
        int i = 0;
        while (s[i] != NULL)
        {
            if (!hasWhiteSpace(s[i][0]))
                return s[i][0];
            i++;
        }
    }
    return NULL;
}

char *spellcheck_word(Hunhandle *h, char *w)
{
    // Check if the word w is wrongly spelled
    if (!Hunspell_spell(h, w))
    {
        // Hunspell suggest severals correct words
        char ***s = malloc(sizeof(char));
        Hunspell_suggest(h, s, w);

        // Return the first suggested value
        if (*s != NULL)
            return s[0][0];
        // char *res = pickWord(s);
        // if (res != NULL)
        //     return res;
    }

    return w;
}

int isSpecial(char c)
{
    char special[24] = " .,;?/!:&\"\'{([-`_)]}=+*";
    for (size_t i = 0; i < 24; i++)
    {
        if (c == special[i])
            return 1;
    }
    return 0;
}

void I2l(char *s)
{
    size_t length = strlen(s);
    for (size_t i = 0; i < length; i++)
    {
        if (s[i] == 'I' && i > 0 && s[i - 1] != '\n')
            s[i] = 'l';
    }
}

char *spellcheck(char *s)
{
    size_t size = strlen(s) + 1;
    char *res = malloc(sizeof(char) * size);

    Hunhandle *h = Hunspell_create(
        "./Dictionnary/en_US.aff",
        "./Dictionnary/en_US.dic");

    char *word = strtok(s, " ");
    char *correctedWord = NULL;
    size_t i = 0;

    while (word != NULL)
    {
        I2l(word);

        correctedWord = spellcheck_word(h, word);

        size_t len = strlen(word);
        size_t cLen = strlen(correctedWord);

        // Reallocate the buffer according to the new word's length
        size += (cLen - len);
        res = realloc(res, size);

        size_t j = 0;
        while (j < cLen)
            res[i++] = correctedWord[j++];

        if (isSpecial(word[len - 1]) && correctedWord[cLen - 1] != '.' && correctedWord[cLen - 1] != ',')
        {
            res[i++] = word[len - 1];
        }

        res[i++] = ' ';

        word = strtok(NULL, " ");
    }

    res[i] = '\0';

    Hunspell_destroy(h);
    free(s);

    i = 0;
    while (res[i])
    {
        if (i > 0 && res[i - 1] != ' ' && res[i - 1] != '\n' && res[i] >= 'A' && res[i] <= 'Z')
            res[i] += 'a' - 'A';
        i++;
    }

    return res;
}
