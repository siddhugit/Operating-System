/*
 * lcdfs.h
 *
 *  Created on: Oct 31, 2015
 *      Author: URM
 */

#ifndef LCDFS_H_
#define LCDFS_H_

void*lcd_open(const char *name,char mode);
void lcd_close(void *fp);
void lcd_create(const char *name);
void lcd_delete(const char *name);
int lcd_read(void *fp,char* buff);
int lcd_write(void *fp,char c);
char** lcd_list();


#endif /* LCDFS_H_ */
