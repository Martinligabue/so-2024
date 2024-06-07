#ifndef ADDRESS_BOOK_H
#define ADDRESS_BOOK_H

int isNumber(char phone[10]);
void query_address_book(char *query, char *sub, char *response);
void add_record(const char *record, char *response);
void modify_record(const char *record, char *response);
void delete_record(const char *record, char *response);
void addDefaults();

#endif // ADDRESS_BOOK_H
