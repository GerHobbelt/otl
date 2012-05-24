/*
 * XXXX deleted info XXXX
 */

//#include "generic_support.h"
#error "do not compile this one"


#include <sstream>
#include <math.h>




void otl_str_to_tm(const char *str, otl_datetime &tm)
{
	tm.fraction = 0;
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	tm.tz_hour = 0;
	tm.tz_minute = 0;
#endif
	int rv = sscanf(str,
	                "%04d-%02d-%02d %02d:%02d:%02d.%03lu"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	                " %hd:%hd"
#endif
	                "",
	                &tm.year,
	                &tm.month,
	                &tm.day,
	                &tm.hour,
	                &tm.minute,
	                &tm.second,
	                &tm.fraction
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	                ,
	                &tm.tz_hour,
	                &tm.tz_minute
#endif
	               );
	tm.frac_precision = min(3, otl_odbc_date_scale);
	tm.fraction = otl_to_fraction(tm.fraction, tm.frac_precision);

	if (rv == 6)
	{
		rv = sscanf(str,
		            "%04d-%02d-%02d %02d:%02d:%02d"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		            " %hd:%hd"
#endif
		            "",
		            &tm.year,
		            &tm.month,
		            &tm.day,
		            &tm.hour,
		            &tm.minute,
		            &tm.second
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		            ,
		            &tm.tz_hour,
		            &tm.tz_minute
#endif
		           );
		tm.fraction = 0;
		tm.frac_precision = 0;
	}
}



void otl_ts_to_str(const otl_datetime &tm, char str[100])
{
	if (otl_odbc_date_scale == 0)
	{
		sprintf(str,
		        "%04d-%02d-%02d %02d:%02d:%02d"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        " %+hd:%hd"
#endif
		        "",
		        tm.year,
		        tm.month,
		        tm.day,
		        tm.hour,
		        tm.minute,
		        tm.second
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        ,
		        tm.tz_hour,
		        tm.tz_minute
#endif
		       );
	}
	else
	{
		int prec = min(3, otl_odbc_date_scale);

		sprintf(str,
		        "%04d-%02d-%02d %02d:%02d:%02d.%0*u"
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        " %+hd:%hd"
#endif
		        "",
		        tm.year,
		        tm.month,
		        tm.day,
		        tm.hour,
		        tm.minute,
		        tm.second,
		        prec,
		        otl_from_fraction(tm.fraction, prec)
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
		        ,
		        tm.tz_hour,
		        tm.tz_minute
#endif
		       );
	}
}



otl_datetime &cvt_SystemTime2OTL_datetime(otl_datetime &dst, const SYSTEMTIME *src)
{
	if (!src)
	{
		otl_datetime t;
		dst = t;
	}
	else
	{
		dst = otl_datetime(src->wYear, src->wMonth, src->wDay, src->wHour, src->wMinute, src->wSecond, src->wMilliseconds, 3);
	}
	return dst;
}



otl_datetime &cvt_FileTime2OTL_datetime(otl_datetime &dst, const FILETIME *src)
{
	SYSTEMTIME t;

	if (src && FileTimeToSystemTime(src, &t))
	{
		cvt_SystemTime2OTL_datetime(dst, &t);
		// check the subsecond part: see if we need to improve on the default millisecond conversion
		LARGE_INTEGER fi;
		fi.HighPart = src->dwHighDateTime;
		fi.LowPart = src->dwLowDateTime;
		fi.QuadPart %= 10000000;
		if (fi.QuadPart % 10000 != 0)
		{
			// subsecond part doesn't fit in the default 'millisecond' fraction precision: adjust!
			dst.fraction = otl_to_fraction(unsigned int(fi.QuadPart), 7);
			dst.frac_precision = 7;
		}
		return dst;
	}
	else
	{
		otl_datetime t;
		dst = t;
		return dst;
	}
}



otl_datetime &cvt_datetimestamp2OTL_datetime(otl_datetime &dst, datetimestamp src)
{
	FILETIME ft;
	LARGE_INTEGER i;

	i.QuadPart = src;
	ft.dwHighDateTime = i.HighPart;
	ft.dwLowDateTime = i.LowPart;

	return cvt_FileTime2OTL_datetime(dst, &ft);
}



SYSTEMTIME &cvt_OTL_datetime2SystemTime(SYSTEMTIME &dst, const otl_datetime *src)
{
	if (src)
	{
		FILETIME ft;

		dst.wYear = WORD(src->year);
		dst.wMonth = WORD(src->month);
		dst.wDay = WORD(src->day);
		dst.wHour = WORD(src->hour);
		dst.wMinute = WORD(src->minute);
		dst.wSecond = WORD(src->second);
		dst.wMilliseconds = WORD(otl_from_fraction(src->fraction, 3));
		dst.wDayOfWeek = 0;

		// to get at the weekday, we convert back & forth:
		if (SystemTimeToFileTime(&dst, &ft))
		{
			if (FileTimeToSystemTime(&ft, &dst))
			{
				return dst;
			}
		}

		// when we get here, the date/timestamp in [src] is bad: out of allowable range or other cruft in sitting in there.
		// signal this by returning a 'zero' time.
	}
	memset(&dst, 0, sizeof(dst));
	return dst;
}



FILETIME &cvt_OTL_datetime2FileTime(FILETIME &dst, const otl_datetime *src)
{
	SYSTEMTIME st;

	cvt_OTL_datetime2SystemTime(st, src);
	if (src && SystemTimeToFileTime(&st, &dst))
	{
		// check if we need to do a better transfer of the subsecond fraction:
		if (src->frac_precision > 3)
		{
			unsigned int nsec100 = otl_from_fraction(src->fraction, 7);

			nsec100 %= 10000;
			if (nsec100 != 0)
			{
				// subsecond part doesn't fit in the default 'millisecond' fraction precision: adjust!
				dst.dwLowDateTime += nsec100;
			}
		}
		return dst;
	}
	dst.dwHighDateTime = 0;
	dst.dwLowDateTime = 0;
	return dst;
}



datetimestamp cvt_OTL_datetime2datetimestamp(const otl_datetime *src)
{
	if (src)
	{
		FILETIME ft;
		LARGE_INTEGER i;
		SYSTEMTIME dst;

		dst.wYear = WORD(src->year);
		dst.wMonth = WORD(src->month);
		dst.wDay = WORD(src->day);
		dst.wHour = WORD(src->hour);
		dst.wMinute = WORD(src->minute);
		dst.wSecond = WORD(src->second);
		dst.wMilliseconds = WORD(otl_from_fraction(src->fraction, 3));
		dst.wDayOfWeek = 0;

		if (SystemTimeToFileTime(&dst, &ft))
		{
			i.HighPart = ft.dwHighDateTime;
			i.LowPart = ft.dwLowDateTime;
			datetimestamp rv = i.QuadPart;
			// check if we need to do a better transfer of the subsecond fraction:
			if (src->frac_precision > 3)
			{
				rv -= rv % 10000000;
				rv += otl_from_fraction(src->fraction, 7);
			}
			return rv;
		}

		// when we get here, the date/timestamp in [src] is bad: out of allowable range or other cruft in sitting in there.
		// signal this by returning a 'zero' time.
	}
	return 0;
}



double cvt_OTL_datetime2excel_datetime(const otl_datetime *src, bool in_1904_mode)
{
	double ret = 0.0;

	if (src)
	{
		datetimestamp t = cvt_OTL_datetime2datetimestamp(src);

		static bool init_done = false;
		static datetimestamp offsets[2] = { 0 };

		if (!init_done)
		{
			otl_datetime epoch0(1900, 1, 1, 0, 0, 0, 0, 0);
			otl_datetime epoch1(1904, 1, 1, 0, 0, 0, 0, 0);
			epoch0 -= 2 * ONE_DAY;             // Excel believes 1900 is a leap year: correct for that here
			epoch1 -= 1 * ONE_DAY;
			offsets[0] = cvt_OTL_datetime2datetimestamp(&epoch0);
			offsets[1] = cvt_OTL_datetime2datetimestamp(&epoch1);

			init_done = true;
		}

		t -= offsets[in_1904_mode];

		ret = t / double(ONE_DAY);

		if (ret < 0.0)
			ret = 0.0;
		else if (ret > (2100 - 1900) * 365)
			ret = 0.0;
	}
	return ret;
}



datetimestamp cvt_SystemTime2datetimestamp(const SYSTEMTIME *src)
{
	if (src)
	{
		FILETIME ft;
		LARGE_INTEGER i;

		if (SystemTimeToFileTime(src, &ft))
		{
			i.HighPart = ft.dwHighDateTime;
			i.LowPart = ft.dwLowDateTime;
			return i.QuadPart;
		}

		// when we get here, the date/timestamp in [src] is bad: out of allowable range or other cruft in sitting in there.
		// signal this by returning a 'zero' time.
	}
	return 0;
}



datetimestamp cvt_FileTime2datetimestamp(const FILETIME *src)
{
	if (src)
	{
		LARGE_INTEGER i;

		i.HighPart = src->dwHighDateTime;
		i.LowPart = src->dwLowDateTime;
		return i.QuadPart;
	}

	// when we get here, the date/timestamp in [src] is bad: out of allowable range or other cruft in sitting in there.
	// signal this by returning a 'zero' time.
	return 0;
}



SYSTEMTIME &cvt_datetimestamp2SystemTime(SYSTEMTIME &dst, datetimestamp src)
{
	FILETIME ft;
	LARGE_INTEGER i;

	i.QuadPart = src;
	ft.dwHighDateTime = i.HighPart;
	ft.dwLowDateTime = i.LowPart;

	if (FileTimeToSystemTime(&ft, &dst))
	{
		return dst;
	}
	memset(&dst, 0, sizeof(dst));
	return dst;
}



/*
 * clip datetime to within the range year [min..max],
 * i.e. minyear/jan/1 00:00:00.000 and maxyear/dec/31 23:59:59.999...
 */
otl_datetime &clip_datetime2min_max(otl_datetime &dst, int min_year, int max_year)
{
	if (dst.year < min_year)
	{
		dst = otl_datetime(min_year, 1, 1, 0, 0, 0);
	}
	else if (dst.year > max_year)
	{
		dst = otl_datetime(max_year, 12, 31, 23, 59, 59, 999999999, 9);
	}
	return dst;
}



/*
 * clip at end / start to ensure begin- end are not farther apart than [timespan]
 *
 * Return !0 when clipping occurred.
 */
int limit_datetime_range(otl_datetime &begin, otl_datetime &end, timespan span, bool clip_at_end)
{
	FILETIME s;
	FILETIME e;
	LARGE_INTEGER si;
	LARGE_INTEGER ei;

	cvt_OTL_datetime2FileTime(s, &begin);
	cvt_OTL_datetime2FileTime(e, &end);
	si.HighPart = s.dwHighDateTime;
	si.LowPart = s.dwLowDateTime;
	ei.HighPart = e.dwHighDateTime;
	ei.LowPart = e.dwLowDateTime;

	// both are in 100nsec units, so no scaling required :-)
	if (ei.QuadPart - si.QuadPart > span)
	{
		if (clip_at_end)
		{
			ei.QuadPart = si.QuadPart + span;
			e.dwHighDateTime = ei.HighPart;
			e.dwLowDateTime = ei.LowPart;
			cvt_FileTime2OTL_datetime(end, &e);
			return 1;
		}
		else
		{
			si.QuadPart = ei.QuadPart - span;
			s.dwHighDateTime = si.HighPart;
			s.dwLowDateTime = si.LowPart;
			cvt_FileTime2OTL_datetime(begin, &s);
			return 1;
		}
	}
	return 0;
}



// a > b ?
int otl_compare_tm(const otl_datetime &a, const otl_datetime &b)
{
#if defined(OTL_ORA_TIMESTAMP) || defined(OTL_ODBC_TIME_ZONE)
	// TBD: adjust tm for tz!
#error TBD
	if (!diff)
		diff = a.tz_hour - b.tz_hour;
	if (!diff)
		diff = a.tz_minute - b.tz_minute;
#endif
	int diff = a.year - b.year;

	if (!diff)
		diff = a.month - b.month;
	if (!diff)
		diff = a.day - b.day;
	if (!diff)
		diff = a.hour - b.hour;
	if (!diff)
		diff = a.minute - b.minute;
	if (!diff)
		diff = a.second - b.second;
	if (!diff)
		diff = int(a.fraction) - int(b.fraction);
	if (diff > 0)
		return 1;
	else if (diff < 0)
		return -1;

	return 0;
}



bool operator ==(const otl_datetime &a, const otl_datetime &b)
{
	return 0 == otl_compare_tm(a, b);
}



bool operator !=(const otl_datetime &a, const otl_datetime &b)
{
	return 0 != otl_compare_tm(a, b);
}



bool operator >(const otl_datetime &a, const otl_datetime &b)
{
	return 0 < otl_compare_tm(a, b);
}



bool operator >=(const otl_datetime &a, const otl_datetime &b)
{
	return 0 <= otl_compare_tm(a, b);
}



bool operator <(const otl_datetime &a, const otl_datetime &b)
{
	return 0 > otl_compare_tm(a, b);
}



bool operator <=(const otl_datetime &a, const otl_datetime &b)
{
	return 0 >= otl_compare_tm(a, b);
}



timespan operator -(const otl_datetime &a, const otl_datetime &b)
{
	datetimestamp ta = cvt_OTL_datetime2datetimestamp(&a);
	datetimestamp tb = cvt_OTL_datetime2datetimestamp(&b);

	return (timespan)ta - (timespan)tb;
}



#if 0
timespan operator -(const datetimestamp &a, const datetimestamp &b)
{
	return (timespan)a - (timespan)b;
}



#endif



otl_datetime &operator +(const otl_datetime &t, timespan duration)
{
	otl_datetime rv = t;

	return datetime_add_timespan(rv, duration);
}



otl_datetime &operator +(timespan duration, const otl_datetime &t)
{
	otl_datetime rv = t;

	return datetime_add_timespan(rv, duration);
}



otl_datetime &operator -(const otl_datetime &t, timespan duration)
{
	otl_datetime rv = t;

	return datetime_add_timespan(rv, -duration);
}



otl_datetime &operator -(timespan duration, const otl_datetime &t)
{
	otl_datetime rv = t;

	return datetime_add_timespan(rv, -duration);
}



otl_datetime &operator +=(otl_datetime &t, timespan duration)
{
	return datetime_add_timespan(t, duration);
}



otl_datetime &operator -=(otl_datetime &t, timespan duration)
{
	return datetime_add_timespan(t, -duration);
}



otl_datetime &datetime_add_timespan(otl_datetime &dst, timespan duration)
{
	FILETIME ft;
	LARGE_INTEGER fi;

	cvt_OTL_datetime2FileTime(ft, &dst);
	fi.HighPart = ft.dwHighDateTime;
	fi.LowPart = ft.dwLowDateTime;
	fi.QuadPart += duration;
	ft.dwHighDateTime = fi.HighPart;
	ft.dwLowDateTime = fi.LowPart;
	cvt_FileTime2OTL_datetime(dst, &ft);
	return dst;
}



/*
 * return offset since midnight in [seconds].
 */
int time_offset_within_day(const otl_datetime &date)
{
	datetimestamp d = cvt_OTL_datetime2datetimestamp(&date);

	d %= ONE_DAY;

	return int((d + 5000000) / 10000000);
}



otl_datetime &zero_hours(otl_datetime &t)
{
	t.hour = 0;
	t.minute = 0;
	t.second = 0;
	t.fraction = 0;
	t.frac_precision = 0;
	return t;
}



datetimestamp &zero_hours(datetimestamp &t)
{
	t -= (t % ONE_DAY);
	return t;
}



otl_datetime &yesterday(otl_datetime &t)
{
	zero_hours(t);
	t -= ONE_DAY;
	return t;
}



datetimestamp &yesterday(datetimestamp &t)
{
	zero_hours(t);
	t -= ONE_DAY;
	return t;
}



otl_datetime &tomorrow(otl_datetime &t)
{
	zero_hours(t);
	t += ONE_DAY;
	return t;
}



datetimestamp &tomorrow(datetimestamp &t)
{
	zero_hours(t);
	t += ONE_DAY;
	return t;
}



datetimestamp now_UTC(void)
{
	FILETIME start_time;

	GetSystemTimeAsFileTime(&start_time);
	return cvt_FileTime2datetimestamp(&start_time);
}



