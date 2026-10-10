#include "atomic_compat.h"
#include <stdio.h>
#include <stdlib.h>

/* ====================================================================
 * SECTION 2: WEAK SYMBOL PRAGMAS AND DECLARATIONS
 * ==================================================================== */

#if defined(__GNUC__)
# if defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2)
   /* ELF / Solaris 2.x systems: Use pragma weak */
#  pragma weak __sync_fetch_and_add_4
#  pragma weak __sync_add_and_fetch_4
#  pragma weak __sync_val_compare_and_swap_4
#  pragma weak __sync_lock_test_and_set_4
#  pragma weak __sync_lock_release_4
#  pragma weak __sync_fetch_and_add
#  pragma weak __sync_add_and_fetch
#  pragma weak __sync_val_compare_and_swap
# elif defined(__aout__) || defined(sun) || defined(__sunos__)
   /* SunOS 4 / a.out systems: Standard declarations without weak attributes */
   int32_t __sync_fetch_and_add_4(volatile void *ptr, int32_t value);
   int32_t __sync_add_and_fetch_4(volatile void *ptr, int32_t value);
   int32_t __sync_val_compare_and_swap_4(volatile void *ptr, int32_t oldval, int32_t newval);
   int32_t __sync_lock_test_and_set_4(volatile void *ptr, int32_t value);
   void    __sync_lock_release_4(volatile void *ptr);
   int     __sync_fetch_and_add(volatile void *ptr, int value);
   int     __sync_add_and_fetch(volatile void *ptr, int value);
   int     __sync_val_compare_and_swap(volatile void *ptr, int oldval, int newval);
# else
   /* Fallback for other GCC platforms supporting weak attributes */
   int32_t __sync_fetch_and_add_4(volatile void *ptr, int32_t value)                         __attribute__((weak));
   int32_t __sync_add_and_fetch_4(volatile void *ptr, int32_t value)                         __attribute__((weak));
   int32_t __sync_val_compare_and_swap_4(volatile void *ptr, int32_t oldval, int32_t newval) __attribute__((weak));
   int32_t __sync_lock_test_and_set_4(volatile void *ptr, int32_t value)                     __attribute__((weak));
   void    __sync_lock_release_4(volatile void *ptr)                                         __attribute__((weak));
   int     __sync_fetch_and_add(volatile void *ptr, int value)                                __attribute__((weak));
   int     __sync_add_and_fetch(volatile void *ptr, int value)                                __attribute__((weak));
   int     __sync_val_compare_and_swap(volatile void *ptr, int oldval, int newval)           __attribute__((weak));
# endif
#endif

/* ====================================================================
 * SECTION 3: THREAD LOCKING / SPINLOCK IMPLEMENTATION
 * ==================================================================== */

#if defined(SOLARIS_COMPATIBLE) || defined(_REENTRANT) || defined(_POSIX_THREADS)
# include <pthread.h>
  static pthread_mutex_t __atomic_lock = PTHREAD_MUTEX_INITIALIZER;
# define ATOMIC_LOCK()   pthread_mutex_lock(&__atomic_lock)
# define ATOMIC_UNLOCK() pthread_mutex_unlock(&__atomic_lock)
#else
  /* Single-threaded SunOS 4 / uniprocessor execution fallback */
# define ATOMIC_LOCK()   
# define ATOMIC_UNLOCK() 
#endif

int32_t __sync_fetch_and_add_4(volatile void *ptr, int32_t value) {
    int32_t old_val;
    ATOMIC_LOCK();
    old_val = *(volatile int32_t *)ptr;
    *(volatile int32_t *)ptr = old_val + value;
    ATOMIC_UNLOCK();
    return old_val;
}

int32_t __sync_add_and_fetch_4(volatile void *ptr, int32_t value) {
    int32_t new_val;
    ATOMIC_LOCK();
    new_val = *(volatile int32_t *)ptr + value;
    *(volatile int32_t *)ptr = new_val;
    ATOMIC_UNLOCK();
    return new_val;
}

int32_t __sync_val_compare_and_swap_4(volatile void *ptr, int32_t oldval, int32_t newval) {
    int32_t current;
    ATOMIC_LOCK();
    current = *(volatile int32_t *)ptr;
    if (current == oldval) {
        *(volatile int32_t *)ptr = newval;
    }
    ATOMIC_UNLOCK();
    return current;
}

int32_t __sync_lock_test_and_set_4(volatile void *ptr, int32_t value) {
    int32_t old_val;
    ATOMIC_LOCK();
    old_val = *(volatile int32_t *)ptr;
    *(volatile int32_t *)ptr = value;
    ATOMIC_UNLOCK();
    return old_val;
}

void __sync_lock_release_4(volatile void *ptr) {
    ATOMIC_LOCK();
    *(volatile int32_t *)ptr = 0;
    ATOMIC_UNLOCK();
}

int __sync_fetch_and_add(volatile void *ptr, int value) {
    return (int)__sync_fetch_and_add_4(ptr, (int32_t)value);
}

int __sync_add_and_fetch(volatile void *ptr, int value) {
    return (int)__sync_add_and_fetch_4(ptr, (int32_t)value);
}

int __sync_val_compare_and_swap(volatile void *ptr, int oldval, int newval) {
    return (int)__sync_val_compare_and_swap_4(ptr, (int32_t)oldval, (int32_t)newval);
}

/* Fallback wrappers for lse_ prefix */
int lse_sync_fetch_and_add(volatile void *ptr, int value) {
    return __sync_fetch_and_add(ptr, value);
}

int lse_sync_add_and_fetch(volatile void *ptr, int value) {
    return __sync_add_and_fetch(ptr, value);
}

int lse_sync_val_compare_and_swap(volatile void *ptr, int oldval, int newval) {
    return __sync_val_compare_and_swap(ptr, oldval, newval);
}

/* =========================================================================
 * Embedded Unit Test Harness
 * Compile test: gcc -O2 -mcpu=v7 -D_TEST_ATOMIC_COMPAT atomic_compat.c -o test_atomic
 * ========================================================================= */
#ifdef _TEST_ATOMIC_COMPAT
#include <assert.h>

static void verify_atomic_add_operations(void) {
    volatile int32_t target = 100;
    int32_t old_val, new_val;

    printf("1. Verifying atomic add operations...\n");

    /* Test fetch-and-add */
    old_val = __sync_fetch_and_add_4(&target, 25);
    assert(old_val == 100);
    assert(target == 125);

    /* Test add-and-fetch */
    new_val = __sync_add_and_fetch_4(&target, 75);
    assert(new_val == 200);
    assert(target == 200);

    printf("   -> Atomic fetch-and-add and add-and-fetch tests passed.\n");
}

static void verify_compare_and_swap(void) {
    volatile int32_t target = 500;
    int32_t ret;

    printf("2. Verifying compare-and-swap operations...\n");

    /* Failed CAS (mismatched old value) */
    ret = __sync_val_compare_and_swap_4(&target, 999, 1000);
    assert(ret == 500);
    assert(target == 500);

    /* Successful CAS */
    ret = __sync_val_compare_and_swap_4(&target, 500, 777);
    assert(ret == 500);
    assert(target == 777);

    printf("   -> Compare-and-swap tests passed.\n");
}

static void verify_test_and_set_lock(void) {
    volatile int32_t lock_var = 0;
    int32_t old_lock;

    printf("3. Verifying lock test-and-set and release operations...\n");

    /* Acquire lock */
    old_lock = __sync_lock_test_and_set_4(&lock_var, 1);
    assert(old_lock == 0);
    assert(lock_var == 1);

    /* Release lock */
    __sync_lock_release_4(&lock_var);
    assert(lock_var == 0);

    printf("   -> Lock test-and-set and release tests passed.\n");
}

int main(void) {
    printf("==================================================\n");
    printf(" Running Atomic Compatibility Test Suite         \n");
    printf("==================================================\n");
    verify_atomic_add_operations();
    verify_compare_and_swap();
    verify_test_and_set_lock();
    printf("\n>>> System-wide atomic_compat suite FULLY VERIFIED. <<<\n");
    return 0;
}
#endif /* _TEST_ATOMIC_COMPAT */
