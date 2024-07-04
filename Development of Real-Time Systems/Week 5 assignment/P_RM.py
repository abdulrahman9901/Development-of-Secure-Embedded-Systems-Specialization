from simso.core.Scheduler import SchedulerInfo
from simso.utils import PartitionedScheduler
from simso.schedulers import scheduler

@scheduler("simso.schedulers.P_RM")
class P_RM(PartitionedScheduler):
    def init(self):
        PartitionedScheduler.init(
            self, SchedulerInfo("simso.schedulers.RM_mono"))
            
    def urm(x):
        return x * (2.0**(1.0/x) - 1.0)
        
    def packer(self):
        # First Fit
        cpus = [[cpu, 0] for cpu in self.processors]
        for index, task in enumerate(self.task_list):
            j = 0
            # Find the processor with the lowest load.
            for i, c in enumerate(cpus):
                if (c[1] + (float(task.wcet) / task.period)) < urm(c[0].size + 1.0):
                    j = i
                    break

            # Affect it to the task.
            self.affect_task_to_processor(task, cpus[j][0])

            # Update utilization.
            cpus[j][1] += float(task.wcet) / task.period
        return True
