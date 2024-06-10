#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../inc/address_book.h"
#include "../inc/common.h"

/*
Global variables.
*/
Record address_book[MAX_RECORDS];
int record_count = 0;
int id = 1;

/*
Check if a string is a number and if has a correct length.
*/
int isValidNumber(char studentId[7])
{
    int length = strlen(studentId);
    if (length != 7)
        return 1;

    for (int i = 0; i < length; i++)
    {
        if (studentId[i] < '0' || studentId[i] > '9')
            return 1;
    }
    return 0;
}

/*
Check if the studentId is unique.
*/
int is_unique_student_id(const char *studentId)
{
    for (int i = 0; i < record_count; i++)
        if (strcmp(address_book[i].studentId, studentId) == 0)
            return 1;
    return 0;
}

/*
Logs operations (add, modify, delete) to a log file.
*/
void log_change(const char *operation, Record *record)
{
    FILE *log_file = fopen(CHANGE_LOG, "a");
    if (log_file == NULL)
    {
        perror("error opening logfile!\n");
        return;
    }
    fprintf(log_file, "Operation: %s, Record ID: %d, Name: %s, Surname: %s, Student ID: %s\n",
            operation, record->id, record->name, record->surname, record->studentId);
    fclose(log_file);

    return;
}

/*
Concatenate query results into a buffer.
*/
void concatenateResults(char *buffer, char *bufferTemp, int i, Record *address_book)
{
    sprintf(bufferTemp, "%d: %s, %s, %s\n",
            address_book[i].id,
            address_book[i].name,
            address_book[i].surname,
            address_book[i].studentId);
    strcat(buffer, bufferTemp);
    return;
}

/*
Query the address book based on a query and subquery.
*/
void query_address_book(char *query, char *subquery, char *response)
{
    char buffer[BUFFER_SIZE];
    char bufferTemp[BUFFER_SIZE];

    memset(response, 0, BUFFER_SIZE);

    for (int i = 0; i <= record_count; i++)
    {
        if (strcmp(subquery, "1") == 0)
        {
            if (strcmp(address_book[i].name, query) == 0)
                concatenateResults(buffer, bufferTemp, i, address_book);
        }
        else if (strcmp(subquery, "2") == 0)
        {
            if (strcmp(address_book[i].surname, query) == 0)
                concatenateResults(buffer, bufferTemp, i, address_book);
        }
        else if (strcmp(subquery, "3") == 0)
        {
            if (strcmp(address_book[i].studentId, query) == 0)
                concatenateResults(buffer, bufferTemp, i, address_book);
        }
        else if ((strcmp(subquery, "4" == 0) && (address_book[i].name != "")) //TODO: non testato
        {
            sprintf(bufferTemp, "%s,%s,%s\n",
            address_book[i].name,
            address_book[i].surname,
            address_book[i].studentId);
            strcat(buffer, bufferTemp);
        }
        else
        {
            sprintf(response, "Invalid selection!\n");
            return;
        }
    }
    if (strlen(buffer) == 0)
        sprintf(response, "Record not found\n");
    else
        sprintf(response, "\n%s", buffer);

    memset(bufferTemp, 0, sizeof(bufferTemp));
    memset(buffer, 0, sizeof(buffer));
    return;
}

/*
Add a record to the address book.
*/
void add_record(const char *record, char *response)
{
    // Check if the address book is full
    if (record_count >= MAX_RECORDS)
    {
        strcpy(response, "Address book is full");
        return;
    }

    // Parse the record string
    char *record_copy = strdup(record); // Make a copy of the record string because strtok modifies the string
    char *name = strtok(record_copy, ",");
    char *surname = strtok(NULL, ",");
    char *studentId = strtok(NULL, ",");

    // Check if all fields were provided
    if (name == NULL || surname == NULL || studentId == NULL)
    {
        strcpy(response, "Invalid record format");
        return;
    }
    // check that studentId is made of only numbers
    if (isValidNumber(studentId) || is_unique_student_id(studentId))
    {
        strcpy(response, "Invalid student Id");
        return;
    }

    Record new_record = address_book[record_count++];
    new_record.id = id++;
    strcpy(new_record.name, name);
    strcpy(new_record.surname, surname);
    strcpy(new_record.studentId, studentId);
    record_count++;

    log_change("ADD", &new_record);

    strcpy(response, ADDED_RECORD);
}

/*
Delete a record from the address book.
*/
void delete_record(const char *recordId, char *response)
{
    int del = atoi(recordId);
    if (del > record_count || del <= 1)
    {
        strcpy(response, "Record doesn't exist\n");
        return;
    }
    record_count--;
    Record *record = &address_book[del - 1];
    if (record->id == 0)
    {
        strcpy(response, "Record already deleted\n");
        return;
    }

    log_change("DELETE", record);

    record->id = 0;
    record->name[0] = '\0';
    record->surname[0] = '\0';
    record->studentId[0] = '\0';

    strcpy(response, DELETED_RECORD);
}

/*
Modify a record in the address book.
*/
void modify_record(const char *record, char *response)
{
    // Parse the record string
    char *record_copy = strdup(record); // Make a copy of the record string because strtok modifies the string
    char *id = strtok(record_copy, ",");
    char *name = strtok(NULL, ",");
    char *surname = strtok(NULL, ",");
    char *studentId = strtok(NULL, ",");

    // Check if all fields were provided
    if (name == NULL || surname == NULL || studentId == NULL)
    {
        strcpy(response, "Invalid record format");
        free(record_copy);
        return;
    }
    int record_id = atoi(id);
    if (record_id < 0 || record_id > record_count || address_book[record_id - 1].id == 0)
    {
        strcpy(response, "Record doesn't exist\n");
        free(record_copy);
        return;
    }

    if (!isValidNumber(studentId))
    {
        strcpy(response, "Invalid student Id");
        free(record_copy);
        return;
    }

    // Add the new record to the surname book
    Record *record_to_modify = &address_book[record_id - 1];
    record_to_modify->id = record_id;
    strcpy(record_to_modify->name, name);
    strcpy(record_to_modify->surname, surname);
    strcpy(record_to_modify->studentId, studentId);

    log_change("MODIFY", record_to_modify);

    strcpy(response, MODIFIED_RECORD);
    free(record_copy);
}

/*
Add default records from a file.
*/
void addDefaults()
{

    FILE *fd;
    fd = fopen(RUBRICA, "r");
    if (fd == NULL)
    {
        perror("Error opening file!\n");
        return;
    }

    // search in the file a line identical to credentials
    char line[256];
    while (fgets(line, sizeof(line), fd))
    {
        line[strcspn(line, "\n")] = '\0';
        char *line_copy = strdup(line); // Make a copy of the record string because strtok modifies the string
        char *name = strtok(line_copy, ",");
        char *surname = strtok(NULL, ",");
        char *studentId = strtok(NULL, ",");
        strcpy(address_book[record_count].name, name);
        strcpy(address_book[record_count].surname, surname);
        strcpy(address_book[record_count].studentId, studentId);
        ;
        address_book[record_count].id = id++;
        record_count++;
    }
    fclose(fd);
}

/*
 Convert a string to lowercase.
*/
void to_lowercase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}
