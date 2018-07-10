
process poll_handle_mail(int32 p_index, int32 processes, volatile int32 *inboxes, int32 *current_value);
void poll_implementation(int32 rounds, int32 processes);

process sync_handle_mail(int32 p_index, int32 processes, volatile int32 *inboxes, int32 *current_value, pid32 *pids);
void sync_implementation(int32 rounds, int32 processes);
