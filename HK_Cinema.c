#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ========== CONSTANTS ==========

#define INITIAL_CAPACITY         4

#define MAX_MOVIES               1000
#define MAX_HALLS                100
#define MAX_SHOWTIMES            5000
#define MAX_TICKETS              50000
#define MAX_DISCOUNTS            1000
#define MAX_USERS                5000

#define CUSTOMER_ROLE            0
#define ADMIN_ROLE               1
#define HALL_PREPARATION_TIME    15
#define NO_DISCOUNT_ID           0
#define VIP_PRICE_MULTIPLIER     1.50f
#define MORNING_PRICE_MULTIPLIER 0.90f

#define USERS_FILE               "data/users.txt"
#define MOVIES_FILE              "data/movies.txt"
#define HALLS_FILE               "data/halls.txt"
#define SHOWTIMES_FILE           "data/showtimes.txt"
#define TICKETS_FILE             "data/tickets.txt"
#define DISCOUNTS_FILE           "data/discounts.txt"

// ========== STRUCTURES ==========
typedef struct{
    int id;
    char username[50];
    char password[50];
    char fullName[100];
    char phone[20];
    int role;
    int isActive;
} User;

typedef struct{
    int id;
    char title[100];
    char genre[50];
    char director[100];
    int duration;
    int ageLimit;
    float rating;
    int isActive;
} Movie;

typedef struct{
    int id;
    char name[50];
    int rows;
    int columns;
    int vipRows;
    char facilities[100];
    int isActive;
} Hall;

typedef struct {
    int id;
    int movieId;
    int hallId;
    char date[11];
    char time[6];
    float ticketPrice;
    int isActive;
} Showtime;

typedef struct {
    int id;
    char code[30];
    float percentage;
    float minimumPrice;
    char startDate[11];
    char endDate[11];
    int usageLimit;
    int timesUsed;
    int isActive;
} Discount;

typedef struct {
    int id;
    int userId;
    int showtimeId;
    int seatNumber;
    char buyerName[100];
    char buyerPhone[20];
    float originalPrice;
    int discountId;
    float finalPrice;
    char purchaseDate[11];
    int isCancelled;
} Ticket;

typedef struct{
    User *users;
    int userCount;
    int userCapacity;
    int currentUserIndex;

    Movie *movies;
    int movieCount;
    int movieCapacity;

    Hall *halls;
    int hallCount;
    int hallCapacity;

    Showtime *showtimes;
    int showtimeCount;
    int showtimeCapacity;

    Discount *discounts;
    int discountCount;
    int discountCapacity;

    Ticket *tickets;
    int ticketCount;
    int ticketCapacity;

} System;

// helper struct for reports
typedef struct {
    int movieId;
    int ticketsSold;
} MovieSales;

// ============== FUNCTION PROTOTYPES ==========
void initializeSystem(System *system);
void freeSystem(System *system);

int ensureUserCapacity(System *system);
int ensureMovieCapacity(System *system);
int ensureHallCapacity(System *system);
int ensureShowtimeCapacity(System *system);
int ensureDiscountCapacity(System *system);
int ensureTicketCapacity(System *system);

void readString(const char *message, char *destination, int maximumLength);
int readIntInRange(const char *message, int minimum, int maximum);
float readFloatInRange(const char *message, float minimum, float maximum);
int isValidDateFormat(const char *date);
int isValidTimeFormat(const char *time);
int isValidPhoneNumber(const char *phone);
void readDate(const char *message, char *destination);
void readTime(const char *message, char *destination);
void readPhoneNumber(const char *message, char *destination, int maximumLength);

long long dateToDayNumber(const char *data);
int timeToMinutes(const char *time);
void addMinutesToTime(const char *time, int minutesToAdd, char *destination);
int countActiveAdmins(const System *system);
int getCurrentDate(char *destination);
const User *getCurrentUser(const System *system);
int movieHasActiveShowtime(const System *system, int movieId);
int countSoldSeats(const System *system, int showtimeId);
int movieHasShowtimes(const System *system, int movieId);
int hallHasShowtimes(const System *system, int hallId);
int showtimeHasTickets(const System *system, int showtimeId);
int discountHasTickets(const System *system, int discountId);

static int createFileIfMissing(const char *fileName);
int createDataFiles(void);
int saveUsers(const System *system);
int loadUsers(System *system);
int saveMovies(const System *system);
int loadMovies(System *system);
int saveHalls(const System *system);
int loadHalls(System *system);
int saveShowtimes(const System *system);
int loadShowtimes(System *system);
int saveDiscounts(const System *system);
int loadDiscounts(System *system);
int saveTickets(const System *system);
int loadTickets(System *system);

void mainMenu(System *system);
void adminMenu(System *system);
void customerMenu(System *system);
void userMenu(System *system);
void movieMenu(System *system);
void hallMenu(System *system);
void showtimeMenu(System *system);
void discountMenu(System *system);
void ticketMenu(System *system);
void salesReportMenu(System *system);

void registerCustomer(System *system);
int loginUser(System *system);
void logoutUser(System *system);
void createFirstAdmin(System *system);

int findUserIndexById(const System *system, int userId);
int findUserIndexByUsername(const System *system, const char *username);
void addUser(System *system);
void listUsers(const System *system);
void searchUser(const System *system);
void displayUserDetails(const User *user);
void editUser(System *system);
void deactivateUser(System *system);
void activateUser(System *system);

int findMovieIndexById(const System *system, int movieId);
void addMovie(System *system);
void listMovies(const System *system);
void searchMovie(const System *system);
int containsIgnoreCase(const char *text, const char *searchText);
void displayMovieDetails(const Movie *movie);
void editMovie(System *system);
void deactivateMovie(System *system);
void activateMovie(System *system);

int findHallIndexById(const System *system, int hallId);
void addHall(System *system);
void listHalls(const System *system);
void searchHall(const System *system);
void displayHallDetails(const Hall *hall);
void editHall(System *system);
void deactivateHall(System *system);
void activateHall(System *system);

int findShowtimeIndexById(const System *system, int showtimeId);
void addShowtime(System *system);
void listShowtimes(const System *system);
void searchShowtime(const System *system);
void displayShowtimeDetails(const System *system, const Showtime *showtime);
void editShowtime(System *system);
void deactivateShowtime(System *system);
void activateShowtime(System *system);
long long showtimeStartInMinutes(const Showtime *showtime);
long long showtimeEndInMinutes(const System *system, const Showtime *showtime);
int findShowtimeConflict(const System *system, int hallId, int movieId, const char *date, const char *time, int ignoredShowtimeId);
void displayHallScheduleForDate(const System *system, int hallId, const char *date, int excludedShowtimeId);
void viewHallSchedule(System *system);
void displayMovieScheduleForDate(const System *system, int movieId, const char *date, int excludeShowtimeId);
void viewMovieSchedule(System *system);

int findDiscountIndexById(const System *system, int discountId);
int findDiscountIndexByCode(const System *system, const char *code);
void addDiscount(System *system);
void listDiscounts(const System *system);
void searchDiscount(const System *system);
void displayDiscountDetails(const Discount *discount);
void editDiscount(System *system);
void deactivateDiscount(System *system);
void activateDiscount(System *system);

int findTicketIndexById(const System *system, int ticketId);
void displayTicketDetails(const System *system, const Ticket *ticket);
int isSeatSold(const System *system, int showtimeId, int seatNumber);
void displaySeatMap(const System *system, int showtimeId);
void listTickets(const System *system);
void searchTicket(const System *system);
void purchaseTicket(System *system);
void cancelTicket(System *system);

void reportTotalSales(const System *system);
void reportMovieIncome(const System *system);
void reportShowtimeIncome(const System *system);
static int compareMovieSales(const void *a, const void *b);
void reportBestSellingMovies(const System *system);
void reportOccupancy(const System *system);
void exportShowtimeBuyers(const System *system);

void listCurrentlyShowingMovies(const System *system);
void searchCurrentlyShowingMovies(const System *system);
void viewActiveShowtimesForMovie(const System *system);
void listAvailableDiscounts(const System *system);
void listMyTickets(const System *system);
void cancelMyTicket(System *system);

// ============= IMPLEMENTATIONS ==========
// ========== System Initialization And Cleanup ==========
void initializeSystem(System *system)
{
    system->users = NULL;
    system->userCount = 0;
    system->userCapacity = 0;
    system->currentUserIndex = -1;

    system->movies = NULL;
    system->movieCount = 0;
    system->movieCapacity = 0;

    system->halls = NULL;
    system->hallCount = 0;
    system->hallCapacity = 0;

    system->showtimes = NULL;
    system->showtimeCount = 0;
    system->showtimeCapacity = 0;

    system->discounts = NULL;
    system->discountCount = 0;
    system->discountCapacity = 0;

    system->tickets = NULL;
    system->ticketCount = 0;
    system->ticketCapacity = 0;
}

void freeSystem(System *system)
{
    free(system->users);
    free(system->movies);
    free(system->halls);
    free(system->showtimes);
    free(system->discounts);
    free(system->tickets);
    
    initializeSystem(system);
}

// ========== Memory Management ==========
int ensureUserCapacity(System *system)
{
    if (system->userCount < system->userCapacity) {
        return 1;
    }

    if (system->userCapacity >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return 0;
    }

    int newCapacity;

    if (system->userCapacity == 0) {
        newCapacity = INITIAL_CAPACITY;
    }
    else {
        newCapacity = system->userCapacity * 2;
    }

    if (newCapacity > MAX_USERS) {
        newCapacity = MAX_USERS;
    }

    User *temporary = realloc(
        system->users,
        newCapacity * sizeof(User)
    );

    if (temporary == NULL) {
        printf("User memory allocation failed.\n");
        return 0;
    }

    system->users = temporary;
    system->userCapacity = newCapacity;

    return 1;
}

int ensureMovieCapacity(System *system)
{
    if (system->movieCount < system->movieCapacity) {
        return 1;
    }

    if (system->movieCapacity >= MAX_MOVIES) {
        printf("Maximum number of movies reached.\n");
        return 0;
    }

    int newCapacity;
    
    if (system->movieCapacity == 0) {
        newCapacity = INITIAL_CAPACITY;
    }
    else {
        newCapacity = system->movieCapacity * 2;

    }
    
    if (newCapacity > MAX_MOVIES) {
            newCapacity = MAX_MOVIES;
        }
    
        Movie *temporary = realloc(
            system->movies,
            newCapacity * sizeof(Movie)
        );
    
        if (temporary == NULL) {
            printf("Memory allocation failed.\n");
            return 0;
        }
    
        system->movies = temporary;
        system->movieCapacity = newCapacity;
    
        return 1;
}

int ensureHallCapacity(System *system)
{
    if (system->hallCount < system->hallCapacity) {
        return 1;
    }

    if (system->hallCapacity >= MAX_HALLS) {
        printf("Maximum number of halls reached.\n");
        return 0;
    }

    int newCapacity;

    if (system->hallCapacity == 0) {
        newCapacity = INITIAL_CAPACITY;
    }
    else {
        newCapacity = system->hallCapacity * 2;
    }

    if (newCapacity > MAX_HALLS) {
        newCapacity = MAX_HALLS;
    }

    Hall *temporary = realloc(
        system->halls,
        newCapacity * sizeof(Hall)
    );

    if (temporary == NULL) {
        printf("Hall memory allocation failed.\n");
        return 0;
    }

    system->halls = temporary;
    system->hallCapacity = newCapacity;

    return 1;
}

int ensureShowtimeCapacity(System *system)
{
    if (system->showtimeCount < system->showtimeCapacity) {
        return 1;
    }

    if (system->showtimeCapacity >= MAX_SHOWTIMES) {
        printf("Maximum number of showtimes reached.\n");
        return 0;
    }

    int newCapacity;

    if (system->showtimeCapacity == 0) {
        newCapacity = INITIAL_CAPACITY;
    }
    else {
        newCapacity = system->showtimeCapacity * 2;
    }

    if (newCapacity > MAX_SHOWTIMES) {
        newCapacity = MAX_SHOWTIMES;
    }

    Showtime *temporary = realloc(
        system->showtimes,
        newCapacity * sizeof(Showtime)
    );

    if (temporary == NULL) {
        printf("Showtime memory allocation failed.\n");
        return 0;
    }

    system->showtimes = temporary;
    system->showtimeCapacity = newCapacity;

    return 1;
}

int ensureDiscountCapacity(System *system)
{
    if (system->discountCount < system->discountCapacity) {
        return 1;
    }

    if (system->discountCapacity >= MAX_DISCOUNTS) {
        printf("Maximum number of discounts reached.\n");
        return 0;
    }

    int newCapacity;
    
    if (system->discountCapacity == 0) {
        newCapacity = INITIAL_CAPACITY;
    }
    else {
        newCapacity = system->discountCapacity * 2;

    }
    
    if (newCapacity > MAX_DISCOUNTS) {
            newCapacity = MAX_DISCOUNTS;
        }
    
        Discount *temporary = realloc(
            system->discounts,
            newCapacity * sizeof(Discount)
        );
    
        if (temporary == NULL) {
            printf("Discount memory allocation failed.\n");
            return 0;
        }
    
        system->discounts = temporary;
        system->discountCapacity = newCapacity;
    
        return 1;
}

int ensureTicketCapacity(System *system)
{
    if (system->ticketCount < system->ticketCapacity) {
        return 1;
    }

    if (system->ticketCapacity >= MAX_TICKETS) {
        printf("Maximum number of tickets reached.\n");
        return 0;
    }

    int newCapacity;

    if (system->ticketCapacity == 0) {
        newCapacity = INITIAL_CAPACITY;
    }
    else {
        newCapacity = system->ticketCapacity * 2;
    }

    if (newCapacity > MAX_TICKETS) {
        newCapacity = MAX_TICKETS;
    }

    Ticket *temporary = realloc(
        system->tickets,
        newCapacity * sizeof(Ticket)
    );

    if (temporary == NULL) {
        printf("Ticket memory allocation failed.\n");
        return 0;
    }

    system->tickets = temporary;
    system->ticketCapacity = newCapacity;

    return 1;
}

// ========== Input And Validation Functions ==========
void readString(const char *message, char *destination, int maximumLength)
{
    char input[500];

    while (1) {
        printf("%s", message);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error. Try again.\n");
            clearerr(stdin);
            continue;
        }

        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0') {
            printf("Input cannot be empty.\n");
            continue;
        }

        if ((int)strlen(input) >= maximumLength) {
            printf("Input is too long. maximum length is %d characters.\n", maximumLength - 1);
            continue;
        }

        strcpy(destination, input);

        return;
    }
}

int readIntInRange(const char *message, int minimum, int maximum)
{
    char input[100];
    char extraCharacter;
    int value;

    while (1) {
        printf("%s", message);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error. Try again.\n");
            clearerr(stdin);
            continue;
        }

        if (sscanf(input, "%d %c", &value, &extraCharacter) != 1) {
            printf("Please enter a valid integer.\n");
            continue;
        }

        if (value < minimum || value > maximum) {
            printf("Enter a value between %d and %d.\n", minimum, maximum);
            continue;
        }

        return value;
    }
}

float readFloatInRange(const char *message, float minimum, float maximum)
{
    char input[100];
    char extraCharacter;
    float value;

    while (1) {
        printf("%s", message);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Input error. Try again.\n");
            clearerr(stdin);
            continue;
        }

        if (sscanf(input, "%f %c", &value, &extraCharacter) != 1) {
            printf("Please enter a valid number.\n");
            continue;
        }

        if ( value < minimum || value > maximum) {
            printf("Enter a value between %.1f and %.1f.\n", minimum, maximum);
            continue;
        }

        return value;
    }
}

int isValidDateFormat(const char *date)
{
    int year;
    int month;
    int day;
    char extraCharacter;

    if (strlen(date) != 10) {
        return 0;
    }

    if (date[4] != '-' || date[7] != '-') {
        return 0;
    }

    if (sscanf(
            date,
            "%4d-%2d-%2d%c",
            &year,
            &month,
            &day,
            &extraCharacter
        ) != 3) {
        return 0;
    }

    if (year < 1) {
        return 0;
    }

    if (month < 1 || month > 12) {
        return 0;
    }

    int daysInMonth;

    switch (month) {
        case 4:
        case 6:
        case 9:
        case 11:
            daysInMonth = 30;
            break;
        
        case 2:
            if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) {
                daysInMonth = 29;
            }
            else {
                daysInMonth = 28;
            }
            break;
        
        default:
            daysInMonth = 31;
    }

    if (day < 1 || day > daysInMonth) {
        return 0;
    }

    return 1;
}

int isValidTimeFormat(const char *time)
{
    int hour;
    int minute;
    char extraCharacter;

    if (strlen(time) != 5) {
        return 0;
    }

    if (time[2] != ':') {
        return 0;
    }

    if (sscanf(
            time,
            "%2d:%2d%c",
            &hour,
            &minute,
            &extraCharacter
        ) != 2) {
        return 0;
    }

    if (hour < 0 || hour > 23) {
        return 0;
    }

    if (minute < 0 || minute > 59) {
        return 0;
    }

    return 1;
}

int isValidPhoneNumber(const char *phone)
{
    int length = strlen(phone);

    if (length < 10 || length > 15) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!isdigit((unsigned char)phone[i])) {
            return 0;
        }
    }

    return 1;
}

void readDate(const char *message, char *destination)
{
    char input[100];

    while (1) {
        readString(message, input, sizeof(input));

        if (!isValidDateFormat(input)) {
            printf("Enter the date in YYYY-MM-DD format.\n");
            continue;
        }

        strcpy(destination, input);
        return;
    }
}

void readTime(const char *message, char *destination)
{
    char input[100];

    while (1) {
        readString(message, input, sizeof(input));

        if (!isValidTimeFormat(input)) {
            printf("Enter the time in HH:MM format.\n");
            continue;
        }

        strcpy(destination, input);
        return;
    }
}

void readPhoneNumber(const char *message, char *destination, int maximumLength)
{
    char input[100];

    while (1) {
        readString(
            message,
            input,
            sizeof(input)
        );

        if (!isValidPhoneNumber(input)) {
            printf(
                "Phone number must contain only "
                "10 to 15 digits.\n"
            );
            continue;
        }

        if ((int)strlen(input) >= maximumLength) {
            printf(
                "Phone number is too long.\n"
            );
            continue;
        }

        strcpy(destination, input);
        return;
    }
}

// ========== Helper Functions ==========
long long dateToDayNumber(const char *date)
{
    int year;
    int month;
    int day;

    sscanf(
        date,
        "%d-%d-%d",
        &year,
        &month,
        &day
    );

    static const int daysBeforeMonth[] = {
        0,    // Before January     [0]
        0,    // Before January     [1]
        31,   // Before February    [2]
        59,   // Before March       [3]
        90,   // Before April       [4]
        120,  // Before May         [5]
        151,  // Before June        [6]
        181,  // Before July        [7]
        212,  // Before August      [8]
        243,  // Before September   [9]
        273,  // Before October     [10]
        304,  // Before November    [11]
        334   // Before December    [12]
    };

    long long previousYears = year - 1;

    long long totalDays =
        previousYears * 365
        + previousYears / 4
        - previousYears / 100
        + previousYears / 400;

    totalDays += daysBeforeMonth[month];
    totalDays += day - 1;

    int isLeapYear =
        year % 400 == 0
        || (year % 4 == 0 && year % 100 != 0);

    if (isLeapYear && month > 2) {
        totalDays++;
    }

    return totalDays;
}

int timeToMinutes(const char *time)
{
    int hour;
    int minute;

    sscanf(
        time,
        "%d:%d",
        &hour,
        &minute
    );

    return hour * 60 + minute;
}

void addMinutesToTime(const char *time, int minutesToAdd, char *destination)
{
    int totalMinutes = timeToMinutes(time) + minutesToAdd;

    totalMinutes %= 24 * 60;

    int newHour = totalMinutes / 60;
    int newMinute = totalMinutes % 60;

    sprintf(destination, "%02d:%02d", newHour, newMinute);
}

int countActiveAdmins(const System *system)
{
    int count = 0;

    for (int i = 0; i < system->userCount; i++) {
        const User *user = &system->users[i];

        if (
            user->isActive &&
            user->role == ADMIN_ROLE
        ) {
            count++;
        }
    }

    return count;
}

int getCurrentDate(char *destination)
{
    time_t currentTime = time(NULL);

    if (currentTime == (time_t)-1) {
        return 0;
    }

    struct tm *currentDate = localtime(&currentTime);

    if (currentDate == NULL) {
        return 0;
    }

    sprintf(
        destination,
        "%04d-%02d-%02d",
        currentDate->tm_year + 1900,
        currentDate->tm_mon + 1,
        currentDate->tm_mday
    );

    return 1;
}

const User *getCurrentUser(const System *system)
{
    if (
        system->currentUserIndex < 0 ||
        system->currentUserIndex >= system->userCount
    ) {
        return NULL;
    }

    return &system->users[system->currentUserIndex];
}

int movieHasActiveShowtime(const System *system, int movieId)
{
    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime =
            &system->showtimes[i];

        if (
            showtime->movieId != movieId ||
            !showtime->isActive
        ) {
            continue;
        }

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        if (hallIndex == -1) {
            continue;
        }

        if (!system->halls[hallIndex].isActive) {
            continue;
        }

        return 1;
    }

    return 0;
}

int countSoldSeats(const System *system, int showtimeId)
{
    int soldSeats = 0;

    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *ticket =
            &system->tickets[i];

        if (
            ticket->showtimeId == showtimeId &&
            !ticket->isCancelled
        ) {
            soldSeats++;
        }
    }

    return soldSeats;
}

int movieHasShowtimes(const System *system, int movieId)
{
    for (int i = 0; i < system->showtimeCount; i++) {
        if (system->showtimes[i].movieId == movieId) {
            return 1;
        }
    }

    return 0;
}

int hallHasShowtimes(const System *system, int hallId)
{
    for (int i = 0; i < system->showtimeCount; i++) {
        if (system->showtimes[i].hallId == hallId) {
            return 1;
        }
    }

    return 0;
}

int showtimeHasTickets(const System *system, int showtimeId)
{
    for (int i = 0; i < system->ticketCount; i++) {
        if (
            system->tickets[i].showtimeId ==
            showtimeId
        ) {
            return 1;
        }
    }

    return 0;
}

int discountHasTickets(const System *system, int discountId)
{
    for (int i = 0; i < system->ticketCount; i++) {
        if (
            system->tickets[i].discountId ==
            discountId
        ) {
            return 1;
        }
    }

    return 0;
}

// ========== File Management ==========
static int createFileIfMissing(const char *fileName)
{
    FILE *file = fopen(fileName, "a");

    if (file == NULL) {
        printf("Could not create or open %s.\n", fileName);
        return 0;
    }

    fclose(file);
    return 1;
}

int createDataFiles(void)
{
    int success = 1;

    if (!createFileIfMissing(MOVIES_FILE)) {
        success = 0;
    }

    if (!createFileIfMissing(HALLS_FILE)) {
        success = 0;
    }

    if (!createFileIfMissing(SHOWTIMES_FILE)) {
        success = 0;
    }

    if (!createFileIfMissing(TICKETS_FILE)) {
        success = 0;
    }

    if (!createFileIfMissing(DISCOUNTS_FILE)) {
        success = 0;
    }

    if (!createFileIfMissing(USERS_FILE)) {
        success = 0;
    }

    return success;
}

int saveUsers(const System *system)
{
    FILE *file = fopen(USERS_FILE, "w");

    if (file == NULL) {
        printf(
            "Could not open %s for writing.\n",
            USERS_FILE
        );
        return 0;
    }

    for (int i = 0; i < system->userCount; i++) {
        const User *user = &system->users[i];

        if (
            fprintf(
                file,
                "%d|%s|%s|%s|%s|%d|%d\n",
                user->id,
                user->username,
                user->password,
                user->fullName,
                user->phone,
                user->role,
                user->isActive
            ) < 0
        ) {
            printf(
                "An error occurred while saving users.\n"
            );

            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        printf(
            "An error occurred while closing %s.\n",
            USERS_FILE
        );
        return 0;
    }

    return 1;
}

int loadUsers(System *system)
{
    FILE *file = fopen(USERS_FILE, "r");

    if (file == NULL) {
        printf(
            "Could not open %s for reading.\n",
            USERS_FILE
        );
        return 0;
    }

    char line[500];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        User user;

        char *token = strtok(line, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        user.id = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        snprintf(
            user.username,
            sizeof(user.username),
            "%s",
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        snprintf(
            user.password,
            sizeof(user.password),
            "%s",
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        snprintf(
            user.fullName,
            sizeof(user.fullName),
            "%s",
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        snprintf(
            user.phone,
            sizeof(user.phone),
            "%s",
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        user.role = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid user record on line %d.\n",
                lineNumber
            );
            continue;
        }

        user.isActive = atoi(token);

        if (user.id < 1) {
            printf(
                "Invalid user ID on line %d.\n",
                lineNumber
            );
            continue;
        }

        if (
            user.role != CUSTOMER_ROLE &&
            user.role != ADMIN_ROLE
        ) {
            printf(
                "Invalid user role on line %d.\n",
                lineNumber
            );
            continue;
        }

        if (
            user.isActive != 0 &&
            user.isActive != 1
        ) {
            printf(
                "Invalid user status on line %d.\n",
                lineNumber
            );
            continue;
        }

        if (
            findUserIndexById(
                system,
                user.id
            ) != -1
        ) {
            printf(
                "Duplicate user ID on line %d "
                "was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (
            findUserIndexByUsername(
                system,
                user.username
            ) != -1
        ) {
            printf(
                "Duplicate username on line %d "
                "was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (!ensureUserCapacity(system)) {
            fclose(file);
            return 0;
        }

        system->users[system->userCount] = user;
        system->userCount++;
    }

    fclose(file);
    return 1;
}

int saveMovies(const System *system)
{
    FILE *file = fopen(MOVIES_FILE, "w");

    if (file == NULL) {
        printf("Could not open %s for writing.\n", MOVIES_FILE);
        return 0;
    }

    for (int i = 0; i < system->movieCount; i++) {
        const Movie *movie = &system->movies[i];

        if (fprintf(
                file,
                "%d|%s|%s|%s|%d|%d|%.2f|%d\n",
                movie->id,
                movie->title,
                movie->genre,
                movie->director,
                movie->duration,
                movie->ageLimit,
                movie->rating,
                movie->isActive
            ) < 0) {

            printf("An error occurred while saving movies.\n");
            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        printf("An error occurred while closing %s.\n", MOVIES_FILE);
        return 0;
    }

    return 1;
}

int loadMovies(System *system)
{
    FILE *file = fopen(MOVIES_FILE, "r");

    if (file == NULL) {
        printf("Could not open %s for reading.\n", MOVIES_FILE);
        return 0;
    }

    char line[500];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        Movie movie;

        char *token = strtok(line, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        movie.id = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(movie.title, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(movie.genre, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(movie.director, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        movie.duration = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        movie.ageLimit = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        movie.rating = strtof(token, NULL);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid movie record on line %d.\n", lineNumber);
            continue;
        }

        movie.isActive = atoi(token);

        if (findMovieIndexById(system, movie.id) != -1) {
            printf(
                "Duplicate movie ID on line %d was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (!ensureMovieCapacity(system)) {
            fclose(file);
            return 0;
        }

        system->movies[system->movieCount] = movie;
        system->movieCount++;
    }

    fclose(file);
    return 1;
}

int saveHalls(const System *system)
{
    FILE *file = fopen(HALLS_FILE, "w");

    if (file == NULL) {
        printf("Could not open %s for writing.\n", HALLS_FILE);
        return 0;
    }

    for (int i = 0; i < system->hallCount; i++) {
        const Hall *hall = &system->halls[i];

        if (fprintf(
                file,
                "%d|%s|%d|%d|%d|%s|%d\n",
                hall->id,
                hall->name,
                hall->rows,
                hall->columns,
                hall->vipRows,
                hall->facilities,
                hall->isActive
            ) < 0) {

            printf("An error occurred while saving halls.\n");
            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        printf("An error occurred while closing %s.\n", HALLS_FILE);
        return 0;
    }

    return 1;
}

int loadHalls(System *system)
{
    FILE *file = fopen(HALLS_FILE, "r");

    if (file == NULL) {
        printf("Could not open %s for reading.\n", HALLS_FILE);
        return 0;
    }

    char line[300];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        Hall hall;
        char *token = strtok(line, "|");

        if (token == NULL) {
            printf("Invalid hall record on line %d.\n", lineNumber);
            continue;
        }

        hall.id = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid hall record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(hall.name, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid hall record on line %d.\n", lineNumber);
            continue;
        }

        hall.rows = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid hall record on line %d.\n", lineNumber);
            continue;
        }

        hall.columns = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid hall record on line %d.\n",
                lineNumber
            );
            continue;
        }

        hall.vipRows = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid hall record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(hall.facilities, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid hall record on line %d.\n", lineNumber);
            continue;
        }

        hall.isActive = atoi(token);

        if (findHallIndexById(system, hall.id) != -1) {
            printf(
                "Duplicate hall ID on line %d was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (hall.id < 1) {
            printf(
                "Invalid hall ID on line %d was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (
            hall.rows < 1 ||
            hall.rows > 100 ||
            hall.columns < 1 ||
            hall.columns > 100 ||
            hall.vipRows < 0 ||
            hall.vipRows > hall.rows
        ) {
            printf(
                "Invalid hall dimensions on line %d "
                "were ignored.\n",
                lineNumber
            );
            continue;
        }

        if (
            hall.isActive != 0 &&
            hall.isActive != 1
        ) {
            printf(
                "Invalid hall status on line %d was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (!ensureHallCapacity(system)) {
            fclose(file);
            return 0;
        }

        system->halls[system->hallCount] = hall;
        system->hallCount++;
    }

    fclose(file);
    return 1;
}

int saveShowtimes(const System *system)
{
    FILE *file = fopen(SHOWTIMES_FILE, "w");

    if (file == NULL) {
        printf(
            "Could not open %s for writing.\n",
            SHOWTIMES_FILE
        );
        return 0;
    }

    for (
        int i = 0;
        i < system->showtimeCount;
        i++
    ) {
        const Showtime *showtime =
            &system->showtimes[i];

        if (fprintf(
                file,
                "%d|%d|%d|%s|%s|%.2f|%d\n",
                showtime->id,
                showtime->movieId,
                showtime->hallId,
                showtime->date,
                showtime->time,
                showtime->ticketPrice,
                showtime->isActive
            ) < 0) {

            printf(
                "An error occurred while saving "
                "showtimes.\n"
            );

            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        printf(
            "An error occurred while closing %s.\n",
            SHOWTIMES_FILE
        );
        return 0;
    }

    return 1;
}

int loadShowtimes(System *system)
{
    FILE *file = fopen(SHOWTIMES_FILE, "r");

    if (file == NULL) {
        printf(
            "Could not open %s for reading.\n",
            SHOWTIMES_FILE
        );
        return 0;
    }

    char line[300];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        Showtime showtime;
        char *token = strtok(line, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        showtime.id = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        showtime.movieId = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        showtime.hallId = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        strcpy(showtime.date, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        strcpy(showtime.time, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        showtime.ticketPrice = strtof(token, NULL);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid showtime record on line %d.\n",
                lineNumber
            );
            continue;
        }

        showtime.isActive = atoi(token);

        if (findMovieIndexById(system, showtime.movieId) == -1) {
            printf("Invalid movie ID on line %d.\n", lineNumber);
            continue;
        }

        if (findHallIndexById(system, showtime.hallId) == -1) {
            printf("Invalid hall ID on line %d.\n", lineNumber);
            continue;
        }

        if (findShowtimeIndexById(system, showtime.id) != -1) {
            printf(
                "Duplicate showtime ID on line %d "
                "was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (!ensureShowtimeCapacity(system)) {
            fclose(file);
            return 0;
        }

        system->showtimes[system->showtimeCount] = showtime;

        system->showtimeCount++;
    }

    fclose(file);
    return 1;
}

int saveDiscounts(const System *system)
{
    FILE *file = fopen(DISCOUNTS_FILE, "w");

    if (file == NULL) {
        printf("Could not open %s for writing.\n", DISCOUNTS_FILE);
        return 0;
    }

    for (int i = 0; i < system->discountCount; i++) {
        const Discount *discount = &system->discounts[i];

        if (fprintf(
                file,
                "%d|%s|%.2f|%.2f|%s|%s|%d|%d|%d\n",
                discount->id,
                discount->code,
                discount->percentage,
                discount->minimumPrice,
                discount->startDate,
                discount->endDate,
                discount->usageLimit,
                discount->timesUsed,
                discount->isActive
            ) < 0) {

            printf("An error occurred while saving discounts.\n");
            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        printf("An error occurred while closing %s.\n", DISCOUNTS_FILE);
        return 0;
    }

    return 1;
}

int loadDiscounts(System *system)
{
    FILE *file = fopen(DISCOUNTS_FILE, "r");

    if (file == NULL) {
        printf("Could not open %s for reading.\n", DISCOUNTS_FILE);
        return 0;
    }

    char line[500];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        Discount discount;

        char *token = strtok(line, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        discount.id = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(discount.code, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        discount.percentage = strtof(token, NULL);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid discount record on line %d.\n",
                lineNumber
            );
            continue;
        }

        discount.minimumPrice = strtof(
            token,
            NULL
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(discount.startDate, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        strcpy(discount.endDate, token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        if (!isValidDateFormat(discount.startDate) ||
            !isValidDateFormat(discount.endDate)) {
            printf("Invalid discount dates on line %d.\n", lineNumber);
            continue;
        }

        discount.usageLimit = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        discount.timesUsed = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf("Invalid discount record on line %d.\n", lineNumber);
            continue;
        }

        discount.isActive = atoi(token);

        if (findDiscountIndexById(system, discount.id) != -1) {
            printf(
                "Duplicate discount ID on line %d was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (findDiscountIndexByCode(system, discount.code) != -1) {
            printf(
                "Duplicate discount code on line %d was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (dateToDayNumber(discount.endDate) <
            dateToDayNumber(discount.startDate)) {
            printf("Invalid discount date range on line %d was ignored.\n", lineNumber);
            continue;
        }

        if (discount.percentage <= 0.0f ||
            discount.percentage > 100.0f) {
            printf("Invalid discount percentage on line %d was ignored.\n", lineNumber);
            continue;
        }

        if (discount.minimumPrice < 0.0f) {
            printf(
                "Invalid minimum price on line %d "
                "was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (discount.usageLimit < 1 ||
            discount.timesUsed < 0 ||
            discount.timesUsed > discount.usageLimit) {
            printf("Invalid discount usage information on line %d was ignored.\n", lineNumber);
            continue;
        }

        if (discount.isActive != 0 &&
            discount.isActive != 1) {
            printf("Invalid discount status on line %d was ignored.\n", lineNumber);
            continue;
        }

        if (!ensureDiscountCapacity(system)) {
            fclose(file);
            return 0;
        }

        system->discounts[system->discountCount] = discount;
        system->discountCount++;
    }

    fclose(file);
    return 1;
}

int saveTickets(const System *system)
{
    FILE *file = fopen(TICKETS_FILE, "w");

    if (file == NULL) {
        printf(
            "Could not open %s for writing.\n",
            TICKETS_FILE
        );
        return 0;
    }

    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *ticket =
            &system->tickets[i];

        if (
            fprintf(
                file,
                "%d|%d|%d|%d|%s|%s|%.2f|%d|%.2f|%s|%d\n",
                ticket->id,
                ticket->userId,
                ticket->showtimeId,
                ticket->seatNumber,
                ticket->buyerName,
                ticket->buyerPhone,
                ticket->originalPrice,
                ticket->discountId,
                ticket->finalPrice,
                ticket->purchaseDate,
                ticket->isCancelled
            ) < 0
        ) {
            printf(
                "An error occurred while saving tickets.\n"
            );

            fclose(file);
            return 0;
        }
    }

    if (fclose(file) != 0) {
        printf(
            "An error occurred while closing %s.\n",
            TICKETS_FILE
        );
        return 0;
    }

    return 1;
}

int loadTickets(System *system)
{
    FILE *file = fopen(TICKETS_FILE, "r");

    if (file == NULL) {
        printf(
            "Could not open %s for reading.\n",
            TICKETS_FILE
        );
        return 0;
    }

    char line[700];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        Ticket ticket;

        char *token = strtok(line, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.id = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.userId = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.showtimeId = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.seatNumber = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        strcpy(
            ticket.buyerName,
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        strcpy(
            ticket.buyerPhone,
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.originalPrice = strtof(
            token,
            NULL
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.discountId = atoi(token);

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.finalPrice = strtof(
            token,
            NULL
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        strcpy(
            ticket.purchaseDate,
            token
        );

        token = strtok(NULL, "|");

        if (token == NULL) {
            printf(
                "Invalid ticket record on line %d.\n",
                lineNumber
            );
            continue;
        }

        ticket.isCancelled = atoi(token);

        if (
            findTicketIndexById(
                system,
                ticket.id
            ) != -1
        ) {
            printf(
                "Duplicate ticket ID on line %d "
                "was ignored.\n",
                lineNumber
            );
            continue;
        }

        if (
            findUserIndexById(
                system,
                ticket.userId
            ) == -1
        ) {
            printf(
                "Invalid user ID on ticket line %d.\n",
                lineNumber
            );
            continue;
        }

        if (
            findShowtimeIndexById(
                system,
                ticket.showtimeId
            ) == -1
        ) {
            printf(
                "Invalid showtime ID on ticket line %d.\n",
                lineNumber
            );
            continue;
        }

        if (
            ticket.discountId != NO_DISCOUNT_ID &&
            findDiscountIndexById(
                system,
                ticket.discountId
            ) == -1
        ) {
            printf(
                "Invalid discount ID on ticket line %d.\n",
                lineNumber
            );
            continue;
        }

        if (!ensureTicketCapacity(system)) {
            fclose(file);
            return 0;
        }

        system->tickets[system->ticketCount] =
            ticket;

        system->ticketCount++;
    }

    fclose(file);
    return 1;
}

// ========== Menus ==========
void mainMenu(System *system)
{
    int choice;

    while (1) {
        if (countActiveAdmins(system) == 0) {
            createFirstAdmin(system);

            if (countActiveAdmins(system) == 0) {
                printf(
                    "An administrator account is required "
                    "to continue.\n"
                );
                continue;
            }
        }

        printf(
            "\n========== HKCinema Management System ==========\n"
        );

        printf("1. Login\n");
        printf("2. Register as Customer\n");
        printf("0. Exit\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            2
        );

        switch (choice) {
            case 1:
                if (!loginUser(system)) {
                    break;
                }

                if (
                    system->users[
                        system->currentUserIndex
                    ].role == ADMIN_ROLE
                ) {
                    adminMenu(system);
                }
                else {
                    customerMenu(system);
                }

                system->currentUserIndex = -1;
                break;

            case 2:
                registerCustomer(system);
                break;

            case 0:
                printf("Closing program...\n");
                return;
        }
    }
}

void adminMenu(System *system)
{
    int choice;

    while (1) {
        printf("\n========== Admin Panel ==========\n");
        printf("1. User Management\n");
        printf("2. Movie Management\n");
        printf("3. Hall Management\n");
        printf("4. Showtime Management\n");
        printf("5. Discount Management\n");
        printf("6. Ticket Management\n");
        printf("7. Sales Reports\n");
        printf("8. Show Memory Status\n");
        printf("0. Logout\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            8
        );

        switch (choice) {
            case 1:
                userMenu(system);
                break;

            case 2:
                movieMenu(system);
                break;

            case 3:
                hallMenu(system);
                break;

            case 4:
                showtimeMenu(system);
                break;

            case 5:
                discountMenu(system);
                break;

            case 6:
                ticketMenu(system);
                break;

            case 7:
                salesReportMenu(system);
                break;

            case 8:
                printf("\n========== Memory State ==========\n");
                printf("\nUser count: %d\n", system->userCount);
                printf("User capacity: %d\n", system->userCapacity);
                
                printf("\nMovie count: %d\n", system->movieCount);
                printf("Movie capacity: %d\n", system->movieCapacity);
                
                printf("\nHall count: %d\n", system->hallCount);
                printf("Hall capacity: %d\n", system->hallCapacity);
                
                printf("\nShowtime count: %d\n", system->showtimeCount);
                printf("Showtime capacity: %d\n", system->showtimeCapacity);

                printf("\nDiscount count: %d\n", system->discountCount);
                printf("Discount capacity: %d\n", system->discountCapacity);

                printf("\nTicket count: %d\n", system->ticketCount);
                printf("Ticket capacity: %d\n", system->ticketCapacity);
                break;

            case 0:
                logoutUser(system);
                return;
        }
    }
}

void customerMenu(System *system)
{
    int choice;

    while (1) {
        const User *currentUser =
            getCurrentUser(system);

        if (currentUser == NULL) {
            return;
        }

        printf(
            "\n========== Customer Panel ==========\n"
        );

        printf(
            "Logged in as: %s\n",
            currentUser->fullName
        );

        printf("1. View Currently Showing Movies\n");
        printf("2. Search Currently Showing Movies\n");
        printf("3. View Showtimes of a Movie\n");
        printf("4. View Available Discounts\n");
        printf("5. Purchase Ticket\n");
        printf("6. View My Tickets\n");
        printf("7. Cancel My Ticket\n");
        printf("0. Logout\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            7
        );

        switch (choice) {
            case 1:
                listCurrentlyShowingMovies(system);
                break;

            case 2:
                searchCurrentlyShowingMovies(system);
                break;

            case 3:
                viewActiveShowtimesForMovie(system);
                break;

            case 4:
                listAvailableDiscounts(system);
                break;

            case 5:
                purchaseTicket(system);
                break;

            case 6:
                listMyTickets(system);
                break;

            case 7:
                cancelMyTicket(system);
                break;

            case 0:
                logoutUser(system);
                return;
        }
    }
}

void userMenu(System *system)
{
    int choice;

    while (1) {
        printf(
            "\n========== User Management ==========\n"
        );

        printf("1. Add User\n");
        printf("2. List Users\n");
        printf("3. Search User\n");
        printf("4. Edit User\n");
        printf("5. Deactivate User\n");
        printf("6. Activate User\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            6
        );

        switch (choice) {
            case 1:
                addUser(system);
                break;

            case 2:
                listUsers(system);
                break;

            case 3:
                searchUser(system);
                break;

            case 4:
                editUser(system);
                break;

            case 5:
                deactivateUser(system);
                break;

            case 6:
                activateUser(system);
                break;

            case 0:
                return;
        }
    }
}

void movieMenu(System *system)
{
    int choice;

    while (1) {
        printf("\n========== Movie Management ==========\n");
        printf("1. Add Movie\n");
        printf("2. List Movies\n");
        printf("3. Search Movie\n");
        printf("4. Edit Movie\n");
        printf("5. Deactivate Movie\n");
        printf("6. Activate Movie\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            6
        );

        switch (choice) {
            case 1:
                addMovie(system);
                break;

            case 2:
                listMovies(system);
                break;

            case 3:
                searchMovie(system);
                break;

            case 4:
                editMovie(system);
                break;

            case 5:
                deactivateMovie(system);
                break;

            case 6:
                activateMovie(system);
                break;

            case 0:
                return;
        }
    }
}

void hallMenu(System *system)
{
    int choice;

    while (1) {
        printf("\n========== Hall Management ==========\n");
        printf("1. Add Hall\n");
        printf("2. List Halls\n");
        printf("3. Search Hall\n");
        printf("4. Edit Hall\n");
        printf("5. Deactivate Hall\n");
        printf("6. Activate Hall\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            6
        );

        switch (choice) {
            case 1:
                addHall(system);
                break;

            case 2:
                listHalls(system);
                break;

            case 3:
                searchHall(system);
                break;

            case 4:
                editHall(system);
                break;

            case 5:
                deactivateHall(system);
                break;

            case 6:
                activateHall(system);
                break;

            case 0:
                return;
        }
    }
}

void showtimeMenu(System *system)
{
    int choice;

    while (1) {
        printf(
            "\n========== Showtime Management ==========\n"
        );
        printf("1. Add Showtime\n");
        printf("2. List Showtimes\n");
        printf("3. Search Showtime\n");
        printf("4. Edit Showtime\n");
        printf("5. Deactivate Showtime\n");
        printf("6. Activate Showtime\n");
        printf("7. View Hall Schedule\n");
        printf("8. View Movie Schedule\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            8
        );

        switch (choice) {
            case 1:
                addShowtime(system);
                break;

            case 2:
                listShowtimes(system);
                break;
            
            case 3:
                searchShowtime(system);
                break;

            case 4:
                editShowtime(system);
                break;

            case 5:
                deactivateShowtime(system);
                break;

            case 6:
                activateShowtime(system);
                break;

            case 7:
                viewHallSchedule(system);
                break;

            case 8:
                viewMovieSchedule(system);
                break;

            case 0:
                return;
        }
    }
}

void discountMenu(System *system)
{
    int choice;

    while (1) {
        printf("\n========== Discount Management ==========\n");
        printf("1. Add Discount\n");
        printf("2. List Discounts\n");
        printf("3. Search Discount\n");
        printf("4. Edit Discount\n");
        printf("5. Deactivate Discount\n");
        printf("6. Activate Discount\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            6
        );

        switch (choice) {
            case 1:
                addDiscount(system);
                break;

            case 2:
                listDiscounts(system);
                break;

            case 3:
                searchDiscount(system);
                break;

            case 4:
                editDiscount(system);
                break;

            case 5:
                deactivateDiscount(system);
                break;

            case 6:
                activateDiscount(system);
                break;

            case 0:
                return;
        }
    }
}

void ticketMenu(System *system)
{
    int choice;

    while (1) {
        printf("\n========== Ticket Management ==========\n");
        printf("1. Purchase Ticket\n");
        printf("2. List Tickets\n");
        printf("3. Search Ticket\n");
        printf("4. View Seat Map\n");
        printf("5. Cancel Ticket\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            5
        );

        switch (choice) {
            case 1:
                purchaseTicket(system);
                break;

            case 2:
                listTickets(system);
                break;

            case 3:
                searchTicket(system);
                break;

            case 4: {
                int showtimeId = readIntInRange(
                    "Enter showtime ID: ",
                    1,
                    1000000
                );

                displaySeatMap(
                    system,
                    showtimeId
                );

                break;
            }

            case 5:
                cancelTicket(system);
                break;

            case 0:
                return;
        }
    }
}

void salesReportMenu(System *system)
{
    int choice;
    while (1) {
        printf("\n========== Sales Reports ==========\n");
        printf("1. Total Sales Summary\n");
        printf("2. Income of a Specific Movie\n");
        printf("3. Income of a Specific Showtime\n");
        printf("4. Best-Selling Movies\n");
        printf("5. Showtime Occupancy Report\n");
        printf("6. Export Showtime Buyers\n");
        printf("0. Back\n");
        
        choice = readIntInRange(
            "Enter your choice: ",
            0,
            6
        );

        switch (choice) {
            case 1:
                reportTotalSales(system); break;
            
            case 2:
                reportMovieIncome(system); break;
            
            case 3:
                reportShowtimeIncome(system); break;
            
            case 4:
                reportBestSellingMovies(system); break;
            
            case 5:
                reportOccupancy(system); break;
            
            case 6:
                exportShowtimeBuyers(system); break;
            
            case 0:
                return;
        }
    }
}

// ========== Register and Login ==========
void registerCustomer(System *system)
{
    User newUser;

    printf(
        "\n========== Customer Registration ==========\n"
    );

    newUser.id = readIntInRange(
        "User ID: ",
        1,
        1000000
    );

    if (
        findUserIndexById(
            system,
            newUser.id
        ) != -1
    ) {
        printf("A user with this ID already exists.\n");
        return;
    }

    readString(
        "Username: ",
        newUser.username,
        sizeof(newUser.username)
    );

    if (
        findUserIndexByUsername(
            system,
            newUser.username
        ) != -1
    ) {
        printf(
            "A user with this username already exists.\n"
        );
        return;
    }

    readString(
        "Password: ",
        newUser.password,
        sizeof(newUser.password)
    );

    readString(
        "Full name: ",
        newUser.fullName,
        sizeof(newUser.fullName)
    );

    readPhoneNumber(
        "Phone number: ",
        newUser.phone,
        sizeof(newUser.phone)
    );

    newUser.role = CUSTOMER_ROLE;
    newUser.isActive = 1;

    if (!ensureUserCapacity(system)) {
        return;
    }

    system->users[system->userCount] = newUser;
    system->userCount++;

    if (!saveUsers(system)) {
        printf(
            "Registration succeeded in memory, "
            "but saving failed.\n"
        );
        return;
    }

    printf(
        "Registration completed successfully. "
        "You can now log in.\n"
    );
}

int loginUser(System *system)
{
    char username[50];
    char password[50];

    printf("\n========== Login ==========\n");

    readString(
        "Username: ",
        username,
        sizeof(username)
    );

    readString(
        "Password: ",
        password,
        sizeof(password)
    );

    int index = findUserIndexByUsername(
        system,
        username
    );

    if (index == -1) {
        printf("Invalid username or password.\n");
        return 0;
    }

    User *user = &system->users[index];

    if (strcmp(user->password, password) != 0) {
        printf("Invalid username or password.\n");
        return 0;
    }

    if (!user->isActive) {
        printf("This account is inactive.\n");
        return 0;
    }

    system->currentUserIndex = index;

    printf(
        "\nWelcome, %s.\n",
        user->fullName
    );

    return 1;
}

void logoutUser(System *system)
{
    system->currentUserIndex = -1;
    printf("You have logged out successfully.\n");
}

void createFirstAdmin(System *system)
{
    User newAdmin;

    printf(
        "\nNo active administrator exists.\n"
        "Please create the first administrator account.\n"
    );

    newAdmin.id = readIntInRange(
        "Admin user ID: ",
        1,
        1000000
    );

    if (
        findUserIndexById(
            system,
            newAdmin.id
        ) != -1
    ) {
        printf("A user with this ID already exists.\n");
        return;
    }

    readString(
        "Admin username: ",
        newAdmin.username,
        sizeof(newAdmin.username)
    );

    if (
        findUserIndexByUsername(
            system,
            newAdmin.username
        ) != -1
    ) {
        printf(
            "A user with this username already exists.\n"
        );
        return;
    }

    readString(
        "Admin password: ",
        newAdmin.password,
        sizeof(newAdmin.password)
    );

    readString(
        "Admin full name: ",
        newAdmin.fullName,
        sizeof(newAdmin.fullName)
    );

    readPhoneNumber(
        "Admin phone number: ",
        newAdmin.phone,
        sizeof(newAdmin.phone)
    );

    newAdmin.role = ADMIN_ROLE;
    newAdmin.isActive = 1;

    if (!ensureUserCapacity(system)) {
        return;
    }

    system->users[system->userCount] = newAdmin;
    system->userCount++;

    if (!saveUsers(system)) {
        printf(
            "Administrator was created in memory, "
            "but saving failed.\n"
        );
        return;
    }

    printf(
        "Administrator account created successfully.\n"
    );
}

// ========== User Management ==========
int findUserIndexById(const System *system, int userId)
{
    for (int i = 0; i < system->userCount; i++) {
        if (system->users[i].id == userId) {
            return i;
        }
    }

    return -1;
}

int findUserIndexByUsername(const System *system, const char *username)
{
    for (int i = 0; i < system->userCount; i++) {
        if (
            strcmp(
                system->users[i].username,
                username
            ) == 0
        ) {
            return i;
        }
    }

    return -1;
}

void addUser(System *system)
{
    User newUser;

    printf("\n========== Add User ==========\n");

    newUser.id = readIntInRange(
        "User ID: ",
        1,
        1000000
    );

    if (
        findUserIndexById(
            system,
            newUser.id
        ) != -1
    ) {
        printf("A user with this ID already exists.\n");
        return;
    }

    readString(
        "Username: ",
        newUser.username,
        sizeof(newUser.username)
    );

    if (
        findUserIndexByUsername(
            system,
            newUser.username
        ) != -1
    ) {
        printf("A user with this username already exists.\n");
        return;
    }

    readString(
        "Password: ",
        newUser.password,
        sizeof(newUser.password)
    );

    readString(
        "Full name: ",
        newUser.fullName,
        sizeof(newUser.fullName)
    );

    readPhoneNumber(
        "Phone number: ",
        newUser.phone,
        sizeof(newUser.phone)
    );

    newUser.role = readIntInRange(
        "Role (0 = Customer, 1 = Admin): ",
        CUSTOMER_ROLE,
        ADMIN_ROLE
    );

    newUser.isActive = 1;

    if (!ensureUserCapacity(system)) {
        return;
    }

    system->users[system->userCount] = newUser;
    system->userCount++;

    if (!saveUsers(system)) {
        printf(
            "User was added to memory, "
            "but saving failed.\n"
        );
        return;
    }

    printf("User added and saved successfully.\n");
}

void listUsers(const System *system)
{
    if (system->userCount == 0) {
        printf("\nNo users have been registered.\n");
        return;
    }

    int activeUsers = 0;

    printf("\n========== User List ==========\n");

    for (int i = 0; i < system->userCount; i++) {
        const User *user = &system->users[i];

        activeUsers++;

        displayUserDetails(user);
    }

    if (activeUsers == 0) {
        printf("No active users were found.\n");
    }
}

void searchUser(const System *system)
{
    if (system->userCount == 0) {
        printf("No users have been registered.\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n========== Search User ==========\n");
        printf("1. Search by ID\n");
        printf("2. Search by Username\n");
        printf("3. Search by Full Name\n");
        printf("4. Search by Phone Number\n");
        printf("5. Search by Role\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            5
        );

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int userId = readIntInRange(
                "Enter user ID: ",
                1,
                1000000
            );

            int index = findUserIndexById(
                system,
                userId
            );

            if (index == -1) {
                printf("User not found.\n");
                continue;
            }

            const User *user =
                &system->users[index];

            printf(
                "\n========== Search Result ==========\n"
            );

            displayUserDetails(user);

            continue;
        }

        int found = 0;

        if (choice == 5) {
            int role = readIntInRange(
                "Role (0 = Customer, 1 = Admin): ",
                CUSTOMER_ROLE,
                ADMIN_ROLE
            );

            for (int i = 0; i < system->userCount; i++) {
                const User *user =
                    &system->users[i];

                if (user->role != role) {
                    continue;
                }

                if (!found) {
                    printf(
                        "\n========== Search Results ==========\n"
                    );
                }

                displayUserDetails(user);

                found = 1;
            }
        }
        else {
            char searchText[100];

            if (choice == 2) {
                readString(
                    "Enter all or part of the username: ",
                    searchText,
                    sizeof(searchText)
                );
            }
            else if (choice == 3) {
                readString(
                    "Enter all or part of the full name: ",
                    searchText,
                    sizeof(searchText)
                );
            }
            else {
                readString(
                    "Enter all or part of the phone number: ",
                    searchText,
                    sizeof(searchText)
                );
            }

            for (int i = 0; i < system->userCount; i++) {
                const User *user =
                    &system->users[i];

                int matches = 0;

                if (choice == 2) {
                    matches = containsIgnoreCase(
                        user->username,
                        searchText
                    );
                }
                else if (choice == 3) {
                    matches = containsIgnoreCase(
                        user->fullName,
                        searchText
                    );
                }
                else {
                    matches = strstr(
                        user->phone,
                        searchText
                    ) != NULL;
                }

                if (!matches) {
                    continue;
                }

                if (!found) {
                    printf(
                        "\n========== Search Results ==========\n"
                    );
                }

                displayUserDetails(user);

                found = 1;
            }
        }

        if (!found) {
            printf("No matching users were found.\n");
        }
    }
}

void displayUserDetails(const User *user)
{
    printf("\nUser ID: %d\n", user->id);
    printf("Username: %s\n", user->username);
    printf("Full Name: %s\n", user->fullName);
    printf("Phone: %s\n", user->phone);

    printf(
        "Role: %s\n",
        user->role == ADMIN_ROLE
            ? "Admin"
            : "Customer"
    );

    printf(
        "Status: %s\n",
        user->isActive
            ? "Active"
            : "Inactive"
    );

    printf("--------------------------------\n");
}

void editUser(System *system)
{
    int userId = readIntInRange(
        "Enter user ID to edit: ",
        1,
        1000000
    );

    int index = findUserIndexById(
        system,
        userId
    );

    if (index == -1) {
        printf("User not found.\n");
        return;
    }

    User *user = &system->users[index];

    printf(
        "\nEditing user: %s\n",
        user->username
    );

    char newUsername[50];

    readString(
        "New username: ",
        newUsername,
        sizeof(newUsername)
    );

    int usernameIndex = findUserIndexByUsername(
        system,
        newUsername
    );

    if (
        usernameIndex != -1 &&
        usernameIndex != index
    ) {
        printf(
            "Another user already uses this username.\n"
        );
        return;
    }

    char newPassword[50];
    char newFullName[100];
    char newPhone[20];

    readString(
        "New password: ",
        newPassword,
        sizeof(newPassword)
    );

    readString(
        "New full name: ",
        newFullName,
        sizeof(newFullName)
    );

    readPhoneNumber(
        "New phone number: ",
        newPhone,
        sizeof(newPhone)
    );

    int newRole = readIntInRange(
        "New role (0 = Customer, 1 = Admin): ",
        CUSTOMER_ROLE,
        ADMIN_ROLE
    );

    strcpy(
        user->username,
        newUsername
    );

    strcpy(
        user->password,
        newPassword
    );

    strcpy(
        user->fullName,
        newFullName
    );

    strcpy(
        user->phone,
        newPhone
    );

    if (user->role == ADMIN_ROLE &&
        newRole == CUSTOMER_ROLE &&
        countActiveAdmins(system) == 1) {
        printf(
            "The last active admin cannot be changed "
            "to a customer.\n"
        );
        return;
    }

    user->role = newRole;

    if (!saveUsers(system)) {
        printf(
            "User was edited, but saving failed.\n"
        );
        return;
    }

    printf("User edited successfully.\n");
}

void deactivateUser(System *system)
{
    int userId = readIntInRange(
        "Enter user ID to deactivate: ",
        1,
        1000000
    );

    int index = findUserIndexById(
        system,
        userId
    );

    if (index == -1) {
        printf("User not found.\n");
        return;
    }

    User *user = &system->users[index];

    if (!user->isActive) {
        printf("This user is already inactive.\n");
        return;
    }

    if (
        user->role == ADMIN_ROLE &&
        countActiveAdmins(system) == 1
    ) {
        printf(
            "The last active admin cannot be deactivated.\n"
        );
        return;
    }

    printf("Username: %s\n", user->username);
    printf("Full Name: %s\n", user->fullName);

    int confirmation = readIntInRange(
        "Are you sure? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (confirmation == 0) {
        printf("Operation cancelled.\n");
        return;
    }

    user->isActive = 0;

    if (!saveUsers(system)) {
        printf(
            "User was deactivated, but saving failed.\n"
        );
        return;
    }

    printf("User deactivated successfully.\n");
}

void activateUser(System *system)
{
    int userId = readIntInRange(
        "Enter user ID to activate: ",
        1,
        1000000
    );

    int index = findUserIndexById(
        system,
        userId
    );

    if (index == -1) {
        printf("User not found.\n");
        return;
    }

    User *user = &system->users[index];

    if (user->isActive) {
        printf("This user is already active.\n");
        return;
    }

    user->isActive = 1;

    if (!saveUsers(system)) {
        printf(
            "User was activated, but saving failed.\n"
        );
        return;
    }

    printf("User activated successfully.\n");
}

// ========== Movie Management ==========
int findMovieIndexById(const System *system, int movieId)
{
    for (int i = 0; i < system->movieCount; i++) {
        if (system->movies[i].id == movieId) {
            return i;
        }
    }
    return -1;
}

void addMovie(System *system)
{
    Movie newMovie;

    printf("\n========== Add Movie ==========\n");

    newMovie.id = readIntInRange("Movie ID: ", 1, 1000000);

    if (findMovieIndexById(system, newMovie.id) != -1) {
        printf("A movie with this ID already exists.\n");
        return;
    }

    readString("Title: ", newMovie.title, sizeof(newMovie.title));

    readString("Genre: ", newMovie.genre, sizeof(newMovie.genre));

    readString("Director: ", newMovie.director, sizeof(newMovie.director));

    newMovie.duration = readIntInRange("Duration in minutes: ", 1, 1000);

    newMovie.ageLimit = readIntInRange("Age limit: ", 0, 21);

    newMovie.rating = readFloatInRange("Rating from 0 to 10: ", 0.0f, 10.0f);

    newMovie.isActive = 1;

    if (!ensureMovieCapacity(system)) {
        return;
    }

    system->movies[system->movieCount] = newMovie;
    system->movieCount++;
    if (!saveMovies(system)) {
        printf("Movie was added to memory, but saving failed.\n");
        return;
    }

    printf("Movie added and saved successfully.\n");
}

void listMovies(const System *system)
{
    if (system->movieCount == 0) {
        printf("\nNo movies have been registered.\n");
        return;
    }

    int activeMovies = 0;

    printf("\n========== Movie List ==========\n");
    for (int i = 0; i < system->movieCount; i++) {
        const Movie *movie = &system->movies[i];

        activeMovies++;

        displayMovieDetails(movie);
    }

    if (activeMovies == 0) {
        printf("No active movies were found.\n");
    }
}

void searchMovie(const System *system)
{
    if (system->movieCount == 0) {
        printf("No movies have been registered.\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n========== Search Movie ==========\n");
        printf("1. Search by ID\n");
        printf("2. Search by Title\n");
        printf("3. Search by Genre\n");
        printf("4. Search by Director\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            4
        );

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int movieId = readIntInRange(
                "Enter movie ID: ",
                1,
                1000000
            );

            int index = findMovieIndexById(
                system,
                movieId
            );

            if (index == -1) {
                printf("Movie not found.\n");
                continue;
            }

            printf(
                "\n========== Search Result ==========\n"
            );

            displayMovieDetails(&system->movies[index]);

            continue;
        }

        char searchText[100];

        if (choice == 2) {
            readString(
                "Enter all or part of the title: ",
                searchText,
                sizeof(searchText)
            );
        }
        else if (choice == 3) {
            readString(
                "Enter all or part of the genre: ",
                searchText,
                sizeof(searchText)
            );
        }
        else {
            readString(
                "Enter all or part of the director's name: ",
                searchText,
                sizeof(searchText)
            );
        }

        int found = 0;

        for (int i = 0; i < system->movieCount; i++) {
            const Movie *movie =
                &system->movies[i];

            int matches = 0;

            if (choice == 2) {
                matches = containsIgnoreCase(
                    movie->title,
                    searchText
                );
            }
            else if (choice == 3) {
                matches = containsIgnoreCase(
                    movie->genre,
                    searchText
                );
            }
            else {
                matches = containsIgnoreCase(
                    movie->director,
                    searchText
                );
            }

            if (!matches) {
                continue;
            }

            if (!found) {
                printf(
                    "\n========== Search Results ==========\n"
                );
            }

            displayMovieDetails(movie);
            found = 1;
        }

        if (!found) {
            printf("No matching movies were found.\n");
        }
    }
}

int containsIgnoreCase(const char *text, const char *searchText)
{
    if (searchText[0] == '\0') {
        return 1;
    }

    for (int i = 0; text[i] != '\0'; i++) {
        int textIndex = i;
        int searchIndex = 0;

        while (
            text[textIndex] != '\0' &&
            searchText[searchIndex] != '\0' &&
            tolower((unsigned char)text[textIndex]) ==
            tolower((unsigned char)searchText[searchIndex])
        ) {
            textIndex++;
            searchIndex++;
        }

        if (searchText[searchIndex] == '\0') {
            return 1;
        }
    }

    return 0;
}

void displayMovieDetails(const Movie *movie)
{
    printf("\nMovie ID: %d\n", movie->id);
    printf("Title: %s\n", movie->title);
    printf("Genre: %s\n", movie->genre);
    printf("Director: %s\n", movie->director);
    printf("Duration: %d minutes\n", movie->duration);
    printf("Age Limit: %d+\n", movie->ageLimit);
    printf("Rating: %.1f / 10\n", movie->rating);

    printf(
        "Status: %s\n",
        movie->isActive
            ? "Active"
            : "Inactive"
    );

    printf("--------------------------------\n");
}

void editMovie(System *system)
{
    int movieId = readIntInRange(
        "Enter movie ID to edit: ",
        1,
        1000000
    );

    int index = findMovieIndexById(system, movieId);

    if (index == -1) {
        printf("Movie not found.\n");
        return;
    }

    if (movieHasShowtimes(system, movieId)) {
        printf(
            "This movie cannot be edited because "
            "it is used by one or more showtimes.\n"
        );
        return;
    }

    Movie *movie = &system->movies[index];

    printf("\nEditing movie: %s\n", movie->title);

    readString(
        "New title: ",
        movie->title,
        sizeof(movie->title)
    );

    readString(
        "New genre: ",
        movie->genre,
        sizeof(movie->genre)
    );

    readString(
        "New director: ",
        movie->director,
        sizeof(movie->director)
    );

    movie->duration = readIntInRange(
        "New duration in minutes: ",
        1,
        1000
    );

    movie->ageLimit = readIntInRange(
        "New age limit: ",
        0,
        21
    );

    movie->rating = readFloatInRange(
        "New rating from 0 to 10: ",
        0.0f,
        10.0f
    );

    if (!saveMovies(system)) {
        printf("Movie was edited, but saving failed.\n");
        return;
    }

    printf("Movie edited successfully.\n");
}

void deactivateMovie(System *system)
{
    int movieId = readIntInRange(
        "Enter movie ID to deactivate: ",
        1,
        1000000
    );

    int index = findMovieIndexById(system, movieId);

    if (index == -1) {
        printf("Movie not found.\n");
        return;
    }

    Movie *movie = &system->movies[index];

    if (!movie->isActive) {
        printf("This movie is already inactive.\n");
        return;
    }

    printf("Movie: %s\n", movie->title);

    int confirmation = readIntInRange(
        "Are you sure? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (confirmation == 0) {
        printf("Operation cancelled.\n");
        return;
    }

    movie->isActive = 0;

    if (!saveMovies(system)) {
        printf("Movie was deactivated, but saving failed.\n");
        return;
    }

    printf("Movie deactivated successfully.\n");
}

void activateMovie(System *system)
{
    int movieId = readIntInRange(
        "Enter movie ID to activate: ",
        1,
        1000000
    );

    int index = findMovieIndexById(system, movieId);

    if (index == -1) {
        printf("Movie not found.\n");
        return;
    }

    Movie *movie = &system->movies[index];

    if (movie->isActive) {
        printf("This movie is already active.\n");
        return;
    }

    movie->isActive = 1;

    if (!saveMovies(system)) {
        printf("Movie was activated, but saving failed.\n");
        return;
    }

    printf("Movie activated successfully.\n");
}

// ========== Hall Management ==========
int findHallIndexById(const System *system, int hallId)
{
    for (int i = 0; i < system->hallCount; i++) {
        if (system->halls[i].id == hallId) {
            return i;
        }
    }

    return -1;
}

void addHall(System *system)
{
    Hall newHall;

    printf("\n========== Add Hall ==========\n");

    newHall.id = readIntInRange(
        "Hall ID: ",
        1,
        1000000
    );

    if (findHallIndexById(system, newHall.id) != -1) {
        printf("A hall with this ID already exists.\n");
        return;
    }

    readString(
        "Hall name: ",
        newHall.name,
        sizeof(newHall.name)
    );

    newHall.rows = readIntInRange(
        "Number of seat rows: ",
        1,
        100
    );

    newHall.columns = readIntInRange(
        "Number of seats per row: ",
        1,
        100
    );

    printf("Total capacity: %d seats\n", newHall.rows * newHall.columns);

    newHall.vipRows = readIntInRange(
        "Number of VIP rows (0 for none): ",
        0,
        newHall.rows
    );

    printf(
        "VIP capacity: %d seats\n",
        newHall.vipRows * newHall.columns
    );

    readString(
        "Facilities (for example VIP, 3D, Dolby): ",
        newHall.facilities,
        sizeof(newHall.facilities)
    );

    newHall.isActive = 1;

    if (!ensureHallCapacity(system)) {
        return;
    }

    system->halls[system->hallCount] = newHall;
    system->hallCount++;

    if (!saveHalls(system)) {
        printf("Hall was added to memory, but saving failed.\n");
        return;
    }

    printf("Hall added and saved successfully.\n");
}

void listHalls(const System *system)
{
    if (system->hallCount == 0) {
        printf("\nNo halls have been registered.\n");
        return;
    }

    int activeHalls = 0;

    printf("\n========== Hall List ==========\n");

    for (int i = 0; i < system->hallCount; i++) {
        const Hall *hall = &system->halls[i];

        activeHalls++;

        displayHallDetails(hall);
    }

    if (activeHalls == 0) {
        printf("No active halls were found.\n");
    }
}

void searchHall(const System *system)
{
    if (system->hallCount == 0) {
        printf("No halls have been registered.\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n========== Search Hall ==========\n");
        printf("1. Search by ID\n");
        printf("2. Search by Name\n");
        printf("3. Search by Facilities\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            3
        );

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int hallId = readIntInRange(
                "Enter hall ID: ",
                1,
                1000000
            );

            int index = findHallIndexById(
                system,
                hallId
            );

            if (index == -1) {
                printf("Hall not found.\n");
                continue;
            }

            printf(
                "\n========== Search Result ==========\n"
            );

            displayHallDetails(
                &system->halls[index]
            );

            continue;
        }

        char searchText[100];

        if (choice == 2) {
            readString(
                "Enter all or part of the hall name: ",
                searchText,
                sizeof(searchText)
            );
        }
        else {
            readString(
                "Enter all or part of the facilities: ",
                searchText,
                sizeof(searchText)
            );
        }

        int found = 0;

        for (int i = 0; i < system->hallCount; i++) {
            const Hall *hall = &system->halls[i];

            int matches;

            if (choice == 2) {
                matches = containsIgnoreCase(
                    hall->name,
                    searchText
                );
            }
            else {
                matches = containsIgnoreCase(
                    hall->facilities,
                    searchText
                );
            }

            if (!matches) {
                continue;
            }

            if (!found) {
                printf(
                    "\n========== Search Results ==========\n"
                );
            }

            displayHallDetails(hall);
            found = 1;
        }

        if (!found) {
            printf("No matching halls were found.\n");
        }
    }
}

void displayHallDetails(const Hall *hall)
{
    printf("\nHall ID: %d\n", hall->id);
    printf("Name: %s\n", hall->name);
    printf("Rows: %d\n", hall->rows);
    printf("Columns: %d\n", hall->columns);
    printf("Capacity: %d seats\n", hall->rows * hall->columns);
    printf("VIP Rows: %d\n", hall->vipRows);
    printf("VIP Seats: %d\n", hall->vipRows * hall->columns);
    printf("Facilities: %s\n", hall->facilities);

    printf(
        "Status: %s\n",
        hall->isActive
            ? "Active"
            : "Inactive"
    );

    printf("--------------------------------\n");
}

void editHall(System *system)
{
    int hallId = readIntInRange(
        "Enter hall ID to edit: ",
        1,
        1000000
    );

    int index = findHallIndexById(
        system,
        hallId
    );

    if (index == -1) {
        printf("Hall not found.\n");
        return;
    }

    if (hallHasShowtimes(system, hallId)) {
        printf(
            "This hall cannot be edited because "
            "it is used by one or more showtimes.\n"
        );
        return;
    }

    Hall *hall = &system->halls[index];

    printf(
        "\nEditing hall: %s\n",
        hall->name
    );

    char newName[50];
    char newFacilities[100];

    readString(
        "New hall name: ",
        newName,
        sizeof(newName)
    );

    int newRows = readIntInRange(
        "New number of seat rows: ",
        1,
        100
    );

    int newColumns = readIntInRange(
        "New number of seats per row: ",
        1,
        100
    );

    int newVipRows = readIntInRange(
        "New number of VIP rows (0 for none): ",
        0,
        newRows
    );

    readString(
        "New facilities: ",
        newFacilities,
        sizeof(newFacilities)
    );

    strcpy(hall->name, newName);
    hall->rows = newRows;
    hall->columns = newColumns;
    hall->vipRows = newVipRows;
    strcpy(hall->facilities, newFacilities);

    if (!saveHalls(system)) {
        printf(
            "Hall was edited, but saving failed.\n"
        );
        return;
    }

    printf(
        "New total capacity: %d seats\n",
        hall->rows * hall->columns
    );

    printf(
        "New VIP capacity: %d seats\n",
        hall->vipRows * hall->columns
    );

    printf("Hall edited successfully.\n");
}

void deactivateHall(System *system)
{
    int hallId = readIntInRange(
        "Enter hall ID to deactivate: ",
        1,
        1000000
    );

    int index = findHallIndexById(system, hallId);

    if (index == -1) {
        printf("Hall not found.\n");
        return;
    }

    Hall *hall = &system->halls[index];

    if (!hall->isActive) {
        printf("This hall is already inactive.\n");
        return;
    }

    printf("Hall: %s\n", hall->name);

    int confirmation = readIntInRange(
        "Are you sure? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (confirmation == 0) {
        printf("Operation cancelled.\n");
        return;
    }

    hall->isActive = 0;

    if (!saveHalls(system)) {
        printf("Hall was deactivated, but saving failed.\n");
        return;
    }

    printf("Hall deactivated successfully.\n");
}

void activateHall(System *system)
{
    int hallId = readIntInRange(
        "Enter hall ID to activate: ",
        1,
        1000000
    );

    int index = findHallIndexById(system, hallId);

    if (index == -1) {
        printf("Hall not found.\n");
        return;
    }

    Hall *hall = &system->halls[index];

    if (hall->isActive) {
        printf("This hall is already active.\n");
        return;
    }

    hall->isActive = 1;

    if (!saveHalls(system)) {
        printf("Hall was activated, but saving failed.\n");
        return;
    }

    printf("Hall activated successfully.\n");
}

// ========== Showtime Management ==========
int findShowtimeIndexById(const System *system, int showtimeId)
{
    for (int i = 0; i < system->showtimeCount; i++) {
        if (system->showtimes[i].id == showtimeId) {
            return i;
        }
    }

    return -1;
}

void addShowtime(System *system)
{
    Showtime newShowtime;

    printf("\n========== Add Showtime ==========\n");

    if (system->movieCount == 0) {
        printf("No movies have been registered.\n");
        return;
    }

    if (system->hallCount == 0) {
        printf("No halls have been registered.\n");
        return;
    }

    newShowtime.id = readIntInRange(
        "Showtime ID: ",
        1,
        1000000
    );

    if (
        findShowtimeIndexById(
            system,
            newShowtime.id
        ) != -1
    ) {
        printf("A showtime with this ID already exists.\n");
        return;
    }

    listMovies(system);

    newShowtime.movieId = readIntInRange(
        "Movie ID: ",
        1,
        1000000
    );

    int movieIndex = findMovieIndexById(
        system,
        newShowtime.movieId
    );

    if (movieIndex == -1) {
        printf("Movie not found.\n");
        return;
    }

    if (!system->movies[movieIndex].isActive) {
        printf("The selected movie is inactive.\n");
        return;
    }

    listHalls(system);

    newShowtime.hallId = readIntInRange(
        "Hall ID: ",
        1,
        1000000
    );

    int hallIndex = findHallIndexById(
        system,
        newShowtime.hallId
    );

    if (hallIndex == -1) {
        printf("Hall not found.\n");
        return;
    }

    if (!system->halls[hallIndex].isActive) {
        printf("The selected hall is inactive.\n");
        return;
    }

    readDate(
        "Date (YYYY-MM-DD): ",
        newShowtime.date
    );

    displayHallScheduleForDate(
        system,
        newShowtime.hallId,
        newShowtime.date,
        -1
    );

    readTime(
        "Time (HH:MM): ",
        newShowtime.time
    );

    int conflictIndex = findShowtimeConflict(
        system,
        newShowtime.hallId,
        newShowtime.movieId,
        newShowtime.date,
        newShowtime.time,
        -1
    );

    if (conflictIndex != -1) {
        const Showtime *conflictingShowtime =
            &system->showtimes[conflictIndex];

        int conflictingMovieIndex =
            findMovieIndexById(
                system,
                conflictingShowtime->movieId
            );

        printf(
            "\nThe selected hall is not available "
            "at this time.\n"
        );

        printf(
            "Conflicting showtime ID: %d\n",
            conflictingShowtime->id
        );

        if (conflictingMovieIndex != -1) {
            printf(
                "Movie: %s\n",
                system->movies[conflictingMovieIndex].title
            );
            

            char endingTime[6];

            addMinutesToTime(
                conflictingShowtime->time,
                system->movies[conflictingMovieIndex].duration + HALL_PREPARATION_TIME,
                endingTime
            );

            printf(
                "From %s to %s\n",
                conflictingShowtime->time,
                endingTime
            );
        }
        return;
    }

    newShowtime.ticketPrice = readFloatInRange(
        "Ticket price: ",
        0.0f,
        10000.0f
    );

    newShowtime.isActive = 1;

    if (!ensureShowtimeCapacity(system)) {
        return;
    }

    system->showtimes[system->showtimeCount] = newShowtime;

    system->showtimeCount++;

    if (!saveShowtimes(system)) {
        printf(
            "Showtime was added to memory, "
            "but saving failed.\n"
        );
        return;
    }

    printf("Showtime added and saved successfully.\n");
}

void listShowtimes(const System *system)
{
    if (system->showtimeCount == 0) {
        printf("\nNo showtimes have been registered.\n");
        return;
    }

    int activeShowtimes = 0;

    printf("\n========== Showtime List ==========\n");

    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime = &system->showtimes[i];

        activeShowtimes++;

        displayShowtimeDetails(system, showtime);
    }

    if (activeShowtimes == 0) {
        printf("No active showtimes were found.\n");
    }
}

void searchShowtime(const System *system)
{
    if (system->showtimeCount == 0) {
        printf("No showtimes have been registered.\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n========== Search Showtime ==========\n");
        printf("1. Search by ID\n");
        printf("2. Search by Movie Title\n");
        printf("3. Search by Hall Name\n");
        printf("4. Search by Date\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            4
        );

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int showtimeId = readIntInRange(
                "Enter showtime ID: ",
                1,
                1000000
            );

            int index = findShowtimeIndexById(
                system,
                showtimeId
            );

            if (index == -1) {
                printf("Showtime not found.\n");
                continue;
            }

            printf(
                "\n========== Search Result ==========\n"
            );

            displayShowtimeDetails(
                system,
                &system->showtimes[index]
            );

            continue;
        }

        char searchText[100];

        if (choice == 2) {
            readString(
                "Enter all or part of the movie title: ",
                searchText,
                sizeof(searchText)
            );
        }
        else if (choice == 3) {
            readString(
                "Enter all or part of the hall name: ",
                searchText,
                sizeof(searchText)
            );
        }
        else {
            readDate(
                "Enter date (YYYY-MM-DD): ",
                searchText
            );
        }

        int found = 0;

        for (int i = 0; i < system->showtimeCount; i++) {
            const Showtime *showtime =
                &system->showtimes[i];

            int matches = 0;

            if (choice == 2) {
                int movieIndex = findMovieIndexById(
                    system,
                    showtime->movieId
                );

                if (movieIndex != -1) {
                    matches = containsIgnoreCase(
                        system->movies[movieIndex].title,
                        searchText
                    );
                }
            }
            else if (choice == 3) {
                int hallIndex = findHallIndexById(
                    system,
                    showtime->hallId
                );

                if (hallIndex != -1) {
                    matches = containsIgnoreCase(
                        system->halls[hallIndex].name,
                        searchText
                    );
                }
            }
            else {
                matches =
                    strcmp(showtime->date, searchText) == 0;
            }

            if (!matches) {
                continue;
            }

            if (!found) {
                printf(
                    "\n========== Search Results ==========\n"
                );
            }

            displayShowtimeDetails(
                system,
                showtime
            );

            found = 1;
        }

        if (!found) {
            printf(
                "No matching showtimes were found.\n"
            );
        }
    }
}

void displayShowtimeDetails(const System *system, const Showtime *showtime)
{
    int movieIndex = findMovieIndexById(
        system,
        showtime->movieId
    );

    int hallIndex = findHallIndexById(
        system,
        showtime->hallId
    );

    printf("\nShowtime ID: %d\n", showtime->id);

    if (movieIndex != -1) {
        printf(
            "Movie: %s (ID: %d)\n",
            system->movies[movieIndex].title,
            showtime->movieId
        );
    }
    else {
        printf(
            "Movie ID: %d (not found)\n",
            showtime->movieId
        );
    }

    if (hallIndex != -1) {
        printf(
            "Hall: %s (ID: %d)\n",
            system->halls[hallIndex].name,
            showtime->hallId
        );
    }
    else {
        printf(
            "Hall ID: %d (not found)\n",
            showtime->hallId
        );
    }

    printf("Date: %s\n", showtime->date);
    printf("Time: %s\n", showtime->time);
    printf("Ticket Price: %.2f\n", showtime->ticketPrice);

    printf(
        "Status: %s\n",
        showtime->isActive
            ? "Active"
            : "Inactive"
    );

    printf("--------------------------------\n");
}

void editShowtime(System *system)
{
    int showtimeId = readIntInRange(
        "Enter showtime ID to edit: ",
        1,
        1000000
    );

    int index = findShowtimeIndexById(system, showtimeId);

    if (index == -1) {
        printf("Showtime not found.\n");
        return;
    }

    if (showtimeHasTickets(system, showtimeId)) {
        printf(
            "This showtime cannot be edited because "
            "tickets have already been created for it.\n"
        );
        return;
    }

    Showtime *showtime = &system->showtimes[index];

    printf("\nEditing showtime ID: %d\n", showtime->id);

    listMovies(system);

    int newMovieId = readIntInRange(
        "New movie ID: ",
        1,
        1000000
    );

    int movieIndex = findMovieIndexById(
        system,
        newMovieId
    );

    if (movieIndex == -1) {
        printf("Movie not found.\n");
        return;
    }

    if (!system->movies[movieIndex].isActive) {
        printf("The selected movie is inactive.\n");
        return;
    }

    listHalls(system);

    int newHallId = readIntInRange(
        "New hall ID: ",
        1,
        1000000
    );

    int hallIndex = findHallIndexById(
        system,
        newHallId
    );

    if (hallIndex == -1) {
        printf("Hall not found.\n");
        return;
    }

    if (!system->halls[hallIndex].isActive) {
        printf("The selected hall is inactive.\n");
        return;
    }

    char newDate[11];
    char newTime[6];

    readDate(
        "New date (YYYY-MM-DD): ",
        newDate
    );

    displayHallScheduleForDate(
        system,
        newHallId,
        newDate,
        showtime->id
    );

    readTime(
        "New time (HH:MM): ",
        newTime
    );

    int conflictingShowtimeIndex = findShowtimeConflict(
        system,
        newHallId,
        newMovieId,
        newDate,
        newTime,
        showtime->id
    );

    if (conflictingShowtimeIndex != -1) {
        Showtime *conflictingShowtime = &system->showtimes[conflictingShowtimeIndex];

        int conflictingMovieIndex = findMovieIndexById(
                system,
                conflictingShowtime->movieId
        );

        Movie *conflictingMovie = &system->movies[conflictingMovieIndex];

        printf("\nThe selected hall is not available at this time.\n");

        printf(
            "Conflicting showtime ID: %d\n",
            conflictingShowtime->id
        );

        printf(
            "Movie: %s\n",
            conflictingMovie->title
        );

        char endingTime[6];

        addMinutesToTime(
            conflictingShowtime->time,
            conflictingMovie->duration + HALL_PREPARATION_TIME,
            endingTime
        );

        printf(
            "From %s to %s\n",
            conflictingShowtime->time,
            endingTime
        );
        
        return;
    }
    
    float newTicketPrice = readFloatInRange(
        "New ticket price: ",
        0.0f,
        10000.0f
    );

    showtime->movieId = newMovieId;
    showtime->hallId = newHallId;

    strcpy(
        showtime->date,
        newDate
    );

    strcpy(
        showtime->time,
        newTime
    );

    showtime->ticketPrice = newTicketPrice;

    if (!saveShowtimes(system)) {
        printf(
            "Showtime was edited, but saving failed.\n"
        );
        return;
    }

    printf("Showtime edited successfully.\n");
}

void deactivateShowtime(System *system)
{
    int showtimeId = readIntInRange(
        "Enter showtime ID to deactivate: ",
        1,
        1000000
    );

    int index = findShowtimeIndexById(
        system,
        showtimeId
    );

    if (index == -1) {
        printf("Showtime not found.\n");
        return;
    }

    Showtime *showtime =
        &system->showtimes[index];

    if (!showtime->isActive) {
        printf(
            "This showtime is already inactive.\n"
        );
        return;
    }

    int movieIndex = findMovieIndexById(
        system,
        showtime->movieId
    );

    if (movieIndex != -1) {
        printf(
            "Movie: %s\n",
            system->movies[movieIndex].title
        );
    }

    printf("Date: %s\n", showtime->date);
    printf("Time: %s\n", showtime->time);

    int confirmation = readIntInRange(
        "Are you sure? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (confirmation == 0) {
        printf("Operation cancelled.\n");
        return;
    }

    showtime->isActive = 0;

    if (!saveShowtimes(system)) {
        printf(
            "Showtime was deactivated, "
            "but saving failed.\n"
        );
        return;
    }

    printf(
        "Showtime deactivated successfully.\n"
    );
}

void activateShowtime(System *system)
{
    int showtimeId = readIntInRange(
        "Enter showtime ID to activate: ",
        1,
        1000000
    );

    int index = findShowtimeIndexById(
        system,
        showtimeId
    );

    if (index == -1) {
        printf("Showtime not found.\n");
        return;
    }

    Showtime *showtime =
        &system->showtimes[index];

    if (showtime->isActive) {
        printf(
            "This showtime is already active.\n"
        );
        return;
    }

    int movieIndex = findMovieIndexById(
        system,
        showtime->movieId
    );

    if (movieIndex == -1) {
        printf(
            "The related movie no longer exists.\n"
        );
        return;
    }

    if (!system->movies[movieIndex].isActive) {
        printf(
            "The related movie is inactive. "
            "Activate the movie first.\n"
        );
        return;
    }

    int hallIndex = findHallIndexById(
        system,
        showtime->hallId
    );

    if (hallIndex == -1) {
        printf(
            "The related hall no longer exists.\n"
        );
        return;
    }

    if (!system->halls[hallIndex].isActive) {
        printf(
            "The related hall is inactive. "
            "Activate the hall first.\n"
        );
        return;
    }

    int conflictIndex = findShowtimeConflict(
        system,
        showtime->hallId,
        showtime->movieId,
        showtime->date,
        showtime->time,
        showtime->id
    );

    if (conflictIndex != -1) {
        printf(
            "This showtime conflicts with active "
            "showtime ID %d.\n",
            system->showtimes[conflictIndex].id
        );
        return;
    }

    showtime->isActive = 1;

    if (!saveShowtimes(system)) {
        printf(
            "Showtime was activated, "
            "but saving failed.\n"
        );
        return;
    }

    printf(
        "Showtime activated successfully.\n"
    );
}

long long showtimeStartInMinutes(const Showtime *showtime)
{
    long long dayNumber = dateToDayNumber(showtime->date);

    int minuteOfDay = timeToMinutes(showtime->time);

    return dayNumber * 24 * 60 + minuteOfDay;
}

long long showtimeEndInMinutes(const System *system, const Showtime *showtime)
{
    int movieIndex = findMovieIndexById(system, showtime->movieId);

    if (movieIndex == -1) {
        return showtimeStartInMinutes(showtime);
    }

    int duration =
        system->movies[movieIndex].duration;

    return showtimeStartInMinutes(showtime) + duration + HALL_PREPARATION_TIME;
}

int findShowtimeConflict(const System *system, int hallId, int movieId, const char *date, const char *time, int ignoredShowtimeId)
{
    Showtime candidate;

    candidate.id = ignoredShowtimeId;
    candidate.movieId = movieId;
    candidate.hallId = hallId;

    strcpy(candidate.date, date);
    strcpy(candidate.time, time);

    candidate.ticketPrice = 0.0f;
    candidate.isActive = 1;

    long long candidateStart = showtimeStartInMinutes(&candidate);

    long long candidateEnd = showtimeEndInMinutes(system, &candidate);

    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *existing =
            &system->showtimes[i];

        /*
         * Ignore the showtime currently being checked.
         */
        if (existing->id == ignoredShowtimeId) {
            continue;
        }

        /*
         * Inactive showtimes do not reserve a hall.
         */
        if (!existing->isActive) {
            continue;
        }

        /*
         * Showtimes in different halls cannot conflict.
         */
        if (existing->hallId != hallId) {
            continue;
        }

        long long existingStart = showtimeStartInMinutes(existing);

        long long existingEnd = showtimeEndInMinutes(system, existing);

        if (candidateStart < existingEnd && existingStart < candidateEnd) {
            return i;
        }
    }

    return -1;
}

void displayHallScheduleForDate(const System *system, int hallId, const char *date, int excludedShowtimeId)
{
    int found = 0;
    
    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime = &system->showtimes[i];

        if (!showtime->isActive) {
            continue;
        }

        if (showtime->id == excludedShowtimeId) {
            continue;
        }

        if (showtime->hallId != hallId) {
            continue;
        }

        if (strcmp(showtime->date, date) != 0) {
            continue;
        }

        int movieIndex = findMovieIndexById(
            system,
            showtime->movieId
        );

        if (movieIndex == -1) {
            continue;
        }

        const Movie *movie = &system->movies[movieIndex];

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        if (hallIndex == -1) {
            continue;
        }

        const Hall *hall = &system->halls[hallIndex];

        char endingTime[6];

        addMinutesToTime(
            showtime->time,
            movie->duration + HALL_PREPARATION_TIME,
            endingTime
        );

        if (found == 0) {
            printf("\nSchedule for Hall \"%s\" on %s:\n", hall->name, date);

            printf(
                "%-12s %-30s %-8s %-8s\n",
                "ID",
                "Movie",
                "From",
                "Until"
            );
            printf(
                "--------------------------------------------------------------\n"
            );
        }

        printf(
            "%-12d %-30s %-8s %-8s\n",
            showtime->id,
            movie->title,
            showtime->time,
            endingTime
        );

        found = 1;
    }

    if (!found) {
        printf("The hall has no showtimes on this date.\n");
    }

    printf("\n");
}

void viewHallSchedule(System *system)
{
    listHalls(system);

    int hallId = readIntInRange(
        "Enter hall ID: ",
        1,
        1000000
    );

    int hallIndex = findHallIndexById(
        system,
        hallId
    );

    if (hallIndex == -1) {
        printf("Hall not found.\n");
        return;
    }

    char date[11];

    readDate(
        "Enter date (YYYY-MM-DD): ",
        date
    );

    displayHallScheduleForDate(
        system,
        hallId,
        date,
        -1
    );
}

void displayMovieScheduleForDate(const System *system, int movieId, const char *date, int excludedShowtimeId)
{
    int found = 0;

    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime = &system->showtimes[i];

        if (!showtime->isActive) {
            continue;
        }

        if (showtime->id == excludedShowtimeId) {
            continue;
        }

        if (showtime->movieId != movieId) {
            continue;
        }

        if (strcmp(showtime->date, date) != 0) {
            continue;
        }

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        if (hallIndex == -1) {
            continue;
        }

        int movieIndex = findMovieIndexById(
            system,
            showtime->movieId
        );

        if (movieIndex == -1) {
            continue;
        }

        const Hall *hall = &system->halls[hallIndex];
        const Movie *movie = &system->movies[movieIndex];

        char endingTime[6];

        addMinutesToTime(
            showtime->time,
            movie->duration + HALL_PREPARATION_TIME,
            endingTime
        );

        if (found == 0) {
            printf(
                "\nSchedule for movie \"%s\" on %s:\n",
                movie->title,
                date
            );

            printf(
                "%-12s %-30s %-8s %-8s\n",
                "ID",
                "Hall",
                "From",
                "Until"
            );

            printf(
                "--------------------------------------------------------------\n"
            );
        }

        printf(
            "%-12d %-30s %-8s %-8s\n",
            showtime->id,
            hall->name,
            showtime->time,
            endingTime
        );

        found = 1;
    }

    if (!found) {
        printf(
            "The movie has no showtimes on this date.\n"
        );
    }

    printf("\n");
}

void viewMovieSchedule(System *system)
{
    if (system->movieCount == 0) {
        printf("No movies have been registered.\n");
        return;
    }

    listMovies(system);

    int movieId = readIntInRange(
        "Enter movie ID: ",
        1,
        1000000
    );

    int movieIndex = findMovieIndexById(
        system,
        movieId
    );

    if (movieIndex == -1) {
        printf("Movie not found.\n");
        return;
    }

    char date[11];

    readDate(
        "Enter date (YYYY-MM-DD): ",
        date
    );

    displayMovieScheduleForDate(
        system,
        movieId,
        date,
        -1
    );
}

// ========== Discount Management ==========
int findDiscountIndexById(const System *system, int discountId)
{
    for (int i = 0; i < system->discountCount; i++) {
        if (system->discounts[i].id == discountId) {
            return i;
        }
    }
    return -1;
}

int findDiscountIndexByCode(const System*system, const char *code)
{
    for (int i = 0; i < system->discountCount; i++) {
        if (strcmp(system->discounts[i].code, code) == 0) {
            return i;
        }
    }
    return -1;
}

void addDiscount(System *system)
{
    Discount newDiscount;

    printf("\n========== Add Discount ==========\n");

    newDiscount.id = readIntInRange(
        "Discount ID: ",
        1,
        1000000
    );

    if (
        findDiscountIndexById(
            system,
            newDiscount.id
        ) != -1
    ) {
        printf("A discount with this ID already exists.\n");
        return;
    }

    readString(
        "Discount code: ",
        newDiscount.code,
        sizeof(newDiscount.code)
    );

    if (
        findDiscountIndexByCode(
            system,
            newDiscount.code
        ) != -1
    ) {
        printf("A discount with this code already exists.\n");
        return;
    }

    newDiscount.percentage = readFloatInRange(
        "Discount percentage: ",
        0.01f,
        100.0f
    );

    newDiscount.minimumPrice = readFloatInRange(
        "Minimum ticket price required: ",
        0.0f,
        1000000.0f
    );

    readDate(
        "Start date (YYYY-MM-DD): ",
        newDiscount.startDate
    );

    readDate(
        "End date (YYYY-MM-DD): ",
        newDiscount.endDate
    );

    if (
        dateToDayNumber(newDiscount.endDate) <
        dateToDayNumber(newDiscount.startDate)
    ) {
        printf(
            "The end date cannot be before the start date.\n"
        );
        return;
    }

    newDiscount.usageLimit = readIntInRange(
        "Usage limit: ",
        1,
        1000000
    );

    newDiscount.timesUsed = 0;
    newDiscount.isActive = 1;

    if (!ensureDiscountCapacity(system)) {
        return;
    }

    system->discounts[system->discountCount] =
        newDiscount;

    system->discountCount++;

    if (!saveDiscounts(system)) {
        printf(
            "Discount was added to memory, "
            "but saving failed.\n"
        );
        return;
    }

    printf("Discount added and saved successfully.\n");
}

void listDiscounts(const System *system)
{
    if (system->discountCount == 0) {
        printf("\nNo discounts have been registered.\n");
        return;
    }

    int activeDiscounts = 0;

    printf("\n========== Discount List ==========\n");

    for (int i = 0; i < system->discountCount; i++) {
        const Discount *discount =
            &system->discounts[i];

        activeDiscounts++;

        displayDiscountDetails(discount);
    }

    if (activeDiscounts == 0) {
        printf("No active discounts were found.\n");
    }
}

void searchDiscount(const System *system)
{
    if (system->discountCount == 0) {
        printf("No discounts have been registered.\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n========== Search Discount ==========\n");
        printf("1. Search by ID\n");
        printf("2. Search by Code\n");
        printf("3. Search by Start Date\n");
        printf("4. Find Discounts Available During a Date Period\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            4
        );

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int discountId = readIntInRange(
                "Enter discount ID: ",
                1,
                1000000
            );

            int index = findDiscountIndexById(
                system,
                discountId
            );

            if (index == -1) {
                printf("Discount not found.\n");
                continue;
            }

            printf(
                "\n========== Search Result ==========\n"
            );

            displayDiscountDetails(
                &system->discounts[index]
            );

            continue;
        }

        if (choice == 2) {
            char searchText[30];
            int found = 0;

            readString(
                "Enter all or part of the discount code: ",
                searchText,
                sizeof(searchText)
            );

            for (int i = 0; i < system->discountCount; i++) {
                const Discount *discount =
                    &system->discounts[i];

                if (
                    !containsIgnoreCase(
                        discount->code,
                        searchText
                    )
                ) {
                    continue;
                }

                if (!found) {
                    printf(
                        "\n========== Search Results ==========\n"
                    );
                }

                displayDiscountDetails(discount);
                found = 1;
            }

            if (!found) {
                printf(
                    "No matching discounts were found.\n"
                );
            }

            continue;
        }

        if (choice == 3) {
            char startDate[11];
            int found = 0;

            readDate(
                "Enter start date (YYYY-MM-DD): ",
                startDate
            );

            for (int i = 0; i < system->discountCount; i++) {
                const Discount *discount =
                    &system->discounts[i];

                if (
                    strcmp(
                        discount->startDate,
                        startDate
                    ) != 0
                ) {
                    continue;
                }

                if (!found) {
                    printf(
                        "\n========== Search Results ==========\n"
                    );
                }

                displayDiscountDetails(discount);
                found = 1;
            }

            if (!found) {
                printf(
                    "No discounts start on this date.\n"
                );
            }

            continue;
        }

        char periodStartDate[11];
        char periodEndDate[11];

        readDate(
            "Enter beginning of period (YYYY-MM-DD): ",
            periodStartDate
        );

        readDate(
            "Enter end of period (YYYY-MM-DD): ",
            periodEndDate
        );

        long long periodStart =
            dateToDayNumber(periodStartDate);

        long long periodEnd =
            dateToDayNumber(periodEndDate);

        if (periodEnd < periodStart) {
            printf(
                "The end of the period cannot be before "
                "the beginning.\n"
            );
            continue;
        }

        int found = 0;

        for (int i = 0; i < system->discountCount; i++) {
            const Discount *discount =
                &system->discounts[i];

            if (!discount->isActive) {
                continue;
            }

            if (discount->timesUsed >= discount->usageLimit) {
                continue;
            }

            long long discountStart =
                dateToDayNumber(discount->startDate);

            long long discountEnd =
                dateToDayNumber(discount->endDate);

            if (
                discountStart <= periodEnd &&
                periodStart <= discountEnd
            ) {
                if (!found) {
                    printf(
                        "\n========== Available Discounts ==========\n"
                    );
                }

                displayDiscountDetails(discount);
                found = 1;
            }
        }

        if (!found) {
            printf(
                "No discounts are available during "
                "this date period.\n"
            );
        }
    }
}

void displayDiscountDetails(const Discount *discount)
{
    printf("\nDiscount ID: %d\n", discount->id);
    printf("Code: %s\n", discount->code);
    printf("Percentage: %.2f%%\n", discount->percentage);
    printf("Minimum Price: %.2f\n", discount->minimumPrice);
    printf("Start Date: %s\n", discount->startDate);
    printf("End Date: %s\n", discount->endDate);
    printf("Usage Limit: %d\n", discount->usageLimit);
    printf("Times Used: %d\n", discount->timesUsed);

    printf(
        "Remaining Uses: %d\n",
        discount->usageLimit > discount->timesUsed
            ? discount->usageLimit - discount->timesUsed
            : 0
    );

    printf(
        "Status: %s\n",
        discount->isActive
            ? "Active"
            : "Inactive"
    );

    printf(
        "Usage Status: %s\n",
        discount->timesUsed >= discount->usageLimit
            ? "Limit Reached"
            : "Available"
    );

    printf("--------------------------------\n");
}

void editDiscount(System *system)
{
    int discountId = readIntInRange(
        "Enter discount ID to edit: ",
        1,
        1000000
    );

    int index = findDiscountIndexById(
        system,
        discountId
    );

    if (index == -1) {
        printf("Discount not found.\n");
        return;
    }

    if (discountHasTickets(system, discountId)) {
        printf(
            "This discount cannot be edited because "
            "it has already been used for a ticket.\n"
        );
        return;
    }

    Discount *discount =
        &system->discounts[index];

    printf(
        "\nEditing discount: %s\n",
        discount->code
    );

    char newCode[30];

    readString(
        "New discount code: ",
        newCode,
        sizeof(newCode)
    );

    int codeIndex = findDiscountIndexByCode(
        system,
        newCode
    );

    if (codeIndex != -1 && codeIndex != index) {
        printf(
            "Another discount already uses this code.\n"
        );
        return;
    }

    float newPercentage = readFloatInRange(
        "New discount percentage: ",
        0.01f,
        100.0f
    );

    float newMinimumPrice = readFloatInRange(
        "New minimum ticket price: ",
        0.0f,
        1000000.0f
    );

    char newStartDate[11];
    char newEndDate[11];

    readDate(
        "New start date (YYYY-MM-DD): ",
        newStartDate
    );

    readDate(
        "New end date (YYYY-MM-DD): ",
        newEndDate
    );

    if (
        dateToDayNumber(newEndDate) <
        dateToDayNumber(newStartDate)
    ) {
        printf(
            "The end date cannot be before the start date.\n"
        );
        return;
    }

    int newUsageLimit = readIntInRange(
        "New usage limit: ",
        1,
        1000000
    );

    if (newUsageLimit < discount->timesUsed) {
        printf(
            "The usage limit cannot be less than "
            "the number of times already used (%d).\n",
            discount->timesUsed
        );
        return;
    }

    strcpy(discount->code, newCode);

    discount->percentage = newPercentage;

    discount->minimumPrice = newMinimumPrice;

    strcpy(
        discount->startDate,
        newStartDate
    );

    strcpy(
        discount->endDate,
        newEndDate
    );

    discount->usageLimit = newUsageLimit;

    if (!saveDiscounts(system)) {
        printf(
            "Discount was edited, but saving failed.\n"
        );
        return;
    }

    printf("Discount edited successfully.\n");
}

void deactivateDiscount(System *system)
{
    int discountId = readIntInRange(
        "Enter discount ID to deactivate: ",
        1,
        1000000
    );

    int index = findDiscountIndexById(
        system,
        discountId
    );

    if (index == -1) {
        printf("Discount not found.\n");
        return;
    }

    Discount *discount =
        &system->discounts[index];

    if (!discount->isActive) {
        printf(
            "This discount is already inactive.\n"
        );
        return;
    }

    printf("Discount code: %s\n", discount->code);
    printf(
        "Percentage: %.2f%%\n",
        discount->percentage
    );

    int confirmation = readIntInRange(
        "Are you sure? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (confirmation == 0) {
        printf("Operation cancelled.\n");
        return;
    }

    discount->isActive = 0;

    if (!saveDiscounts(system)) {
        printf(
            "Discount was deactivated, "
            "but saving failed.\n"
        );
        return;
    }

    printf("Discount deactivated successfully.\n");
}

void activateDiscount(System *system)
{
    int discountId = readIntInRange(
        "Enter discount ID to activate: ",
        1,
        1000000
    );

    int index = findDiscountIndexById(
        system,
        discountId
    );

    if (index == -1) {
        printf("Discount not found.\n");
        return;
    }

    Discount *discount =
        &system->discounts[index];

    if (discount->isActive) {
        printf(
            "This discount is already active.\n"
        );
        return;
    }

    if (discount->timesUsed >= discount->usageLimit) {
        printf(
            "This discount cannot be activated because "
            "its usage limit has been reached.\n"
        );
        return;
    }

    if (
        dateToDayNumber(discount->endDate) <
        dateToDayNumber(discount->startDate)
    ) {
        printf(
            "This discount has an invalid date range.\n"
        );
        return;
    }

    discount->isActive = 1;

    if (!saveDiscounts(system)) {
        printf(
            "Discount was activated, "
            "but saving failed.\n"
        );
        return;
    }

    printf("Discount activated successfully.\n");
}

// ========== Ticket Management ==========
int findTicketIndexById(const System *system, int ticketId)
{
    for (int i = 0; i < system->ticketCount; i++) {
        if (system->tickets[i].id == ticketId) {
            return i;
        }
    }

    return -1;
}

void displayTicketDetails(const System *system, const Ticket *ticket)
{
    int userIndex = findUserIndexById(
        system,
        ticket->userId
    );

    int showtimeIndex = findShowtimeIndexById(
        system,
        ticket->showtimeId
    );

    printf("\nTicket ID: %d\n", ticket->id);

    if (userIndex != -1) {
        printf(
            "Customer: %s (User ID: %d)\n",
            system->users[userIndex].fullName,
            ticket->userId
        );
    }
    else {
        printf(
            "User ID: %d\n",
            ticket->userId
        );
    }

    printf("Buyer Name: %s\n", ticket->buyerName);
    printf("Buyer Phone: %s\n", ticket->buyerPhone);

    if (showtimeIndex != -1) {
        const Showtime *showtime =
            &system->showtimes[showtimeIndex];

        int movieIndex = findMovieIndexById(
            system,
            showtime->movieId
        );

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        printf(
            "Showtime ID: %d\n",
            showtime->id
        );

        if (movieIndex != -1) {
            printf(
                "Movie: %s\n",
                system->movies[movieIndex].title
            );
        }

        if (hallIndex != -1) {
            const Hall *hall =
                &system->halls[hallIndex];

            printf(
                "Hall: %s\n",
                hall->name
            );

            int row =
                (ticket->seatNumber - 1)
                / hall->columns
                + 1;

            int column =
                (ticket->seatNumber - 1)
                % hall->columns
                + 1;

            printf(
                "Seat: %d "
                "(Row %d, Column %d)\n",
                ticket->seatNumber,
                row,
                column
            );
        }
        else {
            printf(
                "Seat Number: %d\n",
                ticket->seatNumber
            );
        }

        printf("Date: %s\n", showtime->date);
        printf("Time: %s\n", showtime->time);
    }
    else {
        printf(
            "Showtime ID: %d\n",
            ticket->showtimeId
        );

        printf(
            "Seat Number: %d\n",
            ticket->seatNumber
        );
    }

    printf(
        "Original Price: %.2f\n",
        ticket->originalPrice
    );

    if (ticket->discountId == NO_DISCOUNT_ID) {
        printf("Discount: None\n");
    }
    else {
        int discountIndex = findDiscountIndexById(
            system,
            ticket->discountId
        );

        if (discountIndex != -1) {
            printf(
                "Discount: %s (ID: %d)\n",
                system->discounts[discountIndex].code,
                ticket->discountId
            );
        }
        else {
            printf(
                "Discount ID: %d\n",
                ticket->discountId
            );
        }
    }

    printf(
        "Final Price: %.2f\n",
        ticket->finalPrice
    );

    printf(
        "Purchase Date: %s\n",
        ticket->purchaseDate
    );

    printf(
        "Status: %s\n",
        ticket->isCancelled
            ? "Cancelled"
            : "Sold"
    );

    printf("--------------------------------\n");
}

int isSeatSold(const System *system, int showtimeId, int seatNumber)
{
    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *ticket =
            &system->tickets[i];

        if (
            ticket->showtimeId == showtimeId &&
            ticket->seatNumber == seatNumber &&
            !ticket->isCancelled
        ) {
            return 1;
        }
    }

    return 0;
}

void displaySeatMap(const System *system, int showtimeId)
{
    int showtimeIndex = findShowtimeIndexById(
        system,
        showtimeId
    );

    if (showtimeIndex == -1) {
        printf("Showtime not found.\n");
        return;
    }

    const Showtime *showtime =
        &system->showtimes[showtimeIndex];

    int hallIndex = findHallIndexById(
        system,
        showtime->hallId
    );

    if (hallIndex == -1) {
        printf("The related hall was not found.\n");
        return;
    }

    const Hall *hall =
        &system->halls[hallIndex];

    int firstVipRow =
        hall->rows - hall->vipRows + 1;

    printf(
        "\n========== Seat Map: %s ==========\n",
        hall->name
    );

    printf(
        "Showtime ID: %d | Date: %s | Time: %s\n",
        showtime->id,
        showtime->date,
        showtime->time
    );

    printf(
        "Regular price: %.2f | VIP price: %.2f\n",
        showtime->ticketPrice,
        showtime->ticketPrice * VIP_PRICE_MULTIPLIER
    );

    printf(
        "\nLegend: [  ] Regular | [V ] VIP | [X ] Sold\n\n"
    );

    for (int row = 1; row <= hall->rows; row++) {
        printf("Row %-3d ", row);

        for (
            int column = 1;
            column <= hall->columns;
            column++
        ) {
            int seatNumber =
                (row - 1) * hall->columns + column;

            if (
                isSeatSold(
                    system,
                    showtimeId,
                    seatNumber
                )
            ) {
                printf("[X%03d] ", seatNumber);
            }
            else if (
                hall->vipRows > 0 &&
                row >= firstVipRow
            ) {
                printf("[V%03d] ", seatNumber);
            }
            else {
                printf("[ %03d] ", seatNumber);
            }
        }

        printf("\n");
    }

    printf("\n");
}

void listTickets(const System *system)
{
    if (system->ticketCount == 0) {
        printf("No tickets have been registered.\n");
        return;
    }

    printf("\n========== Ticket List ==========\n");

    for (int i = 0; i < system->ticketCount; i++) {
        displayTicketDetails(
            system,
            &system->tickets[i]
        );
    }
}

void searchTicket(const System *system)
{
    if (system->ticketCount == 0) {
        printf("No tickets have been registered.\n");
        return;
    }

    int choice;

    while (1) {
        printf("\n========== Search Ticket ==========\n");
        printf("1. Search by Ticket ID\n");
        printf("2. Search by Buyer Name\n");
        printf("3. Search by Buyer Phone\n");
        printf("4. Search by Showtime ID\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            4
        );

        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            int ticketId = readIntInRange(
                "Enter ticket ID: ",
                1,
                1000000
            );

            int index = findTicketIndexById(
                system,
                ticketId
            );

            if (index == -1) {
                printf("Ticket not found.\n");
                continue;
            }

            printf(
                "\n========== Search Result ==========\n"
            );

            displayTicketDetails(
                system,
                &system->tickets[index]
            );

            continue;
        }

        int found = 0;

        if (choice == 4) {
            int showtimeId = readIntInRange(
                "Enter showtime ID: ",
                1,
                1000000
            );

            for (int i = 0; i < system->ticketCount; i++) {
                const Ticket *ticket =
                    &system->tickets[i];

                if (ticket->showtimeId != showtimeId) {
                    continue;
                }

                if (!found) {
                    printf(
                        "\n========== Search Results ==========\n"
                    );
                }

                displayTicketDetails(
                    system,
                    ticket
                );

                found = 1;
            }
        }
        else {
            char searchText[100];

            if (choice == 2) {
                readString(
                    "Enter all or part of the buyer name: ",
                    searchText,
                    sizeof(searchText)
                );
            }
            else {
                readString(
                    "Enter all or part of the phone number: ",
                    searchText,
                    sizeof(searchText)
                );
            }

            for (int i = 0; i < system->ticketCount; i++) {
                const Ticket *ticket =
                    &system->tickets[i];

                int matches;

                if (choice == 2) {
                    matches = containsIgnoreCase(
                        ticket->buyerName,
                        searchText
                    );
                }
                else {
                    matches =
                        strstr(
                            ticket->buyerPhone,
                            searchText
                        ) != NULL;
                }

                if (!matches) {
                    continue;
                }

                if (!found) {
                    printf(
                        "\n========== Search Results ==========\n"
                    );
                }

                displayTicketDetails(
                    system,
                    ticket
                );

                found = 1;
            }
        }

        if (!found) {
            printf("No matching tickets were found.\n");
        }
    }
}

// ========== Reports ==========
void reportTotalSales(const System *system)
{
    int totalSold = 0, totalCancelled = 0;
    float totalIncome = 0.0f;

    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *t = &system->tickets[i];
        if (t->isCancelled) {
            totalCancelled++;
        } else {
            totalSold++;
            totalIncome += t->finalPrice;
        }
    }

    printf("\n========== Total Sales Summary ==========\n");
    printf("Tickets Sold:       %d\n", totalSold);
    printf("Tickets Cancelled:  %d\n", totalCancelled);
    printf("Total Income:       %.2f\n", totalIncome);
    printf("==========================================\n");
}

void reportMovieIncome(const System *system)
{
    listMovies(system);
    int movieId = readIntInRange("Enter movie ID: ", 1, 1000000);
    int idx = findMovieIndexById(system, movieId);
    if (idx == -1) {
        printf("Movie not found.\n");
        return;
    }

    float income = 0.0f;
    int sold = 0;
    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *t = &system->tickets[i];
        if (t->isCancelled) continue;
        int si = findShowtimeIndexById(system, t->showtimeId);
        if (si != -1 && system->showtimes[si].movieId == movieId) {
            income += t->finalPrice;
            sold++;
        }
    }

    printf("\n========== Movie Income Report ==========\n");
    printf("Movie: %s (ID: %d)\n", system->movies[idx].title, movieId);
    printf("Tickets Sold: %d\n", sold);
    printf("Total Income: %.2f\n", income);
    printf("==========================================\n");
}

void reportShowtimeIncome(const System *system)
{
    int showtimeId = readIntInRange("Enter showtime ID: ", 1, 1000000);
    int si = findShowtimeIndexById(system, showtimeId);
    if (si == -1) {
        printf("Showtime not found.\n");
        return;
    }

    const Showtime *st = &system->showtimes[si];
    int movieIdx = findMovieIndexById(system, st->movieId);
    int hallIdx = findHallIndexById(system, st->hallId);
    const char *movieName = (movieIdx != -1) ? system->movies[movieIdx].title : "Unknown";
    const char *hallName = (hallIdx != -1) ? system->halls[hallIdx].name : "Unknown";
    int capacity = (hallIdx != -1) ? system->halls[hallIdx].rows * system->halls[hallIdx].columns : 0;

    float income = 0.0f;
    int sold = 0;
    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *t = &system->tickets[i];
        if (t->isCancelled) continue;
        if (t->showtimeId == showtimeId) {
            income += t->finalPrice;
            sold++;
        }
    }

    float occupancy = (capacity > 0) ? (sold * 100.0f / capacity) : 0.0f;

    printf("\n========== Showtime Report ==========\n");
    printf("Showtime ID: %d\n", st->id);
    printf("Movie: %s\n", movieName);
    printf("Hall: %s\n", hallName);
    printf("Date: %s\n", st->date);
    printf("Time: %s\n", st->time);
    printf("Capacity: %d\n", capacity);
    printf("Sold Tickets: %d\n", sold);
    printf("Occupancy: %.1f%%\n", occupancy);
    printf("Income: %.2f\n", income);
    printf("=====================================\n");
}

static int compareMovieSales(const void *a, const void *b)
{
    const MovieSales *movieA =
        (const MovieSales *)a;

    const MovieSales *movieB =
        (const MovieSales *)b;

    if (
        movieA->ticketsSold <
        movieB->ticketsSold
    ) {
        return 1;
    }

    if (
        movieA->ticketsSold >
        movieB->ticketsSold
    ) {
        return -1;
    }

    return 0;
}

void reportBestSellingMovies(const System *system)
{
    if (system->movieCount == 0) {
        printf("No movies registered.\n");
        return;
    }

    // Count tickets per movie
    MovieSales *sales = malloc(system->movieCount * sizeof(MovieSales));
    if (sales == NULL) {
        printf("Memory allocation error.\n");
        return;
    }

    // Initialize
    for (int i = 0; i < system->movieCount; i++) {
        sales[i].movieId = system->movies[i].id;
        sales[i].ticketsSold = 0;
    }

    // Count tickets
    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *t = &system->tickets[i];
        if (t->isCancelled) continue;
        int si = findShowtimeIndexById(system, t->showtimeId);
        if (si == -1) continue;
        int mid = system->showtimes[si].movieId;
        // find movie sales entry
        for (int j = 0; j < system->movieCount; j++) {
            if (sales[j].movieId == mid) {
                sales[j].ticketsSold++;
                break;
            }
        }
    }

    // Sort descending
    qsort(sales, system->movieCount, sizeof(MovieSales), compareMovieSales);

    printf("\n========== Best-Selling Movies ==========\n");
    printf("%-5s %-30s %-10s\n", "Rank", "Movie", "Tickets");
    printf("--------------------------------------------\n");
    int rank = 1;
    for (int i = 0; i < system->movieCount; i++) {
        if (sales[i].ticketsSold == 0) continue; // skip movies with no sales
        int mi = findMovieIndexById(system, sales[i].movieId);
        const char *title = (mi != -1) ? system->movies[mi].title : "Unknown";
        printf("%-5d %-30s %-10d\n", rank++, title, sales[i].ticketsSold);
    }
    if (rank == 1) {
        printf("No tickets have been sold yet.\n");
    }
    printf("===========================================\n");

    free(sales);
}

void reportOccupancy(const System *system)
{
    if (system->showtimeCount == 0) {
        printf("No showtimes have been registered.\n");
        return;
    }

    printf(
        "\n========== Showtime Occupancy Report ==========\n"
    );

    printf(
        "%-10s %-25s %-15s %-12s %-10s\n",
        "S.Time ID",
        "Movie",
        "Hall",
        "Date",
        "Occupancy"
    );

    printf(
        "--------------------------------------------------------------\n"
    );

    int found = 0;

    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime =
            &system->showtimes[i];

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        if (hallIndex == -1) {
            continue;
        }

        const Hall *hall =
            &system->halls[hallIndex];

        int capacity =
            hall->rows * hall->columns;

        int soldTickets = countSoldSeats(
            system,
            showtime->id
        );

        float occupancy = 0.0f;

        if (capacity > 0) {
            occupancy =
                (float)soldTickets * 100.0f /
                (float)capacity;
        }

        int movieIndex = findMovieIndexById(
            system,
            showtime->movieId
        );

        const char *movieName = "Unknown";

        if (movieIndex != -1) {
            movieName =
                system->movies[movieIndex].title;
        }

        printf(
            "%-10d %-25.25s %-15.15s %-12s %6.1f%%\n",
            showtime->id,
            movieName,
            hall->name,
            showtime->date,
            occupancy
        );

        found = 1;
    }

    if (!found) {
        printf(
            "No valid showtime records were found.\n"
        );
    }

    printf(
        "============================================================\n"
    );
}

void exportShowtimeBuyers(const System *system)
{
    if (system->showtimeCount == 0) {
        printf("No showtimes have been registered.\n");
        return;
    }

    int showtimeId = readIntInRange(
        "Enter showtime ID: ",
        1,
        1000000
    );

    int showtimeIndex = findShowtimeIndexById(
        system,
        showtimeId
    );

    if (showtimeIndex == -1) {
        printf("Showtime not found.\n");
        return;
    }

    const Showtime *showtime =
        &system->showtimes[showtimeIndex];

    int movieIndex = findMovieIndexById(
        system,
        showtime->movieId
    );

    int hallIndex = findHallIndexById(
        system,
        showtime->hallId
    );

    char fileName[100];

    sprintf(
        fileName,
        "buyers_showtime_%d.txt",
        showtimeId
    );

    FILE *file = fopen(
        fileName,
        "w"
    );

    if (file == NULL) {
        printf(
            "Could not create %s.\n",
            fileName
        );
        return;
    }

    const char *movieTitle =
        movieIndex != -1
            ? system->movies[movieIndex].title
            : "Unknown";

    const char *hallName =
        hallIndex != -1
            ? system->halls[hallIndex].name
            : "Unknown";

    fprintf(
        file,
        "========== Showtime Buyers ==========\n"
    );

    fprintf(
        file,
        "Showtime ID: %d\n",
        showtime->id
    );

    fprintf(
        file,
        "Movie: %s\n",
        movieTitle
    );

    fprintf(
        file,
        "Hall: %s\n",
        hallName
    );

    fprintf(
        file,
        "Date: %s\n",
        showtime->date
    );

    fprintf(
        file,
        "Time: %s\n",
        showtime->time
    );

    fprintf(
        file,
        "=====================================\n\n"
    );

    fprintf(
        file,
        "%-10s %-25s %-18s %-10s %-12s\n",
        "Ticket ID",
        "Buyer Name",
        "Phone",
        "Seat",
        "Final Price"
    );

    fprintf(
        file,
        "--------------------------------------------------------------------------\n"
    );

    int buyerCount = 0;
    float totalIncome = 0.0f;

    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *ticket =
            &system->tickets[i];

        if (ticket->showtimeId != showtimeId) {
            continue;
        }

        if (ticket->isCancelled) {
            continue;
        }

        fprintf(
            file,
            "%-10d %-25.25s %-18.18s %-10d %-12.2f\n",
            ticket->id,
            ticket->buyerName,
            ticket->buyerPhone,
            ticket->seatNumber,
            ticket->finalPrice
        );

        buyerCount++;
        totalIncome += ticket->finalPrice;
    }

    fprintf(
        file,
        "\nTotal Buyers: %d\n",
        buyerCount
    );

    fprintf(
        file,
        "Total Income: %.2f\n",
        totalIncome
    );

    if (fclose(file) != 0) {
        printf(
            "The buyer file was created, but closing it failed.\n"
        );
        return;
    }

    if (buyerCount == 0) {
        printf(
            "The file was exported, but this showtime "
            "has no active ticket buyers.\n"
        );
    }
    else {
        printf(
            "Buyer list exported successfully to %s.\n",
            fileName
        );
    }
}

// ========== Customer Functions ==========
void listCurrentlyShowingMovies(const System *system)
{
    int found = 0;

    printf(
        "\n========== Currently Showing Movies ==========\n"
    );

    for (int i = 0; i < system->movieCount; i++) {
        const Movie *movie =
            &system->movies[i];

        if (!movie->isActive) {
            continue;
        }

        if (
            !movieHasActiveShowtime(
                system,
                movie->id
            )
        ) {
            continue;
        }

        printf("\nMovie ID: %d\n", movie->id);
        printf("Title: %s\n", movie->title);
        printf("Genre: %s\n", movie->genre);
        printf("Director: %s\n", movie->director);
        printf("Duration: %d minutes\n", movie->duration);
        printf("Age Limit: %d+\n", movie->ageLimit);
        printf("Rating: %.1f / 10\n", movie->rating);
        printf("--------------------------------\n");

        found = 1;
    }

    if (!found) {
        printf(
            "No movies are currently showing.\n"
        );
    }
}

void searchCurrentlyShowingMovies(const System *system)
{
    if (system->movieCount == 0) {
        printf(
            "No movies have been registered.\n"
        );
        return;
    }

    int choice;

    while (1) {
        printf(
            "\n========== Search Currently Showing Movies ==========\n"
        );

        printf("1. Search by Title\n");
        printf("2. Search by Genre\n");
        printf("3. Search by Director\n");
        printf("0. Back\n");

        choice = readIntInRange(
            "Enter your choice: ",
            0,
            3
        );

        if (choice == 0) {
            return;
        }

        char searchText[100];

        if (choice == 1) {
            readString(
                "Enter all or part of the movie title: ",
                searchText,
                sizeof(searchText)
            );
        }
        else if (choice == 2) {
            readString(
                "Enter all or part of the genre: ",
                searchText,
                sizeof(searchText)
            );
        }
        else {
            readString(
                "Enter all or part of the director's name: ",
                searchText,
                sizeof(searchText)
            );
        }

        int found = 0;

        for (int i = 0; i < system->movieCount; i++) {
            const Movie *movie =
                &system->movies[i];

            /*
             * Customers must not see inactive movies.
             */
            if (!movie->isActive) {
                continue;
            }

            /*
             * The movie must have at least one
             * active showtime.
             */
            if (
                !movieHasActiveShowtime(
                    system,
                    movie->id
                )
            ) {
                continue;
            }

            int matches = 0;

            if (choice == 1) {
                matches = containsIgnoreCase(
                    movie->title,
                    searchText
                );
            }
            else if (choice == 2) {
                matches = containsIgnoreCase(
                    movie->genre,
                    searchText
                );
            }
            else {
                matches = containsIgnoreCase(
                    movie->director,
                    searchText
                );
            }

            if (!matches) {
                continue;
            }

            if (!found) {
                printf(
                    "\n========== Search Results ==========\n"
                );
            }

            printf(
                "\nMovie ID: %d\n",
                movie->id
            );

            printf(
                "Title: %s\n",
                movie->title
            );

            printf(
                "Genre: %s\n",
                movie->genre
            );

            printf(
                "Director: %s\n",
                movie->director
            );

            printf(
                "Duration: %d minutes\n",
                movie->duration
            );

            printf(
                "Age Limit: %d+\n",
                movie->ageLimit
            );

            printf(
                "Rating: %.1f / 10\n",
                movie->rating
            );

            printf(
                "--------------------------------\n"
            );

            found = 1;
        }

        if (!found) {
            printf(
                "No currently showing movies matched "
                "your search.\n"
            );
        }
    }
}

void viewActiveShowtimesForMovie(const System *system)
{
    if (system->movieCount == 0) {
        printf(
            "No movies have been registered.\n"
        );
        return;
    }

    /*
     * Show the customer which movies are currently
     * available before asking for an ID.
     */
    listCurrentlyShowingMovies(system);

    int movieId = readIntInRange(
        "Enter movie ID: ",
        1,
        1000000
    );

    int movieIndex = findMovieIndexById(
        system,
        movieId
    );

    if (movieIndex == -1) {
        printf("Movie not found.\n");
        return;
    }

    const Movie *movie =
        &system->movies[movieIndex];

    if (!movie->isActive) {
        printf(
            "This movie is not currently available.\n"
        );
        return;
    }

    if (
        !movieHasActiveShowtime(
            system,
            movieId
        )
    ) {
        printf(
            "This movie has no active showtimes.\n"
        );
        return;
    }

    int found = 0;

    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime =
            &system->showtimes[i];

        if (!showtime->isActive) {
            continue;
        }

        if (showtime->movieId != movieId) {
            continue;
        }

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        if (hallIndex == -1) {
            continue;
        }

        const Hall *hall =
            &system->halls[hallIndex];

        if (!hall->isActive) {
            continue;
        }

        int totalSeats =
            hall->rows * hall->columns;

        int soldSeats = countSoldSeats(
            system,
            showtime->id
        );

        int remainingSeats =
            totalSeats - soldSeats;

        /*
         * Defensive check in case inconsistent
         * ticket data causes the number to go below zero.
         */
        if (remainingSeats < 0) {
            remainingSeats = 0;
        }

        if (!found) {
            printf(
                "\n========== Active Showtimes for \"%s\" ==========\n",
                movie->title
            );
        }

        printf(
            "\nShowtime ID: %d\n",
            showtime->id
        );

        printf(
            "Hall: %s\n",
            hall->name
        );

        printf(
            "Date: %s\n",
            showtime->date
        );

        printf(
            "Time: %s\n",
            showtime->time
        );

        printf(
            "Base Price: %.2f\n",
            showtime->ticketPrice
        );

        printf(
            "Remaining Seats: %d / %d\n",
            remainingSeats,
            totalSeats
        );

        printf(
            "--------------------------------\n"
        );

        found = 1;
    }

    if (!found) {
        printf(
            "No active showtimes are available "
            "for this movie.\n"
        );
    }
}

void listAvailableDiscounts(const System *system)
{
    if (system->discountCount == 0) {
        printf(
            "No discounts have been registered.\n"
        );
        return;
    }

    char currentDate[11];

    if (!getCurrentDate(currentDate)) {
        printf(
            "The current date could not be determined.\n"
        );
        return;
    }

    long long today =
        dateToDayNumber(currentDate);

    int found = 0;

    for (int i = 0; i < system->discountCount; i++) {
        const Discount *discount =
            &system->discounts[i];

        if (!discount->isActive) {
            continue;
        }

        if (
            discount->timesUsed >=
            discount->usageLimit
        ) {
            continue;
        }

        long long startDate =
            dateToDayNumber(
                discount->startDate
            );

        long long endDate =
            dateToDayNumber(
                discount->endDate
            );

        if (
            today < startDate ||
            today > endDate
        ) {
            continue;
        }

        if (!found) {
            printf(
                "\n========== Available Discounts ==========\n"
            );
        }

        printf(
            "\nCode: %s\n",
            discount->code
        );

        printf(
            "Percentage: %.2f%%\n",
            discount->percentage
        );

        printf(
            "Minimum Price: %.2f\n",
            discount->minimumPrice
        );

        printf(
            "Valid Until: %s\n",
            discount->endDate
        );

        printf(
            "Remaining Uses: %d\n",
            discount->usageLimit -
            discount->timesUsed
        );

        printf(
            "--------------------------------\n"
        );

        found = 1;
    }

    if (!found) {
        printf(
            "No discounts are currently available.\n"
        );
    }
}

void purchaseTicket(System *system)
{
    printf("\n========== Purchase Ticket ==========\n");

    if (system->userCount == 0) {
        printf("No users have been registered.\n");
        return;
    }

    if (system->showtimeCount == 0) {
        printf("No showtimes have been registered.\n");
        return;
    }

    int activeShowtimes = 0;

    printf("\n========== Available Showtimes ==========\n");

    for (int i = 0; i < system->showtimeCount; i++) {
        const Showtime *showtime =
            &system->showtimes[i];

        if (!showtime->isActive) {
            continue;
        }

        int movieIndex = findMovieIndexById(
            system,
            showtime->movieId
        );

        int hallIndex = findHallIndexById(
            system,
            showtime->hallId
        );

        if (
            movieIndex == -1 ||
            hallIndex == -1
        ) {
            continue;
        }

        if (
            !system->movies[movieIndex].isActive ||
            !system->halls[hallIndex].isActive
        ) {
            continue;
        }

        int totalSeats =
            system->halls[hallIndex].rows *
            system->halls[hallIndex].columns;

        int remainingSeats =
            totalSeats -
            countSoldSeats(
                system,
                showtime->id
            );
        
        if (remainingSeats <= 0) {
            continue;
        }

        printf(
            "\nID: %d | Movie: %s | Hall: %s\n",
            showtime->id,
            system->movies[movieIndex].title,
            system->halls[hallIndex].name
        );

        printf(
            "Date: %s | Time: %s | Base Price: %.2f\n",
            showtime->date,
            showtime->time,
            showtime->ticketPrice
        );

        printf(
            "Remaining Seats: %d / %d\n",
            remainingSeats,
            totalSeats
        );

        activeShowtimes++;
    }

    if (activeShowtimes == 0) {
        printf("No active showtimes are available.\n");
        return;
    }

    Ticket newTicket;

    newTicket.id = readIntInRange(
        "New ticket ID: ",
        1,
        1000000
    );

    if (
        findTicketIndexById(
            system,
            newTicket.id
        ) != -1
    ) {
        printf("A ticket with this ID already exists.\n");
        return;
    }

    const User *currentUser =
    getCurrentUser(system);

if (currentUser == NULL) {
    printf("You must be logged in to purchase a ticket.\n");
    return;
}

int userIndex;

if (currentUser->role == CUSTOMER_ROLE) {
    userIndex = system->currentUserIndex;
    newTicket.userId = currentUser->id;
}
else {
    newTicket.userId = readIntInRange(
        "Customer user ID: ",
        1,
        1000000
    );

    userIndex = findUserIndexById(
        system,
        newTicket.userId
    );

    if (userIndex == -1) {
        printf("User not found.\n");
        return;
    }
}

User *user = &system->users[userIndex];

if (!user->isActive) {
    printf("This user account is inactive.\n");
    return;
}

if (user->role != CUSTOMER_ROLE) {
    printf(
        "Tickets can only be purchased for customer accounts.\n"
    );
    return;
}

    newTicket.showtimeId = readIntInRange(
        "Showtime ID: ",
        1,
        1000000
    );

    int showtimeIndex = findShowtimeIndexById(
        system,
        newTicket.showtimeId
    );

    if (showtimeIndex == -1) {
        printf("Showtime not found.\n");
        return;
    }

    Showtime *showtime =
        &system->showtimes[showtimeIndex];

    if (!showtime->isActive) {
        printf("This showtime is inactive.\n");
        return;
    }

    int movieIndex = findMovieIndexById(
        system,
        showtime->movieId
    );

    int hallIndex = findHallIndexById(
        system,
        showtime->hallId
    );

    if (
        movieIndex == -1 ||
        hallIndex == -1
    ) {
        printf(
            "The movie or hall related to this showtime "
            "was not found.\n"
        );
        return;
    }

    Movie *movie =
        &system->movies[movieIndex];

    Hall *hall =
        &system->halls[hallIndex];

    if (!movie->isActive) {
        printf("The related movie is inactive.\n");
        return;
    }

    if (!hall->isActive) {
        printf("The related hall is inactive.\n");
        return;
    }

    int totalSeats =
        hall->rows * hall->columns;

    displaySeatMap(
        system,
        newTicket.showtimeId
    );

    newTicket.seatNumber = readIntInRange(
        "Select seat number: ",
        1,
        totalSeats
    );

    if (
        isSeatSold(
            system,
            newTicket.showtimeId,
            newTicket.seatNumber
        )
    ) {
        printf("This seat has already been sold.\n");
        return;
    }

    int seatRow =
        (newTicket.seatNumber - 1)
        / hall->columns
        + 1;

    int firstVipRow =
        hall->rows - hall->vipRows + 1;

    int isVipSeat =
        hall->vipRows > 0 &&
        seatRow >= firstVipRow;

    newTicket.originalPrice =
        showtime->ticketPrice;

    if (isVipSeat) {
        newTicket.originalPrice *=
            VIP_PRICE_MULTIPLIER;
    }

    int hour;
    sscanf(showtime->time, "%d", &hour);
    if (hour < 12) {
        newTicket.originalPrice *= MORNING_PRICE_MULTIPLIER;
    }

    const char *facilities = hall->facilities;
    float facilityMultiplier = 1.0f;

    if (strstr(facilities, "IMAX") || strstr(facilities, "imax")) {
        facilityMultiplier = 1.30f;
    }
    else if (strstr(facilities, "3D") || strstr(facilities, "3d")) {
        facilityMultiplier = 1.25f;
    }
    else if (strstr(facilities, "Dolby") || strstr(facilities, "dolby")) {
        facilityMultiplier = 1.15f;
    }

    newTicket.originalPrice *= facilityMultiplier;


    newTicket.discountId =
        NO_DISCOUNT_ID;

    newTicket.finalPrice =
        newTicket.originalPrice;

    strcpy(
        newTicket.buyerName,
        user->fullName
    );

    strcpy(
        newTicket.buyerPhone,
        user->phone
    );

    if (!getCurrentDate(newTicket.purchaseDate)) {
        readDate(
            "Enter purchase date (YYYY-MM-DD): ",
            newTicket.purchaseDate
        );
    }

    printf(
        "\nSeat type: %s\n",
        isVipSeat ? "VIP" : "Regular"
    );

    printf(
        "Price before discount: %.2f\n",
        newTicket.originalPrice
    );

    int useDiscount = readIntInRange(
        "Apply a discount? 1 = Yes, 0 = No: ",
        0,
        1
    );

    int discountIndex = -1;

    if (useDiscount) {
        char discountCode[30];

        readString(
            "Discount code: ",
            discountCode,
            sizeof(discountCode)
        );

        discountIndex = findDiscountIndexByCode(
            system,
            discountCode
        );

        if (discountIndex == -1) {
            printf(
                "Discount code was not found. "
                "The ticket will continue without a discount.\n"
            );
        }
        else {
            Discount *discount =
                &system->discounts[discountIndex];

            long long purchaseDay =
                dateToDayNumber(
                    newTicket.purchaseDate
                );

            long long discountStart =
                dateToDayNumber(
                    discount->startDate
                );

            long long discountEnd =
                dateToDayNumber(
                    discount->endDate
                );

            if (!discount->isActive) {
                printf(
                    "This discount is inactive. "
                    "No discount was applied.\n"
                );

                discountIndex = -1;
            }
            else if (
                discount->timesUsed >=
                discount->usageLimit
            ) {
                printf(
                    "This discount has reached its usage limit. "
                    "No discount was applied.\n"
                );

                discountIndex = -1;
            }
            else if (
                purchaseDay < discountStart ||
                purchaseDay > discountEnd
            ) {
                printf(
                    "This discount is not valid on the "
                    "purchase date.\n"
                );

                discountIndex = -1;
            }
            else if (
                newTicket.originalPrice <
                discount->minimumPrice
            ) {
                printf(
                    "This discount requires a minimum price "
                    "of %.2f.\n",
                    discount->minimumPrice
                );

                discountIndex = -1;
            }
            else {
                float discountAmount =
                    newTicket.originalPrice
                    * discount->percentage
                    / 100.0f;

                newTicket.finalPrice =
                    newTicket.originalPrice
                    - discountAmount;

                newTicket.discountId =
                    discount->id;

                discount->timesUsed++;

                printf(
                    "Discount applied: %.2f%%\n",
                    discount->percentage
                );
            }
        }
    }

    newTicket.isCancelled = 0;

    printf(
        "Final price: %.2f\n",
        newTicket.finalPrice
    );

    int confirmation = readIntInRange(
        "Confirm purchase? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (!confirmation) {
        if (discountIndex != -1) {
            Discount *discount =
                &system->discounts[discountIndex];

            if (discount->timesUsed > 0) {
                discount->timesUsed--;
            }
        }

        printf("Purchase cancelled.\n");
        return;
    }

    if (!ensureTicketCapacity(system)) {
        if (discountIndex != -1) {
            Discount *discount =
                &system->discounts[discountIndex];

            if (discount->timesUsed > 0) {
                discount->timesUsed--;
            }
        }

        return;
    }

    system->tickets[system->ticketCount] =
        newTicket;

    system->ticketCount++;

    if (!saveTickets(system)) {
        printf(
            "Ticket was added to memory, "
            "but saving failed.\n"
        );
        return;
    }

    if (!saveDiscounts(system)) {
        printf(
            "Ticket was saved, but discount usage "
            "could not be saved.\n"
        );
    }

    printf("\n========== Purchase Successful ==========\n");

    displayTicketDetails(
        system,
        &system->tickets[
            system->ticketCount - 1
        ]
    );
}

void cancelTicket(System *system)
{
    if (system->ticketCount == 0) {
        printf("No tickets have been registered.\n");
        return;
    }

    int ticketId = readIntInRange(
        "Enter ticket ID to cancel: ",
        1,
        1000000
    );

    int index = findTicketIndexById(
        system,
        ticketId
    );

    if (index == -1) {
        printf("Ticket not found.\n");
        return;
    }

    Ticket *ticket =
        &system->tickets[index];

    if (ticket->isCancelled) {
        printf("This ticket is already cancelled.\n");
        return;
    }

    displayTicketDetails(
        system,
        ticket
    );

    int confirmation = readIntInRange(
        "Confirm cancellation? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (!confirmation) {
        printf("Cancellation stopped.\n");
        return;
    }

    ticket->isCancelled = 1;

    if (ticket->discountId != NO_DISCOUNT_ID) {
        int discountIndex = findDiscountIndexById(
            system,
            ticket->discountId
        );

        if (discountIndex != -1) {
            Discount *discount =
                &system->discounts[discountIndex];

            if (discount->timesUsed > 0) {
                discount->timesUsed--;
            }
        }
    }

    if (!saveTickets(system)) {
        printf(
            "Ticket was cancelled in memory, "
            "but saving failed.\n"
        );
        return;
    }

    if (!saveDiscounts(system)) {
        printf(
            "Ticket was cancelled, but discount usage "
            "could not be saved.\n"
        );
    }

    printf(
        "Ticket cancelled successfully. "
        "The seat is available again.\n"
    );
}

void listMyTickets(const System *system)
{
    const User *currentUser =
        getCurrentUser(system);

    if (currentUser == NULL) {
        printf("You must be logged in.\n");
        return;
    }

    if (currentUser->role != CUSTOMER_ROLE) {
        printf(
            "This option is only available to customers.\n"
        );
        return;
    }

    int found = 0;

    for (int i = 0; i < system->ticketCount; i++) {
        const Ticket *ticket =
            &system->tickets[i];

        if (ticket->userId != currentUser->id) {
            continue;
        }

        if (!found) {
            printf(
                "\n========== My Tickets ==========\n"
            );
        }

        displayTicketDetails(
            system,
            ticket
        );

        found = 1;
    }

    if (!found) {
        printf(
            "You have not purchased any tickets.\n"
        );
    }
}

void cancelMyTicket(System *system)
{
    const User *currentUser =
        getCurrentUser(system);

    if (currentUser == NULL) {
        printf("You must be logged in.\n");
        return;
    }

    if (currentUser->role != CUSTOMER_ROLE) {
        printf(
            "This option is only available to customers.\n"
        );
        return;
    }

    int ticketId = readIntInRange(
        "Enter ticket ID to cancel: ",
        1,
        1000000
    );

    int ticketIndex = findTicketIndexById(
        system,
        ticketId
    );

    if (ticketIndex == -1) {
        printf("Ticket not found.\n");
        return;
    }

    Ticket *ticket =
        &system->tickets[ticketIndex];

    if (ticket->userId != currentUser->id) {
        printf(
            "You cannot cancel another customer's ticket.\n"
        );
        return;
    }

    if (ticket->isCancelled) {
        printf(
            "This ticket is already cancelled.\n"
        );
        return;
    }

    displayTicketDetails(
        system,
        ticket
    );

    int confirmation = readIntInRange(
        "Confirm cancellation? 1 = Yes, 0 = No: ",
        0,
        1
    );

    if (!confirmation) {
        printf("Cancellation stopped.\n");
        return;
    }

    ticket->isCancelled = 1;

    if (ticket->discountId != NO_DISCOUNT_ID) {
        int discountIndex = findDiscountIndexById(
            system,
            ticket->discountId
        );

        if (discountIndex != -1) {
            Discount *discount =
                &system->discounts[discountIndex];

            if (discount->timesUsed > 0) {
                discount->timesUsed--;
            }
        }
    }

    if (!saveTickets(system)) {
        printf(
            "Ticket was cancelled in memory, "
            "but saving failed.\n"
        );
        return;
    }

    if (!saveDiscounts(system)) {
        printf(
            "Ticket was cancelled, but discount usage "
            "could not be saved.\n"
        );
    }

    printf(
        "Ticket cancelled successfully. "
        "The seat is available again.\n"
    );
}

// ========================================
int main(void)
{
    System system;

    initializeSystem(&system);
    
    if (!createDataFiles()) {
        printf("Warning: Some data files could not be created.\n");
    }

    // load section
    if (!loadUsers(&system)) {
        printf("Warning: Users could not be loaded.\n");
    }

    if (!loadMovies(&system)) {
        printf("Warning: Movies could not be loaded.\n");
    }

    if (!loadHalls(&system)) {
        printf("Warning: Halls could not be loaded.\n");
    }

    if (!loadShowtimes(&system)) {
        printf("Warning: Showtimes could not be loaded.\n");
    }

    if (!loadDiscounts(&system)) {
        printf("Warning: Discounts could not be loaded.\n");
    }
    
    if (!loadTickets(&system)) {
        printf("Warning: Tickets could not be loaded.\n");
    }

    // Menu
    mainMenu(&system);

    // save section
    if (!saveUsers(&system)) {
        printf("Warning: Users could not be saved.\n");
    }

    if (!saveMovies(&system)) {
        printf("Warning: Movies could not be saved.\n");
    }

    if (!saveHalls(&system)) {
        printf("Warning: Halls could not be saved.\n");
    }

    if (!saveShowtimes(&system)) {
        printf("Warning: Showtimes could not be saved.\n");
    }

    if (!saveDiscounts(&system)) {
        printf("Warning: Discounts could not be saved.\n");
    }

    if (!saveTickets(&system)) {
        printf(
            "Warning: Tickets could not be saved.\n"
        );
    }

    freeSystem(&system);

    return 0;
}