#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

struct SCHEMA_RULES
{
/*CONTROLS LOG TO STDROUT*/ 
int ONOFF;
};

struct TABLE
{

};


 void logger(int ONOFF)
{
    if(ONOFF)
    {
        printf("%s\n ok we log");
    }
}

/*Give a DB name to use*/
static int  USEDB(char *NAME)
{
    DIR* FD;
    if (NULL == (FD = opendir (NAME))) 
    {
        // fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
        // fclose(common_file);
        return 0;
    }
    return 1;
}

/*
creates a new file, or replaces
returns 1 on success, 0 on failure
*/

static int  INSERT(char * IP, char * FIELD_LIST, char * VALUE_LIST)
{

}

/**/
static int  DELETE(char *IP, char * FIELD_LIST, char * VALUE_LIST)
{

}

/*Reads a value */
static int  READ()
{

}

/*This brings a table in*/
static int TABLE_LOAD()
{

}

/* This is just a sample code, modify it to meet your need */
int main(int argc, char **argv)
{

    struct SCHEMA_RULES local_rules;
    local_rules.ONOFF=0;
    int check_use = USEDB(argv[1]);
    printf("%i This is what happends\n",check_use);
    logger(local_rules.ONOFF);
    // DIR* FD;
    // struct dirent* in_file;
    // FILE    *common_file;
    // FILE    *entry_file;
    // char    buffer[BUFSIZ];

    // /* Openiing common file for writing */
    // common_file = fopen("row", "w");
    // if (common_file == NULL)
    // {
    //     fprintf(stderr, "Error : Failed to open common_file - %s\n", strerror(errno));

    //     return 1;
    // }

    // /* Scanning the in directory */
    // if (NULL == (FD = opendir (in_dir))) 
    // {
    //     fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
    //     fclose(common_file);

    //     return 1;
    // }
    // while ((in_file = readdir(FD))) 
    // {
    //     /* On linux/Unix we don't want current and parent directories
    //      * On windows machine too, thanks Greg Hewgill
    //      */
    //     if (!strcmp (in_file->d_name, "."))
    //         continue;
    //     if (!strcmp (in_file->d_name, ".."))    
    //         continue;
    //     /* Open directory entry file for common operation */
    //     /* TODO : change permissions to meet your need! */
    //     entry_file = fopen(in_file->d_name, "rw");
    //     if (entry_file == NULL)
    //     {
    //         fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
    //         fclose(common_file);

    //         return 1;
    //     }

    //     /* Doing some struf with entry_file : */
    //     /* For example use fgets */
    //     while (fgets(buffer, BUFSIZ, entry_file) != NULL)
    //     {
    //         /* Use fprintf or fwrite to write some stuff into common_file*/
    //     }

    //     /* When you finish with the file, close it */
    //     fclose(entry_file);
    // }

    // /* Don't forget to close common file before leaving */
    // fclose(common_file);

    return 0;
}