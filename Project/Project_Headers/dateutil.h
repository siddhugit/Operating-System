/*
 * dateutil.h
 *
 *  Created on: Nov 15, 2015
 *      Author: URM
 */

#ifndef DATEUTIL_H_
#define DATEUTIL_H_

struct datetime
{
	int month;
	int day;
	int year;
	int hour;
	int minute;
	int seconds;
	int msecs;
};

int getDateTime(struct datetime* dtime);
int setDateTime(const char* dateTimeStr);
const char* getMonthName(int month);
void updateTime();

#endif /* DATEUTIL_H_ */
