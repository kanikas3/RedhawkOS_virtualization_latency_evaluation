irq_end=0
irq_start=0
schedule_end=0
counter=0
irq_time=0
irq_schedule_time=0
irq_enable=0
irq_disable=0
schedule_enable=0
get_log=0
file = open("dmesg.txt", "r")
for line in file:
	if "START_TIMER" in line:
		if (get_log == 0):
			irq_enable=1
			continue
		irq_time += irq_end - irq_start
		irq_schedule_time += schedule_end - irq_end
		get_log=0
		counter += 1;
		irq_start = 0
		irq_end = 0
		schedule_start = 0
		schedule_end = 0
		irq_enable=1
	elif "IRQ_START" in line:
		if (irq_enable == 0):
			schedule_enable=0
			irq_disable=0
			continue
		irq_enable=0
		irq_disable=1
		irq_start = line.split("]");
		irq_start = irq_start[0][1:]
		irq_start = float(irq_start)
	elif "IRQ_EXIT" in line:
		if (irq_disable == 0):
			schedule_enable=0
			irq_enable=0
			continue
		irq_disable=0
		schedule_enable=1
		irq_end = line.split("]");
		irq_end = irq_end[0][1:]
		irq_end = float(irq_end)
	elif "TASK_SWITCH" in line:
		if (schedule_enable == 0):
			irq_enable=0
			irq_disable=0
			continue
		schedule_enable=0
		get_log=1
		schedule_end = line.split("]");
		schedule_end = schedule_end[0][1:]
		schedule_end = float(schedule_end)
print ""
print "IRQ_TIME-> ", irq_time / (counter - 1) * 1000000
print "IRQ_END->SCHEDULE-> ", irq_schedule_time / (counter - 1) * 1000000
