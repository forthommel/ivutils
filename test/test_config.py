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
            ':ROUT:TERM REAR',              # switch output terminals to rear panel
            ':SOUR:FUNC VOLT',              # select voltage source function
            ':SOUR:VOLT:MODE FIX',          # select fixed voltage source mode
            ':SOUR:VOLT:RANG 1000',         # select 20V source range
            ':SENS:FUNC "CURR"',            # select current measurement function
            ':SENS:CURR:PROT '+str(2.e-6),  # set compliance limit to 2 uA
            ':SENS:CURR:RANG:AUTO ON',      # enable current measurement autoranging
            ':SOUR:VOLT:LEV 0',             # set the source voltage level to 0
        ),
        operationCommands = (
            ':OUTP ON', # turn the output on
        ),
        closingCommands = (
            ':OUTP OFF',
        ),
    ),
    #Vramp = range(0, 1050, 50), # Voltages to ramp (start, highest (+1 step), step)
    #Vtest = 1000., # Voltage to test stability (abs value)
    #stableTime = 50, # time for stabilizing after changing voltage (in seconds)
    timeAtTest = 10*60, # timein stability test at voltage Vtest (in seconds)
    numRepetitions = 10, # current values per voltage
    bothPolarities = False,
    rampDown = False,
    # my testing
    Vramp = [n*0.1 for n in range(0, 10, 1)], # Voltages to ramp (start, highest (+1 step), step)
    Vtest = 1., # Voltage to test stability (abs value)
    stableTime = 1, # time for stabilizing after changing voltage (in seconds)
)
