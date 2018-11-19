config = dict(
    ammeter = dict(
        address = 22,
        configCommands = (
            'SYST:ZCOR OFF',
            #'RANG 2e-9',
            #'SYST:ZCH ON',
            #'INIT',
            #'SYST:ZCOR:ACQ',
            #'INIT',
            'SYST:ZCH OFF',
            #'SYST:ZCOR ON',
            'RANG:AUTO ON',
        ),
        operationCommands = ('',),
        closingCommands = ('',),
    ),
    vsource = dict(
        address = 24,
        configCommands = (
            ':ROUT:TERM REAR',
            ':SOUR:FUNC VOLT',
            ':SOUR:VOLT:MODE FIX',
            ':SOUR:VOLT:RANG 1000',
            ':SENS:FUNC "CURR"',
            ':SENS:CURR:PROT '+str(2.e-6),
            ':SENS:CURR:RANG:AUTO ON',
            ':SOUR:VOLT:LEV 0',
        ),
        operationCommands = (
            ':OUTP ON',
        ),
        closingCommands = ('',),
    ),
    Vramp = range(0, 1050, 50), # Voltages to ramp (start, highest (+1 step), step)
    bothPolarities = False,
    rampDown = False,
    Vtest = 1000, # Voltage to test stability (abs value)
    numRepetitions = 10, # current values per voltage
    stableTime = 50, # time for stabilizing after changing voltage (in seconds)
    timeAtTest = 10*60, # timein stability test at voltage Vtest (in seconds)
)

