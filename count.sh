#!/bin/sh

# Coloca o total de linhas em arquivos .cpp .h e .py relevantes dentro do total-lines.txt, junto com a data em que foi contado.

echo $(date) ---- $(count-lines.sh | tail -n1) lines >> total-lines.txt
