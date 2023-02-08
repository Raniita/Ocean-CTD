#
#   Implementation of UNESCO formula to obtain Salinity given the inputs:
#       - Electrical conductivity (uS/cm)
#       - Temperature (Celsius, T90)
#       - Depth (meters)
#
#   Output: Salinity in pps
#
#   Method extracted from: Chapter 3. Determination of salinity [Methods of Seawater Analysis]
#

coef_Rp = {
    "e1": 0.0000207,
    "e2": -0.000000000637,
    "e3": 3.989E-15,
    "d1": 0.03426,
    "d2": 0.0004464,
    "d3": 0.4215,
    "d4": -0.003107
}

coef_rt = {
    "c0": 0.6766097,
    "c1": 0.0200664,
    "c2": 0.0001104259,
    "c3": -0.00000069698,
    "c4": 0.0000000010031
}

coef_AS = {
    "b0": 0.0005,
    "b1": -0.0056,
    "b2": -0.0066,
    "b3": -0.0375,
    "b4": 0.0636,
    "b5": -0.0144,
    "k": 0.0162
}

coef_S = {
    "a0": 0.008,
    "a1": -0.1692,
    "a2": 25.3851,
    "a3": 14.0941,
    "a4": -7.0261,
    "a5": 2.7081
}

def get_salinity(temperature, depth, ce) -> float:
    """
        Inputs:
        - temp (celsius T90)
        - depth (meters)
        - ce (uS/cm)

        Output:
        - S: salinity in pps78 scale
    """
    
    # Transform inputs:
    temp =  float(temperature) * 1.00024
    pressure = float(depth) * 1.0197442889221
    specific_conductance = (float(ce)/ (1+0.02*(temp-25)))/1000

    # Chapter 3. Determination of salinity [Methods of Seawater Analysis]
    R = float(ce) / 42914
    Rp = 1 + pressure * (coef_Rp["e1"] + (coef_Rp["e2"] * pressure) + (coef_Rp["e3"]*pressure**2))/(1 + coef_Rp["d1"]*temp + coef_Rp["d2"]*temp**2 + (coef_Rp["d3"]*coef_Rp["d4"])*R)
    rt = coef_rt["c0"] + coef_rt["c1"]*temp + coef_rt["c2"]*temp**2 + coef_rt["c3"]*temp**3 + coef_rt["c4"]*temp**4
    Rt = R / (Rp*rt)
    AS = (temp-15)/(1+(coef_AS["k"]*(temp-15))) * (coef_AS["b0"]+coef_AS["b1"]*Rt**0.5 + coef_AS["b2"]*Rt+coef_AS["b3"]*Rt**1.5 + coef_AS["b4"]*Rt**2 + coef_AS["b5"]*Rt**2.5)
    S = coef_S["a0"] + coef_S["a1"]*Rt**0.5 + coef_S["a2"]*Rt + coef_S["a3"]*Rt**1.5 + coef_S["a4"]*Rt**2 + coef_S["a5"]*Rt**2.5 + AS

    # Output: salinity (pps)
    return S