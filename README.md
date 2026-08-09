# HK-Cinema

A console-based **Cinema Management System written in C** for managing movies, halls, showtimes, customers, ticket booking, discounts, and sales reports.

The project focuses on applying core C programming concepts to a complete application with persistent file storage and dynamic memory management.

## Features

- Admin and customer authentication
- Movie, hall, showtime, user, and discount management
- Movie search by title, genre, and director
- Showtime scheduling and hall conflict detection
- Dynamic seat maps and seat availability
- Ticket purchasing and cancellation
- Discount codes with validity and usage limits
- Sales, revenue, occupancy, and buyer reports
- Persistent data storage using text files

## Technical Highlights

- Structures and pointers
- Dynamic memory using `malloc`, `realloc`, and `free`
- Dynamic arrays with capacity management
- File handling and persistent storage
- Input validation
- Date and time processing
- Searching and sorting
- Entity relationships using IDs
- Role-based program flow

## Project Structure

```text
HK-Cinema/
├── HK_Cinema.c
├── data/
│   ├── users.txt
│   ├── movies.txt
│   ├── halls.txt
│   ├── showtimes.txt
│   ├── tickets.txt
│   └── discounts.txt
├── README.md
├── .gitignore
└── LICENSE
```

### Single-File Design

The project was developed under a **single-source-file constraint**. To keep the code organized and modular, `HK_Cinema.c` is divided into clearly defined sections for memory management, validation, file handling, authentication, entity management, ticketing, and reporting.

In a larger application, these sections could be separated into individual `.c` and `.h` modules.

## Build and Run

Compile with GCC:

```bash
gcc HK_Cinema.c -o HK_Cinema
```

Run on Linux/macOS:

```bash
./HK_Cinema
```

Run on Windows:

```bash
HK_Cinema.exe
```

Run the program from the project root so it can access the `data/` directory.

## Known Limitations

- Some discount date/time edge cases require additional validation.
- Multiple ticket purchases for the same showtime may be restricted in some cases.
- Passwords are stored as plain text and are intended for demonstration purposes only.

## Future Improvements

- Split the application into multiple `.c` and `.h` modules
- Improve authentication and password security
- Improve multi-seat ticket purchasing
- Add automated testing
- Replace text-file storage with a database

## Author

**Ali Heidari**  
Electrical Engineering student.

## License

This project is available under the MIT License.
