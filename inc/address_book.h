#ifndef ADDRESS_BOOK_H
#define ADDRESS_BOOK_H

#define RUBRICA "res/rubrica.csv"
#define CHANGE_LOG "log/change_log.txt"
#define ADDED_RECORD "Record added successfully!"
#define DELETED_RECORD "Record deleted successfully!"
#define MODIFIED_RECORD "Record modified successfully!"

int isValidNumber(char studentId[7]);
void query_address_book(char *query, char *sub, char *response);
void add_record(const char *record, char *response);
void modify_record(const char *record, char *response);
void delete_record(const char *record, char *response);
void addDefaults();
int is_unique_student_id(const char *studentId);


#endif
