

from os.path import isfile, getsize
from ast import literal_eval as lit_eval
import numpy as np


class SimData(object):

    def __init__(self, filename):
        super(SimData, self).__init__()

        metaData, headerSizeInBytes = self._testFileAndRetrieveMetadata(filename)

        outresX = metaData['outresX']
        dataSize = 4
        channels = metaData['data_channels']
        entriesPerInstant = outresX*channels

        entriesPerInstant += 1*metaData['lines_contain_timestamp']

        totalBytesPerInstant = dataSize*entriesPerInstant

        self._filename = filename
        self._headerSizeInBytes = headerSizeInBytes
        self._entriesPerInstant = entriesPerInstant
        self._metaData = metaData
        self._totalBytesPerInstant = totalBytesPerInstant

        self._buildFieldData()


    @property
    def MetaData(self):
        return self._metaData

    @property
    def Data(self):
        return self._fieldData


    def _testFileAndRetrieveMetadata(self, filename):
        if not isfile(filename):
            print("File not found \'" + filename + "\'")
            raise FileNotFoundError

        with open(filename, mode='rb') as file:
            headerData = file.readline()

        try:
            header = headerData.decode('utf-8').strip('# ')
            metaData = lit_eval(header)
        except:
            raise Exception("Unsupported .oscb file format")

        if type(metaData) is not dict:
            raise Exception("Supported .oscb file by current SimData class is version 4 only. "
                            "For earlier versions using .osc, use class SimData_old.")

        if metaData['data_type'] != 'fp32':
            raise NotImplementedError

        if not metaData['lines_contain_timestamp']:
            raise Exception("Instants should contain timestamps")

        headerSizeInBytes = len(headerData)

        return metaData, headerSizeInBytes


    def _buildFieldData(self):
        md = self.MetaData
        outresX, outresT = md['outresX'], md['outresT']

        with open(self._filename, mode='rb') as file:
            file.seek(self._headerSizeInBytes)

            bufferSize = outresT * self._totalBytesPerInstant

            assert(getsize(self._filename) == (self._headerSizeInBytes+bufferSize))

            buffer = file.read(bufferSize)

        epi = self._entriesPerInstant

        numericData = np.frombuffer(buffer, np.float32)
        numericData.shape = (outresT, epi) # N+1 because there's timestamps included in file.
        timeStamps = numericData[:,0]
        numericData = numericData[:,1:outresX*2+1].reshape(outresT, outresX, md['data_channels'])



        # phi = numericData[:,1:outresX+1]
        # ddtPhi = numericData[:,(outresX+1):(2*outresX+1)]
        # arr_tuple = (phi, ddtPhi)
        # interleaved = np.vstack(arr_tuple).reshape(outresT, outresX, 2)
        # print(interleaved)

        self._timeStamps = timeStamps
        self._fieldData = numericData

        return numericData
