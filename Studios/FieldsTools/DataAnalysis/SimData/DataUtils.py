from ast import literal_eval as lit_eval

from numpy import asarray, linspace, gradient, mean, abs as np_abs, array, sqrt

from Utils import utils

def _open_old(file, line1, verbose):
    param_keys = [val for val in line1.replace(' ', '').strip('#').strip().split('|')]
    param_vals= [float(val) if utils.isfloat(val) else val for val in file.readline().split()]
    phi = [[float(digit) for digit in line.split()] for line in file]
    phi = asarray(phi)

    #grad = np.gradient(phi)
    #H = (grad[0]**2 + grad[1]**2 + np.abs(phi)) + 1.e-6
    dictionary = dict(zip(param_keys, param_vals))
    if verbose:
        for ting in dictionary:
            print(ting, "=", dictionary[ting])
    dictionary.update({'phi': phi, 'filename': file.name})

    if not 'initTime' in dictionary:
        dictionary['initTime'] = 0.

    return dictionary


def _extract_timestamps(phi):
    timestamps = phi[:,:1].flatten()
    phi = phi[:,1:]

    return timestamps, phi


def _open_sim_data(filename, verbose=False):
    with open(filename) as file:
        try:
            line1 = file.readline().strip('# ')
            oscDict = lit_eval(line1)

            if not isinstance(oscDict, dict):
                raise Exception("File is not known oscillon file.")
        except ValueError:
            if verbose: print(utils.WARNING + "THIS .osc FILE FORMAT IS DEPRECATED.")
            oscDict = _open_old(file, line1, verbose)
            oscDict['Ver'] = 1
            oscDict["lines_contain_timestamp"] = False

        if oscDict['Ver'] == 1:
            pass
        elif oscDict['Ver'] == 2:
            line2 = file.readline()
            extraDict = _open_old(file, line2, verbose)
            oscDict.update(extraDict)
        elif oscDict['Ver'] in {3, 4}:
            phi = [[float(digit) for digit in line.split()] for line in file]
            phi = asarray(phi)

            oscDict.update({'phi': phi, 'filename': file.name})
        else:
            raise Exception("Osc version unknown.")

        if oscDict["lines_contain_timestamp"]:
            timestamps, oscDict['phi'] = _extract_timestamps(oscDict['phi'])
            oscDict['mean_dt'] = mean(gradient(timestamps))
            oscDict['timestamps'] = timestamps

    return oscDict