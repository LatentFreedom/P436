Nick Palumbo
Assignment 1 report

1. What is Orphan and Zombie processes?

An orphan process is a process who has a terminated parent process but continues to run.
A zombie process is a process that completed it's execution but has a parent process that has not received a termination notification.

2. What are the changes that occur when the child process calls the exec system call to execute a new program?

The exec system call replaces the child process with a new program. The new program is loaded into the current process space and runs from the entry point in which it is called.
