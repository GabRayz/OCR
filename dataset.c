#include "image.h"
#include "separation.h"
#include "dataset.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <ImageMagick-7/MagickWand/MagickWand.h>

char *concat(char *a, char *b)
{
    int len = strlen(a) + strlen(b) + 1;
    char *res = malloc(sizeof(char) * len);

    strcpy(res, a);
    strcat(res, b);

    return res;
}

Img **read_dataset(int dataCount)
{
    /* 
    Read the dataset/training directory to get images names
     */
    Img **images = malloc(sizeof(Img *) * dataCount);
    char *prefix = "dataset/easy/";
    char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir("./dataset/easy/");
    if (!dataset)
    {
        images[0] = NULL;
        return images;
    }

    // Skip self and parent directory
    readdir(dataset);
    readdir(dataset);

    // For each file up to dataCount
    int i = 0;
    while ((dir = readdir(dataset)) != NULL && i < dataCount)
    {
        // Store the file in the img
        images[i] = img_init(28, 28);
        images[i]->filepath = concat(prefix, dir->d_name);
        // strcpy(images[i]->label, dir->d_name[0]);
        images[i]->label = dir->d_name[0];
        i++;
    }

    if (i < dataCount)
        files[i] = NULL;

    closedir(dataset);
    dataset_to_pixels(images, dataCount);
    return images;
}

Img **read_dataset2()
{
    /* 
    Read the dataset/training directory to get images names
     */
    printf("Reading dataset...\n");

    int dataCount = 1016 * 62;
    Img **images = malloc(sizeof(Img *) * dataCount);
    char *prefix = "./dataset/fonts/";
    // char **files = malloc(sizeof(char *) * dataCount);

    struct dirent *dir;
    DIR *dataset = opendir(prefix);

    assert(dataset != NULL);

    // Skip self and parent directory
    readdir(dataset);
    readdir(dataset);

    int i = 0;
    while ((dir = readdir(dataset)) != NULL && i < 62)
    {
        // Prevent hiddent files
        if (dir->d_name[0] == '.')
            continue;

        char *folderName = concat(prefix, dir->d_name);

        char *tmp = concat(folderName, "/");
        DIR *character = opendir(folderName);
        int num = atoi(dir->d_name);
        char label = num < 11 ? num + '0' - 1 : num < 37 ? num + 'A' - 11 : num + 'a' - 37;
        readdir(character);
        readdir(character);

        struct dirent *dir2;
        int j = 0;
        while ((dir2 = readdir(character)) != NULL && j < 1016)
        {
            // Prevent hiddent files
            if (dir->d_name[0] == '.')
                continue;
            char *fileName = concat(tmp, dir2->d_name);

            // Store the file in the img
            int index = i * 1016 + j;
            images[index] = img_init(128, 128);
            images[index]->filepath = fileName;
            images[index]->label = label;
            j++;
        }

        free(tmp);
        free(folderName);
        closedir(character);
        i++;
    }

    closedir(dataset);
    dataset_to_pixels(images, dataCount);
    return images;
}

void dataset_to_pixels(Img **images, int dataCount)
{

    fputs("\e[?25l", stdout); /* hide the cursor */

    for (int i = 0; i < dataCount && images[i]; i++)
    {

        printf("\r%d / %d", i + 1, dataCount);
        MagickWand *mw = NewMagickWand();
        Img *image = images[i];

        if (MagickReadImage(mw, image->filepath) == MagickTrue)
        {
            // printf("File opened successfuly\n");
            // MagickAdaptiveResizeImage(mw, 28, 28);
            MagickExportImagePixels(mw, 0, 0, MagickGetImageWidth(mw), MagickGetImageHeight(mw), "R", DoublePixel, image->pixels);

            //print_image(image);
        }
        else
            printf("FAILED: %s\n", image->filepath);
        DestroyMagickWand(mw);
    }

    fputs("\e[?25h", stdout); /* show the cursor */
    printf("\n");
}

// char *itoa(int num)
// {

// }

void create_dataset_from_img(char *dirpath, Img **images, int dataCount, int index)
{
    for (int i = 0; i < dataCount; i++)
    {
        if (i % 73 == 0)
            index++;
        char *label = malloc(sizeof(char) * 5);
        sprintf(label, "%u", (unsigned char)images[i]->label);

        char truc[] = {'_', index / 100 + 48, (index / 10) % 10 + 48, index % 10 + 48, '.', 'p', 'n', 'g', '\0'};
        char *tmp = concat(dirpath, label);
        char *filepath = concat(tmp, truc);
        
        // print_image(images[i]);
        printf("%s\n", filepath);
        img_save(images[i], filepath);

        free(tmp);
        free(filepath);
    }
}

void	*ft_print_memory(void *addr, unsigned int size);

LinkedList *read_dataset3(char *filepath)
{
    /* Read the dataset without knowing number of files */
    
    LinkedList *images = list_init();
    DIR *dir = opendir(filepath);

    readdir(dir);
    readdir(dir);
    struct dirent *file;
    int i = 0;
    while ((file = readdir(dir)) != NULL)
    {
        // Store the file in the img
        Img *image = img_init(28, 28);
        image->filepath = concat(filepath, file->d_name);
        int c = atoi(file->d_name);
        ft_print_memory(&c, 8);
        image->label = c;
        
        list_insert(images, node_init(image));
        i++;
    }

    // Img ** list = (Img **)list_to_array(images);
    //dataset_to_pixels(list, i);
    return images;
}

void ft_putchar(char c) {
    printf("%c", c);
}

void	ft_put_point_str(char *str, int len)
{
	int i;

	i = 0;
	while (i < len)
	{
		if (str[i] >= 32 && str[i] <= 126)
			ft_putchar(str[i]);
		else
			ft_putchar('.');
		i++;
	}
}

int		ft_power(int nb, int power)
{
	int res;

	if (power < 0)
		return (0);
	res = 1;
	while (power > 0)
	{
		res *= nb;
		power--;
	}
	return (res);
}

void	ft_put2hexa(char *str, int len)
{
	char			*base;
	int				i;
	unsigned char	cache;

	i = 0;
	base = "0123456789abcdef";
	while (i < 16)
	{
		if (i < len)
		{
			cache = str[i];
			ft_putchar(base[cache / 16]);
			ft_putchar(base[cache % 16]);
		}
		else
			ft_put_point_str("  ", 2);
		i++;
		if (i % 2 == 0)
			ft_putchar(' ');
	}
}

void	ft_put_addr(unsigned int addr, int split)
{
	char	*base;
	int		i;
	int		p;

	base = "0123456789abcdef";
	i = 0;
	while (i < 8)
	{
		p = ft_power(16, 7 - i);
		ft_putchar(base[addr / p]);
		addr %= p;
		i++;
		if (split && i % 4 == 0)
			ft_putchar(' ');
	}
	ft_put_point_str(": ", 2);
}

void	*ft_print_memory(void *addr, unsigned int size)
{
	unsigned int	i;
	int				left;

	i = 0;
	while (i < size)
	{
		if (i % 16 == 0)
		{
			left = size - i;
			ft_put_addr(i, 0);
			ft_put2hexa((char *)addr + i, (left >= 16 ? 16 : left));
			ft_put_point_str((char *)addr + i, (left >= 16 ? 16 : left));
			ft_putchar('\n');
		}
		i++;
	}
	return (addr);
}
