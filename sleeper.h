 /*
  *   This program is free software: you can redistribute it and/or modify
  *   it under the terms of the GNU General Public License as published by
  *   the Free Software Foundation, either version 3 of the License, or
  *   (at your option) any later version.
  *
  *   This program is distributed in the hope that it will be useful,
  *   but WITHOUT ANY WARRANTY; without even the implied warranty of
  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *   GNU General Public License for more details.
  *
  *   You should have received a copy of the GNU General Public License
  *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

#include <stdio.h> 
#include "time.h"		// for msleep
#include "signal.h"		// for msleep

int __nsleep(const struct timespec *req, struct timespec *rem)
{
	struct timespec temp_rem;
	if(nanosleep(req,rem)==-1)
		__nsleep(rem,&temp_rem);
	else
		return 1;
}
 
int msleep(unsigned long milisec)
{
	struct timespec req={0},rem={0};
	time_t sec=(int)(milisec/1000);
	milisec=milisec-(sec*1000);
	req.tv_sec=sec;
	req.tv_nsec=milisec*1000000L;
	__nsleep(&req,&rem);
	return 1;
}

