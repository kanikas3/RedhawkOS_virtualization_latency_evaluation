#ifndef __PROJECT5_H_
#define __PROJECT5_H_

#include<linux/kprobes.h>

#define PROJECT5_DECLARE_JPROBE(i, str) \
		static struct jprobe probe##i = { \
			.entry   = project5_##str, \
			.kp = { \
				.symbol_name    = #str,\
			}, \
		};

#define PROJECT5_REGISTER_JPROBE(i) \
        ret = register_jprobe(&probe##i); \
        if (ret < 0) { \
                pr_err("register_jprobe failed for %d, error %d\n", i, ret); \
                return ret; \
        }


#define PROJECT5_UNREGISTER_JPROBE(i) \
        unregister_jprobe(&probe##i);

#endif
