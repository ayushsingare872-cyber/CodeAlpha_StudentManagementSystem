#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "students.dat"
#define TEMP_FILE "temp.dat"

typedef struct {
    int   roll;
    char  name[50];
    float marks;
} Student;

/* ---------- helpers ---------- */
void clearInput(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

int readInt(const char *prompt) {
    int n;
    printf("%s", prompt);
    while (scanf("%d", &n) != 1) {
        clearInput();
        printf("Invalid number. %s", prompt);
    }
    clearInput();
    return n;
}

float readFloat(const char *prompt) {
    float f;
    printf("%s", prompt);
    while (scanf("%f", &f) != 1) {
        clearInput();
        printf("Invalid number. %s", prompt);
    }
    clearInput();
    return f;
}

void readName(char *dest, int size) {
    printf("Enter name: ");
    fgets(dest, size, stdin);
    dest[strcspn(dest, "\n")] = '\0';
}

int rollExists(int roll) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Student s;
    if (!fp) return 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

void printHeader(void) {
    printf("\n%-8s %-25s %-8s\n", "Roll", "Name", "Marks");
    printf("---------------------------------------------\n");
}

void printStudent(const Student *s) {
    printf("%-8d %-25s %-8.2f\n", s->roll, s->name, s->marks);
}

/* ---------- features ---------- */
void addStudent(void) {
    Student s;
    s.roll = readInt("Enter roll number: ");
    if (rollExists(s.roll)) {
        printf("Error: Roll number already exists.\n");
        return;
    }
    readName(s.name, sizeof(s.name));
    s.marks = readFloat("Enter marks: ");

    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) { printf("Error opening file.\n"); return; }
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("Record added successfully.\n");
}

void displayStudents(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Student s;
    int count = 0;

    if (!fp) { printf("No records found.\n"); return; }
    printHeader();
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printStudent(&s);
        count++;
    }
    fclose(fp);
    if (count == 0) printf("No records found.\n");
    else printf("\nTotal records: %d\n", count);
}

void searchStudent(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    Student s;
    int roll, found = 0;

    if (!fp) { printf("No records found.\n"); return; }
    roll = readInt("Enter roll number to search: ");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            printHeader();
            printStudent(&s);
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (!found) printf("Student with roll %d not found.\n", roll);
}

void updateStudent(void) {
    FILE *fp = fopen(FILE_NAME, "rb+");
    Student s;
    int roll, found = 0;

    if (!fp) { printf("No records found.\n"); return; }
    roll = readInt("Enter roll number to update: ");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) {
            printf("Current record:");
            printHeader();
            printStudent(&s);
            printf("\nEnter new details (roll number stays the same)\n");
            readName(s.name, sizeof(s.name));
            s.marks = readFloat("Enter marks: ");
            fseek(fp, -(long)sizeof(Student), SEEK_CUR);
            fwrite(&s, sizeof(Student), 1, fp);
            found = 1;
            break;
        }
    }
    fclose(fp);
    if (found) printf("Record updated successfully.\n");
    else printf("Student with roll %d not found.\n", roll);
}

void deleteStudent(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *tmp;
    Student s;
    int roll, found = 0;

    if (!fp) { printf("No records found.\n"); return; }
    tmp = fopen(TEMP_FILE, "wb");
    if (!tmp) { fclose(fp); printf("Error creating temp file.\n"); return; }

    roll = readInt("Enter roll number to delete: ");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.roll == roll) found = 1;
        else fwrite(&s, sizeof(Student), 1, tmp);
    }
    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(FILE_NAME);
        rename(TEMP_FILE, FILE_NAME);
        printf("Record deleted successfully.\n");
    } else {
        remove(TEMP_FILE);
        printf("Student with roll %d not found.\n", roll);
    }
}

/* ---------- main ---------- */
int main(void) {
    int choice;

    do {
        printf("\n===== Student Management System =====\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Update Student\n");
        printf("5. Delete Student\n");
        printf("6. Exit\n");
        choice = readInt("Enter your choice: ");

        switch (choice) {
            case 1: addStudent();      break;
            case 2: displayStudents(); break;
            case 3: searchStudent();   break;
            case 4: updateStudent();   break;
            case 5: deleteStudent();   break;
            case 6: printf("Goodbye!\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 6);

    return 0;
}
