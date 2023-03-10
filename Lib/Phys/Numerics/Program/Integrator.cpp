#include "Integrator.h"

// #include "Maps/RtoR/Model/RtoRFieldState.h"

NumericalIntegration::NumericalIntegration(const void *dPhi, OutputManager *outputManager)
    : outputManager(outputManager),
      dt(Allocator::getInstance().getNumericParams().getdt()),
      steps(0)
{ }

NumericalIntegration::~NumericalIntegration()
{    std::cout << histogram;     }

void NumericalIntegration::step(PosInt nSteps) {
    histogram.startMeasure();
    stepper->step(dt, nSteps);
    histogram.storeMeasure(nSteps);

    steps += nSteps;

    output();
}

OutputPacket NumericalIntegration::getOutputInfo(){
    /* Aqui pegamos o campo toda santa vez que queremos fazer output dele.
     * Isso eh feito para que, quando o campo eh integrado na GPU, ele possa
     * ser trazido de la. Quando integrado na CPU, isso nao acontece.
     */
    return OutputPacket(stepper->getFieldState(), stepper->getSpaces(), steps, getT());;
}

void NumericalIntegration::runFullIntegration()
{
    size_t n = Allocator::getInstance().getNumericParams().getn();

    while(steps < n){
        const size_t nStepsUntilNextOutput = outputManager->computeNStepsToNextOutput(steps);

        step(nStepsUntilNextOutput);
    }

    // Para cumprir com os steps quebrados:
    if(steps < n) step(n-steps);

    outputManager->notifyIntegrationFinished(getOutputInfo());
}

void NumericalIntegration::output(){
    OutputPacket info = getOutputInfo();
    outputManager->output(info);
}

size_t NumericalIntegration::getSteps() const { return steps; }

floatt NumericalIntegration::getT() { return floatt(getSteps()) * dt; }

const BenchmarkHistogram &NumericalIntegration::getHistogram() const {
    return histogram;
}
