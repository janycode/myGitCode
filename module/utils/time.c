/******************************************************************************
  @file    time.c
  @brief   The time utility functions used in various places in core-app.

  DESCRIPTION
  Time common utility functions

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2015 jrd Incorporated.
  All Rights Reserved. jrd Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/


/******************************************************************************


                        EDIT HISTORY FOR MODULE

when       who            what, where, why
--------   ---      ---------------------------------------------------------- 
2013.11.20   li.kuang             create

******************************************************************************/

/********************************* Includes ***********************************/
#include "string.h"
#include <stdio.h>
#include <time.h>
#include "time.h"
#include "jrd_oem.h"

/*********************************** Locals ***********************************/
#define TIME_JUL_OFFSET_S                 432000UL
#define TIME_JUL_QUAD_YEAR                (366+(3*365))
#define TIME_JUL_BASE_YEAR                1980
#define JRD_QMI_SMS_TIMEZONE_MIN_YEAR     2000

typedef enum{
    YEAR = 0,
    MONTH,
    DAY,
    HOUR,
    MINUTE,
    SECOND
}date_format;

typedef struct
{
  /* Year [1980..2100] */
  uint16                          year;
  /* Month of year [1..12] */
  uint16                          month;
  /* Day of month [1..31] */
  uint16                          day;
  /* Hour of day [0..23] */
  uint16                          hour;
  /* Minute of hour [0..59] */
  uint16                          minute;
  /* Second of minute [0..59] */
  uint16                          second;
  /* Day of the week [0..6] [Monday .. Sunday] */
  uint16                          day_of_week;
}time_julian_type;

static const uint16 year_tab[] = {
  0,                              /* Year 0 (leap year) */
  366,                            /* Year 1             */
  366+365,                        /* Year 2             */
  366+365+365,                    /* Year 3             */
  366+365+365+365                 /* Bracket year       */
};

/* The norm_month_tab table holds the number of cumulative days that have
   elapsed as of the end of each month during a non-leap year. */

static const uint16 norm_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+28,                                /* Feb */
  31+28+31,                             /* Mar */
  31+28+31+30,                          /* Apr */
  31+28+31+30+31,                       /* May */
  31+28+31+30+31+30,                    /* Jun */
  31+28+31+30+31+30+31,                 /* Jul */
  31+28+31+30+31+30+31+31,              /* Aug */
  31+28+31+30+31+30+31+31+30,           /* Sep */
  31+28+31+30+31+30+31+31+30+31,        /* Oct */
  31+28+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+28+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/* The leap_month_tab table holds the number of cumulative days that have
   elapsed as of the end of each month during a leap year. */

static const uint16 leap_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+29,                                /* Feb */
  31+29+31,                             /* Mar */
  31+29+31+30,                          /* Apr */
  31+29+31+30+31,                       /* May */
  31+29+31+30+31+30,                    /* Jun */
  31+29+31+30+31+30+31,                 /* Jul */
  31+29+31+30+31+30+31+31,              /* Aug */
  31+29+31+30+31+30+31+31+30,           /* Sep */
  31+29+31+30+31+30+31+31+30+31,        /* Oct */
  31+29+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+29+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/* The day_offset table holds the number of days to offset
   as of the end of each year. */

static const uint16 day_offset[] = {
  1,                                    /* Year 0 (leap year) */
  1+2,                                  /* Year 1             */
  1+2+1,                                /* Year 2             */
  1+2+1+1                               /* Year 3             */
};

/*********************************** Code *************************************/
static dword div4x2
(
  dword dividend,       /* Dividend, note dword     */
  word  divisor,         /* Divisor                  */
  word  *rem_ptr    /* Pointer to the remainder */
)
{
  *rem_ptr = (word) (dividend % divisor);

  return (dividend / divisor);

} /* END div4x2 */
                                                               
static void time_jul_from_secs(uint32 secs, time_julian_type *julian)
{
  /* Loop index */
  unsigned int /* fast */         i;

  /* Days since beginning of year or quad-year */
  uint16                          days;

  /* Quad years since base date */
  unsigned int /* fast */         quad_years;

  /* Leap-year or non-leap year month table */
  const uint16                    *month_table;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Add number of seconds from Jan 1 to Jan 6 from input seconds
     in order to have number of seconds since Jan 1, 1980 for calculation */

  secs += TIME_JUL_OFFSET_S;


  /* Divide elapsed seconds by 60: remainder is seconds of Julian date;
     quotient is number of elapsed minutes. */

  secs = div4x2 ( secs, 60, &julian->second );


  /* Divide elapsed minutes by 60: remainder is minutes of Julian date;
     quotient is elapsed hours. */

  secs = div4x2 ( secs, 60, &julian->minute );


  /* Divide elapsed hours by 24: remainder is hours of Julian date;
     quotient is elapsed days. */

  secs = div4x2 ( secs, 24, &julian->hour );


  /* Now things get messier. We have number of elapsed days. The 1st thing
     we do is compute how many leap year sets have gone by. We multiply
     this value by 4 (since there are 4 years in a leap year set) and add
     in the base year. This gives us a partial year value. */

  quad_years = div4x2( secs, TIME_JUL_QUAD_YEAR, &days );

  julian->year = TIME_JUL_BASE_YEAR + (4 * quad_years);


  /* Now we use the year_tab to figure out which year of the leap year
     set we are in. */

  for ( i = 0; days >= year_tab[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }

  /* Subtract out days prior to current year. */
  days -= year_tab[ i ];
  
  /* Use search index to arrive at current year. */
  julian->year += i;  


  /* Take the day-of-week offset for the number of quad-years, add in
     the day-of-week offset for the year in a quad-year, add in the number
     of days into this year. */

  julian->day_of_week =
       (day_offset[3] * quad_years + day_offset[i] + days) % 7;


  /* Now we know year, hour, minute and second. We also know how many days
     we are into the current year. From this, we can compute day & month. */


  /* Use leap_month_tab in leap years, and norm_month_tab in other years */

  month_table = (i == 0) ? leap_month_tab : norm_month_tab;


  /* Search month-table to compute month */

  for ( i = 0; days >= month_table[ i + 1 ]; i++ )
  {
    /* No-op. Table seach. */
  }


  /* Compute & store day of month. */
  julian->day = days - month_table[ i ] + 1;  

  /* Store month. */
  julian->month = i + 1;


} /* time_jul_from_secs */

static uint32 time_jul_to_secs( const time_julian_type          *julian)
{
  /* Time in various units (days, hours, minutes, and finally seconds) */
  uint32                          time;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* First, compute number of days contained in the last whole leap
     year set. */

  time = ( (julian->year - TIME_JUL_BASE_YEAR) / 4L ) * TIME_JUL_QUAD_YEAR;


  /* Now, add in days since last leap year to start of this month. */

  if ( (julian->year & 0x3) == 0 )
  {
    /* If this is a leap year, add in only days since the beginning of the
       year to start of this month. */

    time += leap_month_tab[ julian->month - 1 ];
  }
  else
  {
    /* If this is not a leap year, add in days since last leap year plus
       days since start of this year to start of this month. */

    time += year_tab[ julian->year & 0x3 ];

    time += norm_month_tab[ julian->month - 1 ];
  }

  /* Add in days in current month. */
  time += julian->day - 1;

  /* Add in elapsed hours, minutes, and seconds  */
  time = time * 24  +  julian->hour;
  time = time * 60  +  julian->minute;
  time = time * 60  +  julian->second;


  /* Subtract number of seconds from base (GPS) date of 6 Jan 1980 to
     calculation base date of 1 Jan 1980 */

  time -= TIME_JUL_OFFSET_S;


  /* Return elapsed seconds. */
  return time;

} /* time_jul_to_secs */

/*********************************** API *************************************/

void  change_time_to_string_for_EE(char* des_time,uint32 time_span)
{
  time_julian_type out_time;
    JRD_MEMSET(&out_time, 0,sizeof(&out_time));

    if(des_time == NULL)
    return;
  
  time_jul_from_secs(time_span,&out_time);

     JRD_SPRINTF(des_time,"%02d-%02d-%d %02d:%02d:%02d",out_time.day,out_time.month,out_time.year,out_time.hour,out_time.minute,out_time.second);
}
void change_time_to_string_for_general(char* des_time,uint32 time_span)
{
    struct timeval tv;
    time_t time;
    struct tm *p_time;

    if (des_time == NULL) {
        return;
    }

    time = time_span;
    p_time = localtime(&time);
    strftime(des_time, 22, "%Y-%m-%d %H:%M:%S", p_time);
}

/************************************************/
/*modiyf for USA customize time format hh:mm:ss AM(PM) month/day/year 
houailing:2013-06-11
/************************************************/

void change_time_to_string_for_usa(char* des_time,uint32 time_span)
{
  time_julian_type out_time;
  char time_sign = 'A'; 
  int time_h_data =0;
  
    JRD_MEMSET(&out_time, 0,sizeof(&out_time));

    if(des_time == NULL)
    return;
  
  time_jul_from_secs(time_span,&out_time);

  if(out_time.hour == 0)
  {
        time_sign = 'A'; 
      time_h_data = 12;
  }
  else if ((0 < out_time.hour)&&(out_time.hour < 12))
  {
        time_sign = 'A'; 
      time_h_data =out_time.hour;
  }
  else
  {
        time_sign = 'P'; 
      time_h_data = out_time.hour-12;
  }

     JRD_SPRINTF(des_time,"%02d:%02d:%02d %cM %02d/%02d/%d",time_h_data,out_time.minute,out_time.second,time_sign,out_time.month,out_time.day,out_time.year);

}


uint32 change_time_to_sms_span(char * time_str)
{
   char * p;
   char *delim = "-: ";
   int cursor = 0;
   uint16 tmp_result =0;
   time_julian_type out_time;
   uint32 tim_span = 0;

   JRD_MEMSET(&out_time, 0,sizeof(&out_time));

   if(time_str == NULL)
    return 0;

   p = strtok(time_str,delim);
   while(p)
   {
     switch(cursor)
     {
     case YEAR:
       out_time.year = (uint16)JRD_ATOI(p);
       break;
     case MONTH :
       out_time.month = (uint16)JRD_ATOI(p);
       break;
     case DAY:
       out_time.day = (uint16)JRD_ATOI(p);
       break;
     case HOUR:
       out_time.hour = (uint16)JRD_ATOI(p);
       break;
     case MINUTE:
       out_time.minute = (uint16)JRD_ATOI(p);
       break;
     case SECOND:
       out_time.second = (uint16)JRD_ATOI(p);
       break;
     default:
       break;
     }
    cursor ++; 
    p = strtok(NULL,delim);
   }
   tim_span = time_jul_to_secs(&out_time);

   return tim_span;
}

void jrd_qmi_timestamp_to_secs
( 
 wms_timestamp_s_type  time,  /* Pointer to Time Stamp        */
 uint32 *time_stick 
)
{
    uint8 tmp;
    time_julian_type clk; /* Pointer to the result buffer */
    
    tmp = time.year;
    clk.year = (tmp & 0x0F) + ((tmp >> 4) * 10) + JRD_QMI_SMS_TIMEZONE_MIN_YEAR; 
    tmp = time.month;
    clk.month= (tmp & 0x0F) + ((tmp >> 4) * 10);
    tmp = time.day;
    clk.day= (tmp & 0x0F) + ((tmp >> 4) * 10);
    tmp = time.hour;
    clk.hour= (tmp & 0x0F) + ((tmp >> 4) * 10);
    tmp = time.minute;
    clk.minute= (tmp & 0x0F) + ((tmp >> 4) * 10);
    tmp = time.second;
    clk.second= (tmp & 0x0F) + ((tmp >> 4) * 10);
    
    *time_stick = time_jul_to_secs(&clk);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.year:%d\n",time.year);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.month:%d\n",time.month);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.day:%d\n",time.day);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.hour:%d\n",time.hour);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.minute:%d\n",time.minute);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.second:%d\n",time.second);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time.timezone:%d\n",time.timezone);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: new_msg_node->node.time:%d\n",*time_stick);
}

void   jrd_qmi_secs_to_timestamp
(
 uint32 time_stick,
 wms_timestamp_s_type *time
)
{
    uint8 tmp;
    time_julian_type clk; /* Pointer to the result buffer */

    time_jul_from_secs(time_stick, &clk);
    
    tmp = clk.year - JRD_QMI_SMS_TIMEZONE_MIN_YEAR;
    time->year = (tmp/10)<<4 | (tmp%10);
    tmp = clk.month;
    time->month = (tmp/10)<<4 | (tmp%10);
    tmp = clk.day;
    time->day = (tmp/10)<<4 | (tmp%10);
    tmp = clk.hour;
    time->hour = (tmp/10)<<4 | (tmp%10);
    tmp = clk.minute;
    time->minute = (tmp/10)<<4 | (tmp%10);
    tmp = clk.second;
    time->second = (tmp/10)<<4 | (tmp%10);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: new_msg_node->node.time:%d\n",time_stick);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->year:%d\n",time->year);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->month:%d\n",time->month);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->day:%d\n",time->day);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->hour:%d\n",time->hour);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->minute:%d\n",time->minute);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->second:%d\n",time->second);
    JRD_OEM_LOG_INFO(JRD_OEM_LOG_LOW, "CONNIE: time->timezone:%d\n",time->timezone);
}

