Nick Palumbo
Assignment 3 report

1. Which implementation consumes more space ? Why ? Give an objective answer comparing your implementation vs Xinu [5]

The implementation that consumes more space is the Xinu implementation because an array of structures is allocated. Not every index in the array has a value so the space is unused. The implementation I made only uses as much space as there are links in the list. Each link is allocated memory and attached in the list so that we only consume the memory needed.

However, a disadvantage is that the array implementation was able to look up the tail of the list given on the head efficiently because the tail would be the next position within the table. An implementation that could be used is a circular queue so that the head and tail are the previous and next items respectively. With a circular queue implementation as a doubly linked list we could have a more efficient enqueue at the end of the list in our FIFO implementation. 


2. Which implementation may take less time for basic process manipulation ? A comparison for time to FIFO and non-FIFO queues with your implementation vs Xinu [5]

The Xinu implementation may take less time for basic process manipulation becaue of the array structure and location of the head and tail nodes. The tail can easily be found given only the head of the queue index because the tail is simply an increment of head. The doubly linked list, as said above, could try to have a similar lookup time if the doubly linked list queue was a circular queue where the tail and head were conencted. 


Given n nodes, if we want to find the end of the queue there would be different lookup time to determine where the tail in the queue is. We would have to traverse the entire pointer list to determine where the tail was while the array would just increment 1. However, when determining where to insert a item the time would be the same because there is no efficient way implemented to traverse a list faster than checking each item for an insert key desired.

With that being said there is a trade off of memory efficiency and lookup time. In a low level embeded system it may be good to have a larger lookup time with a tradeoff of more memory to use. However, the doubly linked list circular queue could make this an even better choice when memory is a concern. 