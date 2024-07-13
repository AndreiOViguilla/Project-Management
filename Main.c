#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>

typedef struct {
    int Personnel_ID;
    char Personnel_Name[50];
    char Personnel_Username[50];
    char Personnel_Pass[50];
    int status;
    int access_level;
} Personnel;

typedef struct {
    int taskID;
    char name[100];
    int sequence;
    char description[100];
    int projectID;
    char startdate[100];
    char enddate[100];
    int totalduration;
    int status;
    int personnelID;
} Task;

typedef struct {
    int projectID;
    char name[100];
    int status;
    float completion;
    char description[100];
    int managerID;
    Task tasks[10];
} Project;

void login();
void admin_menu();
void user_menu();
void manager_menu();

void zero_out(Personnel* personnel, Task* task, Project* project) {
    int i, j;

    // this function makes every project with tasks 0
    for (i = 0; i < 10; i++) {
        project[i].projectID = 0;
        project[i].name[0] = '\0'; // if the declaration of the variable is a string or character it doesnt make it zero but a space
        project[i].status = 0;
        project[i].completion = 0.0;
        project[i].description[0] = '\0';
        project[i].managerID = 0;
        for (j = 0; j < 10; j++) {
            project[i].tasks[j].taskID = 0;
            project[i].tasks[j].name[0] = '\0';
            project[i].tasks[j].sequence = 0;
            project[i].tasks[j].description[0] = '\0';
            project[i].tasks[j].projectID = 0;
            project[i].tasks[j].startdate[0] = '\0';
            project[i].tasks[j].enddate[0] = '\0';
            project[i].tasks[j].totalduration = 0;
            project[i].tasks[j].status = 0;
            project[i].tasks[j].personnelID = 0;
        }
    }
    // this function makes every values in a personnel struct 0
    for (i = 0; i < 50; i++) {
        personnel[i].Personnel_ID = 0;
        personnel[i].Personnel_Name[0] = '\0';
        personnel[i].Personnel_Username[0] = '\0';
        personnel[i].Personnel_Pass[0] = '\0';
        personnel[i].status = 0;
        personnel[i].access_level = 0;
    }
    // this function makes every values in the task struct 0
    for (i = 0; i < 10; i++) {
        task[i].taskID = 0;
        task[i].name[0] = '\0';
        task[i].sequence = 0;
        task[i].description[0] = '\0';
        task[i].projectID = 0;
        task[i].startdate[0] = '\0';
        task[i].enddate[0] = '\0';
        task[i].totalduration = 0;
        task[i].status = 0;
        task[i].personnelID = 0;
    }
}

void exportPersonnel(Personnel* personnels, int* numpersonnel) {
    int i;
    char filename[100];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(filename, "personnel_%d%02d%02d.dat", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday); 
    FILE* file = fopen(filename, "w"); //
    if (file == NULL) {
        printf("Error: Could not create file %s.\n", filename);
        return;
    }
    for (i = 0; i < 50; i++) {
        Personnel personnel = personnels[i];
        if (strlen(personnel.Personnel_Name) != 0) {
            fprintf(file, "%d %s\n%s\n%s\n%d\n%d\n\n", personnel.Personnel_ID,
                    personnel.Personnel_Name, personnel.Personnel_Username, personnel.Personnel_Pass,
                    personnel.status, personnel.access_level);
        }
    }
    fclose(file);
    printf("Personnel exported successfully to files: %s, personnel.txt\n", filename);
}

void readPersonnelFromFile(Personnel* personnels, int* numpersonnel, char* fileget) {
    char dir_name[256];
    char file_name[256];
    struct dirent *dir_entry;
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }
    time_t current_time = time(NULL);
    struct tm *tm = localtime(&current_time);
    strftime(dir_name, sizeof(dir_name), "personnel_%Y%m%d", tm);
    int closest_date_diff = INT_MAX;
    while ((dir_entry = readdir(dir)) != NULL) {
        if (strncmp(dir_entry->d_name, "personnel_", 10) == 0) {
            // check if the file name starts with "personnel_"
            // and has ".dat" extension
            int len = strlen(dir_entry->d_name);
            if (len > 13 && strncmp(&dir_entry->d_name[len - 4], ".dat", 4) == 0) {
                // extract date from the file name
                int year, month, day;
                if (sscanf(&dir_entry->d_name[10], "%4d%2d%2d", &year, &month, &day) == 3) {
                    // calculate the difference between the date in the file name and current date
                    int date_diff = abs(year * 10000 + month * 100 + day - (tm->tm_year + 1900) * 10000 - (tm->tm_mon + 1) * 100 - tm->tm_mday);
                    if (date_diff < closest_date_diff) {
                        closest_date_diff = date_diff;
                        strcpy(file_name, dir_entry->d_name);
                    }
                }
            }
        }
    }
    closedir(dir);
    if (closest_date_diff == INT_MAX) {
        printf("No personnel file found\n");
        return;
    }
    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        printf("Error opening file: %s\n", file_name);
        return;
    }
    *numpersonnel = 0;
    Personnel personnel;
    while (fscanf(fp, "%d %s\n%s\n%s\n%d\n%d\n\n", &personnel.Personnel_ID,
                  personnel.Personnel_Name, personnel.Personnel_Username, personnel.Personnel_Pass,
                  &personnel.status, &personnel.access_level) == 6) {
        personnels[*numpersonnel] = personnel;
        (*numpersonnel)++;
    }
    fclose(fp);
    strcpy(fileget, file_name);
}

void Assigntask(Project* projects, int* numProjects, Personnel* personnels) {
    int taskID, personnelID;
    int i, j, taskFound, personnelFound;
    printf("Active Tasks:\n"); // finds that are active
    for (i = 0; i < 10; i++) { // loops in the projects
        for (j = 0; j < 10; j++) { // loops in the tasks
            if (projects[i].tasks[j].status == 1) { // if the status of the tasks are one it considered active, if ts 2 its archieved and it would be skipped by the program
                printf("%d: %s\n", projects[i].tasks[j].taskID, projects[i].tasks[j].name); // prints out active tasks given by the if statement
            }
        }
    }
    printf("Enter Task ID: ");
    scanf("%d", &taskID); // asks the user what would be the taskID to use for assigning a personnel
    taskFound = 0;
    for (i = 0; i < 10 && !taskFound; i++) {
        for (j = 0; j < 10 && !taskFound; j++) {
            if (projects[i].tasks[j].taskID == taskID) { // finds the taskID from the struct
                taskFound = 1; // this helps in breaking the loop if the program finds the exact personnel
            }
        }
    }
    if (!taskFound) {
        printf("Error: Task with ID %d not found.\n", taskID);
        return;
    }
    printf("Active Personnel:\n");
    for (i = 0; i < 50; i++) {
        if (personnels[i].status == 1 && personnels[i].access_level == 3) { // this function finds personnels that are active and are users, if they are archived or active. manager or admin won't be assigned to the task
            printf("%d: %s\n", personnels[i].Personnel_ID, personnels[i].Personnel_Name);
        }
    }
    printf("Enter Personnel ID: ");
    scanf("%d", &personnelID);
    // asks the user for the personnelID which can be later use in finding the personnelID in struct
    personnelFound = 0;
    // this helps to break the loop if the personnel is found
    for (i = 0; i < 50 && !personnelFound; i++) {
        // loop from the personnel struct
        if (personnels[i].Personnel_ID == personnelID) {
            // finds the personnel
            personnelFound = 1;
        }
    }
    if (!personnelFound) {
        printf("Error: Personnel with ID %d not found.\n", personnelID); // prints if the loops stops while not finding any personnel ID
        return;
    }
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            if (projects[i].tasks[j].taskID == taskID && projects[i].tasks[j].status == 1) { // this helps finds tasks that are status 1 and early on the taskID that needs to be assigned it by someone
                projects[i].tasks[j].personnelID = personnelID; // this changes the values of the task's personnelID so that there would be someone assigned to it
                projects[i].tasks[j].status = 2; // this changes the status of the tasks's to be "In Progress" so that when assigning a project again this wont pop up in this function
                printf("Personnel with ID %d assigned to Task %d.\n", personnelID, taskID);
                return;
            }
        }
    }
    printf("Error: No available Task.\n"); // prints if there is no tasks found
}

void showDelayedTasks(Project* projects, int numProjects, int numTask) {
    // this function shows the delayed task of the enddate is earlier than today
    int i, j;
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now); // convert the time value to a struct tm value
    char date[100];
    strftime(date, 100, "%m/%d/%Y", current_time); // format the current time as a date string

    printf("Delayed Tasks:\n");
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            char *enddate = projects[i].tasks[j].enddate;
            struct tm tm_enddate = {0};
            sscanf(enddate, "%d/%d/%d", &tm_enddate.tm_mon, &tm_enddate.tm_mday, &tm_enddate.tm_year); // reads the string format and puts the corresponding values to the desired variables
            tm_enddate.tm_year -= 1900;
            tm_enddate.tm_mon -= 1;
            time_t enddate_t = mktime(&tm_enddate); // mktime converts local calendar time to time since epoch
            double timcal = difftime(now, enddate_t); // finds the difference of the now and the end_date time, timcal is in negatives it is considered as delayed tasks

            if (timcal > 0 && projects[i].tasks[j].status != 3 && (projects[i].tasks[j].status == 1 || projects[i].tasks[j].status == 2)) { // it makes sure that status of the project is 1 or 2, since 3 is considered aas a finished task
                printf("Task ID: %d\n", projects[i].tasks[j].taskID); // displays the projects' taskss info.
                printf("Name: %s\n", projects[i].tasks[j].name);
                printf("Project ID: %d\n", projects[i].tasks[j].projectID);
                printf("End Date: %s\n", projects[i].tasks[j].enddate);
                printf("Status: %d\n\n", projects[i].tasks[j].status);
            } // this shows only if the timecal is negative and if the tasks are not yet done. If they are done then they are considered not delayed
        }
    }
}

int countTasks(Project project) {
    // this function counts how many tasks are there in a project
    int numTasks = 0;
    while (project.tasks[numTasks].taskID != 0) {
        numTasks++;
    }
    return numTasks; // returns to how many tasks did it scan
}

void updateProjectCompletion(Project* projects, int projectID) {
    // only this function can change the project completion
    int numTasks = 0; // this determines how many tasks are there
    int numDoneTasks = 0; // this determines how many finished tasks are there
    int i = 0;
    int found = 0; // this helps in breaking the loop
    while (i < 10) {
        if (projects[i].projectID == projectID) { // checks whether the user input has the project ID indicated
            found = 1; // this helps break the code
            int j = 0;
            while (j < 10 && projects[i].tasks[j].taskID > 0) { // if the task id is zero it doesnt consider it as a task
                numTasks++; // this increments on how many tasks are there in a project if its found
                if (projects[i].tasks[j].status == 3) {
                    numDoneTasks++; // this increments if there is a finished task done
                }
                j++; // increases until it reaches the desired number
            }
            if (numTasks > 0) {
                projects[i].completion = ((float)numDoneTasks / numTasks) * 100; // this solves the number of tasks there are and completed tasks, solves for its completion or percentage
                if (projects[i].completion == 100.00) {
                    projects[i].status = 3; // if the value of the completion is 100% then this would mark the project as done
                } else if (projects[i].completion < 100.00) { // if the project is less than 100% it is not considered as done
                    projects[i].status = 1; // if the value of the completion is less than 100% then this would mark the project as not done
                }
                printf("There are a total of (%d) task/s, and with those tasks there are (%d) finished tasks", numTasks, numDoneTasks);
                printf("Project Completion updated to %.2f%%\n", projects[i].completion); // this updates the project completion
            } else {
                printf("Project has no tasks yet.\n"); // if the project does not have any task it will show this
            }
        }
        i++;
    }
    if (!found) {
        printf("Invalid project ID.\n"); // if the project id is not found then this will pop up
    }
    return;
}

void readTasksFromFile(Task* tasks, int* numTasks) {
    char dir_name[256];
    char file_name[256];
    struct dirent *dir_entry;
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }
    time_t current_time = time(NULL);
    struct tm *tm = localtime(&current_time);
    strftime(dir_name, sizeof(dir_name), "task_%Y%m%d", tm);
    int closest_date_diff = INT_MAX;
    while ((dir_entry = readdir(dir)) != NULL) {
        if (strncmp(dir_entry->d_name, "task_", 5) == 0) {
            // check if the file name starts with "task_"
            // and has ".txt" extension
            int len = strlen(dir_entry->d_name);
            if (len > 8 && strncmp(&dir_entry->d_name[len - 4], ".txt", 4) == 0) {
                // extract date from the file name
                int year, month, day;
                if (sscanf(&dir_entry->d_name[5], "%4d%2d%2d", &year, &month, &day) == 3) {
                    // calculate the difference between the date in the file name and current date
                    int date_diff = abs(year * 10000 + month * 100 + day - (tm->tm_year + 1900) * 10000 - (tm->tm_mon + 1) * 100 - tm->tm_mday);
                    if (date_diff < closest_date_diff) {
                        closest_date_diff = date_diff;
                        strcpy(file_name, dir_entry->d_name);
                    }
                }
            }
        }
    }
    closedir(dir);
    if (closest_date_diff == INT_MAX) {
        printf("No task file found\n");
        return;
    }
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error opening task file\n");
        return;
    }
    *numTasks = 0; // starts the task count on 0
    Task task;
    while (fscanf(fp, "%d %s\n%d\n%s\n%d\n%s\n%s\n%d\n%d\n%d\n\n", &task.taskID,
                  task.name, &task.sequence, task.description, &task.projectID, task.startdate,
                  task.enddate, &task.totalduration, &task.status, &task.personnelID) == 10) {
        *(tasks + *numTasks) = task;
        (*numTasks)++;
        // reads the task data from the text file and increment on "numTasks"
        // this function gets the number of tasks that are present inside the file (active tasks)
    }
    fclose(fp);
}

void readProjectsFromFile(Project* projects, int* numProjects) {
    char dir_name[256];
    char file_name[256];
    struct dirent *dir_entry;
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }
    time_t current_time = time(NULL);
    struct tm *tm = localtime(&current_time);
    strftime(dir_name, sizeof(dir_name), "project_%Y%m%d", tm);
    int closest_date_diff = INT_MAX;
    while ((dir_entry = readdir(dir)) != NULL) {
        if (strncmp(dir_entry->d_name, "project_", 8) == 0) {
            // check if the file name starts with "project_"
            // and has ".dat" extension
            int len = strlen(dir_entry->d_name);
            if (len > 11 && strncmp(&dir_entry->d_name[len - 4], ".txt", 4) == 0) {
                // extract date from the file name
                int year, month, day;
                if (sscanf(&dir_entry->d_name[8], "%4d%2d%2d", &year, &month, &day) == 3) {
                    // calculate the difference between the date in the file name and current date
                    int date_diff = abs(year * 10000 + month * 100 + day - (tm->tm_year + 1900) * 10000 - (tm->tm_mon + 1) * 100 - tm->tm_mday);
                    if (date_diff < closest_date_diff) {
                        closest_date_diff = date_diff;
                        strcpy(file_name, dir_entry->d_name);
                    }
                }
            }
        }
    }
    closedir(dir);
    if (closest_date_diff == INT_MAX) {
        printf("No project file found\n");
        return;
    }
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
        printf("Error opening project file\n");
        return;
    }
    *numProjects = 0; // starts the project count on 0
    Project project;
    while (fscanf(fp, "%d %s\n%d\n%f\n%s\n%d\n\n", &project.projectID,
                  project.name, &project.status, &project.completion, project.description,
                  &project.managerID) == 6) {
        *(projects + *numProjects) = project;
        (*numProjects)++;
        // reads the project data from the text file and increment on "numProjects"
        // this function gets the number of projects that are present inside the file (active projects)
    }
    fclose(fp);
}

void placeTaskInProject(Task* tasks, int numTasks, Project* projects, int numProjects) {
    int numTasksPerProject[numProjects];
    // turns all elements to 0
    int i, j;
    for (i = 0; i < numProjects; i++) {
        numTasksPerProject[i] = 0;
    }
    for (i = 0; i < numTasks; i++) {
        int tasksAdded = 0;
        // find a matching project ID
        for (j = 0; j < numProjects; j++) {
            // if the project ID of the current task matches the project ID of the current project and the current project has not exceeded the task limit
            if (tasks[i].projectID == projects[j].projectID && tasksAdded == 0 && numTasksPerProject[j] < 10) {
                // add the task to the current project
                projects[j].tasks[numTasksPerProject[j]] = tasks[i];
                // update the number of tasks added to the current project
                numTasksPerProject[j]++; // this helps breaking the loop
                tasksAdded = 1; // this break the if statement
            }
        }
        // print an error message if we couldn't find a project
        if (tasksAdded == 0) {
            printf("Error: No project found for task %d\n", tasks[i].taskID); // if there are tasks that does not have any project ID in the struct this will pop up
        }
    }
}

void exportProjects(Project* projects, int* numProjects) {
    // gets the current date
    char filename[100];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(filename, "project_%d%02d%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    // prints an error message if "filename" could not be created
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not create export file\n");
        return;
    }

    // goes through all the active projects and prints the contents into the export file
    int i;
    for (i = 0; i < *numProjects; i++) {
        Project project = projects[i]; // this place the values to the struct
        fprintf(file, "%d %s\n", project.projectID, project.name);
        fprintf(file, "%d\n", project.status);
        fprintf(file, "%f\n", project.completion);
        fprintf(file, "%s\n", project.description);
        fprintf(file, "%d\n\n", project.managerID);
    }
    fclose(file);
    printf("Projects exported successfully to file: %s.\n", filename);
}

void exportTasks(Project* projects, int *numProjects) {
    // gets the current date
    char filename[20];
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(filename, 20, "task_%Y%m%d.txt", timeinfo);

    // prints an error message if "filename" could not be created
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not create file %s.\n", filename);
        return;
    }
    // goes through all active tasks inside each project and prints them on the export file
    int i, j;

    for (i = 0; i < *numProjects; i++) {
        for (j = 0; j < 10; j++) {
            if (projects[i].tasks[j].taskID != 0) {
                fprintf(fp, "%d %s\n%d\n%s\n%d\n%s\n%s\n%d\n%d\n%d\n\n",
                        projects[i].tasks[j].taskID, projects[i].tasks[j].name,
                        projects[i].tasks[j].sequence, projects[i].tasks[j].description,
                        projects[i].tasks[j].projectID,
                        projects[i].tasks[j].startdate,
                        projects[i].tasks[j].enddate, projects[i].tasks[j].totalduration,
                        projects[i].tasks[j].status, projects[i].tasks[j].personnelID);
            }
        }
    }

    fclose(fp);
    printf("Tasks exported successfully to file: %s.\n", filename); // prints when tasks is successfully exported or created a new file
}

int getNextTaskID(Project *projects) {
    int i;
    int j;
    int maxID = 0;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            if (projects[i].tasks[j].taskID > maxID) {
                maxID = projects[i].tasks[j].taskID;
            }
        }
    }
    return maxID + 1; // this changes the task ID of the
}

int calculateTotalDuration(char* startdate, char* enddate) {
    // two empty struct tm variables to represent the start and end dates.
    struct tm start = {0}, end = {0};
    // initialize variables to hold the day, month, and year values for the start and end dates.
    int start_day, start_month, start_year, end_day, end_month, end_year;
    // the start date string into its individual day, month, and year components using sscanf.
    if (sscanf(startdate, "%d/%d/%d", &start_month, &start_day, &start_year) != 3) {
        // if the sscanf call returns a value other than 3, it means the start date string was not in the expected format.
        // print an error message and return -1 to indicate failure.
        printf("Error: Invalid start date format.\n");
        return -1;
    }
    // set the day, month, and year fields of the start struct tm variable based on the parsed values.
    start.tm_mday = start_day;
    start.tm_mon = start_month - 1; // months in struct tm are zero-indexed, so we subtract 1 from the parsed month value.
    start.tm_year = start_year - 1900; // years in struct tm are relative to 1900.
    // the end date string and set the fields of the end struct tm variable in the same way as for the start date.
    if (sscanf(enddate, "%d/%d/%d", &end_month, &end_day, &end_year) != 3) {
        printf("Error: Invalid end date format.\n");
        return -1;
    }
    end.tm_mday = end_day;
    end.tm_mon = end_month - 1;
    end.tm_year = end_year - 1900;
    // mktime function to convert the start and end struct tm variables into timestamps (i.e., seconds since January 1, 1970).
    time_t start_secs = mktime(&start);
    time_t end_secs = mktime(&end);
    // calculate the number of days between the start and end timestamps using difftime, and convert the result to an integer.
    int days = ((int) difftime(end_secs, start_secs) / (60 * 60 * 24));
    // return the number of days as the output of the function.
    return days;
}

void showTasktoUpdate(Project* projects, int* numProjects, int *numTask) {
    // Initialize variables for counting the total number of tasks.
    int i, j;
    int taskCount = 0;
    // Iterate through all projects and count the number of tasks in each project.
    for (i = 0; i < *numProjects; i++) {
        taskCount += countTasks(projects[i]);
    }
    // If no tasks were found, print a message and return.
    if (taskCount == 0) {
        printf("No tasks found.\n");
        return;
    }
    // Otherwise, print a header indicating that the following details are for tasks.
    printf("==== Task Details ====\n");
    // Iterate through all projects and print details of each task in each project.
    for (i = 0; i < *numProjects; i++) {
        // Count the number of tasks in the current project.
        int numTasks = countTasks(projects[i]);
        // Iterate through all tasks in the current project and print their details.
        for (j = 0; j < numTasks; j++) {
            // Print the task ID and name for the current task.
            printf("%d. Task ID: %d\n", j+1, projects[i].tasks[j].taskID);
            printf("Name: %s\n", projects[i].tasks[j].name);
        }
    }
}

void showProjectDetails(Project* projects, int* numProjects, int *numTask) {
    // prints an error message if there are currently no active projects
    if (*numProjects == 0) {
        printf("No projects found.\n");
        return;
    }
    printf("\n\n");
    // Loop through all active projects and tasks and display them on screen
    int i, j;
    printf("==== Project Details ====\n");

    for (i = 0; i < *numProjects; i++) {
        // Display project information
        printf("Project ID: %d\n", projects[i].projectID);
        printf("Name: %s\n", projects[i].name);
        printf("Status: %d\n", projects[i].status);
        printf("Completion: %.2f%%\n", projects[i].completion);
        printf("Description: %s\n", projects[i].description);
        printf("Manager ID: %d\n", projects[i].managerID);
        // Count the number of tasks for the current project
        int numTasks = countTasks(projects[i]);
        printf("Tasks (%d):\n", numTasks);
        // Loop through all tasks for the current project and display their information
        for (j = 0; j < numTasks; j++) {
            printf("Task ID: %d\n", projects[i].tasks[j].taskID);
            printf("Name: %s\n", projects[i].tasks[j].name);
            printf("Sequence: %d\n", projects[i].tasks[j].sequence);
            printf("Description: %s\n", projects[i].tasks[j].description);
            printf("Start Date: %s\n", projects[i].tasks[j].startdate);
            printf("End Date: %s\n", projects[i].tasks[j].enddate);
            // If the total duration is not -1 (i.e. it has been set), display it
            if (projects[i].tasks[j].totalduration != -1) {
                printf("Total Duration: %d\n", projects[i].tasks[j].totalduration);
            }
            // Display the current status and personnel ID for the task
            printf("Status: %d\n", projects[i].tasks[j].status);
            printf("Personnel ID: %d\n", projects[i].tasks[j].personnelID);
        }
        printf("\n");
    }
    // Display the manager menu
    manager_menu(projects, numProjects, numTask);
}

void addTask(Project* projects, int* numProjects, int *numTask) {
    int projectID, taskCount, i;
    printf("==== All Projects ====\n");
    for (i = 0; i < *numProjects; i++) { // goes through all active projects and tasks and displays them on screen
        printf("Project ID: %d\n", projects[i].projectID);
        printf("Name: %s\n", projects[i].name);
    }
    printf("\n\n");
    printf("Enter project ID: "); // gets the project ID so that the new task will be assigned there
    scanf("%d", &projectID);
    int projectFound = 0;
    for (i = 0; i < *numProjects; i++) {
        if (projects[i].projectID == projectID) { // finds the project with a matching ID
            projectFound = 1;
            taskCount = 0;
            // Finds the next available slot for a new task within the project.
            while (projects[i].tasks[taskCount].taskID != 0 && taskCount < 10) {
                taskCount++;
            }
            if (taskCount >= 10) { // prints an error message if the project contains the maximum amount of tasks (10)
                printf("Error: Project already has 10 tasks.\n\n");
                return;
            }

            // Assigns the new task to the project.
            Task newTask;
            newTask.taskID = *numTask + 1;
            newTask.projectID = projectID;
            newTask.sequence = taskCount + 1;

            // Gets user input for the task name and description.
            printf("Make sure to add _ whenever there are spaces in your inputs\n");
            printf("Example: Task Name: Project_Mercury\n\n");

            printf("\n\nEnter task name: "); //
            scanf("%s", newTask.name);
            printf("\nEnter task description: "); //
            scanf("%s", newTask.description);

            do { // gets user input for the start and end dates
                printf("\nEnter task start date (MM/DD/YYYY): ");
                scanf("%s", newTask.startdate);
                printf("\nEnter task end date (MM/DD/YYYY): ");
                scanf("%s", newTask.enddate);
                if (strcmp(newTask.enddate, newTask.startdate) < 0) {
                    printf("Error: End date cannot be earlier than start date.\n"); // checks whether the inputed date is valid
                }
            } while (strcmp(newTask.enddate, newTask.startdate) < 0);

            newTask.totalduration = calculateTotalDuration(newTask.startdate, newTask.enddate); // Calculates the total duration of the task and assigns it to the task.
            // Assigns the new task to the project and increments the total number of tasks.
            newTask.personnelID = 0;
            newTask.status = 1;
            projects[newTask.projectID - 1].tasks[newTask.sequence - 1] = newTask;
            (*numTask)++;
            printf("Task added successfully.\n");
        }
    }
    if (projectFound == 0) {
        printf("Error: Project with ID %d not found.\n", projectID);
    }
    manager_menu(projects, numProjects, numTask);
}

void updateTask(int* taskID, Project* projects, int* numProjects, int* numTask) {
    int i, j, projectFound = 0, taskFound = 0;
    // Loop through all the projects
    for (i = 0; i < *numProjects; i++) {
        // Loop through all the tasks in the current project
        for (j = 0; j < 10; j++) {
            // If a task with the given task ID is found
            if (projects[i].tasks[j].taskID == *taskID) {
                projectFound = 1;
                taskFound = 1;
                // Prompt the user to enter the new task name and description
                printf("Make sure to add _ whenever there are spaces in your inputs\n");
                printf("Example: Task Name: Project_Mercury\n\n");
                printf("Enter new task name: ");
                scanf("%s", projects[i].tasks[j].name);
                printf("Enter new task description: ");
                scanf("%s", projects[i].tasks[j].description);

                // Display a message indicating that the task has been successfully updated
                printf("Task updated successfully.\n");
            }
        }
        // If the task has been found, exit the loop
        if (taskFound == 1) {
            i = *numProjects;
        }
    }
    // If the task has not been found, display an error message
    if (projectFound == 0) {
        printf("Error: Task with ID %d not found.\n", *taskID);
    }

    // Return to the manager menu
    manager_menu(projects, numProjects, numTask);
}

void addnewproj(Project *projects, int *numProjects, int *numTask) {
    if (*numProjects >= 10) {
        printf("Maximum number of projects reached (10).\n");
        return;
    }

    Project newProj = {0};
    newProj.projectID = projects[*numProjects-1].projectID + 1; // this checks the num of projects and add 1 for the new project ID
    printf("Enter project name: ");
    scanf("%s", newProj.name);
    printf("\nEnter project description: ");
    scanf("%s", newProj.description);
    newProj.status = 1; // indicates that the project is not yet done
    newProj.completion = 0;
    newProj.managerID = 0;

    int i;
    for (i = 0; i < 10; i++) { // makes every tasks in a new project 0
        newProj.tasks[i].taskID = 0;
        newProj.tasks[i].sequence = 0;
        newProj.tasks[i].projectID = newProj.projectID;
        newProj.tasks[i].totalduration = 0;
        newProj.tasks[i].status = 1;
        newProj.tasks[i].personnelID = 0;
    }
    projects[*numProjects] = newProj;
    *numProjects += 1;
    printf("Project added successfully.\n");
    manager_menu(projects, numProjects, numTask);
}

void showPersonnelList(Personnel* personnelList) {
    int i;

    printf("Active Personnel List:\n");
    printf("ID\tName\t\tUsername\tAccess Level\n");
    printf("-----------------------------------------------\n");
    for (i = 0; i < 50; i++) {
        if (personnelList[i].status == 1) { // goes through all account slots and displays accounts that are not archived
            printf("\n\n%d\t%s\t\t%s\t\t%d\n", personnelList[i].Personnel_ID,
                   personnelList[i].Personnel_Name, personnelList[i].Personnel_Username,
                   personnelList[i].access_level);
        }
    }
}

int main() {
    login();
    return 0;
}

void login() {
    // Initialize variables
    Project projects[10]; // variable to be used in getting the projects in a "project_YYYYMMDD.txt" file
    Personnel personnels[50]; // variable to be used in getting the personnels in a "personnel_YYYYMMDD.dat" file
    Task task[100]; // variable to be used in getting the tasks in a "task_YYYYMMDD.txt" file
    char getfile[100];
    int UserIdvalue;
    int numPersonnel = 0;
    int numProjects = 0;
    int numTask = 0;
    int nchoice = 0;
    // Initialize arrays
    zero_out(personnels, task, projects);
    readProjectsFromFile(projects, &numProjects); // gets values of projects from files
    readTasksFromFile(task, &numTask); // gets values of tasks from file
    readPersonnelFromFile(personnels, &numPersonnel, getfile); // gets values of personnel from file
    // Assign tasks to projects
    placeTaskInProject(task, numTask, projects, numProjects); // inputs tasks to their corresponding projects depends on how they are assigned

    // Initialize variables for login
    char username[20], password[20], name[20];
    int is_logged_in = 0, Accessrole = 0, PersonnelID[5], active[2];
    char file_username[20], file_password[20];
    int access;
    system("clear");
    printf("=== Welcome to Project Management Institute ===\n"); // shows the title screen of the project management
    printf("== Project Management Systems ==\n\n\n");
    printf("Do you have an existing account? \n\n\n");
    printf("[1] - Yes, I have an existing account\n[2] - No, I don't have I'll use a preloaded account \n\n\n");
    printf("Enter your choice: ");
    scanf("%d", &nchoice); fflush(stdin); // user must have a choice whether if the user have an existing account or going to use a preloaded account
    switch (nchoice) {
        case 1: // Existing account
            system("clear");
            printf("=== Login ===\n");
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);
            FILE* fp1;
            fp1 = fopen(getfile, "rb"); // open file for reading
            if (fp1 == NULL) { // check if file was successfully opened
                printf("Error: Unable to open file.\n");
            }
            int i = 0;
            while (i < 50 && !is_logged_in && fscanf(fp1, "%d %s\n%s\n%s\n%d\n%d\n\n", &personnels[i].Personnel_ID, personnels[i].Personnel_Name,
                                                    personnels[i].Personnel_Username, personnels[i].Personnel_Pass,
                                                    &personnels[i].status, &personnels[i].access_level) == 6) {
                if (strcmp(username, personnels[i].Personnel_Username) == 0 && strcmp(password, personnels[i].Personnel_Pass) == 0) {
                    // set flag to indicate successful login and access role
                    is_logged_in = 1;
                    Accessrole = personnels[i].access_level;
                    UserIdvalue = personnels[i].Personnel_ID;
                }
                i++;
            }
            fclose(fp1);
            system("clear");
            break;
        case 2: // Preloaded account
            system("clear");
            printf("=== Login ===\n");
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);
            FILE* fp;
            fp = fopen("preloaded.txt", "r"); // open preloaded.txt file for reading
            if (fp == NULL) { // check if file was successfully opened
                printf("Error: Unable to open file.\n");
            }
            // read from file line by line and check if login credentials match
            while (fscanf(fp, "%d %s\n%s\n%s\n%d\n%d", PersonnelID, name, file_username, file_password, active, &access) != EOF) {
                if (strcmp(username, file_username) == 0 && strcmp(password, file_password) == 0) {
                    // set flag to indicate successful login and access role
                    is_logged_in = 1;
                    Accessrole = access;
                    printf("Login successful!\n");
                }
            }
            fclose(fp);
            system("clear");
            break;
        default:
            login();
            break;
    }
    // check access role and call corresponding menu function
    if (Accessrole == 1) {
        admin_menu(personnels, &numPersonnel, projects, &numProjects);
    } else if (Accessrole == 2) {
        manager_menu(projects, &numProjects, &numTask, personnels);
    } else if (Accessrole == 3) {
        user_menu(personnels, projects, &UserIdvalue, &numProjects);
    } else {
        printf("An Error Occured... Please Try Again!\n"); // display error message and call login function again
        login();
    }
}

void EnterUserDetails(Personnel* personnel) {
    int lower = 1000;
    int upper = 1300;
    srand(time(NULL));
    int num = (rand() % (upper - lower + 1)); // provides a random personnel ID number from the range 1000 to 1300 and stores it in "num"
    printf("Make sure to add _ whenever there are spaces in your inputs\n");
    printf("Example: Task Name: Project_Mercury\n\n");
    (*personnel).Personnel_ID = num; // stores the randomly generated ID to the Personnel struct member ".Personnel_ID"
    printf("Personnel ID is %d", (*personnel).Personnel_ID); // displays the randomly generated ID number
    printf("\nEnter Personnel Name: \n");
    scanf("%s", (*personnel).Personnel_Name); // stores the user input into the Personnel struct member ".Personnel_Name"

    printf("\nEnter Personnel Username: \n");
    scanf("%s", (*personnel).Personnel_Username); // stores the user input into the Personnel struct member ".Personnel_Username"
    printf("\nEnter Personnel Password: \n");
    scanf("%s", (*personnel).Personnel_Pass); // stores the user input into the Personnel struct member ".Personnel_Pass"
    printf("\nEnter Personnel Account Status:\n[1] - active\n[2] - archived\n");
    scanf("%d", &((*personnel).status)); // stores the user input into the Personnel struct member ".status"
    printf("\nEnter Access Level of Account:\n[1] - admin\n[2] - manager\n[3] - user\n");
    scanf("%d", &((*personnel).access_level)); // stores the user input into the Personnel struct member ".access_level"
}

void Add_user(Personnel* personnels, int* numPersonnel) {
    int i;
    int vacant_found = 0;
    int n;
    printf("\n\n");
    for (i = 0; i < 50; i++) { // displays which account slots are vacant or unavailable
        if (personnels[i].Personnel_Name[0] == '\0')
            printf("Slot no. %d is Open\n", i);
        else
            printf("Slot %d is no longer available\n", i);
    }
    while(!vacant_found) { // user can select a vacant account slot and begin to enter the personnels details
        printf("\n\nSelect a vacant Account Slot (0-49): "); // user can select a vacant account slot and begin to enter the personnels details
        if (scanf("%d", &n) != 1) {
            printf("\nInvalid input. Please enter a number.\n");
            while (getchar() != '\n');
        } else if (n >= 0 && n <= 49) { // if the user input is within the range of 0 - 49, the program will ask for the user inputs for the personnels details
            if (personnels[n].Personnel_Name[0] == '\0') {
                EnterUserDetails(&personnels[n]);
                (*numPersonnel)++;
                vacant_found = 1;
                admin_menu(personnels, numPersonnel);
            } else {
                printf("\n\nThat account slot is already taken.\nPlease select a vacant account slot: "); // prints an error message if the slot is already taken
            }
        } else {
            printf("\nInvalid slot number. Please enter a number between 0 and 49.\n"); // prints an error message if the number entered is below 0 and more than 49
        }
    }
}

void Delete_user(Personnel* personnels, int* numPersonnel) {
    int n, i;
    int account_found = 0;
    int index = -1;
    printf("\n\n");
    for (i = 0; i < 50 && !account_found; i++) { // goes through all account slots
        if (personnels[i].Personnel_ID == 0) // if the account slot is empty, the program will display that the slot is available
            printf("Slot %d is Open\n", i);
        else if (personnels[i].Personnel_ID == n) {
            account_found = 1;
            index = i;
        } else
            printf("\nSlot no.%d with ID number %d is taken\n", i, personnels[i].Personnel_ID); // if the account slot is taken, the program will display the slot number and personnel ID attached to it
    }
    while(!account_found) {
        printf("\n\nEnter the account ID of the account you wish to delete: "); // asks the user to input the account ID of the profile they want to delete
        scanf("%d", &n);
        for(i = 0; i < 50 && !account_found; i++) { // goes through all account slots
            if(personnels[i].Personnel_ID == n) { // checks if the user input matches one of the account IDs
                account_found = 1;
                index = i;
            }
        }

        if(!account_found) {
            printf("\nAccount with ID %d was not found. Please enter a valid account ID.\n", n); // prints an error message if the user inputs an invalid account ID
            scanf("%d", &n);
        }
    }
    if(index != -1) { // after the user inputs a valid ID, each member of the structure will be wiped
        personnels[index].Personnel_ID = 0;
        strcpy(personnels[index].Personnel_Name, "");
        strcpy(personnels[index].Personnel_Username, "");
        strcpy(personnels[index].Personnel_Pass, "");
        personnels[index].access_level = 0;
        personnels[index].status = 0;
        printf("\nAccount with ID %d has been deleted.\n", n);
        (*numPersonnel)--;
        admin_menu(personnels, numPersonnel);
    }
}

void UpdateUserDetails(Personnel* personnel) {
    printf("Make sure to add _ whenever there are spaces in your inputs\n");
    printf("Example: Task Name: Project_Mercury\n\n");
    printf("\nEnter a new Personnel Name: ");
    scanf("%s", (*personnel).Personnel_Name); // stores the user input into the Personnel struct member ".Personnel_Name"
    printf("\nEnter a new Personnel UserName: ");
    scanf("%s", (*personnel).Personnel_Username); // stores the user input into the Personnel struct member ".Personnel_Username"

    printf("\nEnter a new Personnel Password: ");
    scanf("%s", (*personnel).Personnel_Pass); // stores the user input into the Personnel struct member ".Personnel_Pass"
    printf("\nEnter the new Access Level of Account: ");
    scanf("%d", &((*personnel).access_level)); // stores the user input into the Personnel struct member ".access_level"
}

void Update_user(Personnel* personnels, int* numPersonnel) {
    int i, n;

    for (i = 0; i < 50; i++) { // displays which account slots are vacant or unavailable, along with their respective ID numbers and Names
        if (personnels[i].Personnel_Name[0] == '\0')
            printf("Slot no. %d is Open\n", i);
        else
            printf("Slot no. %d with ID number %d and name %s is taken\n", i, personnels[i].Personnel_ID, personnels[i].Personnel_Name);
    }

    printf("\n\nEnter the slot you wish to update: "); // stores the user input in "n"
    scanf("%d", &n);

    if (n < 0 || n >= 50 || personnels[n].Personnel_Name[0] == '\0') { // notifies the user if the personnel profile they want to update exists
        printf("Invalid ID or personnel does not exist\n");
        return;
    }

    UpdateUserDetails(&personnels[n]); // function to get user inputs and update the personnels profile
    admin_menu(personnels, numPersonnel);
}

void Archive_user(Personnel* personnels, int* numpersonnel) {
    int i;

    for (i = 0; i < 50; i++) {
        if (personnels[i].Personnel_Name[0] == '\0') // displays which account slots are vacant or unavailable
            printf("Slot no. %d is Open\n", i);
        else
            printf("Slot %d is no longer available\n", i);
    }
    int n;
    printf("\nEnter slot to be archived: "); // gets user input to be stored in "n"
    scanf("%d", &n); // "n" will be used to get the specific personnel slot to be archived
    personnels[n].status = 2;
    (*numpersonnel)--; // reduces the count in "numpersonnel" (updates the number of active personnel)
}

void Assign_user(Project* projects, Personnel* personnels, int* numProjects) {
    int projectID, personnelID;
    int i, j, projectFound, personnelFound;
    printf("Active Projects:\n"); // goes through all active projects and displays them on screen along with their project IDs
    for (i = 0; i < *numProjects; i++) {
        if (projects[i].status == 1) {
            printf("%d: %s\n", projects[i].projectID, projects[i].name);
        }
    }
    printf("Enter Project ID: "); // gets the project ID from the user input
    scanf("%d", &projectID);
    projectFound = 0;
    for (i = 0; i < *numProjects && !projectFound; i++) { // goes through all active projects
        if (projects[i].projectID == projectID && projects[i].status == 1) { // if the project ID matches the user input then that will be the project that will be assigned to a specific user
            projectFound = 1;
        }
    }
    if (!projectFound) {
        printf("Error: Project with ID %d not found.\n", projectID);
        return;
    }

    printf("Active Personnel with Manager Access:\n"); // goes through all user accounts and displays the ones with manager access
    for (i = 0; i < 50; i++) {
        if (personnels[i].status == 1 && personnels[i].access_level == 2) {
            printf("%d: %s\n", personnels[i].Personnel_ID, personnels[i].Personnel_Name);
        }
    }

    printf("Enter Personnel ID: "); // gets the personnel ID from the user input
    scanf("%d", &personnelID);
    personnelFound = 0;

    for (i = 0; i < 50 && !personnelFound; i++) { // goes through all personnel
        if (personnels[i].status == 1 && personnels[i].access_level == 2 && personnels[i].Personnel_ID == personnelID) { // checks if the personnel ID matches the user input
            personnelFound = 1;
        }
    }

    if (!personnelFound) {
        printf("Error: Personnel with ID %d and access level 2 not found.\n", personnelID);
        return;
    }

    for (j = 0; j < 10; j++) { // assigns the project to the personnel ID inputted by the user
        if (projects[j].projectID == projectID && projects[j].status == 1) {
            projects[j].managerID = personnelID;
            projects[j].status = 2;
            printf("Personnel with ID %d assigned in Project %d.\n", personnelID, projectID);
            return;
        }
    }

    printf("Error: No available Project.\n");
}

void view_Tasktoday(Personnel* personnels, Project* projects, int UserIdvalue) {
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    char today[100];
    strftime(today, 100, "%m/%d/%Y", current_time); // gets the current time
    int taskCount = 0; // counter for how many tasks
    int i, j; // counter for loops
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            if (projects[i].tasks[j].personnelID == UserIdvalue && projects[i].tasks[j].status != 3) {
                struct tm tm_startdate = {0}; // converts the current time to a tm structure
                sscanf(projects[i].tasks[j].startdate, "%d/%d/%d", &tm_startdate.tm_mon, &tm_startdate.tm_mday, &tm_startdate.tm_year);
                tm_startdate.tm_year -= 1900;
                tm_startdate.tm_mon -= 1;

                struct tm tm_enddate = {0}; // gets the end date of each task
                sscanf(projects[i].tasks[j].enddate, "%d/%d/%d", &tm_enddate.tm_mon, &tm_enddate.tm_mday, &tm_enddate.tm_year);
                tm_enddate.tm_year -= 1900; // adjusts the year to be the correct number of years since 1900
                tm_enddate.tm_mon -= 1;
                time_t enddate_t = mktime(&tm_enddate); // converts the end date tm structure to a time_t value
                double diff_seconds = difftime(now, enddate_t); // calculates the difference between the current time and the end date
                if (diff_seconds <= 0) { // displays the tasks within the range of the start date and end date
                    printf("Task ID: %d\n", projects[i].tasks[j].taskID);
                    printf("Task Name: %s\n", projects[i].tasks[j].name);
                    printf("Project ID: %d\n", projects[i].projectID);
                    printf("Task Description: %s\n", projects[i].tasks[j].description);
                    printf("Task Start Date: %s\n", projects[i].tasks[j].startdate);
                    printf("Task End Date: %s\n", projects[i].tasks[j].enddate);
                    printf("Task Duration: %d\n", projects[i].tasks[j].totalduration);
                    printf("Task Status: %d\n", projects[i].tasks[j].status);
                    printf("\n");
                    taskCount++;
                }
            }
        }
    }
    if (taskCount == 0) { // if there are no tasks found that is assigned to the user this will print out
        printf("No tasks assigned to the user for today.\n");
    } else { // this will show how many tasks that has been assigned to the user for today
        printf("You have (%d) task/s that have been assigned to you for today.\n", taskCount);
    }
}

void view_Alltask(Personnel* personnels, Project* projects, int UserIdvalue) {
    int j, k;
    int taskCount = 0;
    printf("\nBelow are all the assigned tasks of the user\n");
    printf("============================================\n\n");

    for (j = 0; j < 10; j++) { // goes through all the tasks that are assigned to the user
        for (k = 0; k < 10; k++) {
            if (projects[j].tasks[k].personnelID == UserIdvalue) { // User ID value is used to check if the task is assigned to the user before it is displayed
                printf("Task ID: %d\n", projects[j].tasks[k].taskID);
                printf("Task Name: %s\n", projects[j].tasks[k].name);
                printf("Task Sequence: %d\n", projects[j].tasks[k].sequence);
                printf("Project ID: %d\n", projects[j].tasks[k].projectID);
                printf("Start Date: %s\n", projects[j].tasks[k].startdate);
                printf("End Date: %s\n", projects[j].tasks[k].enddate);
                printf("Task Status: %d\n\n", projects[j].tasks[k].status);
                taskCount++;
            }
        }
    }
    printf("You have (%d) task/s that have been assigned to you.\n\n", taskCount);
    if (taskCount == 0) {
        printf("No tasks assigned to the user.\n");
    }
}

void update_TaskStatus(Personnel* personnels, Project* projects, int UserIdvalue) {
    int taskID, status;
    int found = 0;
    int j, k;
    // Display all tasks assigned to the user
    printf("\nBelow are all of your assigned task/s\n");
    printf("============================================\n");
    for (j = 0; j < 10 && !found; j++) {
        for (k = 0; k < 10 && !found; k++) {
            if (projects[j].tasks[k].personnelID == UserIdvalue) {
                printf("Task ID: %d\n", projects[j].tasks[k].taskID);
                printf("Task Name: %s\n", projects[j].tasks[k].name);
                printf("Task Sequence: %d\n", projects[j].tasks[k].sequence);
                printf("Project ID: %d\n", projects[j].tasks[k].projectID);
                printf("Start Date: %s\n", projects[j].tasks[k].startdate);
                printf("End Date: %s\n", projects[j].tasks[k].enddate);
                printf("Task Status: %d\n", projects[j].tasks[k].status);
                printf("\n");
            }
        }
    }
    // Get the task ID from the user
    printf("Enter the task ID you want to update the status for: ");
    scanf("%d", &taskID);
    // Loop through all tasks to find the task with the given ID
    for (j = 0; j < 10 && !found; j++) {
        for (k = 0; k < 10 && !found; k++) {
            if (projects[j].tasks[k].taskID == taskID && projects[j].tasks[k].personnelID == UserIdvalue) {
                found = 1;
                printf("Enter the new status for the task:\n");
                printf("1 - Not Started\n");
                printf("2 - In Progress\n");
                printf("3 - Done\n");
                scanf("%d", &status);
                projects[j].tasks[k].status = status;
                printf("Task status updated successfully!\n");
            }
        }
    }
    // If the task with the given ID is not found
    if (found == 0) {
        printf("Task with ID %d not found or not assigned to the user.\n", taskID);
    }
}

void admin_menu(Personnel *personnels, int *numPersonnel, Project* projects, int* numProjects) {
    int choice;
    system("clear");
    printf("=== Admin Menu ===\n");
    printf("1. Add New User\n");
    printf("2. Update User\n");
    printf("3. Delete User\n");
    printf("4. Archive User\n");
    printf("5. Assign User\n");
    printf("6. Back to Main Menu\n");
    printf("Enter your choice: ");
    scanf("%d", &choice); fflush(stdin);
    switch (choice) {
        case 1:
            system("clear");
            printf("=== New User ===\n");
            Add_user(personnels, &*numPersonnel);
            break;
        case 2:
            system("clear");
            printf("=== Update User ===\n");
            Update_user(personnels, &*numPersonnel);
            break;
        case 3:
            system("clear");
            printf("=== Delete User ===\n");
            Delete_user(personnels, &*numPersonnel);
            break;
        case 4:
            system("clear");
            printf("=== Archive User ===\n");
            Archive_user(personnels, &*numPersonnel);
            admin_menu(personnels, numPersonnel, projects, &*numProjects);
            break;
        case 5:
            system("clear");
            printf("=== Assign User ===\n");
            Assign_user(projects, personnels, &*numProjects);
            exportProjects(projects, &*numProjects);
            admin_menu(personnels, numPersonnel, projects, &*numProjects);
            break;
        case 6:
            system("clear");
            exportPersonnel(personnels, &*numPersonnel);
            login();
            break;
        default:
            printf("Invalid choice. Try again.\n");
            admin_menu(personnels, numPersonnel, projects, &*numProjects);
            break;
    }
}

void manager_menu(Project* projects, int *numProjects, int *numTask, Personnel* personnel) {
    int taskID;
    int projectID;
    int choice;
    printf("=== Manager Menu ===\n");
    printf("1. Add New Project\n");
    printf("2. Add New Task\n");
    printf("3. Update Task\n");
    printf("4. Show Personnel List\n");
    printf("5. Assign Task\n");
    printf("6. Show Project Details\n");
    printf("7. Show Delayed Task\n");
    printf("8. Run Project Completion\n");
    printf("9. Back to Main Menu\n");
    printf("Enter your choice: ");
    scanf("%d", &choice); fflush(stdin);
    switch (choice) {
        case 1:
            system("clear");
            printf("=== Add Project ===\n\n");
            addnewproj(projects, &*numProjects, &*numTask);
            break;
        case 2:
            system("clear");
            printf("=== New Task ===\n\n");
            addTask(projects, &*numProjects, &*numTask);
            break;
        case 3:
            system("clear");
            printf("=== Update Task ===\n\n");
            showTasktoUpdate(projects, &*numProjects, &*numTask);
            printf("What Task do you like to change? (Enter It's ID) : ");
            scanf("%d", &taskID);
            updateTask(&taskID, projects, &*numProjects, &*numTask);
            break;
        case 4:
            system("clear");
            printf("=== Personnel List ===\n\n");
            showPersonnelList(personnel);
            manager_menu(projects, &*numProjects, &*numTask, personnel);
            break;
        case 5:
            system("clear");
            printf("=== Assign Task ===\n\n");
            Assigntask(projects, &*numProjects, personnel);
            manager_menu(projects, numProjects, numTask, personnel);
            break;
        case 6:
            system("clear");
            printf("=== Project Details ===\n\n");
            showProjectDetails(projects, &*numProjects, &*numTask);
            break;
        case 7:
            system("clear");
            printf("=== Delayed Task ===\n\n");
            showDelayedTasks(projects, *numProjects, *numTask);
            manager_menu(projects, numProjects, numTask, personnel);
            break;
        case 8:
            system("clear");
            printf("=== Project Completion ===\n\n");
            printf("What Project you want to see its completion? (Enter its ID): ");
            scanf("%d", &projectID);
            updateProjectCompletion(projects, projectID);
            manager_menu(projects, numProjects, numTask, personnel);
            break;
        case 9:
            printf("=== Back to menu ===\n");
            exportTasks(projects, &*numProjects);
            exportProjects(projects, &*numProjects);
            login();
            break;
        default:
            printf("Invalid choice. Try again ===\n");
            manager_menu(projects, numProjects, numTask, personnel);
            break;
    }
}

void user_menu(Personnel* personnels, Project* projects, int* UserIdvalue, int* numProjects) {
    int choice;
    printf("=================\n");
    printf("Welcome! Project ID: %d\n", *UserIdvalue);
    printf("=== User Menu ===\n");
    printf("1. Show Today's Task\n");
    printf("2. Show All Assigned Task\n");
    printf("3. Update Task Status\n");
    printf("4. Back to Main Menu\n");
    printf("Enter your choice: ");
    scanf("%d", &choice); fflush(stdin);
    switch (choice) {
        case 1:
            system("clear");
            printf("=== Today's Task ===\n\n");
            view_Tasktoday(personnels, projects, *UserIdvalue);
            user_menu(personnels, projects, UserIdvalue, numProjects);
            break;
        case 2:
            system("clear");
            printf("=== All Assigned Task ===\n\n");
            view_Alltask(personnels, projects, *UserIdvalue);
            user_menu(personnels, projects, UserIdvalue, numProjects);
            break;
        case 3:
            system("clear");
            printf("=== Update Task Status ===\n\n");
            update_TaskStatus(personnels, projects, *UserIdvalue);
            exportTasks(projects, &*numProjects);
            system("clear");
            user_menu(personnels, projects, UserIdvalue, numProjects);
            break;
        case 4:
            printf("=== Back to menu ===\n\n");
            login();
            break;
        default:
            printf("Invalid choice. Try again.\n");
            break;
    }
}
