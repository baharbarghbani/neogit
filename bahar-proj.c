#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
char *username;
char *email;
FILE *repo_list;

int run_init(char const *argv[], int argc);
int config(char const *argv[], int argc);
int global_config(char const *argv[], int argc);
int alias(char const* argv[], int argc);
int global_alias(char const* argv[], int argc);


int main(char const *argv[], int argc)
{
    if (argc < 2)
    {
        fprintf(stderr, "Invalid command: too few arguements\n");
        return 1;
    }
    email = (char *)malloc(1000);
    username = (char *)malloc(1000);
    if (strcmp(argv[1], "init") == 0)
    {
        return run_init(argv, argc);
    }
    else if ((strcmp(argv[1], "config") == 0) && (strcmp(argv[2], "--global") != 0))
    {
        return config(argv, argc);
    }
    else if ((strcmp(argv[1], "config") == 0) && (strcmp(argv[2], "--global") == 0))
    {
        return global_config(argv, argc);
    }
    else if ((strcmp(argv[1], "config") == 0 ) && (strstr(argv[2], "alias.") != NULL))
    {
        return alias(argv, argc);
    }
    else
    {
        fprintf(stderr, "Invalid neogit command\n");
        return 1;
    }

    return 0;
}
please write
int run_init(char const *argv[], int argc)
{
    char cwd[2000];
    char tmp_cwd[2000];
    bool exists = false;
    struct dirent *entry;
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "No current working directory in init\n");
        return 1;
    }
    do
    {
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            fprintf(stderr, "Error openning directory in init\n");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if ((entry->d_type == DT_DIR) && (strcmp(entry->d_name, ".neogit") == 0))
            {
                exists = true;
                break;
            }
            closedir(dir);

            if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
                return 1;

            if (strcmp(tmp_cwd, "/") != 0)
            {
                if (chdir("..") != 0)
                    return 1;
            }
        }
    } while (strcmp(tmp_cwd, "/") != 0);
    if (chdir(cwd) != 0)
        return 1;
    repo_list = fopen("/mnt/c/c_proj/neogit/src/repolist.txt", "a");
    if (!exists)
    {
        if (mkdir(".neogit", 0755) != 0)
        {
            return 1;
        }
        strcat(cwd, "/.neogit");
        fprintf(repo_list, "%s\n", cwd);
    }
    else
    {
        fprintf(stderr, "Repository has already been initialized\n");
        return 1;
    }

    return 0;
}
int config(char const *argv[], int argc)
{
    if (argc != 4)
    {
        fprintf(stderr, "Wrong number of arguements\n");
        return 1;
    }
    if ((strcmp(argv[2], "user.name") != 0) && strcmp(argv[2], "user.email") != 0)
    {
        fprintf(stderr, "Invalid local config command\n");
        return 1;
    }
    char *cwd = (char *)malloc(2000);
    char *tmp_cwd = (char *)malloc(2000);
    char *address = (char *)malloc(2000);
    struct dirent *entry;
    bool exists = 0;
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error in configuration\n");
        return 1;
    }
    do
    {
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            fprintf(stderr, "Error occured\n");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if ((entry->d_type == DT_DIR) && (strcmp(entry->d_name, ".neogit") == 0))
            {
                getcwd(address, sizeof(address));
                exists = true;
                break;
            }
        }
        closedir(dir);
        if (exists)
            break;

        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
        {
            fprintf(stderr, "Error occured\n");
            return 1;
        }
        if (strcmp(tmp_cwd, "/") != 0)
        {
            if (chdir("..") != 0)
            {
                fprintf(stderr, "Error occured\n");
                return 1;
            }
        }
    } while (strcmp(tmp_cwd, "/") != 0);
    if (exists)
    {
        strcat(address, "/.neogit");
        chdir(address);
        DIR* dir = opendir(".");
        if (mkdir("config", 0755) != 0)
        {
            fprintf(stderr, "Error occured in config\n");
            return 1;
        }
        closedir(dir);
        strcat(address, "/config");
        chdir(address);
        dir = opendir(".");
        if (strcmp(argv[2], "user.name") == 0)
        {
            FILE *user;
            strcat(address, "/username");
            user = fopen(address, "w");
            if (user == NULL)
            {
                fprintf(stderr, "Error occured\n");
                return 1;
            }
            strcpy(username, argv[3]);
            fprintf(user, "%s\n", username);
            fclose(user);
        }
        else if (strcmp(argv[2], "user.email") == 0)
        {
            FILE *user;
            strcat(address, "/useremail");
            user = fopen(address, "w");
            if (user == NULL)
            {
                fprintf(stderr, "Error occured\n");
                return 1;
            }
            strcpy(email, argv[3]);
            fprintf(user, "%s\n", email);
            fclose(user);
        }
        else
        {
            fprintf(stderr, "Error in local config\n");
            return 1;
        }
        closedir(dir);
    }
    else
    {
        fprintf(stderr, "Repository hasn't been initialized\n");
        return 1;
    }
    free(cwd);
    free(tmp_cwd);
    free(address);
    return 0;
}
int global_config(char const *argv[], int argc)
{
    if (argc != 5)
    {
        fprintf(stderr, "Wrong number of arguements\n");
        return 1;
    }
    if ((strcmp(argv[3], "user.name") != 0) && (strcmp(argv[3], "user.email") != 0))
    {
        fprintf(stderr, "Invalid global config command\n");
        return 1;
    }
    char *address = "/mnt/c/c_proj/neogit/src";
    chdir(address);
    DIR *dir = opendir(".");
    struct dirent *entry;
    bool exists = false;
    char *cwd = (char *)malloc(2000);
    repo_list = fopen(repo_list, "r");
    char* repo = (char*)malloc(2000);
    while ((entry = readdir(dir)) != NULL)
    {
        if ((entry->d_type == DT_DIR) && (strcmp(entry->d_name, "global_config") == 0))
        {
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                fprintf(stderr, "Error in global configuration\n");
                return 1;
            }
            exists = true;
            break;
        }
    }
    closedir(dir);
    if (!exists)
    {
        if (mkdir("global_config", 0755) != 0)
        {
            fprintf(stderr, "Error occured in global config\n");
            return 1;
        }
    }
    strcat(cwd, "/global_config");
    chdir(cwd);
    dir = opendir(".");
    if (strcmp(argv[3], "user.name") == 0)
    {
        strcat(cwd, "/global_name");
        FILE *file = fopen(cwd, "w");
        strcpy(username, argv[4]);
        fprintf(file, "%s\n", username);
        fclose(file);
    }
    else if (strcmp(argv[3], "user.email") == 0)
    {
        strcat(cwd, "/global_email");
        FILE *file = fopen(cwd, "w");
        strcpy(email, argv[4]);
        fprntf(file, "%s\n", email);
            fclose(file);
    }
    else
    {
        fprintf(stderr, "Error in global configuration\n");
        return 1;
    }
    closefir(dir);
   while(fgets(repo, "%s", repo_list) != NULL)
   {
        int length = strlen(repo);
        repo[length - 1] = '\0';
        strcat(repo,"/config");
        chdir(repo);
        DIR* dir = opendir(".");
        if(strcmp(argv[3], "user.name") == 0)
        {
            strcat(repo, "/username");
            FILE* file = fopen(repo, "w");
            strcpy(username, argv[4]);
            fprintf(file, "%s\n", username);
            fclose(file);
        }
        else if(strcmp(argv[3], "user.email") == 0)
        {
            strcat(repo, "/useremail");
            FILE* file = fopen(repo, "w");
            strcpy(email, argv[4]);
            fprintf(file, "%s\n", email);
            fclose(file);
        }
        else{
            fprintf(stderr, "Invalid configuration command\n");
            return 1;
        }
        closedir(dir);
   }
    free(address);
    free(repo);
    fclose(repo_list);
    return 0;
}
int alias(char const* argv[], int argc)
{
    if(argc != 4)
    {
        fprintf(stderr, "Wrong number of arguements\n");
        return 1;
    }
    struct dirent* entry;
    bool exists = false;
    char* cwd = (char*)malloc(2000);
    char* tmp_cwd = (char*)malloc(2000);
    if(getcwd(cwd, sizeof(cwd)) == NULL)
    {
        fprintf(stderr, "Error occured in alias\n");
        return 1;
    }

    do
    {
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            fprintf(stderr, "Error openning directory in init\n");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL)
        {
            if ((entry->d_type == DT_DIR) && (strcmp(entry->d_name, ".neogit") == 0))
            {
                if(getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
                {
                    fprintf(stderr, "Error occured in alias\n");
                    return 1;
                }
                exists = true;
                closedir(dir);
                break;
            }

            if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
                return 1;

            if (strcmp(tmp_cwd, "/") != 0)
            {
                if (chdir("..") != 0)
                    return 1;
            }
        }
        if(exists)
        {
            break;
        }
    } while (strcmp(tmp_cwd, "/") != 0);
    strcat(tmp_cwd, "/.neogit");
    chdir(tmp_cwd);
    DIR* dir = opendir(".");
    if(dir == NULL)
    {
        fprintf(stderr, "Error occured in alias\n");
        return 1;
    }
    strcat(tmp_cwd, "/alias.txt");
    FILE *alias_file = fopen(tmp_cwd, "a");
    if(alias_file == NULL)
    {
        fprintf(stderr, "Error occured in openning alias.txt\n");
        return 1;
    }
    char* name = (char*)malloc(1000);
    sscanf(argv[2], "alias.%s", name);
    char* command = (char*)malloc(2000);
    strcpy(command, argv[3]);
    command = command + strlen("neogit") + 1;
    char* tmp = (char*)malloc(2000);
    FILE* commands = fopen("/mnt/c/c_proj/neogit/src/commands.txt", "r");
    if(commands == NULL)
    {
        fprintf(stderr, "Error occured in opeening commands file\n");
        return 1;
    }
    bool ok= false;
    while(fgets(tmp, 2000, commands) != NULL)
    {
        if(strcmp(command, tmp) == 0)
        {
            ok = true;
            break;
        }
    }
    if(ok)
    {
        fprintf(alias_file, "%s  %s\n", name, command);
    }
    else{
        fprintf(stderr, "Given command isn't a neogit command\n");
        return 1;
    }
    closedir(dir);
    fclose(commands);
    fclose(alias_file);
    free(tmp);
    free(command);
    free(name);
    free(cwd);
    free(tmp_cwd);
    return 0;
}
int global_alias(char const* argv[], int argc)
{
    if(argc != 5)
    {
        fprintf(stderr, "Wrong number of arguements in alias global\n");
        return 0;
    }
    FILE *global = fopen("/mnt/c/c_proj/neogti/src/alias_global.txt", "a");  //
    char* lines = (char*)malloc(2000);
    FILE* source = fopen("/mnt/c/c_proj/neogit/src/commands.txt", "r");

    





    free(lines);
    return 0;
}