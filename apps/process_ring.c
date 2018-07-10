#include <xinu.h>

process poll_handle_mail(int32 p_index, int32 processes, volatile int32 *inboxes, int32 *current_value, int32 *current_round) {
  while (*current_value >= 0) {
    /* POlling until producer has produced correct mailbox_number */
    while (inboxes[p_index] == -1 && *current_value >= 0);
    /* Print information on process round */
    printf("Ring Element %d : Round %d : ", p_index, *current_round);
    printf("Value: %d\n", *current_value);
    /* Rewrite processes inbox to state it has read it */
    inboxes[p_index] = -1;
    /* Make sure there is another inbox to be written to */
    *current_value -= 1;
    if (*current_value >= 0) {
      /* Find neighbor */
      if (p_index + 1 > processes-1) {
	*current_round += 1;
	inboxes[0] = *current_value;
      } else {
	inboxes[p_index+1] = *current_value;
      }
    }
  }
  return OK;
}

void poll_implementation(int32 rounds, int32 processes) {
  volatile int32 inboxes[NPROC]; /* Inboxes to be used */
  volatile int32 current_value = (processes * rounds) - 1; /* Value to count down */
  volatile int32 current_round = 0;
  /* Fill process array with processes */
  int32 p_index, i;
  /* Set inboxes to -1 to start */
  for (i = 0; i < processes; i ++) {
    inboxes[i] = -1;
  }
  for (p_index = 0; p_index < processes; p_index++) {
    resume(create(poll_handle_mail, 1024, 20, "poll_handle_mail", 5, p_index, processes, &inboxes, &current_value, &current_round));
  }
  /* Set the first inbox to start round 0 */
  inboxes[0] = current_value;
  while(current_value >= 0);
}

process sync_handle_mail(int32 p_index, int32 processes, volatile int32 *inboxes, int32 *current_value, pid32 *pids, int32 *current_round) {
  while (*current_value >= 0) {
    /* SYNCing until sent message */
    inboxes[p_index] = receive();
    /* Print information on process round */
    printf("Ring Element %d : Round %d : ", p_index, *current_round);
    printf("Value: %d\n", inboxes[p_index]);
    /* Rewrite processes inbox to state it has read it */
    inboxes[p_index] = -1;
    /* Make sure there is another inbox to be written to */
    *current_value -= 1;
    if (*current_value >= 0) {
      /* Find neighbor */
      if (p_index + 1 < processes) {
	send(pids[p_index+1],*current_value);
      } else {
	*current_round += 1;
	send(pids[0],*current_value);
      }
    }
  }
  return OK;
}

void sync_implementation(int32 rounds, int32 processes) {
  volatile int32 inboxes[NPROC]; /* Inboxes to be used */
  volatile int32 current_value = (processes * rounds) - 1; /* Value to count down */
  volatile pid32 pids[NPROC];
  volatile int32 current_round = 0;
  /* Fill process array with processes */
  int32 p_index;
  for (p_index = 0; p_index < processes; p_index++) {
    pids[p_index] = create(sync_handle_mail, 1024, 20, "sync_handle_mail", 6, p_index, processes, &inboxes, &current_value, &pids, &current_round);
    resume(pids[p_index]);
  }
  /* Set the first inbox to start round 0 */
  send(pids[0],current_value);
  receive();
}
