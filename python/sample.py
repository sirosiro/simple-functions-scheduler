import time
from sfs import SFS, SFS_tg, sfs_scheduler
from frcc import FRC, FRCGapCheckStart, FRCGapCheck, GetFreeRunCounter, FRCInterrupt

# Dummy interrupt functions for FRCInterrupt
def di_dummy():
    pass

def ei_dummy():
    pass

# Register dummy interrupt functions with FRCC
FRCInterrupt(di_dummy, ei_dummy)

# --- Task Definitions ---

task_counter_a = 0
task_counter_b = 0
task_counter_c = 0

def task_a():
    global task_counter_a
    task_counter_a += 1
    work_buffer = sfs_scheduler.work()
    if work_buffer:
        # Example of using the work buffer
        # In C, you might cast this to a struct. Here, we'll just store a simple value.
        work_buffer[0] = task_counter_a % 256
    print(f"Task A executed. Count: {task_counter_a}, Work[0]: {work_buffer[0] if work_buffer else 'N/A'}")
    if task_counter_a >= 10:
        print("Task A killing itself.")
        sfs_scheduler.kill()

def task_b():
    global task_counter_b
    task_counter_b += 1
    print(f"Task B executed. Count: {task_counter_b}")
    if task_counter_b == 5:
        print("Task B is changing Task C's priority to 1.")
        # Find Task C's TCB (not really needed since we change by name)
        # Assuming Task C exists
        sfs_scheduler.change("TaskC", 1, task_c) # Change C's priority to higher

def task_c():
    global task_counter_c
    task_counter_c += 1
    work_buffer_c = sfs_scheduler.work()
    if work_buffer_c:
        work_buffer_c[0] = task_counter_c % 256

    work_buffer_a = sfs_scheduler.other_work("TaskA")
    val_a = work_buffer_a[0] if work_buffer_a else 'N/A'

    print(f"Task C executed. Count: {task_counter_c}, My Work[0]: {work_buffer_c[0] if work_buffer_c else 'N/A'}, TaskA Work[0]: {val_a}")
    if task_counter_c >= 15:
        print("Task C killing itself.")
        sfs_scheduler.kill()

# --- Main Simulation Loop ---

def main():
    print("Initializing SFS scheduler...")
    sfs_scheduler.initialize()

    # Create FRC timers
    frc_timer1 = FRC()
    frc_timer2 = FRC()

    # Start tasks
    print("Forking tasks...")
    sfs_scheduler.fork("TaskA", 3, task_a) # Lower priority
    sfs_scheduler.fork("TaskB", 2, task_b)
    sfs_scheduler.fork("TaskC", 4, task_c) # Even lower priority initially

    # Start FRC timers
    FRCGapCheckStart(frc_timer1, 5000) # 5000 ms = 5 seconds
    FRCGapCheckStart(frc_timer2, 2000) # 2000 ms = 2 seconds

    start_sim_time = time.time()
    
    # Run the scheduler for a limited time or until no tasks are left
    while True:
        current_sim_time = time.time()
        if current_sim_time - start_sim_time > 10: # Run simulation for max 10 seconds
            print("Simulation time limit reached.")
            break

        # Check FRC timers
        if FRCGapCheck(frc_timer1) == 0:
            print(f"--- FRC Timer 1 (5s) elapsed at {GetFreeRunCounter()} ---")
            FRCGapCheckStart(frc_timer1, 5000) # Restart timer 1

        if FRCGapCheck(frc_timer2) == 0:
            print(f"--- FRC Timer 2 (2s) elapsed at {GetFreeRunCounter()} ---")
            # This timer is one-shot, so not restarting
            # FRCGapCheckStop(frc_timer2) can also be called if needed

        tasks_dispatched = sfs_scheduler.dispatch()
        if tasks_dispatched == 0 and sfs_scheduler._p_pool is not None:
             # If dispatch returns 0 and there are no active tasks, 
             # it means all tasks have killed themselves or none were forked.
             # Check if there are tasks in the pool (meaning they were killed)
            print("No active tasks to dispatch. Exiting simulation.")
            break

        # Small delay to simulate time passing and avoid busy-waiting
        time.sleep(0.1) # Simulate 100ms interval

    print("\nSimulation finished.")
    print(f"Final Task A count: {task_counter_a}")
    print(f"Final Task B count: {task_counter_b}")
    print(f"Final Task C count: {task_counter_c}")

if __name__ == "__main__":
    main()
