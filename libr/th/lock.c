/* radare - LGPL - Copyright 2009 pancake<@nopcode.org> */

#include <r_th.h>

/* locks/mutex/sems */

R_API int r_th_lock_init(struct r_th_lock_t *thl)
{
	int ret;
	thl->refs = 0;
#if HAVE_PTHREAD
	ret = pthread_mutex_init (&thl->lock, NULL)?R_TRUE:R_FALSE;
#elif __WIN32__
	//thl->lock = CreateSemaphore(NULL, 0, 1, NULL);
	InitializeCriticalSection(&thl->lock);
#endif
	return ret;
}

R_API struct r_th_lock_t *r_th_lock_new()
{
	struct r_th_lock_t *thl = MALLOC_STRUCT(struct r_th_lock_t);
	if (!r_th_lock_init(thl))
		R_FREE(thl);
	return thl;
}

R_API int r_th_lock_wait(struct r_th_lock_t *thl)
{
#if HAVE_PTHREAD
	r_th_lock_enter(thl);
	r_th_lock_enter(thl); // locks here
	r_th_lock_leave(thl); // releases previous mutex
#elif __WIN32__
	WaitForSingleObject(thl->lock, INFINITE);
#else
	while(r_th_lock_check());
#endif
	return 0;
}

R_API int r_th_lock_enter(struct r_th_lock_t *thl)
{
#if HAVE_PTHREAD
	pthread_mutex_lock(&thl->lock);
#elif __WIN32__
	EnterCriticalSection(&thl->lock);
#endif
	return ++thl->refs;
}

R_API int r_th_lock_leave(struct r_th_lock_t *thl)
{
#if HAVE_PTHREAD
	pthread_mutex_unlock(&thl->lock);
#elif __WIN32__
	LeaveCriticalSection(&thl->lock);
	//ReleaseSemaphore (thl->lock, 1, NULL);
#endif
	if (thl->refs>0)
		thl->refs--;
	return thl->refs;
}

R_API int r_th_lock_check(struct r_th_lock_t *thl)
{
//w32 // TryEnterCriticalSection(&thl->lock);
	return thl->refs;
}

R_API void *r_th_lock_free(struct r_th_lock_t *thl)
{
	if (thl) {
#if HAVE_PTHREAD
		pthread_mutex_destroy(&thl->lock);
#elif __WIN32__
		DeleteCriticalSection(&thl->lock);
		CloseHandle(thl->lock);
#endif
		free(thl);
	}
	return NULL;
}
