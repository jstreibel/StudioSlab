from .c_launcher import sim_python

def LaunchField(phi, dphidt, xMin, xMax):
	assert(phi.shape == dphidt.shape)

	print(phi, dphidt, xMin, xMax)

	sim_python.setPhi(phi)
	sim_python.setdPhidt(dphidt)
	sim_python.setMetaData(xMin, xMax)

	sim_python.run()
