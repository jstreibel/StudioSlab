/*
 *  CPUStrategy.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 16/12/2010.
 *  No rights reserved.
 *
 */

#include "SSMGenerationStrategy.h"

class CPUStrategy : public SSMGenerationStrategy
{
public:
	CPUStrategy(){
		this->location = HOST;
		powerOfTwoMaps = true;
		squareMaps = true;
		squareLight = true;
		generatesIntermediateData = false;
		mapWidth = 64;
		mapHeight = 64;
	}
	
	void computeSSM(){
		
	}
};