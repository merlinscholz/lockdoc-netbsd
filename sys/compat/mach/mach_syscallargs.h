/* $NetBSD: mach_syscallargs.h,v 1.21 2009/12/14 00:58:37 matt Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.10 2009/01/13 22:27:43 pooka Exp
 */

#ifndef _MACH_SYS_SYSCALLARGS_H_
#define	_MACH_SYS_SYSCALLARGS_H_

#define	MACH_SYS_MAXSYSARGS	9

#undef	syscallarg
#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

#undef check_syscall_args
#define check_syscall_args(call) \
	typedef char call##_check_args[sizeof (struct call##_args) \
		<= MACH_SYS_MAXSYSARGS * sizeof (register_t) ? 1 : -1];

struct mach_sys_msg_trap_args {
	syscallarg(mach_msg_header_t *) msg;
	syscallarg(mach_msg_option_t) option;
	syscallarg(mach_msg_size_t) send_size;
	syscallarg(mach_msg_size_t) rcv_size;
	syscallarg(mach_port_name_t) rcv_name;
	syscallarg(mach_msg_timeout_t) timeout;
	syscallarg(mach_port_name_t) notify;
};
check_syscall_args(mach_sys_msg_trap)

struct mach_sys_msg_overwrite_trap_args {
	syscallarg(mach_msg_header_t *) msg;
	syscallarg(mach_msg_option_t) option;
	syscallarg(mach_msg_size_t) send_size;
	syscallarg(mach_msg_size_t) rcv_size;
	syscallarg(mach_port_name_t) rcv_name;
	syscallarg(mach_msg_timeout_t) timeout;
	syscallarg(mach_port_name_t) notify;
	syscallarg(mach_msg_header_t *) rcv_msg;
	syscallarg(mach_msg_size_t) scatter_list_size;
};
check_syscall_args(mach_sys_msg_overwrite_trap)

struct mach_sys_semaphore_signal_trap_args {
	syscallarg(mach_port_name_t) signal_name;
};
check_syscall_args(mach_sys_semaphore_signal_trap)

struct mach_sys_semaphore_signal_all_trap_args {
	syscallarg(mach_port_name_t) signal_name;
};
check_syscall_args(mach_sys_semaphore_signal_all_trap)

struct mach_sys_semaphore_signal_thread_trap_args {
	syscallarg(mach_port_name_t) signal_name;
	syscallarg(mach_port_name_t) thread;
};
check_syscall_args(mach_sys_semaphore_signal_thread_trap)

struct mach_sys_semaphore_wait_trap_args {
	syscallarg(mach_port_name_t) wait_name;
};
check_syscall_args(mach_sys_semaphore_wait_trap)

struct mach_sys_semaphore_wait_signal_trap_args {
	syscallarg(mach_port_name_t) wait_name;
	syscallarg(mach_port_name_t) signal_name;
};
check_syscall_args(mach_sys_semaphore_wait_signal_trap)

struct mach_sys_semaphore_timedwait_trap_args {
	syscallarg(mach_port_name_t) wait_name;
	syscallarg(unsigned int) sec;
	syscallarg(mach_clock_res_t) nsec;
};
check_syscall_args(mach_sys_semaphore_timedwait_trap)

struct mach_sys_semaphore_timedwait_signal_trap_args {
	syscallarg(mach_port_name_t) wait_name;
	syscallarg(mach_port_name_t) signal_name;
	syscallarg(unsigned int) sec;
	syscallarg(mach_clock_res_t) nsec;
};
check_syscall_args(mach_sys_semaphore_timedwait_signal_trap)

struct mach_sys_map_fd_args {
	syscallarg(int) fd;
	syscallarg(mach_vm_offset_t) offset;
	syscallarg(mach_vm_offset_t *) va;
	syscallarg(mach_boolean_t) findspace;
	syscallarg(mach_vm_size_t) size;
};
check_syscall_args(mach_sys_map_fd)

struct mach_sys_task_for_pid_args {
	syscallarg(mach_port_t) target_tport;
	syscallarg(int) pid;
	syscallarg(mach_port_t *) t;
};
check_syscall_args(mach_sys_task_for_pid)

struct mach_sys_pid_for_task_args {
	syscallarg(mach_port_t) t;
	syscallarg(int *) x;
};
check_syscall_args(mach_sys_pid_for_task)

struct mach_sys_macx_swapon_args {
	syscallarg(char *) name;
	syscallarg(int) flags;
	syscallarg(int) size;
	syscallarg(int) priority;
};
check_syscall_args(mach_sys_macx_swapon)

struct mach_sys_macx_swapoff_args {
	syscallarg(char *) name;
	syscallarg(int) flags;
};
check_syscall_args(mach_sys_macx_swapoff)

struct mach_sys_macx_triggers_args {
	syscallarg(int) hi_water;
	syscallarg(int) low_water;
	syscallarg(int) flags;
	syscallarg(mach_port_t) alert_port;
};
check_syscall_args(mach_sys_macx_triggers)

struct mach_sys_swtch_pri_args {
	syscallarg(int) pri;
};
check_syscall_args(mach_sys_swtch_pri)

struct mach_sys_syscall_thread_switch_args {
	syscallarg(mach_port_name_t) thread_name;
	syscallarg(int) option;
	syscallarg(mach_msg_timeout_t) option_time;
};
check_syscall_args(mach_sys_syscall_thread_switch)

struct mach_sys_clock_sleep_trap_args {
	syscallarg(mach_port_name_t) clock_name;
	syscallarg(mach_sleep_type_t) sleep_type;
	syscallarg(int) sleep_sec;
	syscallarg(int) sleep_nsec;
	syscallarg(mach_timespec_t *) wakeup_time;
};
check_syscall_args(mach_sys_clock_sleep_trap)

struct mach_sys_timebase_info_args {
	syscallarg(mach_timebase_info_t) info;
};
check_syscall_args(mach_sys_timebase_info)

struct mach_sys_wait_until_args {
	syscallarg(u_int64_t) deadline;
};
check_syscall_args(mach_sys_wait_until)

struct mach_sys_timer_destroy_args {
	syscallarg(mach_port_name_t) name;
};
check_syscall_args(mach_sys_timer_destroy)

struct mach_sys_timer_arm_args {
	syscallarg(mach_port_name_t) name;
	syscallarg(mach_absolute_time_t) expire_time;
};
check_syscall_args(mach_sys_timer_arm)

struct mach_sys_timer_cancel_args {
	syscallarg(mach_port_name_t) name;
	syscallarg(mach_absolute_time_t *) result_time;
};
check_syscall_args(mach_sys_timer_cancel)

/*
 * System call prototypes.
 */

int	mach_sys_reply_port(struct lwp *, const void *, register_t *);

int	mach_sys_thread_self_trap(struct lwp *, const void *, register_t *);

int	mach_sys_task_self_trap(struct lwp *, const void *, register_t *);

int	mach_sys_host_self_trap(struct lwp *, const void *, register_t *);

int	mach_sys_msg_trap(struct lwp *, const struct mach_sys_msg_trap_args *, register_t *);

int	mach_sys_msg_overwrite_trap(struct lwp *, const struct mach_sys_msg_overwrite_trap_args *, register_t *);

int	mach_sys_semaphore_signal_trap(struct lwp *, const struct mach_sys_semaphore_signal_trap_args *, register_t *);

int	mach_sys_semaphore_signal_all_trap(struct lwp *, const struct mach_sys_semaphore_signal_all_trap_args *, register_t *);

int	mach_sys_semaphore_signal_thread_trap(struct lwp *, const struct mach_sys_semaphore_signal_thread_trap_args *, register_t *);

int	mach_sys_semaphore_wait_trap(struct lwp *, const struct mach_sys_semaphore_wait_trap_args *, register_t *);

int	mach_sys_semaphore_wait_signal_trap(struct lwp *, const struct mach_sys_semaphore_wait_signal_trap_args *, register_t *);

int	mach_sys_semaphore_timedwait_trap(struct lwp *, const struct mach_sys_semaphore_timedwait_trap_args *, register_t *);

int	mach_sys_semaphore_timedwait_signal_trap(struct lwp *, const struct mach_sys_semaphore_timedwait_signal_trap_args *, register_t *);

int	mach_sys_init_process(struct lwp *, const void *, register_t *);

int	mach_sys_map_fd(struct lwp *, const struct mach_sys_map_fd_args *, register_t *);

int	mach_sys_task_for_pid(struct lwp *, const struct mach_sys_task_for_pid_args *, register_t *);

int	mach_sys_pid_for_task(struct lwp *, const struct mach_sys_pid_for_task_args *, register_t *);

int	mach_sys_macx_swapon(struct lwp *, const struct mach_sys_macx_swapon_args *, register_t *);

int	mach_sys_macx_swapoff(struct lwp *, const struct mach_sys_macx_swapoff_args *, register_t *);

int	mach_sys_macx_triggers(struct lwp *, const struct mach_sys_macx_triggers_args *, register_t *);

int	mach_sys_swtch_pri(struct lwp *, const struct mach_sys_swtch_pri_args *, register_t *);

int	mach_sys_swtch(struct lwp *, const void *, register_t *);

int	mach_sys_syscall_thread_switch(struct lwp *, const struct mach_sys_syscall_thread_switch_args *, register_t *);

int	mach_sys_clock_sleep_trap(struct lwp *, const struct mach_sys_clock_sleep_trap_args *, register_t *);

int	mach_sys_timebase_info(struct lwp *, const struct mach_sys_timebase_info_args *, register_t *);

int	mach_sys_wait_until(struct lwp *, const struct mach_sys_wait_until_args *, register_t *);

int	mach_sys_timer_create(struct lwp *, const void *, register_t *);

int	mach_sys_timer_destroy(struct lwp *, const struct mach_sys_timer_destroy_args *, register_t *);

int	mach_sys_timer_arm(struct lwp *, const struct mach_sys_timer_arm_args *, register_t *);

int	mach_sys_timer_cancel(struct lwp *, const struct mach_sys_timer_cancel_args *, register_t *);

int	mach_sys_get_time_base_info(struct lwp *, const void *, register_t *);

#endif /* _MACH_SYS_SYSCALLARGS_H_ */
