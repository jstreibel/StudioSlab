#include "NumericalIntegration.h"

#include "Maps/RtoR/Model/RtoRFieldState.h"


#define GENERATE_FOR_NTHREADS(N) \
case (N): \
stepper = new StepperRK<N, RtoR::FieldState>(dPhi); \
break;


NumericalIntegration::NumericalIntegration(const void *dPhi, OutputManager *outputManager)
        : outputManager(outputManager), dt(Allocator::getInstance().getNumericParams().getdt()), steps(0)
{
    const unsigned int numThreads = Allocator::getInstance().getDevice().get_nThreads();

    switch (numThreads) {
        GENERATE_FOR_NTHREADS(1);
        GENERATE_FOR_NTHREADS(2);
        GENERATE_FOR_NTHREADS(3);
        GENERATE_FOR_NTHREADS(4);
        GENERATE_FOR_NTHREADS(5);
        GENERATE_FOR_NTHREADS(6);
        GENERATE_FOR_NTHREADS(7);
        GENERATE_FOR_NTHREADS(8);
        GENERATE_FOR_NTHREADS(9);
        GENERATE_FOR_NTHREADS(10);
        GENERATE_FOR_NTHREADS(11);
        GENERATE_FOR_NTHREADS(12);
        GENERATE_FOR_NTHREADS(13);
        GENERATE_FOR_NTHREADS(14);
        GENERATE_FOR_NTHREADS(15);
        GENERATE_FOR_NTHREADS(16);
        default:
            throw "Number of threads must be <= 16.";
    }

    output(); // output do estado inicial
}

NumericalIntegration::~NumericalIntegration() {
    // std::cout << histogram;
}

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
