#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BusSeat {
    int seatNumber;
    int isBooked;
    char passengerName[20];
};

struct Bus {
    struct BusSeat *seats;
    int capacity;
};

int countAvailable(struct Bus *bus) {
    int count = 0;
    int i = 0;
    while(i < bus->capacity) {
        if(!bus->seats[i].isBooked) count++;
        i++;
    }
    return count;
}

void initializeBus(struct Bus *bus, int capacity) {
    bus->capacity = capacity;
    bus->seats = (struct BusSeat*)malloc(capacity * sizeof(struct BusSeat));
    if(bus->seats == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    int idx = 0;
    while(idx < capacity) {
        bus->seats[idx].seatNumber = idx + 1;
        bus->seats[idx].isBooked = 0;
        strcpy(bus->seats[idx].passengerName, "");
        idx++;
    }
}

void freeSeats(struct Bus *bus) {
    if(bus->seats != NULL) {
        free(bus->seats);
        bus->seats = NULL;
    }
    bus->capacity = 0;
}

void saveBusToFile(struct Bus *bus, char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) return;

    fprintf(file, "%d\n", bus->capacity);
    int j = 0;
    while(j < bus->capacity) {
        fprintf(file, "%d %d %s\n",
                bus->seats[j].seatNumber,
                bus->seats[j].isBooked,
                bus->seats[j].passengerName);
        j++;
    }
    fclose(file);
}

int loadBusFromFile(struct Bus *bus, char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 0;

    int cap;
    if(fscanf(file, "%d", &cap) != 1 || cap != bus->capacity) {
        fclose(file);
        return 0;
    }

    freeSeats(bus);
    bus->seats = (struct BusSeat*)malloc(bus->capacity * sizeof(struct BusSeat));
    if(bus->seats == NULL) return 0;

    int k = 0;
    while(k < bus->capacity) {
        int seatNum, booked;
        char passenger[20] = "";
        if(fscanf(file, "%d %d %s", &seatNum, &booked, passenger) == 3) {
            bus->seats[k].seatNumber = seatNum;
            bus->seats[k].isBooked = booked;
            strcpy(bus->seats[k].passengerName, passenger);
        } else {
            bus->seats[k].isBooked = 0;
            strcpy(bus->seats[k].passengerName, "");
        }
        k++;
    }

    fclose(file);
    return 1;
}

void displayAvailableSeats(struct Bus *bus) {
    printf("\nAvailable Seats: ");
    int availCount = countAvailable(bus);
    int m = 0;
    while(m < bus->capacity) {
        if(!bus->seats[m].isBooked) {
            printf("%d ", bus->seats[m].seatNumber);
        }
        m++;
    }
    if(availCount == 0) printf("None");
    printf("\nTotal Available Seats: %d\n", availCount);
}

int bookSeat(struct Bus *bus, int seatNo, char name[]) {
    if(seatNo < 1 || seatNo > bus->capacity) return 0;

    struct BusSeat *seat = &bus->seats[seatNo - 1];
    if(seat->isBooked) return 0;

    seat->isBooked = 1;
    strcpy(seat->passengerName, name);
    return 1;
}

int cancelSeat(struct Bus *bus, int seatNo) {
    if(seatNo < 1 || seatNo > bus->capacity) return 0;

    struct BusSeat *seat = &bus->seats[seatNo - 1];
    if(!seat->isBooked) return 0;

    seat->isBooked = 0;
    strcpy(seat->passengerName, "");
    return 1;
}

void displayBusStatus(struct Bus *bus) {
    printf("\n-------------------------------------------------------------\n");
    printf("| Seat No |    Status    |         Passenger Name           |\n");
    printf("-------------------------------------------------------------\n");

    int n = 0;
    while(n < bus->capacity) {
        printf("|   %02d    | %-10s | %-30s |\n",
               bus->seats[n].seatNumber,
               bus->seats[n].isBooked ? "BOOKED" : "EMPTY",
               bus->seats[n].isBooked ? bus->seats[n].passengerName : "-");
        n++;
    }

    printf("-------------------------------------------------------------\n");
}

int main() {
    struct Bus myBus;
    int capacity = 42;
    int choice, seatNo;
    char name[20];

    initializeBus(&myBus, capacity);
    loadBusFromFile(&myBus, "bus_status.txt");

    while(1) {
        printf("\n=== BUS RESERVATION SYSTEM ===\n");
        printf("1. View available seats\n");
        printf("2. Book a seat\n");
        printf("3. Cancel a seat\n");
        printf("4. View full bus status\n");
        printf("5. Save and exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        while(getchar() != '\n');

        if(choice == 1) {
            displayAvailableSeats(&myBus);
        } else if(choice == 2) {
            printf("Enter seat number: ");
            scanf("%d", &seatNo);
            while(getchar() != '\n');
            printf("Enter passenger name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            if(bookSeat(&myBus, seatNo, name))
                printf("Seat %d booked for %s.\n", seatNo, name);
            else
                printf("Could not book seat %d.\n", seatNo);
        } else if(choice == 3) {
            printf("Enter seat number to cancel: ");
            scanf("%d", &seatNo);
            while(getchar() != '\n');
            if(cancelSeat(&myBus, seatNo))
                printf("Seat %d booking canceled.\n", seatNo);
            else
                printf("Could not cancel seat %d.\n", seatNo);
        } else if(choice == 4) {
            displayBusStatus(&myBus);
        } else if(choice == 5) {
            saveBusToFile(&myBus, "bus_status.txt");
            freeSeats(&myBus);
            break;
        } else {
            printf("Invalid option.\n");
        }
    }
    return 0;
}
