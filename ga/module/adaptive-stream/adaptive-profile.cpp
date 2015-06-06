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
	ga_error("adaptive-stream: rules count: %d\n",engine->numberOfRuleBlocks());
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
		input->setRange(0.000, 1.000);

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
		output->setRange(0.000, 1.000);

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

	engine->configure(new fl::AlgebraicProduct, new fl::AlgebraicSum, new fl::AlgebraicProduct, new fl::AlgebraicSum, new fl::Centroid);

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
	for(int i=0; i<listOutput.size(); i++)
	{
		if(listOutput[i]->getName() == "Crf"){
			//listOutput[i]->defuzzify();
			Crf = listOutput[i]->getOutputValue();
			ga_error("adaptive-stream: Crf: %s\n", fl::Op::str(Crf));
		}else if(listOutput[i]->getName() == "Bitrate"){
			Bitrate = listOutput[i]->getOutputValue();
			ga_error("adaptive-stream: Bitrate: %s\n", fl::Op::str(Bitrate));
		}else if(listOutput[i]->getName() == "Vbv"){
			Vbv = listOutput[i]->getOutputValue();
			ga_error("adaptive-stream: Vbv-Buffer: %s\n", fl::Op::str(Vbv));
		}
	}
	ga_ioctl_reconfigure_t params;
	params.id = 0;
	params.bitrateKbps = Bitrate;
	params.bufsize = Vbv;
	params.crf = Crf;
	params.framerate_n = 60;
	params.framerate_d = 1;
	return params;
}

ga_ioctl_reconfigure_t selectProfile(fl::scalar loss, fl::scalar delay, fl::scalar jitter) //input loss, delay, jitter
{
	ga_error("adaptive-stream: reconfigure %s %s %s\n", fl::Op::str(loss), fl::Op::str(delay), fl::Op::str(jitter));

	for(int i=0; i<listInput.size(); i++)
	{
		if(listInput[i]->getName() == "Loss"){
			listInput[i]->setInputValue(loss);
			ga_error("adaptive-stream: Loss: %d\n", fl::Op::str(listInput[i]->getInputValue()));
		}else if(listInput[i]->getName() == "Delay"){
			listInput[i]->setInputValue(delay);
			ga_error("adaptive-stream: Delay: %d\n", fl::Op::str(listInput[i]->getInputValue()));
		}else if(listInput[i]->getName() == "Jitter"){
			listInput[i]->setInputValue(jitter);
			ga_error("adaptive-stream: Jitter: %d\n", fl::Op::str(listInput[i]->getInputValue()));
		}
	}
	engine->process();
	return createParam();
}