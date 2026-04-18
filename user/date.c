#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int get_days_in_month(int year, int month) {
    if (month == 1 && is_leap(year)) return 29;
    return days_in_month[month];
}

static void print_two_digits(int n) {
    if (n < 10) printf("0");
    printf("%d", n);
}

static void print_date(uint64 ns_uint) {
    long ns = (long)ns_uint;

    long seconds = ns / 1000000000;
    long nsec_part = ns % 1000000000;

    if (nsec_part < 0) {
        nsec_part += 1000000000;
        seconds -= 1;
    }
    
    int is_negative = (seconds < 0);
    if (is_negative) seconds = -seconds;

    int year = is_negative ? 1969 : 1970;
    int month = is_negative ? 11 : 0;
    int day = 1;
    int hour = 0;
    int minute = 0;
    int second = 0;

    if (is_negative) {
        while (seconds > 0) {
            int year_days = is_leap(year) ? 366 : 365;
            long year_seconds = year_days * 24L * 3600L;
            if (seconds < year_seconds) break;

            seconds -= year_seconds;
            year--;
        }

        while (seconds > 0) {
            int month_days = get_days_in_month(year, month);
            long month_seconds = month_days * 24L * 3600L;

            if (seconds < month_seconds) break;

            seconds -= month_seconds;
            month--;

            if (month < 0) {
                month = 11;
                year--;
            }
        }

        int month_days = get_days_in_month(year, month);
        day = month_days - (seconds / (24 * 3600));
        seconds %= (24 * 3600);

        if (day < 1) {
            month--;
            if (month < 0) {
                month = 11;
                year--;
            }
            day = get_days_in_month(year, month) + day;
        }

        hour = 23 - (seconds / 3600);
        seconds %= 3600;

        minute = 59 - (seconds / 60);
        seconds %= 60;

        second = 59 - seconds;

        if (second < 0) {
            second = 59;
            minute--;
        }
        if (minute < 0) {
            minute = 59;
            hour--;
        }
        if (hour < 0) {
            hour = 23;
            day--;
        }
        if (day < 1) {
            month--;
            if (month < 0) { month = 11; year--; }
            day = get_days_in_month(year, month);
        }
        
    } else {
        while (1) {
            int year_days = is_leap(year) ? 366 : 365;
            long year_seconds = year_days * 24L * 3600L;
            if (seconds < year_seconds) break;
            seconds -= year_seconds;
            year++;
        }

        while (1) {
            int month_days = get_days_in_month(year, month);
            long month_seconds = month_days * 24L * 3600L;
            if (seconds < month_seconds) break;
            seconds -= month_seconds;
            month++;
        }

        day = seconds / (24 * 3600) + 1;
        seconds %= (24 * 3600);

        hour = seconds / 3600;
        seconds %= 3600;

        minute = seconds / 60;
        second = seconds % 60;
    }

    printf("%d", year);
    printf("-");

    print_two_digits(month + 1);
    printf("-");

    print_two_digits(day);
    printf(" ");
    print_two_digits(hour);
    printf(":");

    print_two_digits(minute);
    printf(":");

    print_two_digits(second);
    printf(".");

    printf("%ld", nsec_part);
    printf("\n");
}

int main(int argc, char *argv[]) {
    uint64 ns;
    if (gettime(&ns) < 0) {
        fprintf(2, "Error: failed to get time.\n");
        exit(1);
    }

    print_date(ns);
    exit(0);
}