
import time
from typing import Callable, Optional

# Constants based on the C definition
FRC_ONE_SHOT_UNUSED = -1
FRC_ONE_SHOT_MONITORING = 1
FRC_ONE_SHOT_EXPIRED = 0

class FRC:
    """Structure for Free Run Counter Gap Check"""
    def __init__(self):
        self.one_shot: int = FRC_ONE_SHOT_UNUSED
        self.start_point: int = 0
        self.stop_gap: int = 0

# Global Free Run Counters (simulated)
_g_free_run_counter_mini: int = 0  # 8-bit counter
_g_free_run_counter: int = 0      # 32-bit counter

# Scaling factor to simulate counter increment frequency
# For simplicity, let's say 1 unit of counter = 1 millisecond
# So, 1000 units = 1 second
_COUNTER_SCALE_MS = 1000

_di_function: Optional[Callable[[], None]] = None
_ei_function: Optional[Callable[[], None]] = None

_start_time = time.monotonic()

def _update_counters():
    """Internal function to update simulated global counters."""
    global _g_free_run_counter_mini, _g_free_run_counter
    elapsed_ms = int((time.monotonic() - _start_time) * _COUNTER_SCALE_MS)

    _g_free_run_counter = elapsed_ms & 0xFFFFFFFF  # Simulate 32-bit wrap-around
    _g_free_run_counter_mini = elapsed_ms & 0xFF    # Simulate 8-bit wrap-around

def FRCInterrupt(di: Callable[[], None], ei: Callable[[], None]):
    """
    Registers interrupt disable/enable functions.
    In this Python simulation, these functions are called but do nothing
    as Python's GIL handles thread safety, and we're not dealing with
    actual hardware interrupts.
    """
    global _di_function, _ei_function
    _di_function = di
    _ei_function = ei

def GetFreeRunCounter() -> int:
    """
    Returns the current 32-bit free run counter value.
    Simulates interrupt-safe access.
    """
    if _di_function:
        _di_function()
    _update_counters()
    counter_value = _g_free_run_counter
    if _ei_function:
        _ei_function()
    return counter_value

def GetFreeRunGap(v_farst_count: int, v_second_count: int) -> int:
    """
    Calculates the time difference between two 32-bit counter values,
    handling counter rollover.
    """
    if v_second_count >= v_farst_count:
        return v_second_count - v_farst_count
    else:
        # Rollover occurred (2^32 for unsigned long)
        return (0xFFFFFFFF - v_farst_count) + v_second_count + 1

def GetFreeRunGapMini(v_past_count: int) -> int:
    """
    Calculates the time difference between the current 8-bit counter
    and a past 8-bit counter value, handling rollover.
    """
    _update_counters()
    i_now_count = _g_free_run_counter_mini
    if i_now_count >= v_past_count:
        return i_now_count - v_past_count
    else:
        # Rollover occurred (2^8 for unsigned char)
        return (0xFF - v_past_count) + i_now_count + 1

def FRCGapCheckStart(v_gap_chk: FRC, v_stop_gap: int):
    """
    Initializes an FRC structure for time elapsed checking.
    Records the current counter value as StartPoint and sets the StopGap.
    """
    v_gap_chk.start_point = GetFreeRunCounter()
    v_gap_chk.stop_gap = v_stop_gap
    v_gap_chk.one_shot = FRC_ONE_SHOT_MONITORING

def FRCGapCheck(v_gap_chk: FRC) -> int:
    """
    Checks if the time interval set in FRCGapCheckStart has elapsed.
    Returns 0 if time has elapsed, otherwise returns remaining time.
    """
    if v_gap_chk.one_shot == FRC_ONE_SHOT_MONITORING:
        current_time = GetFreeRunCounter()
        elapsed = GetFreeRunGap(v_gap_chk.start_point, current_time)
        if elapsed >= v_gap_chk.stop_gap:
            v_gap_chk.one_shot = FRC_ONE_SHOT_EXPIRED
            return 0  # Elapsed
        else:
            return v_gap_chk.stop_gap - elapsed # Remaining time
    elif v_gap_chk.one_shot == FRC_ONE_SHOT_EXPIRED:
        return 0 # Already elapsed
    
    return -1 # FRC_ONE_SHOT_UNUSED

def FRCGapCheckStop(v_gap_chk: FRC):
    """
    Stops and invalidates the time elapsed check for the FRC structure.
    """
    v_gap_chk.one_shot = FRC_ONE_SHOT_UNUSED
    v_gap_chk.start_point = 0
    v_gap_chk.stop_gap = 0
