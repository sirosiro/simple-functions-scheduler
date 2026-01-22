
import threading
from typing import Callable, List, Optional, Dict

SFS_NAME_SIZE = 16
SFS_WORK_SIZE = 32
BODY = 8

class SFS_tg:
    """Task Control Block (TCB)"""
    def __init__(self, name: str = "", order: int = 0, p_function: Optional[Callable[[], None]] = None):
        self.name: str = name
        self.order: int = order
        self.p_front: Optional[SFS_tg] = None
        self.p_back: Optional[SFS_tg] = None
        self.p_function: Optional[Callable[[], None]] = p_function
        self.work: bytearray = bytearray(SFS_WORK_SIZE)

class SFS:
    """Simple Function Scheduler"""

    def __init__(self):
        self._sfs_tasks: List[SFS_tg] = [SFS_tg() for _ in range(BODY)]
        self._p_task: Optional[SFS_tg] = None
        self._p_pool: Optional[SFS_tg] = None
        self._p_now: Optional[SFS_tg] = None
        self._task_map: Dict[str, SFS_tg] = {}
        self.initialize()

    def initialize(self):
        """Initializes the scheduler."""
        self._p_pool = self._sfs_tasks[0]
        for i in range(BODY - 1):
            self._sfs_tasks[i].p_back = self._sfs_tasks[i+1]
        self._sfs_tasks[BODY - 1].p_back = None
        self._p_task = None
        self._p_now = None
        self._task_map = {}
        return 0

    def _regist(self, p_regist: SFS_tg):
        """Registers a task in the active task list, sorted by order."""
        if self._p_task is None:
            self._p_task = p_regist
            p_regist.p_front = None
            p_regist.p_back = None
            return

        p_find = self._p_task
        while p_find:
            if p_regist.order < p_find.order:
                p_regist.p_front = p_find.p_front
                p_regist.p_back = p_find
                if p_find.p_front:
                    p_find.p_front.p_back = p_regist
                else:
                    self._p_task = p_regist
                p_find.p_front = p_regist
                return
            if p_find.p_back is None:
                p_find.p_back = p_regist
                p_regist.p_front = p_find
                p_regist.p_back = None
                return
            p_find = p_find.p_back

    def _giveup(self):
        """Removes the current task from the active list and returns it to the pool."""
        p_give = self._p_now
        if p_give is None:
            return

        if p_give.p_front:
            p_give.p_front.p_back = p_give.p_back
        else:
            self._p_task = p_give.p_back

        if p_give.p_back:
            p_give.p_back.p_front = p_give.p_front
        
        del self._task_map[p_give.name]

        p_give.p_back = self._p_pool
        self._p_pool = p_give
        self._p_now = None
        
    def dispatch(self) -> int:
        """Dispatches and executes all tasks in the current active list."""
        count = 0
        self._p_now = self._p_task
        while self._p_now:
            p_next = self._p_now.p_back
            if self._p_now.p_function:
                self._p_now.p_function()
            count += 1
            self._p_now = p_next
        return count

    def fork(self, name: str, order: int, func: Callable[[], None]) -> int:
        """Creates a new task and registers it with the scheduler."""
        if self._p_pool is None:
            return -1  # No available TCB

        p_new_task = self._p_pool
        self._p_pool = self._p_pool.p_back

        p_new_task.name = name
        p_new_task.order = order
        p_new_task.p_function = func
        
        self._regist(p_new_task)
        self._task_map[name] = p_new_task
        
        return 0

    def kill(self) -> int:
        """Marks the currently running task for termination."""
        if self._p_now:
            self._p_now.p_function = self._giveup
        return 0
    
    def change(self, name: str, order: int, entry_point: Callable[[], None]) -> int:
        """Changes the name, priority, and entry point of the currently running task."""
        if self._p_now is None:
            return -1 # Should not happen

        # Update name in map
        if self._p_now.name in self._task_map:
            del self._task_map[self._p_now.name]
        self._task_map[name] = self._p_now
        
        self._p_now.name = name
        self._p_now.p_function = entry_point

        # If order changes, we need to re-insert it
        if self._p_now.order != order:
            self._p_now.order = order
            
            # Unlink from the list
            if self._p_now.p_front:
                self._p_now.p_front.p_back = self._p_now.p_back
            else:
                self._p_task = self._p_now.p_back
            
            if self._p_now.p_back:
                self._p_now.p_back.p_front = self._p_now.p_front

            # Re-register
            self._regist(self._p_now)
            
        return 0

    def work(self) -> Optional[bytearray]:
        """Returns the work buffer of the currently running task."""
        if self._p_now:
            return self._p_now.work
        return None

    def other_work(self, name: str) -> Optional[bytearray]:
        """Returns the work buffer of a task specified by its name."""
        task = self._task_map.get(name)
        if task:
            return task.work
        return None

sfs_scheduler = SFS()
