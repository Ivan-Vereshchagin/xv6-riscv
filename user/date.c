#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void print_two_digits(int n) {
    if (n < 10) printf("0");
    printf("%d", n);
}

void print_date(uint64 ns) {
    uint64 seconds = ns / 1000000000;
    uint64 nsec_part = ns % 1000000000;
    int year = 1970;

    while (1) {
        int year_seconds = is_leap(year) ? 366 * 24 * 3600 : 365 * 24 * 3600;
        if (seconds < year_seconds) break;
        seconds -= year_seconds;
        year++;
    }
    
    int month = 0;
    while (1) {
        int month_days = days_in_month[month];
        if (month == 1 && is_leap(year)) month_days++;
        
        if (seconds < month_days * 24 * 3600) break;

        seconds -= month_days * 24 * 3600;
        month++;
    }
    
    int day = seconds / (24 * 3600) + 1;
    seconds %= (24 * 3600);
    
    int hour = seconds / 3600;
    seconds %= 3600;
    int minute = seconds / 60;
    int second = seconds % 60;

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

    printf("%d", (int)nsec_part);
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
