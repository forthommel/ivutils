config = dict(
    ammeter = dict(
        address = 22,
        preConfiguration = (
            'test1',
        ),
    ),
    vsource = dict(
        address = 24,
        preConfiguration = (
            'test2',
        ),
    ),
    Vramp = range(0, 1050, 50), # Voltages to ramp (start, highest (+1 step), step)
    bothPolarities = False,
    rampDown = False,
    compl = 2.e-6,
    Vtest = 1000, # Voltage to test stability (abs value)
    numRepetitions = 10, # current values per voltage
    stableTime = 50, # time for stabilizing after changing voltage (in seconds)
    timeAtTest = 10*60, # timein stability test at voltage Vtest (in seconds)
    modules = ('ammeter', 'vsource'), # modules to use
)

