
/* Producer/Consumer - function prototypes */
process producer(sid32 mutex, sid32 items, sid32 spaces, sid32 done_sem,
                 int32 total_bytes, int32 max_delay);
process consumer(sid32 mutex, sid32 items, sid32 spaces, sid32 done_sem,
		   int32 total_bytes, int32 max_delay);


/* Readers/Writers - function prototypes */
process writer(sid32 room_empty, sid32 done_sem,
	       int32 w_cycles, int32 max_delay);
process reader(int32 *readers, sid32 mutex, sid32 room_empty, sid32 done_sem,
	       int32 total_bytes, int32 max_delay);

  
/* Dining Philosopher - funciton prototypes */
process philosopher(int32 n_philosophers, sid32 forks[], int32 n_cycles, sid32 footman, sid32 done_sem, int32 max_delay, int32 n);
int32 right(int32 i, int32 n_philosophers);
int32 left(int32 i);
void get_forks(int32 i, sid32 footman, int32 n_philosophers, sid32 forks[]);
void put_forks(int32 i, sid32 footman, int32 n_philosophers, sid32 forks[]);
