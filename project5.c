/* Project5.c
 *
 * Module for getting timestamps for cyclictest
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/hrtimer.h>
#include <linux/sched.h>
#include <linux/ktime.h>
#include <linux/preempt_mask.h>
#include "project5.h"

/**
 * @brief Defines the timer base for a given thread
 */
static struct hrtimer *g_timer = NULL;

/**
 * @brief Checks if task is cyclictest and it belongs to a thread
 *
 * @param p Structure of the task
 *
 * @return Return True if allowed otherwise false
 */
static bool project5_task_allowed(struct task_struct *p)
{
	if (g_timer == NULL)
		return false;

	/* Only proceed for cyclictest */
	if (strncmp(p->comm, "cyclictest", 10))
		return false;

	/* Only proceed for cyclictest threads and not for parent which runs all
	 * threads */
	if (p->pid == p->tgid)
		return false;

	return true;
}


/**
 * @brief Calls when finish_task_swtich is called
 *
 * @param rq NOT_USED
 * @param prev NOT_USED
 */
static void project5_finish_task_switch(void *rq, struct task_struct *prev)
{
	if (!project5_task_allowed(current))
		jprobe_return();

	printk(KERN_INFO "<TASK_SWITCH>\n");

	jprobe_return();
}

/**
 * @brief Calls when ttwu_do_wakeup is called
 *
 * @param rq NOT_USED
 * @param p task struct of process to be woken
 * @param wake_flags NOT_USED
 */
static void project5_ttwu_do_wakeup(void *rq,
				    struct task_struct *p, int wake_flags)
{
	if (!project5_task_allowed(p))
		jprobe_return();

	printk(KERN_INFO "<IRQ_EXIT>\n");

	jprobe_return();
}


/**
 * @brief Calls when __remove_timer is called
 *
 * @param timer Timer Pointer to be compared
 * @param base NOT_USED
 * @param newstate NOT_USED
 * @param reprogram NOT_USED
 */
static void project5___remove_hrtimer(struct hrtimer *timer,
		      struct hrtimer_clock_base *base,
		      unsigned long newstate, int reprogram)
{
	if (g_timer != timer)
		jprobe_return();

	printk(KERN_INFO "<IRQ_START>\n");

	jprobe_return();
}

/**
 * @brief Calls when high resolution timer is set
 *
 * @param timer Pointer for the timer to store
 * @param tim NOT USED
 * @param delta_ns NOT USED
 * @param mode NOT USED
 *
 * @return 0 always
 */
static int project5_hrtimer_start_range_ns(struct hrtimer *timer, ktime_t tim,
				unsigned long delta_ns,
				const enum hrtimer_mode mode)
{
	if (strncmp(current->comm, "cyclictest", 10))
		jprobe_return();

	if (current->pid == current->tgid)
		jprobe_return();

	g_timer = timer;

	printk(KERN_INFO "<START_TIMER>\n");

	jprobe_return();

	return 0;
}

/**
 * @brief Declare all the probes
 */
PROJECT5_DECLARE_JPROBE(1, __remove_hrtimer);
PROJECT5_DECLARE_JPROBE(2, hrtimer_start_range_ns);
PROJECT5_DECLARE_JPROBE(3, ttwu_do_wakeup);
PROJECT5_DECLARE_JPROBE(4, finish_task_switch);

/**
 * @brief Calls for initialization of the module
 *
 * @return 0 always
 */
static int __init myinit(void)
{
	int ret;
	/* Register all the jprobes */
	PROJECT5_REGISTER_JPROBE(1);
	PROJECT5_REGISTER_JPROBE(2);
	PROJECT5_REGISTER_JPROBE(3);
	PROJECT5_REGISTER_JPROBE(4);

	return 0;
}

/**
 * @brief Calls for exit of the modules
 */
static void __exit myexit(void)
{
	/* Unregister all the jprobes */
	PROJECT5_UNREGISTER_JPROBE(1);
	PROJECT5_UNREGISTER_JPROBE(2);
	PROJECT5_UNREGISTER_JPROBE(3);
	PROJECT5_UNREGISTER_JPROBE(4);
}

/**
 * @brief Module init registration
 */
module_init(myinit);

/**
 * @brief Module deinit registration
 */
module_exit(myexit);

MODULE_AUTHOR("Abhishek Chauhan");
MODULE_LICENSE("GPL");
