#ifndef ADDRESS_BOOK_H
#define ADDRESS_BOOK_H

int query_address_book(const char *query, char *response);
void add_record(const char *record, char *response);
void modify_record(const char *record, char *response);
void delete_record(const char *record, char *response);

#endif // ADDRESS_BOOK_H
