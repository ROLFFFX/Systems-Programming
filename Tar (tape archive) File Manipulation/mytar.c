/* THIS  CODE  WAS MY OWN WORK , IT WAS  WRITTEN  WITHOUT  CONSULTING  ANY
SOURCES  OUTSIDE  OF  THOSE  APPROVED  BY THE  INSTRUCTOR. Yuxuan Shi, yshi373, 2599711  */

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>
#include <unistd.h>
#include "inodemap.h"

#define MAGICNUM 0x7261746D
extern char const **Map;    //define external Map to free it during the end of exec

typedef struct {    //create option menu struct to store getopt input
    int createArchive;
    int extractArchive;
    int printArchive;
    int isArchive;
    int isDirectory;
    char *archiveName;
    char *directoryName;
} option_menu;

void checkMultipleModes(int options_counter);
void my_fwrite_close(void *ptr, size_t size, size_t nmemb, FILE *stream, FILE *closed);
void my_fwrite_free(void *ptr, size_t size, size_t nmemb, FILE *stream, void *freed);
void createArchive(option_menu option_menu);
void createArchive_aux(FILE *archive_file, char *directory_tree);
void extractArchive(option_menu option_menu);
void* extractArchive_aux(FILE *archive_file);
void extract_file(FILE *archive_file, char *file_name, mode_t file_mode, long file_modification_time, long file_size, long inode_number);
void* printArchive(option_menu option_menu);
void* printArchive_aux(FILE *archive_file);
DIR * my_opendir(char *name);
void free_inodemap();

int main(int argc, char *argv[])
{
    // initialize option_menu, all flags set to 0
    option_menu option_menu;
    option_menu.createArchive = 0;
    option_menu.extractArchive = 0;
    option_menu.printArchive = 0;
    option_menu.isArchive = 0;
    option_menu.isDirectory = 0;
    // initialize options to take getopt input, and options_counter for error handling
    int options;
    int options_counter = 0;
    
    while ((options = getopt(argc, argv, "cxtf:")) != -1){
        switch (options)
        {
        case 'c':
            options_counter++;
            checkMultipleModes(options_counter);
            option_menu.createArchive = 1;
            break;
        case 'x':
            options_counter++;
            checkMultipleModes(options_counter);
            option_menu.extractArchive = 1;
            break;
        case 't':
            options_counter++;
            checkMultipleModes(options_counter);
            option_menu.printArchive = 1;
            break;
        case 'f':
            option_menu.archiveName = optarg;
            option_menu.isArchive = 1;
            break;
        default:
            fprintf(stderr, "Error: No tarfile specified\n");
            exit(-1);
        }
    }
    if (optind < argc) {    // handles 'c': get directory name
        for (int i = optind; i < argc; i++){
            if (option_menu.createArchive == 1){
                option_menu.directoryName = argv[i];
                option_menu.isDirectory = 1;
                break;
            }
        }
    }

    // Error Handlers
    if (options_counter == 0){  // handles no mode specified
        fprintf(stderr, "Error: No mode specified\n");
        exit(-1);
    }
    if ((option_menu.createArchive == 1) && option_menu.directoryName == NULL){ // handles no directory target specified
        fprintf(stderr, "Error: No directory target specified\n");
        exit(-1);
    }
    if (option_menu.archiveName == NULL){   // handles no tarfile specified
        fprintf(stderr, "Error: No tarfile specified\n");
        exit(-1);
    }
    if ((option_menu.createArchive == 1)){ // handles errors in c mode
        struct stat file_stat;
        if (lstat(option_menu.directoryName, &file_stat) != 0){  // handles specified target doesnt exist
            fprintf(stderr, "Specified target(\"%s\") does not exist.\n", option_menu.directoryName);
            exit(-1);
        } else if (!S_ISDIR(file_stat.st_mode)){    // handles specified target isnt directory
            fprintf(stderr, "Specified target(\"%s\") is not a directory.\n", option_menu.directoryName);
            exit(-1);
        }
    }
    // assign actions according to options.

    if (option_menu.createArchive == 1){
        createArchive(option_menu);
    }
    if (option_menu.extractArchive == 1){
        extractArchive(option_menu);
    }
    if (option_menu.printArchive == 1){
        printArchive(option_menu);
    }
    free_inodemap();
    exit(0);
}

void checkMultipleModes(int options_counter){
    if (options_counter > 1){
        fprintf(stderr, "Error: Multiple modes specified.\n");
        exit(-1);
    }
}

// -c
void createArchive(option_menu option_menu){
    FILE *archive_file = fopen(option_menu.archiveName, "w+");  //open file in reading&writing mode
        if (archive_file == NULL){
            perror("fopen");
            exit(-1);
        }
        // Magic Number / 4 bytes
        int magic_number = MAGICNUM;
        my_fwrite_close(&magic_number, 4, 1, archive_file, archive_file);   //write magic number
        struct stat file_stat;
        if (lstat(option_menu.directoryName, &file_stat)!=0){   //get file stat
            perror("lstat");
            exit(-1);
        }
        // file inode number / 8 bytes
        long inode_number = (long)file_stat.st_ino; 
        my_fwrite_close(&inode_number, 8, 1, archive_file, archive_file);   //write inode number
        set_inode(inode_number, option_menu.directoryName); //add to HashMap
        // file name length / 4 bytes
        int file_name_length = strlen(option_menu.directoryName);
        my_fwrite_close(&file_name_length, 4, 1, archive_file, archive_file);    //write file name length
        // file name / (file name length) bytes
        my_fwrite_close(option_menu.directoryName, file_name_length, 1, archive_file, archive_file);    //write file name
        // file mode / 4 bytes
        int file_mode = (int)file_stat.st_mode;
        my_fwrite_close(&file_mode, 4, 1, archive_file, archive_file);  //write file mode
        // file modification time / 8 bytes / in seconds
        my_fwrite_close(&file_stat.st_mtime, 8, 1, archive_file, archive_file);  //write file modification time
        // file content / (file size) bytes
        createArchive_aux(archive_file, option_menu.directoryName);  //recursively write directory content to archive

        if (fclose(archive_file) != 0){ //close archive file
            perror("fclose");
            exit(-1);
        }
}

// recursively writes directory content into archive
void createArchive_aux(FILE *archive_file, char *directory_tree){
    struct dirent *entry;
    struct stat file_stat;
    char *path;

    DIR *input_directory = opendir(directory_tree);
    if (input_directory == NULL){
        perror("opendir");
        exit(-1);
    }
    //traverse directory
    for (entry = readdir(input_directory); entry != NULL; entry = readdir(input_directory)) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            //calculate the required length and allocate memory
            int path_length = strlen(directory_tree) + strlen(entry->d_name) + 2;
            path = (char *)malloc(path_length);
            if (path == NULL){
                perror("malloc");
                exit(-1);
            }
            //construct the full path
            strncpy(path, directory_tree, path_length);
            strncat(path, "/", path_length - strlen(path));
            strncat(path, entry->d_name, path_length - strlen(path));
            if (lstat(path, &file_stat) != 0){
                perror("lstat");
                free(path);
                exit(-1);
            }
            //skip . and .. and symbolic link
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 && !S_ISLNK(file_stat.st_mode)){
                //inode number
                long inode_num = (long) file_stat.st_ino; 
                my_fwrite_free(&inode_num, 8, 1, archive_file, path);
                //file name length
                int file_name_length = strlen(path);
                my_fwrite_free(&file_name_length, 4, 1, archive_file, path);
                //file name, i.e. relative path
                my_fwrite_free(path, file_name_length, 1, archive_file, path);
                if(!get_inode( file_stat.st_ino )) {    //if no collision, first encounter
                    set_inode(file_stat.st_ino, path);
                    //file mode
                    mode_t mode = (mode_t) file_stat.st_mode;
                    my_fwrite_free(&mode, 4, 1, archive_file, path);
                    //file modification time
                    my_fwrite_free(&file_stat.st_mtime, 8, 1, archive_file, path);
                    //if not directory, have two extra fields size and content
                    if (!S_ISDIR(file_stat.st_mode)){
                        //file size
                        long file_size = (long) file_stat.st_size;
                        my_fwrite_free(&file_size, 8, 1, archive_file, path);
                        //file content
                        if (file_size != 0) {
                            FILE *inputFile = fopen(path, "r");
                            if (inputFile == NULL) {
                                perror("fopen");
                                free(path);
                                exit(-1);
                            }
                            char *read_contents = malloc(file_stat.st_size * sizeof(char));
                            if (read_contents == NULL) {
                                perror("malloc");
                                fclose(inputFile);
                                free(path);
                                exit(-1);
                            }
                            if (!fread(read_contents, file_stat.st_size, 1, inputFile)) {
                                perror("fread");
                                free(read_contents);
                                fclose(inputFile);
                                free(path);
                                exit(-1);
                            }
                            if (!fwrite(read_contents, file_stat.st_size, 1, archive_file)) {
                                perror("fwrite");
                                free(read_contents);
                                fclose(inputFile);
                                free(path);
                                exit(-1);
                            }
                            free(read_contents);
                            if (fclose(inputFile) != 0){
                                perror("fclose");
                                free(path);
                                exit(-1);
                            }
                        }
                    }
                }
                //recursively depth first traversal if is directory
                if (S_ISDIR(file_stat.st_mode)) {
                    createArchive_aux(archive_file, path);
                }
                free(path);
            }
        }
    }
    if (closedir(input_directory) != 0){
        perror("closedir");
        exit(-1);
    }
}

//-x
void extractArchive(option_menu option_menu){
    FILE *archive_file = fopen(option_menu.archiveName,"r"); //open in read mode
    if (archive_file == NULL) {
        perror("fopen");
        exit(-1);
    }
    //check magic number
    int magic_number;
    if (!fread(&magic_number, 4, 1, archive_file)){
        perror("fread");
        exit(-1);
    }
    if (magic_number != MAGICNUM){
        fprintf(stderr, "Error: Bad magic number (%d), should be %d.\n", magic_number, MAGICNUM);    //handle error for bad magic number
        exit(-1);
    }
    void* ptr = extractArchive_aux(archive_file);   //if magic number is correct and file is successfully openedd, call actual extract archive function
    free(ptr);  //free file name after the extraction is complete
    if (fclose(archive_file) != 0){
        perror("fclose");
        exit(-1);
    }
}

void * extractArchive_aux(FILE *archive_file){
    long inode_number;
    void * ptr;
    while(1){ //traverse archive file
        // inode number
        long inode_number;
        if (!fread(&inode_number, 8, 1, archive_file)){
            if (feof(archive_file)){
                break;
            }
            perror("fread");
            exit(-1);
        }
        // file name length
        int file_name_length;
        if (!fread(&file_name_length, 4, 1, archive_file)){
            perror("fread");
            exit(-1);
        }
        // file name
        char *file_name = malloc((file_name_length + 1)*sizeof(char)); // to include null terminator
        if (file_name == NULL){
            perror("malloc");
            exit(-1);
        }
        // start identifying hardlinks, files, or dir
        if (!fread(file_name, file_name_length, 1, archive_file)){
            perror("fread");
            exit(-1);
        }
        file_name[file_name_length] = '\0'; 

        //assign actions accordingly
        if (get_inode(inode_number) != NULL){    //if repeated inode, it's hardlink.
            //the destination for link should be a relative path name.
            char *destination_path = (char *)malloc((strlen(get_inode(inode_number)) + 1) * sizeof(char));   //set up space for destination path using inode number
            if (destination_path == NULL){
                perror("malloc");
                exit(-1);
            }
            if (strcpy(destination_path, get_inode(inode_number)) == NULL){
                perror("strcpy");
                exit(-1);
            }
            // printf("%s\n", file_name);
            if (link(destination_path, file_name) != 0){    //establish hard link
                perror("link");
                exit(-1);
            }
            free(destination_path);
        } else {    //keep reading mode and mode time to see if dir
            // file mode
            mode_t file_mode;
            if (!fread(&file_mode, 4, 1, archive_file)){
                perror("fread");
                free(file_name);
                exit(-1);
            }
            // file modification time
            time_t file_modification_time;
            if (!fread(&file_modification_time, 8, 1, archive_file)){
                perror("fread");
                free(file_name);
                exit(-1);
            }
            // check if is dir or file
            if (S_ISDIR((mode_t)file_mode)){    //dir
                if (mkdir(file_name, (mode_t)file_mode) != 0){ //don't worry about mod time, only mkdir
                    perror("mkdir");
                    exit(-1);
                }
                //create copy of file name so map is not altered by change in file name
                char *file_name_copy = malloc((file_name_length + 1) * sizeof(char));
                if (strcpy(file_name_copy, file_name) == NULL){
                    perror("strcpy");
                    exit(-1);
                }
                const char *file_name_copy_deep = file_name_copy;
                free(file_name_copy);
                // set_inode(inode_number, file_name_copy_deep);
            } else {    //get info for file
                // file size
                long file_size;
                if (!fread(&file_size, 8, 1, archive_file)){
                    perror("fread");
                    free(file_name);
                    exit(-1);
                }
                extract_file(archive_file, file_name, file_mode, file_modification_time, file_size, inode_number);
            }
            set_inode(inode_number, file_name);
        }
        ptr = file_name;
    }
    return (ptr);
}


void extract_file(FILE *archive_file, char *file_name, mode_t file_mode, long file_modification_time, long file_size, long inode_number){
    FILE *curr_file = fopen(file_name, "w+");
    if (curr_file == NULL){
        perror("fopen");
        exit(-1);
    }
    // get file content and write into file
    if (file_size != 0){    //handles empty file
        char *file_content = malloc(file_size);
        if (file_content == NULL){
            perror("malloc");
            fclose(curr_file);
            exit(-1);
        }
        // read in file content
        if (!fread(file_content, file_size, 1, archive_file)){
            perror("fread");
            free(file_content);
            fclose(curr_file);
            exit(-1);
        }
        // write content to curr file
        if (!fwrite(file_content, file_size, 1, curr_file)){
            perror("fwrite for file contents");
            free(file_content);
            fclose(curr_file);
            exit(-1);
        }
        free(file_content);
    }
    if (fclose(curr_file) != 0) {
        perror("fclose");
        exit(-1);
    }
    // file modification time
    struct timeval curr_time;
    if (gettimeofday(&curr_time, NULL) != 0) {  //modify time struct, setup first arg
        perror("gettimeofday for current time");
        fclose(curr_file);
        exit(-1);
    }
    //set up second arg
    struct timeval file_modification_time_struct;
    file_modification_time_struct.tv_sec = file_modification_time;
    file_modification_time_struct.tv_usec = 0;
    //set up arg required by utimes
    struct timeval times[2] = {curr_time, file_modification_time_struct};
    if (utimes(file_name, times) != 0){
        perror("utimes");
        exit(-1);
    }        
    if (chmod(file_name, (mode_t)file_mode) != 0){  //set permissions
        perror("chmod");
        exit(-1);
    }
}

void* printArchive(option_menu option_menu){
    FILE *archive_file = fopen(option_menu.archiveName, "r");
    if (archive_file == NULL){
        perror("fopen");
        exit(-1);
    }
    // check magic number
    int magic_number;
    if (!fread(&magic_number, 4, 1, archive_file)){
        perror("fread");
        exit(-1);
    }
    if (magic_number != MAGICNUM){
        fprintf(stderr, "Error: Bad magic number (%d), should be %d.\n", magic_number, MAGICNUM);    //handle error for bad magic number
        exit(-1);
    }
    void* ptr = printArchive_aux(archive_file);
    if (fclose(archive_file) != 0){
        perror("fclose");
        exit(-1);
    }
    return (ptr);   //return the pointer to be freed after entire execution is complete
}

void* printArchive_aux(FILE *archive_file){
    void *ptr;
    while(1) {
        // inode number
        long inode_number;
        if (fread(&inode_number, 8, 1, archive_file) != 1) {
            if (feof(archive_file)) {
                break;
            } //break if end of file
            perror("fread");
            exit(-1);
        }
        // file name length
        int file_name_length;
        if (!fread(&file_name_length, 4, 1, archive_file)){
            perror("fread");
            exit(-1);
        }
        // file name
        char *file_name = (char*)malloc((file_name_length) * sizeof(char));
        if (file_name == NULL){
            perror("malloc");
            exit(-1);
        }
        if (!fread(file_name, file_name_length, 1, archive_file)){
            perror("fread");
            exit(-1);
        }
        strncat(file_name, "", 1);
        // start determining if hard link
        if (get_inode(inode_number) != NULL){
                printf("%s/ -- inode: %llu\n", file_name, (long long unsigned int)inode_number); //print hardlink

        } else {
            // continue getting info for files and dirs
            // file mode
            mode_t mode;
            if (!fread(&mode, 4, 1, archive_file)) {
                perror("fread");
                exit(-1);
            }
            // file modification time
            time_t file_modification_time;
            if (!fread(&file_modification_time, 8, 1, archive_file)){
                perror("fread");
                exit(-1);
            }
            set_inode(inode_number, file_name);
            // start determining if dir
            if (S_ISDIR(mode)){
                printf("%s/ -- inode: %lu, mode %o, mtime: %llu\n", file_name, inode_number, (mode&0777), (unsigned long long)file_modification_time);  //print last 9 bits of permissions only
            } else {
                //getting rest of information for regular files
                //file size
                long size;
                if (!fread(&size, 8, 1, archive_file)){
                    perror("fread");
                    exit(-1);
                }
                if (size != 0){ //handles empty file
                    //file content
                    char *file_content = (char*)malloc(size * sizeof(char));
                    if (file_content == NULL){
                        perror("malloc");
                        exit(-1);
                    } if (!fread(file_content, size, 1, archive_file)) {
                        perror("fread");
                        free(file_content);
                        exit(-1);
                    }
                    free(file_content);
                }
                if ((mode & S_IXUSR) || (mode & S_IXGRP) || (mode & S_IXOTH)){   //executable
                    printf("%s* -- inode: %lu, mode: %o, mtime: %llu, size: %lu\n", file_name, inode_number, (mode&0777), (unsigned long long)file_modification_time, size);
                } else {
                    printf("%s -- inode: %lu, mode: %o, mtime: %llu, size: %lu\n", file_name, inode_number, (mode&0777), (unsigned long long)file_modification_time, size);
                }
            }
        }
        ptr = file_name;
    }
    return (ptr);
}

// custom hooks: use accordingly
void my_fwrite_close(void *ptr, size_t size, size_t nmemb, FILE *stream, FILE *closed){
    int response = fwrite(ptr, size, nmemb, stream);
    if (response != nmemb){
        perror("fwrite");
        fclose(closed);
        exit(-1);
    }
}

void my_fwrite_free(void *ptr, size_t size, size_t nmemb, FILE *stream, void *freed){
    int response = fwrite(ptr, size, nmemb, stream);
    if (response != nmemb){
        perror("fwrite");
        free(freed);
        exit(-1);
    }
}

DIR * my_opendir(char *name){
    DIR *ptr = opendir(name);
    if (ptr == NULL){
        perror("opendir");
        exit(-1);
    }
    return ptr;
}

void free_inodemap() {
    if (Map != NULL) {
        for (int i = 0; i < MAPSIZE; i++) {
            if (Map[i] != NULL) {
                free((void*)Map[i]);
            }
        }
        free(Map);
    }
}
