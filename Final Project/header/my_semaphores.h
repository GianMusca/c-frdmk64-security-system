#include <os.h>
#include <stdbool.h>

extern OS_SEM magnetic_updated_sem;
extern OS_MUTEX bitmap_mutex;
extern OS_MUTEX id_mutex;
extern OS_SEM send_email_sem;
extern OS_MUTEX email_mutex;
extern OS_MUTEX id_list_mutex;
