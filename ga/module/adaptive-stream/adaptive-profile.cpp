/*
* Copyright (c) 2015 Suliadi Marsetya
*
*
*/

#include <fl/Headers.h>
#include <stdio.h>
#include <errno.h>
#include <list>
#include "ga-common.h"
#include "ga-conf.h"
#include "ga-module.h"
#include "adaptive-profile.h"
#include <vector>

fl::Engine* engine;
std::vector<fl::InputVariable*> listInput;
std::vector<fl::OutputVariable*> listOutput;

std::string concatStr(char* str, int i)
{
	std::stringstream sstm;
	sstm << str << i;
	return sstm.str();
}

void test()
{
	ga_error("adaptive-stream: input variables count: %d\n",engine->numberOfInputVariables());
	ga_error("adaptive-stream: output variables count: %d\n",engine->numberOfOutputVariables());
	ga_error("adaptive-stream: ruleblocks count: %d\n",engine->numberOfRuleBlocks());
}

void parseTerm(std::string tmp, std::string key, int type, fl::Variable* var)
{
	ga_error("adaptive-stream: parsing term\n");
	if(type==0)
		var = (fl::InputVariable*) var;
	else
		var = (fl::OutputVariable*) var;

	if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"trapezoid")==0)
	{
		var->addTerm(new fl::Trapezoid(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point3").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point4").c_str())));
		//ga_error("adaptive-stream: trapezoid term\n");
	}
	else if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"triangle")==0)
	{
		var->addTerm(new fl::Triangle(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point3").c_str())));
		//ga_error("adaptive-stream: triangle term\n");
	}
	else if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"ramp")==0)
	{
		var->addTerm(new fl::Ramp(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str())));
		//ga_error("adaptive-stream: ramp term\n");
	}
	else if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"zshape")==0)
	{
		var->addTerm(new fl::ZShape(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str())));
		//ga_error("adaptive-stream: zshape term\n");
	}
	else if(strcmp(ga_conf_mapreadv(tmp.c_str(),(key+"-type").c_str(),NULL,16),"sshape")==0)
	{
		var->addTerm(new fl::SShape(ga_conf_mapreadv(tmp.c_str(),(key+"-name").c_str(),NULL,10),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point1").c_str()),
			ga_conf_mapreaddouble(tmp.c_str(),(key+"-point2").c_str())));
		//ga_error("adaptive-stream: sshape term\n");
	}
}

bool parseConf()
{
	int inNum = ga_conf_readint("adaptive-input-num");
	int outNum = ga_conf_readint("adaptive-output-num");
	int ruleNum = ga_conf_readint("adaptive-rule-num");
	
	ga_error("adaptive-stream: fuzzy input = %d\n", inNum);
	ga_error("adaptive-stream: fuzzy output = %d\n", outNum);
	ga_error("adaptive-stream: fuzzy rules = %d\n", ruleNum);

	while(inNum>0)
	{
		fl::InputVariable* input = new fl::InputVariable;
		input->setEnabled(true);
		std::string tmp =  concatStr("adaptive-input", inNum);
		input->setName(ga_conf_mapreadv(tmp.c_str(),"name",NULL,10));
		input->setRange(ga_conf_mapreaddouble(tmp.c_str(),"range-min"), ga_conf_mapreaddouble(tmp.c_str(),"range-max"));
		ga_error("adaptive-stream: range: %f, %f\n",ga_conf_mapreaddouble(tmp.c_str(),"range-min"), ga_conf_mapreaddouble(tmp.c_str(),"range-max"));
		int termNum = ga_conf_mapreadint(tmp.c_str(),"term-num");
		while(termNum>0)
		{
			std::string key = concatStr("term", termNum);
			parseTerm(tmp,key,0,input);
			termNum--;
		}
		engine->addInputVariable(input);
		listInput.push_back(input);
		inNum--;
	}

	ga_error("adaptive-stream: input loaded\n");

	while(outNum>0)
	{
		fl::OutputVariable* output = new fl::OutputVariable;
		output->setEnabled(true);
		std::string tmp = concatStr("adaptive-output", outNum);
		output->setName(ga_conf_mapreadv(tmp.c_str(),"name",NULL,10));
		output->setRange(ga_conf_mapreaddouble(tmp.c_str(),"range-min"), ga_conf_mapreaddouble(tmp.c_str(),"range-max"));
		ga_error("adaptive-stream: range: %f, %f\n",ga_conf_mapreaddouble(tmp.c_str(),"range-min"), ga_conf_mapreaddouble(tmp.c_str(),"range-max"));
		//output->fuzzyOutput()->setAccumulation(new fl::Maximum());
		//output->setDefuzzifier(new fl::Centroid(200));
		output->setDefaultValue(fl::nan);
		output->setLockPreviousOutputValue(false);
		output->setLockOutputValueInRange(false);

		int termNum = ga_conf_mapreadint(tmp.c_str(),"term-num");
		while(termNum>0)
		{
			std::string key = concatStr("term", termNum);
			parseTerm(tmp,key,0,output);
			termNum--;
		}
		engine->addOutputVariable(output);
		listOutput.push_back(output);
		outNum--;
	}
	
	ga_error("adaptive-stream: output loaded\n");

	fl::RuleBlock* rules = new fl::RuleBlock;
	rules->setEnabled(true);
	rules->setName("");
	//rules->setConjunction(new fl::Minimum());
	//rules->setDisjunction(new fl::Maximum());
	//rules->setActivation(new fl::Minimum());

	while(ruleNum>0)
	{
		std::string key = concatStr("adaptive-rule", ruleNum);
		std::string set = std::string(ga_conf_readv(key.c_str(),NULL,128));
		ga_error("adaptive-stream: %s\n",set.c_str());
		rules->addRule(fl::Rule::parse(set, engine));
		ruleNum--;
	}
	engine->addRuleBlock(rules);
	ga_error("adaptive-stream: rules loaded\n");

	engine->configure("Minimum","Maximum","Minimum","Maximum","Centroid");
	//engine->configure("AlgebraicProduct", "AlgebraicSum", "AlgebraicProduct", "AlgebraicSum", "Centroid");

	std::string status;
    if (!engine->isReady(&status))
		std::cout << "adaptive-stream: error: " << status << std::endl;
	
	test();
	return true;
}

void profileMain()
{
	ga_error("adaptive-stream: Fuzzy engine init\n");
	engine = new fl::Engine;
	engine->setName("Adaptive-stream");
	parseConf();
}

ga_ioctl_reconfigure_t createParam()
{
	fl::scalar Crf, Bitrate, Vbv;

	engine->process();
	for(int i=0; i<listOutput.size(); i++)
	{
		if(listOutput[i]->getName() == "Crf"){
			Crf = listOutput[i]->getOutputValue();
			std::cout << "adaptive-stream: Crf: " << fl::Op::str(Crf) << std::endl;
		}else if(listOutput[i]->getName() == "Bitrate"){
			Bitrate = listOutput[i]->getOutputValue();
			std::cout << "adaptive-stream: Bitrate: " << fl::Op::str(Bitrate) << std::endl;
		}else if(listOutput[i]->getName() == "Vbv"){
			Vbv = listOutput[i]->getOutputValue();
			std::cout << "adaptive-stream: Vbv-Buffer: " << fl::Op::str(Vbv) << std::endl;
		}
	}
	ga_ioctl_reconfigure_t params;
	params.id = 0;
	params.bitrateKbps = (int) Bitrate;
	params.bufsize = (int) Vbv;
	params.crf = (int) Crf;
	params.framerate_n = 60;
	params.framerate_d = 1;
	return params;
}

ga_ioctl_reconfigure_t selectProfile(float loss, float delay, unsigned jitter) //input loss, delay, jitter
{
	ga_error("adaptive-stream: reconfigure %.3f %.3f %u\n", loss, delay, jitter);

	for(int i=0; i<listInput.size(); i++)
	{
		if(listInput[i]->getName() == "Loss"){
			fl::scalar sloss = listInput[i]->getMinimum() + loss + 0.000001;
			listInput[i]->setInputValue(sloss);
			std::cout << "adaptive-stream: Loss: " << fl::Op::str(sloss) << std::endl;
		}else if(listInput[i]->getName() == "Delay"){
			fl::scalar sdelay = listInput[i]->getMinimum() + delay;
			listInput[i]->setInputValue(sdelay);
			std::cout << "adaptive-stream: Delay: " << fl::Op::str(sdelay) << std::endl;
		}else if(listInput[i]->getName() == "Jitter"){
			fl::scalar sjitt = listInput[i]->getMinimum() + jitter;
			listInput[i]->setInputValue(sjitt);
			std::cout << "adaptive-stream: Jitter: " << fl::Op::str(sjitt) << std::endl;
		}
	}
	return createParam();
}