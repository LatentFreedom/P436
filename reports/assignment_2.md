Nick Palumbo
Assignment 2 report

1. What are 2 things that you learned from this assignment?
I learned that it is better to synchronize processes then just poll on a value. It is better to synchronize becaue there may be a process that should use a volatile value before another process and if the processes are out of sync then the value may be changed before it is needed. 
I also learned that create has a lot of stack magic to make it work. The function calls itself to make the function call in create look like it was already running which is interesting in itself.


2. What are 2 mistakes that you made in this assignment, and how did you discover and fix these mistakes?
The first mistake I made was that I did not know how to create an array based off of a dynamic value. I tried to use getmem because malloc was not defined in my Xinu implementation. So I did not have a malloc.c file. I decided to drop the array and began implementing a single value that could be passed around which got my polling functions to work.
The second mistake came when I was trying to implement the message passing for snchronization. The problem arose when I had to figure out why my handle_mail process was stalling before printing out any information. I got it to work by fixing my send functionality. I was not sending the information to a good pid. After fixing this problem I was ablt to run everything.


3. If you were restarting this assignment, what would you change about how you went about completing this assignment?
I would change the fact that I spent so much time trying to implement a producer consumer concept. I in the end made a single process to be created and call the function handle_mail. The process I created dose the producing and consuming. I had a hard time trying to comprehend the assignment and why we needed inboxes and how to go about creating it will malloc or getmem. 